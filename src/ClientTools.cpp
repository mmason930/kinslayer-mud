#include "conf.h"
#include "utils.h"
#include "db.h"
#include "Descriptor.h"
#include "ClientTools.h"
#include "handler.h"
#include "spells.h"
#include "interpreter.h"
#include "constants.h"
#include "rooms/Room.h"
#include "rooms/Exit.h"
#include "commands/infrastructure/CommandUtil.h"
#include "commands/infrastructure/CommandInfo.h"
#include "js/js.h"
#include "js/js_utils.h"
#include "utils/ClientSession.h"
#include "items/ObjectSelection.h"
#include <boost/uuid/uuid_io.hpp>

namespace {
bool playable(Descriptor *d) {
    return d->loggedIn && d->connected == CON_PLAYING && d->character &&
        !d->character->IsPurged() && !d->original && !IS_NPC(d->character) &&
        d->character->in_room && !d->str && !d->character->editorInterfaceInstance;
}
std::string keyword(Object *obj) {
    std::istringstream words(obj->getName() ? obj->getName() : "");
    std::string word;
    while (words >> word) {
        if (word.size() < 64 && word.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-") == std::string::npos)
            return word;
    }
    return "";
}
Json::Value item(Object *obj, int slot) {
    Json::Value value;
    value["id"] = boost::uuids::to_string(obj->objID);
    value["name"] = obj->GetSDesc();
    value["keyword"] = keyword(obj);
    value["slot"] = slot;
    value["weight"] = obj->Weight();
    value["actions"] = Json::Value(Json::arrayValue);
    auto &actions = value["actions"];
    actions.append("examine");
    if (slot >= 0) actions.append("remove");
    else {
        if (CAN_WEAR(obj, ITEM_WEAR_WIELD)) actions.append("wield");
        if (CAN_WEAR(obj, ITEM_WEAR_HOLD) || obj->getType() == ITEM_LIGHT) actions.append("hold");
        if (obj->obj_flags.wear_flags & ~(ITEM_WEAR_TAKE | ITEM_WEAR_WIELD | ITEM_WEAR_HOLD)) actions.append("wear");
        if (obj->getType() == ITEM_FOOD) actions.append("eat");
        if (obj->getType() == ITEM_DRINKCON) actions.append("drink");
        actions.append("value"); actions.append("sell"); actions.append("drop");
    }
    return value;
}
}

void Descriptor::processClientTools(const Json::Value &request) {
    Json::Value response;
    response["method"] = "Client Tools";
    if (request["requestId"].isInt()) response["requestId"] = request["requestId"];
    const std::string action = request["action"].isString() ? request["action"].asString() : "";
    response["action"] = action;
    Json::FastWriter writer;
    auto reply = [&] { sendWebSocketCommand(writer.write(response)); };
    if (!playable(this)) { response["error"] = "Sign in and leave the editor to use game tools."; reply(); return; }
    time_t now = time(nullptr);
    if (toolsRequestSecond != now) { toolsRequestSecond = now; toolsRequestCount = 0; }
    if (++toolsRequestCount > 10) { response["error"] = "Please wait a moment."; reply(); return; }
    if (action == "item" || action == "buy") {
        if (commandQueue.size() >= 20 || showstr_count) {
            response["error"] = "Finish the current command or pager first.";
        } else {
            // Resolve against current game state when this reaches the normal input queue.
            commandQueue.push_back(std::string("\1client:") + writer.write(request));
            response["queued"] = true;
        }
        reply(); return;
    }
    if (action != "init" && action != "snapshot" && action != "settings") {
        response["error"] = "Unknown game tools request."; reply(); return;
    }
    try {
        auto manager = JSManager::get()->executeExpression("global.clientTools").to_object();
        auto result = manager.call("response", writer.write(request), lookupValue(character));
        Json::Reader reader;
        Json::Value scriptResponse;
        if (result.is_string() && reader.parse(result.to_std_string(), scriptResponse))
            response["companion"] = scriptResponse;
    } catch (const std::exception &e) {
        response["companionError"] = "Tutorial and preferences are temporarily unavailable.";
        MudLog(BRF, LVL_APPR, TRUE, "Client tools: %s", e.what());
    }
    if (action == "settings") { reply(); return; }
    if (action == "init") {
        response["sessionToken"] = ClientSession::issue(GET_NAME(character), GET_PASSWD(character));
        response["commands"] = Json::Value(Json::arrayValue);
        auto commands = CommandUtil::get()->getCommandVector();
        for (unsigned i = 0; i < commands.size(); ++i) {
            auto command = commands[i];
            if (!character->HasAccessToCommand(i) || GET_LEVEL(character) < command->view_minimum_level || command->command.empty()) continue;
            Json::Value entry;
            entry["name"] = command->command;
            entry["social"] = command->is_social;
            response["commands"].append(entry);
        }
    }
    response["room"] = character->in_room->getVnum();
    response["inventory"] = Json::Value(Json::arrayValue);
    response["equipment"] = Json::Value(Json::arrayValue);
    response["targets"] = Json::Value(Json::arrayValue);
    response["exits"] = Json::Value(Json::arrayValue);
    const bool canSee = AWAKE(character) && !AFF_FLAGGED(character, AFF_BLIND) &&
        (!character->in_room->isDark() || CAN_SEE_IN_DARK(character)) &&
        !(character->dizzy_time && TAINT_CALC(character));
    response["canSee"] = canSee;
    response["gold"] = character->points.gold;
    unsigned count = 0;
    for (Object *obj = character->carrying; obj && count < 500; obj = obj->next_content) {
        if (visibleOwnedItem(character, obj)) { response["inventory"].append(item(obj, -1)); ++count; }
    }
    for (int slot = 0; slot < NUM_WEARS; ++slot) {
        Json::Value entry;
        entry["slot"] = slot; entry["label"] = equipment_types[slot];
        auto obj = GET_EQ(character, slot);
        if (visibleOwnedItem(character, obj)) entry["item"] = item(obj, slot);
        else if (obj) entry["unseen"] = true;
        response["equipment"].append(entry);
    }
    if (canSee) {
        for (Character *other = character->in_room->people; other; other = other->next_in_room) {
            if (other != character && !other->IsPurged() && CAN_SEE(character, other)) {
                Json::Value target;
                target["name"] = PERS(other, character);
                response["targets"].append(target);
            }
        }
        for (int dir = 0; dir < NUM_OF_DIRS; ++dir) {
            auto exit = character->in_room->dir_option[dir];
            if (exit && exit->getToRoom() && !exit->isDisabled() && !((exit->getHiddenLevel() > 0) && exit->isClosed())) {
                Json::Value value; value["name"] = dirs[dir]; value["closed"] = exit->isClosed();
                response["exits"].append(value);
            }
        }
    }
    if (request["shop"].isBool() && request["shop"].asBool()) response["shop"] = clientShopSnapshot(character);
    reply();
}

std::string Descriptor::resolveClientAction(const Json::Value &request) {
    if (!playable(this) || showstr_count) return "";
    if (!request["room"].isInt() || request["room"].asInt() != character->in_room->getVnum()) return "";
    if (request["action"] == "buy") return clientShopCommand(character, request);
    if (!request["id"].isString() || !request["verb"].isString()) return "";
    std::string id = request["id"].asString(), verb = request["verb"].asString();
    Object *selected = nullptr;
    int slot = -1;
    for (Object *obj = character->carrying; obj; obj = obj->next_content)
        if (boost::uuids::to_string(obj->objID) == id) selected = obj;
    for (int i = 0; i < NUM_WEARS; ++i)
        if (GET_EQ(character, i) && boost::uuids::to_string(GET_EQ(character, i)->objID) == id) { selected = GET_EQ(character, i); slot = i; }
    if (!visibleOwnedItem(character, selected)) return "";
    auto allowed = item(selected, slot)["actions"];
    bool valid = false;
    for (const auto &action : allowed) if (action.asString() == verb) valid = true;
    std::string word = keyword(selected);
    if (!valid || word.empty()) return "";
    // Tutorial 501 deliberately teaches literal keywords and validates those
    // commands. For other play, an exact selector disambiguates duplicate names.
    if (character->countJS(501)) return verb + " " + word;
    return verb + " @" + id;
}

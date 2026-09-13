// The runner includes the real Descriptor map producer against these small
// world fixtures, so visibility transitions can be tested without a database.
#include "jsoncpp/json.h"
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

constexpr int CON_PLAYING = 0, NUM_OF_DIRS = 6;
constexpr int NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3, UP = 4, DOWN = 5;
struct Room;
struct Exit {
    Room *destination;
    bool closed = false, hidden = false, disabled = false;
    bool isDisabled() const { return disabled; }
    bool isClosed() const { return closed; }
    int getHiddenLevel() const { return hidden ? 10 : 0; }
    Room *getToRoom() const { return destination; }
};
struct Sector { std::string getStandardName() const { return "City"; } };
struct Room {
    int id;
    std::string name;
    bool dark = false;
    Exit *dir_option[NUM_OF_DIRS] = {};
    Sector sector;
    int getVnum() const { return id; }
    const char *getName() const { return name.c_str(); }
    bool isDark() const { return dark; }
    Sector *getSector() { return &sector; }
};
struct Character {
    Room *in_room;
    bool awake = true, blind = false, darkVision = false;
    bool dizzy_time = false, taint = false, purged = false;
    bool IsPurged() const { return purged; }
};
#define AWAKE(ch) ((ch)->awake)
#define AFF_FLAGGED(ch, flag) ((ch)->blind)
#define TAINT_CALC(ch) ((ch)->taint)
#define CAN_SEE_IN_DARK(ch) ((ch)->darkVision)

struct Descriptor {
    Character *character;
    bool loggedIn = true;
    int connected = CON_PLAYING;
    std::string lastMiniMap;
    std::vector<Json::Value> sent;
    void sendWebSocketMiniMapCommand();
    void sendWebSocketCommand(const std::string &message) {
        Json::Value value;
        Json::Reader reader;
        assert(reader.parse(message, value));
        sent.push_back(value);
    }
};

#include "mini-map-under-test.inc"

static bool hasRoom(const Json::Value &map, int id) {
    for (unsigned i = 0; i < map["rooms"].size(); ++i)
        if (map["rooms"][i]["id"].asInt() == id) return true;
    return false;
}
static bool hasExit(const Json::Value &map, int from, int to) {
    for (unsigned i = 0; i < map["exits"].size(); ++i)
        if (map["exits"][i]["from"].asInt() == from && map["exits"][i]["to"].asInt() == to) return true;
    return false;
}

int main() {
    Room start{1, "Start"}, secret{2, "Secret chamber"}, beyond{3, "Beyond secret"};
    Exit door{&secret, false, true}, farther{&beyond};
    start.dir_option[NORTH] = &door;
    secret.dir_option[NORTH] = &farther;
    Character player{&start};
    Descriptor client{&player};
    auto update = [&]() -> const Json::Value & {
        client.sendWebSocketMiniMapCommand();
        assert(!client.sent.empty());
        return client.sent.back();
    };

    // Opening a hidden door exposes the route. Closing it removes both the
    // connection and rooms reachable only through it, even after an earlier map.
    assert(hasRoom(update(), 3));
    door.closed = true;
    const auto &hidden = update();
    assert(hidden["rooms"].size() == 1);
    assert(hidden["exits"].size() == 0);
    assert(client.lastMiniMap.find("Secret chamber") == std::string::npos);
    assert(client.lastMiniMap.find("Beyond secret") == std::string::npos);
    door.closed = false;
    assert(hasRoom(update(), 3));
    door.disabled = true;
    assert(!hasRoom(update(), 2));
    door.disabled = false;

    // An ordinary closed door may show its far side, but traversal stops there.
    door.hidden = false;
    door.closed = true;
    const auto &closed = update();
    assert(hasRoom(closed, 2) && !hasRoom(closed, 3));

    // A separately visible route must not make the hidden door appear.
    Room side{4, "Side passage"};
    Exit around{&side}, intoSecret{&secret};
    start.dir_option[EAST] = &around;
    side.dir_option[NORTH] = &intoSecret;
    door.hidden = true;
    const auto &alternate = update();
    assert(hasRoom(alternate, 2));
    assert(!hasExit(alternate, 1, 2));
    assert(!hasRoom(alternate, 3)); // Outside the two-exit radius on this route.

    // The same room becoming dark invalidates its previously visible snapshot.
    start.dark = true;
    const auto &dark = update();
    assert(dark["currentRoomId"].isNull());
    assert(dark["rooms"].size() == 0 && dark["exits"].size() == 0);
    assert(dark["message"].asString() == "It is too dark to see.");
    size_t count = client.sent.size();
    update();
    assert(client.sent.size() == count);
    player.darkVision = true;
    assert(hasRoom(update(), 1));
    player.darkVision = false;
    assert(update()["currentRoomId"].isNull());
    start.dark = false;
    assert(hasRoom(update(), 1));

    start.dir_option[EAST] = nullptr;
    door.closed = false;
    secret.dark = true;
    const auto &darkNeighbor = update();
    assert(hasRoom(darkNeighbor, 2) && !hasRoom(darkNeighbor, 3));
    assert(client.lastMiniMap.find("Secret chamber") == std::string::npos);
    player.blind = true;
    assert(update()["rooms"].size() == 0);
    player.blind = false;
    client.loggedIn = false;
    assert(update()["rooms"].size() == 0);
    std::cout << "Mini map visibility: hidden doors, door transitions, alternate paths, darkness, night vision, restoration, radius and authentication passed.\n";
}

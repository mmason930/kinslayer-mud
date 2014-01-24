#include "PlayerPortalCommandProcessor.h"
#include "utils.h"
#include "js.h"
#include "jsoncpp/json.h"
#include "constants.h"

#include <vector>

extern int top_of_objt;
extern std::vector<Object*> obj_proto;
extern std::vector<int> ItemCount;

PlayerPortalCommandProcessor::PlayerPortalCommandProcessor() {}
PlayerPortalCommandProcessor::~PlayerPortalCommandProcessor() {}

PlayerPortalJavaScriptCommandProcessor::PlayerPortalJavaScriptCommandProcessor() {}
PlayerPortalJavaScriptCommandProcessor::~PlayerPortalJavaScriptCommandProcessor() {}

void PlayerPortalJavaScriptCommandProcessor::process(PlayerPortalDescriptor *descriptor, Json::Value command)
{
	try
	{
		Json::FastWriter writer;
		std::stringstream jsStream;

		flusspferd::object commandProcessor = JSManager::get()->executeExpression("global.webSocketCommandProcessor").to_object();
		flusspferd::value returnValue = commandProcessor.call("processCommand", writer.write(command, true), descriptor->getUserId());
		
		//flusspferd::value returnValue = JSManager::get()->executeExpression("global.webSocketCommandProcessor.processCommand('" + writer.write(command, true) + "', " + MiscUtil::toString(descriptor->getUserId()) + ");");
		
		if (returnValue.is_string())
			descriptor->send(returnValue.to_std_string());
	}
	catch (flusspferd::exception e)
	{
		MudLog(BRF, 100, TRUE, "Could not execute player portal command `%s`: %s", command["method"].asString().c_str(), e.what());
	}
}

PlayerPortalLoadItemFlagsCommandProcess::PlayerPortalLoadItemFlagsCommandProcess() {}
PlayerPortalLoadItemFlagsCommandProcess::~PlayerPortalLoadItemFlagsCommandProcess() {}

void PlayerPortalLoadItemFlagsCommandProcess::process(PlayerPortalDescriptor *descriptor, Json::Value command)
{
	try
	{
		Json::FastWriter writer;
		std::stringstream jsStream;

		Json::Value response;
		response["method"] = command["method"];
		response["itemTypes"] = Json::Value(Json::arrayValue);
		for (int itemTypeId = 0; itemTypeId < NUM_ITEM_TYPES; ++itemTypeId)
		{
			Json::Value itemType;
			
			itemType["id"] = itemTypeId;
			itemType["name"] = item_types[itemTypeId];
			response["itemTypes"].append(itemType);
		}

		response["itemWears"] = Json::Value(Json::arrayValue);
		for (int itemWearId = 0; itemWearId < NUM_ITEM_WEARS; ++itemWearId)
		{
			Json::Value itemWear;

			itemWear["id"] = itemWearId;
			itemWear["name"] = wear_bits[itemWearId];
			response["itemWears"].append(itemWear);
		}

		response["extraFlags"] = Json::Value(Json::arrayValue);
		for (int itemExtraId = 0; itemExtraId < NUM_ITEM_FLAGS; ++itemExtraId)
		{
			Json::Value itemExtra;

			itemExtra["id"] = itemExtraId;
			itemExtra["name"] = extra_bits[itemExtraId];
			response["extraFlags"].append(itemExtra);
		}

		descriptor->send(writer.write(response, true));
	}
	catch (flusspferd::exception e)
	{
		MudLog(BRF, 100, TRUE, "Could not execute player portal command `%s`: %s", command["method"].asString().c_str(), e.what());
	}
}

PlayerPortalLoadObjectListCommandProcess::PlayerPortalLoadObjectListCommandProcess() {}
PlayerPortalLoadObjectListCommandProcess::~PlayerPortalLoadObjectListCommandProcess() {}

void PlayerPortalLoadObjectListCommandProcess::process(PlayerPortalDescriptor *descriptor, Json::Value command)
{
	try
	{
		Json::FastWriter writer;
		std::stringstream jsStream;

		Json::Value response;
		response["method"] = command["method"];
		response["objects"] = Json::Value(Json::arrayValue);
		int lowVnum = -1, highVnum = -1, wearType = -1, itemType = -1;

		if (!command["lowVnum"].isNull() && command["lowVnum"].isInt())
			lowVnum = command["lowVnum"].asInt();
		if (!command["highVnum"].isNull() && command["highVnum"].isInt())
			highVnum = command["highVnum"].asInt();


		if (!command["itemType"].isNull() && command["itemType"].isInt())
			itemType = command["itemType"].asInt();

		itemType = itemType >= 0 && itemType < NUM_ITEM_TYPES ? itemType : -1;

		if (!command["wearType"].isNull() && command["wearType"].isInt())
			wearType = command["wearType"].asInt();

		wearType = wearType >= 0 && wearType < NUM_ITEM_WEARS ? wearType : -1;

		Log("Low Vnum: %d, High Vnum: %d, Wear Type: %d, Item Type: %d", lowVnum, highVnum, wearType, itemType);

		for (Object *objectPrototype : obj_proto)
		{
			if (lowVnum != -1 && lowVnum > objectPrototype->getVnum())
				continue;
			if (highVnum != -1 && highVnum < objectPrototype->getVnum())
				continue;
			if (itemType != -1 && itemType != objectPrototype->getType())
				continue;
			if (wearType != -1 && !OBJWEAR_FLAGGED(objectPrototype, (1 << wearType)))
				continue;

			Json::Value objectValue;

			objectValue["vnum"] = objectPrototype->getVnum();
			objectValue["shortDescription"] = objectPrototype->GetSDesc();
			objectValue["itemType"] = (int)objectPrototype->getType();
			objectValue["wearFlags"] = objectPrototype->obj_flags.wear_flags;
			objectValue["extraFlags"] = objectPrototype->obj_flags.extra_flags;
			objectValue["weight"] = GET_OBJ_WEIGHT(objectPrototype);
			objectValue["cost"] = objectPrototype->obj_flags.cost;
			objectValue["ob"] = objectPrototype->obj_flags.offensive;
			objectValue["db"] = objectPrototype->obj_flags.dodge;
			objectValue["pb"] = objectPrototype->obj_flags.parry;
			objectValue["abs"] = objectPrototype->obj_flags.absorb;
			objectValue["bash"] = objectPrototype->BashRating();
			objectValue["num"] = ItemCount[objectPrototype->item_number];

			response["objects"].append(objectValue);
		}

		Log("Message Length: %lu", writer.write(response, true).size());

		descriptor->send(writer.write(response, true));
	}
	catch (flusspferd::exception e)
	{
		MudLog(BRF, 100, TRUE, "Could not execute player portal command `%s`: %s", command["method"].asString().c_str(), e.what());
	}
}

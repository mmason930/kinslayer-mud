#include "PlayerPortalCommandProcessor.h"
#include "utils.h"
#include "js/js.h"
#include "constants.h"
#include "StringUtil.h"

#include <boost/regex.hpp>

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
	catch (flusspferd::exception &e)
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
	catch (flusspferd::exception &e)
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
		Json::Value itemTypeValue = command["itemType"];
		Json::Value itemWearValue = command["itemWear"];
		Json::Value itemExtraValue = command["itemExtra"];
		Json::Value triggerVnumsValue = command["triggers"];
		std::string namelistPatternString, sdescPatternString, ldescPatternString;
		boost::regex namelistRegex, sdescRegex, ldescRegex;
		int lowVnum = -1, highVnum = -1;
		std::vector<int> itemTypes, itemWears, itemExtras, triggerVnums;

		if (!command["lowVnum"].isNull() && command["lowVnum"].isInt())
			lowVnum = command["lowVnum"].asInt();
		if (!command["highVnum"].isNull() && command["highVnum"].isInt())
			highVnum = command["highVnum"].asInt();

		if(!command["namelist"].isNull())
		{
			namelistPatternString = command["namelist"].asString();
			if(!namelistPatternString.empty())
				namelistRegex = boost::regex(namelistPatternString);
		}
		if(!command["shortDescription"].isNull())
		{
			sdescPatternString = command["shortDescription"].asString();
			if(!sdescPatternString.empty())
				sdescRegex = boost::regex(sdescPatternString);
		}
		if(!command["longDescription"].isNull())
		{
			ldescPatternString = command["longDescription"].asString();

			if(!ldescPatternString.empty())
				ldescRegex = boost::regex(ldescPatternString);
		}

		if(!triggerVnumsValue.isNull() && triggerVnumsValue.isArray())
		{
			for(auto iter = triggerVnumsValue.begin();iter != triggerVnumsValue.end();++iter)
			{
				std::string triggerVnumString = (*iter).asString();
				if(MiscUtil::isInt(triggerVnumString))
					triggerVnums.push_back(atoi(triggerVnumString.c_str()));
			}
		}
		if(!itemExtraValue.isNull() && itemExtraValue.isArray())
		{
			for(auto iter = itemExtraValue.begin();iter != itemExtraValue.end();++iter)
			{
				std::string itemExtraValue = (*iter).asString();
				if(MiscUtil::isInt(itemExtraValue))
					itemExtras.push_back(atoi(itemExtraValue.c_str()));
			}
		}
		if(!itemWearValue.isNull() && itemWearValue.isArray())
		{
			for(auto iter = itemWearValue.begin();iter != itemWearValue.end();++iter)
			{
				std::string itemWearValue = (*iter).asString();
				if(MiscUtil::isInt(itemWearValue))
					itemWears.push_back(atoi(itemWearValue.c_str()));
			}
		}
		if(!itemTypeValue.isNull() && itemTypeValue.isArray())
		{
			for(auto iter = itemTypeValue.begin();iter != itemTypeValue.end();++iter)
			{
				std::string itemTypeValue = (*iter).asString();
				if(MiscUtil::isInt(itemTypeValue))
					itemTypes.push_back(atoi(itemTypeValue.c_str()));
			}
		}

		Log("Low Vnum: %d, High Vnum: %d, Wear Types: %d, Item Types: %d, Item Extras: %d, Triggers: %d", lowVnum, highVnum, (int)itemWears.size(), (int)itemTypes.size(), (int)itemExtras.size(), (int)triggerVnums.size());
		Log("LDesc Valid: %s, SDesc Valid: %s, Namelist Valid: %s", StringUtil::yesNo(!sdescRegex.empty()).c_str(), StringUtil::yesNo(!ldescRegex.empty()).c_str(), StringUtil::yesNo(!namelistRegex.empty()).c_str());

		for (Object *objectPrototype : obj_proto)
		{
			if (lowVnum != -1 && lowVnum > objectPrototype->getVnum())
				continue;
			if (highVnum != -1 && highVnum < objectPrototype->getVnum())
				continue;
			Log("...1");
			if (!itemTypes.empty() && std::find(itemTypes.begin(), itemTypes.end(), objectPrototype->getType()) == itemTypes.end())
				continue;
			Log("...2");
			if(!itemWears.empty() && std::none_of(itemWears.begin(), itemWears.end(), [&](int itemWear) {
				return OBJWEAR_FLAGGED(objectPrototype, (1 << itemWear));
			})) {
				continue;
			}
			Log("...3");
			if(!itemExtras.empty() && std::none_of(itemExtras.begin(), itemExtras.end(), [&](int itemExtra) {
				return OBJ_FLAGGED(objectPrototype, itemExtra);
			})) {
				continue;
			}
			Log("...4");
			if(!triggerVnums.empty() && std::none_of(objectPrototype->js_scripts->begin(), objectPrototype->js_scripts->end(), [&](JSTrigger *trigger) {
				return std::find(triggerVnums.begin(), triggerVnums.end(), trigger->vnum) != triggerVnums.end();
			}))
			{
				continue;
			}
			Log("...5");
			try
			{
				if(!ldescRegex.empty() && !boost::regex_search(objectPrototype->description, ldescRegex))
			 		continue;
			}
			catch(std::exception &e)
			{
				MudLog(BRF, LVL_APPR, TRUE, "Invalid regular expression: %s", ldescPatternString.c_str());
			}
			Log("...6");
			try
			{
				if(!sdescRegex.empty() && !boost::regex_search(objectPrototype->short_description, sdescRegex))
			 		continue;
			}
			catch(std::exception &e)
			{
				MudLog(BRF, LVL_APPR, TRUE, "Invalid regular expression: %s", sdescPatternString.c_str());
			}
			Log("...7");
			try
			{
				if(!namelistRegex.empty() && !boost::regex_search(objectPrototype->name, namelistRegex))
			 		continue;
			}
			catch(std::exception &e)
			{
				MudLog(BRF, LVL_APPR, TRUE, "Invalid regular expression: %s", namelistPatternString.c_str());
			}
			Log("...8");
			bool skip = false;
			for(int valueNumber = 0;valueNumber < sizeof(objectPrototype->obj_flags.value);++valueNumber)
			{
				char variableName[10];
				snprintf(variableName, sizeof(variableName), "val%d", valueNumber);
				if(!command[variableName].isNull() && command[variableName].isInt() && command[variableName].asInt() != objectPrototype->obj_flags.value[valueNumber])
				{
					skip = true;
					break;
				}
			}
			Log("...9");
			if(skip)
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
	catch (flusspferd::exception &e)
	{
		MudLog(BRF, 100, TRUE, "Could not execute player portal command `%s`: %s", command["method"].asString().c_str(), e.what());
	}
}

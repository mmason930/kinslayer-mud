#include "../../conf.h"
#include "../../sysdep.h"

#include "../../structs.h"
#include "../../spells.h"
#include "../../utils.h"
#include "../../comm.h"
#include "../../interpreter.h"
#include "../../handler.h"
#include "../../db.h"
#include "../../screen.h"
#include "../../constants.h"
#include "../../olc.h"
#include "../../clans.h"
#include "../../shop.h"
#include "../../stats.h"
#include "../../customcmds.h"
#include "../../weaves.h"
#include "../../mobs.h"
#include "../../Descriptor.h"
#include "../../zones.h"

#include "../../MiscUtil.h"
#include "../../CharacterUtil.h"
#include "../../StringUtil.h"
#include "../../UserType.h"
#include "../../EntityType.h"
#include "../../rooms/Room.h"

extern Object *object_list;
extern Character *character_list;

CommandHandler do_locate = DEFINE_COMMAND
{
	struct ObjectValueEntry
	{
		int vnum;
		boost::uuids::uuid objectId;

		ObjectValueEntry()
		{
			vnum = -1;
		}
	};

	struct NumericRange
	{
		std::list< std::function<bool(const long numberToTest)> > testFunctions;

		bool exists()
		{
			return testFunctions.size() > 0;
		}

		bool matches(const long numberToTest)
		{
			for (auto testFunction : testFunctions)
			{
				if (!testFunction(numberToTest))
					return false;
			}
			return true;
		}

		bool parseRange(const std::string &value)
		{
			std::string token, numericValue;
			std::string::size_type pos;
			std::function<bool(const long numberToTest, const long number)> wrappingFunction;

			for (pos = 0; pos < value.length() && !isdigit(value[pos]); ++pos);

			token = value.substr(0, pos);
			numericValue = value.substr(pos);

			if (!MiscUtil::isInt(numericValue))
				return false;

			if (token.empty())
				wrappingFunction = [&](const long numberToTest, const long number) -> bool { return numberToTest == number; };
			else if (token == ">")
				wrappingFunction = [&](const long numberToTest, const long number) -> bool { return numberToTest >  number; };
			else if (token == ">=")
				wrappingFunction = [&](const long numberToTest, const long number) -> bool { return numberToTest >= number; };
			else if (token == "<")
				wrappingFunction = [&](const long numberToTest, const long number) -> bool { return numberToTest < number; };
			else if (token == "<=")
				wrappingFunction = [&](const long numberToTest, const long number) -> bool { return numberToTest <= number; };
			else
				return false;

			testFunctions.push_back(std::function<bool(const long numberToTest)>(std::bind(wrappingFunction, std::placeholders::_1, MiscUtil::convert<long>(numericValue))));
			return true;
		}
	};

	int position = -1;
	int typeObj = -1, typeMob = -1, typePlayer = -1, typeRoom = -1;
	NumericRange vnum, zone, roomNumber, cost, gold, db, pb, ob, abs;
	std::auto_ptr<std::list<ObjectValueEntry>> wearing;
	ObjectValueEntry *currentObjectValueEntry;

	std::string name, lastOption;
	std::map<std::string, std::function<bool(const std::string &type, const std::string &value)>> optionToOperationMap;
	std::map<std::string, std::string> optionToValueMap;
	std::list<Entity *> entityList;
	boost::uuids::string_generator uuidGenerator;
	std::stringstream outputBuffer;
	
	std::function<bool(Object *obj)> objectComparatorFunction = [&](Object *obj) -> bool
	{
		if (obj == NULL)
			return false;
		
		if (currentObjectValueEntry->vnum != -1)
			return obj->getVnum() == currentObjectValueEntry->vnum;

		return (currentObjectValueEntry->objectId != obj->objID);
	};


	auto numericRangeOperation =
	[&](const std::string &option, const std::string &value, NumericRange *numericRange) -> bool
	{
		if (!numericRange->parseRange(value))
		{
			ch->send("Invalid value for option '%s' : '%s'\r\n", option.c_str(), value.c_str());
			return false;
		}

		return true;
	};

	optionToOperationMap["type"] =
	[&](const std::string &option, const std::string &value) -> bool
	{
		typeObj = typeObj == -1 ? 0 : typeObj;
		typeMob = typeMob == -1 ? 0 : typeMob;
		typePlayer = typePlayer == -1 ? 0 : typePlayer;
		typeRoom = typeRoom == -1 ? 0 : typeRoom;
		if (!strn_cmp(value, "mob", value.size()))
			typeMob = 1;
		else if (!strn_cmp(value, "obj", value.size()))
			typeObj = 1;
		else if (!strn_cmp(value, "player", value.size()))
			typePlayer = 1;
		else if (!strn_cmp(value, "room", value.size()))
			typeRoom = 1;
		else
		{
			ch->send("Invalid value for option '%s': '%s'\r\n", option.c_str(), value.c_str());
			return false;
		}
		return true;
	};

	optionToOperationMap["room"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &roomNumber);
	optionToOperationMap["cost"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &cost);
	optionToOperationMap["gold"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &gold);
	optionToOperationMap["vnum"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &vnum);
	optionToOperationMap["zone"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &zone);
	optionToOperationMap["ob"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &ob);
	optionToOperationMap["db"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &db);
	optionToOperationMap["pb"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &pb);
	optionToOperationMap["abs"] = std::bind(numericRangeOperation, std::placeholders::_1, std::placeholders::_2, &abs);

	optionToOperationMap["pos"] =
	[&](const std::string &option, const std::string &value) -> bool
	{
		if (!str_cmp(value, "standing"))
			position = POS_STANDING;
		else if (!str_cmp(value, "fighting"))
			position = POS_FIGHTING;
		else if (!str_cmp(value, "sitting"))
			position = POS_SITTING;
		else if (!str_cmp(value, "resting"))
			position = POS_RESTING;
		else if (!str_cmp(value, "sleeping"))
			position = POS_SLEEPING;
		else if (!str_cmp(value, "stunned"))
			position = POS_STUNNED;
		else if (!str_cmp(value, "mort") || !str_cmp(value, "mortally") || !str_cmp(value, "mortallywounded"))
			position = POS_MORTALLYW;
		else if (!str_cmp(value, "dead"))
			position = POS_DEAD;
		else if (!str_cmp(value, "flying"))
			position = POS_FLYING;
		else
		{
			ch->send("Invalid value for option '%s' : '%s'\r\n", option.c_str(), value.c_str());
			return false;
		}

		return true;
	};

	optionToOperationMap["wearing"] =
	[&](const std::string &option, const std::string &value) -> bool
	{
		if (wearing.get() == NULL)
			wearing.reset(new std::list<ObjectValueEntry>());

		ObjectValueEntry objectValueEntry;

		if (MiscUtil::isInt(value))
			objectValueEntry.vnum = atoi(value.c_str());
		else
		{
			try {
				objectValueEntry.objectId = uuidGenerator(value);
			}
			catch (std::exception e)
			{
				ch->send("Invalid value for option '%s' : '%s'\r\n", option.c_str(), value.c_str());
				return false;
			}
		}

		wearing->push_back(objectValueEntry);

		return true;
	};

	skip_spaces(&argument);
	while (*argument)
	{
		char arg[MAX_INPUT_LENGTH];
		HalfChop(argument, arg, argument);

		if (*arg == '-')
		{
			auto operationIter = optionToOperationMap.find(arg + 1);

			if (!lastOption.empty())
			{
				ch->send("No value specified for option '%s'\r\n", lastOption.c_str());
				return;
			}

			if (operationIter == optionToOperationMap.end())
			{
				ch->send("Unknown option '%s' supplied.\r\n", arg);
				return;
			}

			lastOption = (arg + 1);
		}
		else if (lastOption.empty())
		{
			ch->send("No option for value '%s'\r\n", arg);
			return;
		}
		else
		{//
			if (!optionToOperationMap[lastOption](lastOption, arg))
				return;
			lastOption.clear();
		}
	}

	if (!lastOption.empty())
	{
		ch->send("No value specified for option '%s'\r\n", lastOption.c_str());
		return;
	}

	if ((typeMob || typePlayer) && !cost.exists())
	{
		for (Character *target = character_list; target; target = target->next)
		{
			if (target->IsPurged())
				continue;
			if (!typeMob && target->getUserType() == UserType::mob)
				continue;
			if (!typePlayer && target->getUserType() == UserType::player)
				continue;
			if (vnum.exists() && !vnum.matches(target->getVnum()))
				continue;
			if (gold.exists() && !gold.matches(target->points.gold))
				continue;

			Room *room = target->in_room;
			if (zone.exists() && (!room || !zone.matches(room->getZone()->getVnum())))
				continue;
			if (!roomNumber.exists() && (!room || !roomNumber.matches(room->getVnum())))
				continue;

			//Save these chese for last as they are more costly
			if (ob.exists() && !ob.matches(target->Offense()))
				continue;
			if (db.exists() && !ob.matches(target->Dodge()))
				continue;
			if (pb.exists() && !ob.matches(target->Parry()))
				continue;
			if (abs.exists() && !ob.matches(target->Absorb()))
				continue;

			if (wearing.get() != NULL)
			{
				bool allFound = true;
				for (ObjectValueEntry objectValueEntry : (*(wearing.get())))
				{
					currentObjectValueEntry = &objectValueEntry;
					if (!target->anyEquipmentMeetsCritera(objectComparatorFunction))
					{
						allFound = false;
						break;
					}
				}

				if (!allFound)
					continue;
			}

			entityList.push_back((Entity*)target);
		}
	}

	if (typeObj && position == -1 && wearing.get() == NULL && !gold.exists())
	{
		for (Object *obj = object_list; obj; obj = obj->next)
		{
			if (obj->IsPurged())
				continue;
			if (vnum.exists() && !vnum.matches(obj->getVnum()))
				continue;
			if (cost.exists() && !cost.matches(GET_OBJ_COST(obj)))
				continue;
			if (ob.exists() && !ob.matches(obj->GetTotalOffense()))
				continue;
			if (db.exists() && !ob.matches(obj->GetTotalDodge()))
				continue;
			if (pb.exists() && !ob.matches(obj->GetTotalParry()))
				continue;
			if (abs.exists() && !ob.matches(obj->GetTotalAbsorb()))
				continue;

			Room *room = obj->getRoom();
			if (zone.exists() && (!room || !zone.matches(room->getZone()->getVnum())))
				continue;
			if (roomNumber.exists() && (!room || !roomNumber.matches(room->getVnum())))
				continue;

			entityList.push_back((Entity*)obj);
		}
	}

	if (typeRoom && position == -1 && wearing.get() == NULL && !cost.exists() && !gold.exists() && !ob.exists() && !pb.exists() && !db.exists() && !abs.exists())
	{
		for (Room *room : World)
		{
			if (room->IsPurged())
				continue;
			if (vnum.exists() && !vnum.matches(room->getVnum()))
				continue;

			if (zone.exists() && !zone.matches(room->getZone()->getVnum()))
				continue;
			if (roomNumber.exists() && !roomNumber.matches(room->getVnum()))
				continue;

			entityList.push_back((Entity*)room);
		}
	}

	outputBuffer << "Vnum     Type               ID                            Loc                       Name" << std::endl;
	outputBuffer << "=====================================================================================================================" << std::endl;

	for (Entity *entity : entityList)
	{
		if (position != -1)
		{
			Character *target = dynamic_cast<Character *>(entity);
			if (target == NULL || GET_POS(target) != position)
				continue;
		}

		int entityVnum = -1;
		Character *target;
		Room *room;
		Object *obj;

		if (target = dynamic_cast<Character*>(entity))
			entityVnum = entity->getEntityType() == EntityType::mob ? target->getVnum() : entityVnum;
		else if (obj = dynamic_cast<Object*>(entity))
			entityVnum = obj->getVnum();
		else if (room = dynamic_cast<Room*>(entity))
			entityVnum = room->getVnum();

		Room *entityRoom = entity->getRoom();

		outputBuffer << std::setw(6) << std::right << (entityVnum == -1 ? "" : MiscUtil::toString(entityVnum)) << "  "
			<< std::setw(6) << std::left << entity->getEntityType()->getStandardName() << " "
			<< std::setw(37) << std::right << entity->getDisplayableId() << "   "
			<< std::setw(6) << std::right << (entityRoom ? MiscUtil::toString(entityRoom->getVnum()) : "") << "       "
			<< entity->getDisplayName() << std::endl;
	}

	page_string(ch->desc, (char *)outputBuffer.str().c_str(), TRUE);
};


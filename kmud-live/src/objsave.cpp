/* ************************************************************************
*   File: objsave.c                                     Part of CircleMUD *
*  Usage: loading/saving player objects for rent and crash-save           *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "spells.h"
#include "utils.h"
#include "screen.h"
#include "olc.h"

#include "MiscUtil.h"
#include "StringUtil.h"
#include "CharacterUtil.h"
#include "UserLogoutType.h"
#include "Descriptor.h"

extern Index *obj_index;
extern Descriptor *descriptor_list;
extern Object *boot_object( Object obj);
extern Object *object_list;
extern Character *character_list;

void obj_from_char(struct object_data *object);

std::set< int > Room::corpseRooms;

/* Extern functions */
ACMD(do_action);
ACMD(do_tell);
ACMD(do_retrieve);
SPECIAL(receptionist);
SPECIAL(cryogenicist);

//Local functions
int find_eq_pos(Character *ch, Object *obj, char *arg, bool msg);

Object *Object::bootLiveObject( const sql::Row &MyRow, bool recursive )
{
	Object *obj = (NULL);

	if( atoi(MyRow["special_type"].c_str()) != SPECIAL_NONE )
	{
		int special_type = atoi(MyRow["special_type"].c_str()) ;
		/* Special */

		std::stringstream QueryBuffer2;
		sql::Query MyQuery;

		QueryBuffer2 << "SELECT * FROM object_specials WHERE id='" << MyRow["id"] << "';";
		try {
			MyQuery = gameDatabase->sendQuery(QueryBuffer2.str());
		} catch( sql::QueryException e ) {
			MudLog(BRF, LVL_APPR, TRUE, "Unable to load item (special data) for object ID #%d: %s",
				MyRow["id"].c_str(), e.getMessage().c_str());
			return (NULL);
		}

		if( MyQuery->hasNextRow() )
		{
			boost::uuids::string_generator uuidGenerator;

			sql::Row MyRow2 = MyQuery->getRow();
			obj = create_obj( false );
			obj->objID = uuidGenerator(MyRow2["id"].c_str());
			obj->name = str_dup(MyRow2["name"].c_str());
			obj->short_description = str_dup(MyRow2["sdesc"].c_str());
			obj->description = str_dup(MyRow2["ldesc"].c_str());
			obj->obj_flags.value[0] = atoi(MyRow2["val0"].c_str());
			obj->obj_flags.value[1] = atoi(MyRow2["val1"].c_str());
			obj->obj_flags.value[2] = atoi(MyRow2["val2"].c_str());
			obj->obj_flags.value[3] = atoi(MyRow2["val3"].c_str());
			obj->obj_flags.value[4] = atoi(MyRow2["val4"].c_str());
			obj->obj_flags.value[5] = atoi(MyRow2["val5"].c_str());
			obj->obj_flags.value[6] = atoi(MyRow2["val6"].c_str());
			obj->obj_flags.value[7] = atoi(MyRow2["val7"].c_str());
			obj->obj_flags.value[8] = atoi(MyRow2["val8"].c_str());
			obj->obj_flags.value[9] = atoi(MyRow2["val9"].c_str());
			obj->obj_flags.value[10] = atoi(MyRow2["val10"].c_str());
			obj->obj_flags.value[11] = atoi(MyRow2["val11"].c_str());
			obj->obj_flags.extra_flags = atoi(MyRow2["extra_flags"].c_str());
			obj->obj_flags.wear_flags = atoi(MyRow2["wear_flags"].c_str());
			obj->obj_flags.bitvector[0] = atol(MyRow2["bitvector0"].c_str());
			obj->obj_flags.bitvector[1] = atol(MyRow2["bitvector1"].c_str());
			obj->obj_flags.bitvector[2] = atol(MyRow2["bitvector2"].c_str());
			obj->obj_flags.bitvector[3] = atol(MyRow2["bitvector3"].c_str());
			obj->obj_flags.weight = (float)atof(MyRow2["weight"].c_str());
			obj->obj_flags.cost = atoi(MyRow2["cost"].c_str());
			obj->obj_flags.cost_per_day = atoi(MyRow2["cost_per_day"].c_str());
			obj->obj_flags.offensive = (sh_int)atoi(MyRow2["offensive"].c_str());
			obj->obj_flags.parry = (sh_int)atoi(MyRow2["parry"].c_str());
			obj->obj_flags.dodge = (sh_int)atoi(MyRow2["dodge"].c_str());

			obj->scalp = new ScalpData();
			obj->scalp->is_scalp		= (special_type == SPECIAL_SCALP || special_type == SPECIAL_PLAYER_SCALP);
			obj->scalp->player_scalp	= (special_type == SPECIAL_PLAYER_SCALP);
			obj->scalp->scalped = StringUtil::parseBoolean(MyRow2["scalped"]);
			obj->scalp->level = atoi(MyRow2["level"].c_str());
			obj->scalp->race = atoi(MyRow2["race"].c_str());
			obj->obj_flags.timer = atoi(MyRow2["timer"].c_str());
			if( special_type == SPECIAL_CORPSE )
			{
				obj->obj_flags.type_flag = ITEM_CONTAINER;
				obj->scalp->name = MyRow2["extra_str1"];
			}
		}
		else {
			return (0);//Error. Object flagged as special yet no special found... Bad!
		}
	}
	else
	{
		/* Normal */
		int rnum = real_object(atoi(MyRow["vnum"].c_str()));
		if( rnum == -1 )
			return (NULL);
		obj = read_object(rnum, REAL, false, false);

		boost::uuids::string_generator uuidGenerator;
		obj->objID = uuidGenerator(MyRow["id"].c_str());
		obj->obj_flags.bitvector[0] = atol(MyRow["bitv0"].c_str());
		obj->obj_flags.bitvector[1] = atol(MyRow["bitv1"].c_str());
		obj->obj_flags.bitvector[2] = atol(MyRow["bitv2"].c_str());
		obj->obj_flags.bitvector[3] = atol(MyRow["bitv3"].c_str());

		if( GET_OBJ_TYPE(obj) == ITEM_DRINKCON )
		{
			GET_OBJ_VAL(obj, 2) = atoi(MyRow["obj_extra0"].c_str());
			GET_OBJ_VAL(obj, 1) = atoi(MyRow["obj_extra1"].c_str());
		}
		else if( GET_OBJ_TYPE(obj) == ITEM_KEY )
		{
			GET_OBJ_VAL(obj, 0) = atoi(MyRow["obj_extra0"].c_str());
		}
	}
	if( MyRow["retool_name"].empty() == false )
		obj->retool_name = str_dup( MyRow["retool_name"].c_str() );
	if( MyRow["retool_sdesc"].empty() == false )
		obj->retool_sdesc = str_dup( MyRow["retool_sdesc"].c_str() );
	if( MyRow["retool_ldesc"].empty() == false )
		obj->retool_desc = str_dup( MyRow["retool_ldesc"].c_str() );
	if( MyRow["retool_exdesc"].empty() == false )
	{
		obj->retool_ex_desc = new ExtraDescription();
		obj->retool_ex_desc->keyword = str_dup(obj->getName());
		obj->retool_ex_desc->description = str_dup(MyRow["retool_exdesc"].c_str());
	}

	if( !MyRow["obj_str"].empty() )
		obj->action_description = str_dup(MyRow["obj_str"].c_str());
	obj->creator = MyRow["creator"];
	obj->createdDatetime = DateTime(MyRow.getTimestamp("created_datetime"));
	obj->Money = atoi(MyRow["gold"].c_str());
	if( !recursive || atoi(MyRow["num_holding"].c_str()) <= 0 ) return obj;

	/* Load any items that this object contains */
	if( recursive )
		obj->loadItems();
	return (obj);
}

void Object::addToBatchInsertStatement(sql::BatchInsertStatement &batchInsertStatement, bool recursive=false)
{
	
}

std::list< Object* > Object::loadItemList( bool recursive )
{
	return Object::loadItemList( recursive,
		(this->IsValidChest() ? 'C' : 'O'),
		(this->IsValidChest() ? ToString(getRoom()->vnum) : ToString(this->objID))
	);
}
void Object::loadItems()
{
	std::list< Object* > ObjList = this->loadItemList( false );
	for( std::list<Object*>::iterator oIter = ObjList.begin();oIter != ObjList.end();++oIter )
	{
		(*oIter)->loadItems();
		obj_to_obj( (*oIter), this );
	}
}
Object *Object::loadSingleItem(const boost::uuids::uuid &oID, bool recursive)
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	Object *obj;

	QueryBuffer
		<< "SELECT o.*,"
		<< "(SELECT COUNT(*) FROM objects o2 WHERE o2.holder_type='O' AND o2.holder_id=o.id) AS num_holding,"
		<< "retool.retool_name,retool.retool_sdesc,retool.retool_ldesc,retool.retool_exdesc "
		<< "FROM objects o "
		<< "LEFT JOIN object_retools retool ON retool.id=o.id "
		<< "WHERE o.id='" << oID << "';";
	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Unable to load single item #%d: %s",
			ToString(oID).c_str(), e.getMessage().c_str());
		return NULL;
	}
	if( MyQuery->hasNextRow() )
		obj = Object::bootLiveObject( MyQuery->getRow(), (false) );
	else
		obj = 0;
	if( obj && recursive )
		obj->loadItems();
	return (obj);
}
std::list< Object* > Object::loadItemList( bool recursive, const char holderType, const std::string &holderID )
{
	std::list< std::pair<Object*,int> > ItemPairs = Object::loadItemPairs( recursive, holderType, holderID );
	std::list< Object* > ItemList;

	//We want the same data, minus the position.
	for(std::list< std::pair<Object*,int> >::iterator ipIter = ItemPairs.begin();ipIter != ItemPairs.end();++ipIter)
	{
		ItemList.push_back( (*ipIter).first );
	}
	return ItemList;
}

std::list< std::pair<Object*,int> > Object::loadItemPairs(
	bool recursive, const char holderType, const std::string &holderID )
{
	std::list< std::pair<Object*,int> > ObjList;

	std::stringstream QueryBuffer;
	sql::Query MyQuery;

	QueryBuffer
		<< "SELECT o.*,"
		<< "(SELECT COUNT(*) FROM objects o2 WHERE o2.holder_type='O' AND o2.holder_id=o.id) AS num_holding,"
		<< "retool.retool_name,retool.retool_sdesc,retool.retool_ldesc,retool.retool_exdesc "
		<< "FROM objects o "
		<< "LEFT JOIN object_retools retool ON retool.id=o.id "
		<< "WHERE o.holder_type='" << holderType << "' "
		<< "AND o.holder_id='" << holderID << "';";
	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Unable to load items held by ID %d(%c): %s",
			holderType, holderID.c_str(), e.getMessage().c_str());
		return (ObjList);
	}
	while( MyQuery->hasNextRow() )
	{
		sql::Row MyRow = MyQuery->getRow();
		Object *obj = Object::bootLiveObject( MyRow, (false) );
		if( obj != NULL )
			ObjList.push_back( std::pair<Object*,int>(obj, atoi(MyRow["pos"].c_str())) );
	}
	if( recursive )
	{
		for( std::list< std::pair<Object*,int> >::iterator oIter = ObjList.begin();oIter != ObjList.end();++oIter )
		{
			std::list< Object* > ObjList2 = (*oIter).first->loadItemList( true );
			for( std::list< Object* >::iterator oIter2 = ObjList2.begin();oIter2 != ObjList2.end();++oIter2 )
			{
				ObjList.push_back( std::pair<Object*,int>((*oIter2), (-1)) );
			}
		}
	}
	return (ObjList);
}
std::list< Object* > Room::loadItemList( bool recursive )
{
	return Object::loadItemList( recursive, 'R', ToString(this->vnum) );
}
void Room::loadItems( const std::list< Object* > &lItems )
{
	for( std::list< Object* >::const_iterator oIter = lItems.begin();oIter != lItems.end();++oIter )
	{
		Object *obj = (*oIter);

		obj->loadItems();
		obj->MoveToRoom(this, false);
	}
}
void Room::loadItems()
{
	std::list< Object* > ObjList = loadItemList( false );
	loadItems( ObjList );
}
/******
 * Loads all items held or worn by this player from the database to the MUD.
 *  - recursive - boolean - If set to true, load contents of each item, and adds them as entries to the list.
 *                          If false, load only the item, which will load only inventory / eq without contents.
 * Returns: A list of pairs. Each entry to this list consists of a single item along with its position, if worn.
 *          If not worn, the std::pair::second will be set to -1.
 *
 * ~~Originally By  : Galnor, 2009
 * ~~Last Updated By: 
 ******/
std::list< std::pair<Object*,int> > Character::loadItemList( bool recursive )
{
	return Object::loadItemPairs( recursive, 'P', ToString(this->player.idnum) );
}

/******
 * Running this method should load all of the player's inventory and equipment into the MUD, and place those
 * items in their appropriate location(inv,eq). All items contained within those pieces of equipment will
 * also be loaded.
 *
 * ~~Originally By  : Galnor, 2009
 * ~~Last Updated By: 
 ******/
void Character::loadItems()
{
	std::list< std::pair<Object*,int> > ObjList = this->loadItemList( false );
	for( std::list< std::pair<Object*,int> >::iterator oIter = ObjList.begin();oIter != ObjList.end();++oIter )
	{
		Object *obj = (*oIter).first;
		int pos = (*oIter).second;

		obj->loadItems();

		if( pos != -1 )
			equip_char(this, obj, pos);
		else
			obj_to_char(obj,this);
	}
}

/******
 * This function will save a single object to a top level(root) container. A root container is a player,
 * chest, object, auction, or anything that may contain items that itself is not contained by something.
 *
 * ~~Originally By  : Galnor, 2011-06-12
 * ~~Last Updated By: 
 ******/
void Object::saveItemToTopLevelHolder(const char holderType, const std::string &holderId, Object *obj)
{
	std::list<Object *> items;

	items.push_back(obj);

	Object::saveHolderItems(holderType, holderId, holderType, holderId, items, false);
}

void Object::saveTopLevelHolderItems(const char holderType, const std::string &holderId, const std::list<Object *> &contents)
{
	Object::saveHolderItems(holderType, holderId, holderType, holderId, contents, true);
}

void threadedQueryExecutor(sql::Connection connection, const std::string &query)
{
	connection->sendRawQuery(query);
}

void Object::saveMultipleHolderItems(const std::map<std::pair<char, std::string>, std::list<Object *>> &holderTypeAndIdToContentsMap, bool deleteHolderContents)
{
	std::stringstream queryBuffer;
	auto holdTypeAndIdIter = holderTypeAndIdToContentsMap.begin();
	Clock clockA, clockB, clockC1, clockC2, clockD1, clockD2, clockD3, clockD4, clockD5;

	try
	{
		gameDatabase->sendRawQuery("DROP TABLE IF EXISTS `tempTopLevelHolder`");

		queryBuffer << "CREATE TABLE `tempTopLevelHolder`("
					<< "  `holder_type` char(1) not null,"
					<< "  `holder_id` varchar(36) not null,"
					<< "  UNIQUE KEY `holder_type_holder_id`(`holder_type`,`holder_id`),"
					<< " KEY `holder_type`(`holder_type`),"
					<< " KEY `holder_id`(`holder_id`)"
					<< ") ENGINE=MyISAM";

		gameDatabase->sendRawQuery(queryBuffer.str());
		gameDatabase->sendRawQuery("ALTER TABLE `tempTopLevelHolder` DISABLE KEYS");

		sql::BatchInsertStatement tempTopLevelHolderBatchInsertStatement(gameDatabase, "tempTopLevelHolder", 10000);

		tempTopLevelHolderBatchInsertStatement.addField("holder_type");
		tempTopLevelHolderBatchInsertStatement.addField("holder_id");

		tempTopLevelHolderBatchInsertStatement.start();

		for(holdTypeAndIdIter = holderTypeAndIdToContentsMap.begin();holdTypeAndIdIter != holderTypeAndIdToContentsMap.end();++holdTypeAndIdIter)
		{
			tempTopLevelHolderBatchInsertStatement.beginEntry();

			//first is the pair, whose first is the holder type(char), and second is the holder id(string).
			tempTopLevelHolderBatchInsertStatement.putChar( (*holdTypeAndIdIter).first.first );
			tempTopLevelHolderBatchInsertStatement.putString( (*holdTypeAndIdIter).first.second );

			tempTopLevelHolderBatchInsertStatement.endEntry();
		}
	
		tempTopLevelHolderBatchInsertStatement.finish();
		gameDatabase->sendRawQuery("ALTER TABLE `tempTopLevelHolder` ENABLE KEYS");

		if(deleteHolderContents) {

			queryBuffer.str("");
			queryBuffer	
					<<	" DELETE"
					<<	"   objects.*,"
					<<	"   object_specials.*,"
					<<	"   object_retools.* "
					<<	" FROM("
					<<	"   objects, "
					<<  "   tempTopLevelHolder)"
					<<	" LEFT JOIN object_specials ON objects.id=object_specials.id "
					<<	" LEFT JOIN object_retools ON objects.id=object_retools.id "
					<<	" WHERE objects.top_level_holder_type=tempTopLevelHolder.holder_type"
					<<	" AND objects.top_level_holder_id=tempTopLevelHolder.holder_id";
			
			gameDatabase->sendRawQuery(queryBuffer.str());
		}
		gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempObjects, tempObjectRetools, tempObjectSpecials;");

		gameDatabase->sendRawQuery("CREATE TABLE tempObjects LIKE objects;");
		gameDatabase->sendRawQuery("CREATE TABLE tempObjectRetools LIKE object_retools;");
		gameDatabase->sendRawQuery("CREATE TABLE tempObjectSpecials LIKE object_specials;");

		sql::BatchInsertStatement tempObjectsBatchInsertStatement(gameDatabase, "tempObjects", 10000, true);
		sql::BatchInsertStatement tempObjectRetoolsBatchInsertStatement(gameDatabase, "tempObjectRetools", 10000, true);
		sql::BatchInsertStatement tempObjectSpecialsBatchInsertStatement(gameDatabase, "tempObjectSpecials", 10000, true);

		Object::addFieldsToBatchInsertStatement(tempObjectsBatchInsertStatement, tempObjectRetoolsBatchInsertStatement, tempObjectSpecialsBatchInsertStatement);

		tempObjectsBatchInsertStatement.start();
		tempObjectRetoolsBatchInsertStatement.start();
		tempObjectSpecialsBatchInsertStatement.start();

		
		for(holdTypeAndIdIter = holderTypeAndIdToContentsMap.begin();holdTypeAndIdIter != holderTypeAndIdToContentsMap.end();++holdTypeAndIdIter)
		{
			for(auto iter = (*holdTypeAndIdIter).second.begin();iter != (*holdTypeAndIdIter).second.end();++iter)
			{
				Object *object = (*iter);
				clockC1.turnOn();
				object->saveItems( true, (*holdTypeAndIdIter).first.first, (*holdTypeAndIdIter).first.second, (*holdTypeAndIdIter).first.first, (*holdTypeAndIdIter).first.second, tempObjectsBatchInsertStatement, tempObjectRetoolsBatchInsertStatement, tempObjectSpecialsBatchInsertStatement, true );
				clockC1.turnOff();
			}
		}
		
		tempObjectsBatchInsertStatement.finish();
		tempObjectRetoolsBatchInsertStatement.finish();
		tempObjectSpecialsBatchInsertStatement.finish();

		//Anything currently contained by one of the objects we're about to insert back into the objects
		//table needs to be put into limbo to prevent anomalies. The containment will be
		//corrected with the upcoming insertion.
		gameDatabase->sendRawQuery(
			"UPDATE"
			"  objects,"
			"  tempObjects "
			"SET"
			"  objects.holder_id='',"
			"  objects.holder_type='' "
			"WHERE objects.holder_type='O' "
			"AND   objects.holder_id=tempObjects.id;"
		);

		//Delete the items we're about to update, if they exist.
		gameDatabase->sendRawQuery(
			"DELETE"
			"  object_specials.*,"
			"  object_retools.*,"
			"  objects.* "
			"FROM"
			"  tempObjects "
			"LEFT JOIN objects ON objects.id=tempObjects.id "
			"LEFT JOIN object_specials ON object_specials.id=tempObjects.id "
			"LEFT JOIN object_retools ON object_retools.id=tempObjects.id"
		);

		//Insert the items that we are updating.
		gameDatabase->sendRawQuery("INSERT DELAYED INTO objects SELECT * FROM tempObjects");
		gameDatabase->sendRawQuery("INSERT DELAYED INTO object_retools SELECT * FROM tempObjectRetools");
		gameDatabase->sendRawQuery("INSERT DELAYED INTO object_specials SELECT * FROM tempObjectSpecials");

	}
	catch(sql::QueryException e) {

		MudLog(BRF, LVL_APPR, TRUE, "Could not save items: %s", e.getMessage().c_str());
	}
}

void Object::addFieldsToBatchInsertStatement(sql::BatchInsertStatement &objectBatchInsertStatement, sql::BatchInsertStatement &objectRetoolBatchInsertStatement, sql::BatchInsertStatement &objectSpecialBatchInsertStatement)
{
	objectBatchInsertStatement.addField("id");
	objectBatchInsertStatement.addField("vnum");
	objectBatchInsertStatement.addField("pos");
	objectBatchInsertStatement.addField("creator");
	objectBatchInsertStatement.addField("bitv0");
	objectBatchInsertStatement.addField("bitv1");
	objectBatchInsertStatement.addField("bitv2");
	objectBatchInsertStatement.addField("bitv3");
	objectBatchInsertStatement.addField("obj_extra0");
	objectBatchInsertStatement.addField("obj_extra1");
	objectBatchInsertStatement.addField("holder_type");
	objectBatchInsertStatement.addField("holder_id");
	objectBatchInsertStatement.addField("top_level_holder_type");
	objectBatchInsertStatement.addField("top_level_holder_id");
	objectBatchInsertStatement.addField("created_datetime");
	objectBatchInsertStatement.addField("obj_str");
	objectBatchInsertStatement.addField("special_type");
	objectBatchInsertStatement.addField("gold");
	
	objectRetoolBatchInsertStatement.addField("id");
	objectRetoolBatchInsertStatement.addField("retool_name");
	objectRetoolBatchInsertStatement.addField("retool_sdesc");
	objectRetoolBatchInsertStatement.addField("retool_ldesc");
	objectRetoolBatchInsertStatement.addField("retool_exdesc");

	objectSpecialBatchInsertStatement.addField("id");
	objectSpecialBatchInsertStatement.addField("special_type");
	objectSpecialBatchInsertStatement.addField("name");
	objectSpecialBatchInsertStatement.addField("sdesc");
	objectSpecialBatchInsertStatement.addField("ldesc");
	objectSpecialBatchInsertStatement.addField("val0");
	objectSpecialBatchInsertStatement.addField("val1");
	objectSpecialBatchInsertStatement.addField("val2");
	objectSpecialBatchInsertStatement.addField("val3");
	objectSpecialBatchInsertStatement.addField("val4");
	objectSpecialBatchInsertStatement.addField("val5");
	objectSpecialBatchInsertStatement.addField("val6");
	objectSpecialBatchInsertStatement.addField("val7");
	objectSpecialBatchInsertStatement.addField("val8");
	objectSpecialBatchInsertStatement.addField("val9");
	objectSpecialBatchInsertStatement.addField("val10");
	objectSpecialBatchInsertStatement.addField("val11");
	objectSpecialBatchInsertStatement.addField("extra_flags");
	objectSpecialBatchInsertStatement.addField("wear_flags");
	objectSpecialBatchInsertStatement.addField("bitvector0");
	objectSpecialBatchInsertStatement.addField("bitvector1");
	objectSpecialBatchInsertStatement.addField("bitvector2");
	objectSpecialBatchInsertStatement.addField("bitvector3");
	objectSpecialBatchInsertStatement.addField("weight");
	objectSpecialBatchInsertStatement.addField("cost");
	objectSpecialBatchInsertStatement.addField("cost_per_day");
	objectSpecialBatchInsertStatement.addField("offensive");
	objectSpecialBatchInsertStatement.addField("parry");
	objectSpecialBatchInsertStatement.addField("dodge");
	objectSpecialBatchInsertStatement.addField("level");
	objectSpecialBatchInsertStatement.addField("race");
	objectSpecialBatchInsertStatement.addField("scalped");
	objectSpecialBatchInsertStatement.addField("extra_str1");
	objectSpecialBatchInsertStatement.addField("timer");
}

/******
 * This function will completely re-synchronize a container's contents if deleteHolderContents is set to true.
 *
 * ~~Originally By  : Galnor, 2011-06-12
 ******/
void Object::saveHolderItems(const char holderType, const std::string &holderId, const char topLevelHolderType, const std::string &topLevelHolderId, const std::list<Object *> &contents, bool deleteHolderContents)
{
	try {
		std::stringstream query;

		if(deleteHolderContents) {

			//Delete all items currently cotained by this holder.
			query	<<	"DELETE"
					<<	"  objects.*,"
					<<	"  object_specials.*,"
					<<	"  object_retools.* "
					<<	"FROM"
					<<	"  objects "
					<<	"LEFT JOIN object_specials ON objects.id=object_specials.id "
					<<	"LEFT JOIN object_retools ON objects.id=object_retools.id "
					<<	"WHERE objects.top_level_holder_type='" << holderType << "' "
					<<	"AND objects.top_level_holder_id='" << sql::escapeString(holderId) << "';";
			
			gameDatabase->sendRawQuery(query.str());
		}

		gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempObjects, tempObjectRetools, tempObjectSpecials;");

		gameDatabase->sendRawQuery("CREATE TABLE tempObjects LIKE objects;");
		gameDatabase->sendRawQuery("CREATE TABLE tempObjectRetools LIKE object_retools;");
		gameDatabase->sendRawQuery("CREATE TABLE tempObjectSpecials LIKE object_specials;");

		sql::BatchInsertStatement tempObjectsBatchInsertStatement(gameDatabase, "tempObjects", 10000, true);
		sql::BatchInsertStatement tempObjectRetoolsBatchInsertStatement(gameDatabase, "tempObjectRetools", 10000, true);
		sql::BatchInsertStatement tempObjectSpecialsBatchInsertStatement(gameDatabase, "tempObjectSpecials", 10000, true);

		Object::addFieldsToBatchInsertStatement(tempObjectsBatchInsertStatement, tempObjectRetoolsBatchInsertStatement, tempObjectSpecialsBatchInsertStatement);

		tempObjectsBatchInsertStatement.start();
		tempObjectRetoolsBatchInsertStatement.start();
		tempObjectSpecialsBatchInsertStatement.start();

		//Add each item to the temp tables.
		std::list<Object *>::const_iterator iter;
		for(iter = contents.begin();iter != contents.end();++iter)
		{
			Object *object = (*iter);

			object->saveItems( true, holderType, holderId, holderType, holderId, tempObjectsBatchInsertStatement, tempObjectRetoolsBatchInsertStatement, tempObjectSpecialsBatchInsertStatement, true );
		}

		tempObjectsBatchInsertStatement.finish();
		tempObjectRetoolsBatchInsertStatement.finish();
		tempObjectSpecialsBatchInsertStatement.finish();

		//Anything currently contained by one of the objects we're about to insert back into the objects
		//table needs to be put into limbo to prevent anomalies. The containment will be
		//corrected with the upcoming insertion.
		gameDatabase->sendRawQuery(
			"UPDATE"
			"  objects,"
			"  tempObjects "
			"SET"
			"  objects.holder_id='',"
			"  objects.holder_type='' "
			"WHERE objects.holder_type='O' "
			"AND   objects.holder_id=tempObjects.id;"
		);

		//Delete the items we're about to update, if they exist.
		gameDatabase->sendRawQuery(
			"DELETE"
			"  object_specials.*,"
			"  object_retools.*,"
			"  objects.* "
			"FROM"
			"  tempObjects "
			"LEFT JOIN objects ON objects.id=tempObjects.id "
			"LEFT JOIN object_specials ON object_specials.id=tempObjects.id "
			"LEFT JOIN object_retools ON object_retools.id=tempObjects.id"
		);

		//Insert the items that we are updating.
		gameDatabase->sendRawQuery("INSERT INTO objects SELECT * FROM tempObjects");
		gameDatabase->sendRawQuery("INSERT INTO object_retools SELECT * FROM tempObjectRetools");
		gameDatabase->sendRawQuery("INSERT INTO object_specials SELECT * FROM tempObjectSpecials");
	}
	catch(sql::QueryException e) {

		MudLog(BRF, LVL_APPR, TRUE, "Could not save items. Holder Type='%c', Holder Id='%s' : %s", holderType, holderId.c_str(), e.getMessage().c_str());
	}
}

/******
 * Removes all items previously possessed by this player from the database, on a top-level basis(no contents).
 * All items currently possessed by the player are then saved, recursively, so as to save each item's contents.
 *
 * ~~Originally By  : Galnor, 2009
 * ~~Last Updated By: 
 ******/
void Character::itemSave()
{
	if(IS_NPC(this))
		return;

	std::list<Object*> items;

	//Equipment.
	for(int equipment_position = 0; equipment_position < NUM_WEARS;++equipment_position )
	{
		if( this->equipment[equipment_position] != NULL )
			items.push_back(this->equipment[equipment_position]);
	}
	//Inventory.
	for( Object *obj = this->carrying;obj;obj = obj->next_content )
	{
		items.push_back(obj);
	}

	Object::saveTopLevelHolderItems('P', MiscUtil::toString(this->player.idnum), items);
}
void Room::itemSave()
{
	std::list<Object*> items;

	for( Object *obj = this->contents;obj;obj = obj->next_content )
	{
		items.push_back(obj);
	}

	Object::saveTopLevelHolderItems('R', MiscUtil::toString(this->vnum), items);
}

void Room::corpseSave()
{
	std::list<Object*> corpses;
	
	for( Object *obj = this->contents;obj;obj = obj->next_content )
	{
		if(IS_CORPSE(obj))
			corpses.push_back(obj);
	}

	Object::saveTopLevelHolderItems('R', MiscUtil::toString(this->vnum), corpses);
}
void Room::saveCorpseRooms()
{

}

void Object::itemSave()
{
	std::list<Object *> items;

	for(Object *obj = contains;obj;obj = obj->next_content) {

		items.push_back(obj);
	}

	Object::saveTopLevelHolderItems( this->IsValidChest() ? 'C' : 'O', this->IsValidChest() ? ToString(this->getRoom()->vnum) : ToString(this->objID), items);
}

void Object::saveItems( bool self, char holderType, const std::string &holderID, char topLevelHolderType, const std::string &topLevelHolderID, sql::BatchInsertStatement &tempObjectsBatchInsertStatement, sql::BatchInsertStatement &tempObjectRetoolsBatchInsertStatement, sql::BatchInsertStatement &tempObjectSpecialsBatchInsertStatement, bool contents )
{
	int obj_extra0 = 0, obj_extra1 = 0;
	int specialType = 0;
	if( self )
	{
		/* Determine if the item is a special type (scalp, corpse) */
		if( this->scalp )
		{
			if( IS_CORPSE(this) )
				specialType = SPECIAL_CORPSE;
			else if( this->scalp->player_scalp )
				specialType = SPECIAL_PLAYER_SCALP;
			else if( this->scalp->is_scalp )
				specialType = SPECIAL_SCALP;
		}

		if( GET_OBJ_TYPE(this) == ITEM_DRINKCON )
		{
			obj_extra0 = GET_OBJ_VAL(this, 2);
			obj_extra1 = GET_OBJ_VAL(this, 1);
		}
		else if( GET_OBJ_TYPE(this) == ITEM_KEY )
		{
			obj_extra0 = GET_OBJ_VAL(this, 0);
		}

		tempObjectsBatchInsertStatement.beginEntry();

		tempObjectsBatchInsertStatement.putString(MiscUtil::toString(this->objID));
		tempObjectsBatchInsertStatement.putInt(GET_OBJ_VNUM(this));
		tempObjectsBatchInsertStatement.putInt(worn_by ? (worn_on) : (-1));
		tempObjectsBatchInsertStatement.putString(creator);
		tempObjectsBatchInsertStatement.putLong(this->obj_flags.bitvector[0]);
		tempObjectsBatchInsertStatement.putLong(this->obj_flags.bitvector[1]);
		tempObjectsBatchInsertStatement.putLong(this->obj_flags.bitvector[2]);
		tempObjectsBatchInsertStatement.putLong(this->obj_flags.bitvector[3]);
		tempObjectsBatchInsertStatement.putInt(obj_extra0);
		tempObjectsBatchInsertStatement.putInt(obj_extra1);
		tempObjectsBatchInsertStatement.putChar(holderType);
		tempObjectsBatchInsertStatement.putString(holderID);
		tempObjectsBatchInsertStatement.putChar(topLevelHolderType);
		tempObjectsBatchInsertStatement.putString(topLevelHolderID);
		tempObjectsBatchInsertStatement.putString(MiscUtil::encodeDateTime(createdDatetime));
		tempObjectsBatchInsertStatement.putString(((action_description && (GET_OBJ_TYPE(this)==ITEM_NOTE)) ? this->action_description : ""));
		tempObjectsBatchInsertStatement.putInt(specialType);
		tempObjectsBatchInsertStatement.putString(MiscUtil::toString(this->Money));
		tempObjectsBatchInsertStatement.endEntry();

		if( IsRetooled() ) {
			tempObjectRetoolsBatchInsertStatement.beginEntry();

			tempObjectRetoolsBatchInsertStatement.putString(MiscUtil::toString(this->objID));
			tempObjectRetoolsBatchInsertStatement.putString(retool_name ? retool_name : "");
			tempObjectRetoolsBatchInsertStatement.putString(retool_sdesc ? retool_sdesc : "");
			tempObjectRetoolsBatchInsertStatement.putString(retool_desc ? retool_desc : "");
			tempObjectRetoolsBatchInsertStatement.putString(retool_ex_desc ? retool_ex_desc->description : "");
			
			tempObjectRetoolsBatchInsertStatement.endEntry();
		}

		if( specialType != SPECIAL_NONE )
		{
			tempObjectSpecialsBatchInsertStatement.beginEntry();

			std::string ExtraStr1;

			if( specialType == SPECIAL_CORPSE )
				ExtraStr1 = this->scalp->name;

			tempObjectSpecialsBatchInsertStatement.putString(MiscUtil::toString(this->objID));
			tempObjectSpecialsBatchInsertStatement.putInt(specialType);
			tempObjectSpecialsBatchInsertStatement.putString(name);
			tempObjectSpecialsBatchInsertStatement.putString(short_description);
			tempObjectSpecialsBatchInsertStatement.putString(description);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[0]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[1]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[2]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[3]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[4]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[5]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[6]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[7]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[8]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[9]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[10]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.value[11]);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.extra_flags);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.wear_flags);
			tempObjectSpecialsBatchInsertStatement.putLong(this->obj_flags.bitvector[0]);
			tempObjectSpecialsBatchInsertStatement.putLong(this->obj_flags.bitvector[1]);
			tempObjectSpecialsBatchInsertStatement.putLong(this->obj_flags.bitvector[2]);
			tempObjectSpecialsBatchInsertStatement.putLong(this->obj_flags.bitvector[3]);
			tempObjectSpecialsBatchInsertStatement.putDouble(this->obj_flags.weight);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.cost);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.cost_per_day);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.offensive);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.parry);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.dodge);
			tempObjectSpecialsBatchInsertStatement.putInt(this->scalp->level);
			tempObjectSpecialsBatchInsertStatement.putInt(this->scalp->race);
			tempObjectSpecialsBatchInsertStatement.putInt(this->scalp->scalped ? 1 : 0);
			tempObjectSpecialsBatchInsertStatement.putString(ExtraStr1);
			tempObjectSpecialsBatchInsertStatement.putInt(this->obj_flags.timer);

			tempObjectSpecialsBatchInsertStatement.endEntry();
		}
	}
	if( contents )
	{
		// Save all contained items
		for(Object *obj = this->contains;obj;obj = obj->next_content)
		{
			bool isChest = this->IsValidChest();
			obj->saveItems(
			true,
				(isChest ? 'C' : 'O'),
				(isChest ? ToString(this->getRoom()->vnum) : ToString(this->objID)),
			topLevelHolderType,
			topLevelHolderID,
			tempObjectsBatchInsertStatement,
			tempObjectRetoolsBatchInsertStatement,
			tempObjectSpecialsBatchInsertStatement,
			true);
		}
	}
}

//TODO: Let's remove this.
void crashsaveAll()
{
	Character *i;

	for(i = character_list;i;i = i->next)
	{
		if( i->IsPurged() ) continue;
		i->itemSave();
	}
}
void crashListRent(Character *ch, char *name)
{
	unsigned int count = 1;
	std::list< std::pair<Object*,int> >::iterator oIter;
	std::list< std::pair<Object*,int> > objectList;
	std::stringstream outputBuffer;
	Object *i;
	Character *vict;

	get_char_cols(ch);

	if( !name || !playerExists(name) )
	{
		ch->send( NOPERSON );
		return;
	}

	if( (vict = CharacterUtil::loadCharacter(name)) == NULL )
	{
		ch->send(NOPERSON);
		return;
	}

	objectList = vict->loadItemList( true );
	delete (vict);

	outputBuffer << StringUtil::cap(StringUtil::allLower(name)) << " has the following items:\r\n\n";
	outputBuffer << "Num  Vnum                     UUID                        Creator                              Name\r\n";
	outputBuffer << "-----------------------------------------------------------------------------------------------------------\r\n";

	oIter = objectList.begin();

	while(oIter != objectList.end())
	{
		i = (*oIter).first;
		std::string creator = StringUtil::truncateWithEllipses((i)->creator, 35);
		std::string shortDescription = (i)->short_description;
		std::string objectId = ToString(i->objID);

		outputBuffer << std::left << std::setw(3) << (count) << "  ";
		outputBuffer << bld << cyn << std::left << std::setw(5) << (i->getVnum())    << nrm << "   ";
		outputBuffer << bld << cyn << std::left <<                 objectId          << nrm << "   ";
		outputBuffer << bld << grn << std::left << std::setw(35)<< (creator)         << nrm << "  ";
		outputBuffer <<        cyn << std::left <<                 (shortDescription)<< nrm << std::endl;

		++oIter;
		++count;
	}

	while(!objectList.empty())
	{
		objectList.front().first->Extract();
		objectList.pop_front();
	}

	char *cStringBuffer = new char[outputBuffer.str().size() + 1];
	strcpy(cStringBuffer, outputBuffer.str().c_str());
	outputBuffer.clear();
	page_string(ch->desc, cStringBuffer, TRUE);

	delete[] cStringBuffer;
}

void crashExtractObjects(Object *obj)
{
	if (obj)
	{
		crashExtractObjects(obj->contains);
		crashExtractObjects(obj->next_content);
		obj->Extract();
	}
}

int crashIsUnrentable(Object * obj)
{
	if (!obj)
		return 0;
	if (IS_OBJ_STAT(obj, ITEM_NORENT))
		return 1;

	return 0;
}

void crashExtractNoRents(Object * obj)
{
	Character *ch;

	if(obj)
	{
		crashExtractNoRents(obj->contains);
		crashExtractNoRents(obj->next_content);

		if (crashIsUnrentable(obj))
		{
			if (obj->carried_by)
				ch = obj->carried_by;
			else if (obj->worn_by)
				ch = obj->worn_by;
			else
				return;

			/* Let's have a heart, and save people the loss of master weapons over crashes. Galnor: 6-26-2004 */
			if(GET_OBJ_VNUM(obj) != GET_MASTER_WEAPON(ch))
			{
				obj->Extract(true);
			}
		}
	}
}

/* ************************************************************************
* Routines used for the receptionist					  *
************************************************************************* */

// Serai - 09/13/05 - bad recursive loop
int getRentCost(Object *obj)
{
	int cost = GET_OBJ_RENT(obj);

	for(obj = obj->contains; obj; obj = obj->next_content)
	{
		cost += getRentCost(obj);
	}

	return (cost);
}

int crashOfferRent(Character *ch, Character *recep)
{
	int cost = 0, i = 0;
	Object *obj = 0;

	for(obj = ch->carrying;obj;obj = obj->next_content)
	{
		cost += getRentCost(obj);
	}
	for(i = 0;i < NUM_WEARS;++i)
	{
		if(GET_EQ(ch, i))
		{
			cost += getRentCost(GET_EQ(ch, i));
		}
	}

	sprintf(buf, "$n tells you, 'Rent will cost you %lu gold, %lu silver, and %lu copper per day.'", CalcGold(cost),
	        CalcSilver(cost), CalcCopper(cost));
	Act(buf, FALSE, recep, 0, ch, TO_VICT);
	return cost;
}

void crashRentDeadline(Character * ch, Character * recep, long cost)
{
	long rent_deadline;

	if(Conf->csd.free_rent)
	{
		Act("$n tells you, 'Rent for you is free.'", FALSE, recep, 0, ch, TO_VICT);
		return;
	}

	if(cost)
		rent_deadline = ((ch->points.gold) / cost);
	else
		rent_deadline = 90;

	sprintf(buf,
	        "$n tells you, 'You can rent for %ld day%s with the coins you have.'",
	        rent_deadline, (rent_deadline > 1) ? "s" : "");
	Act(buf, FALSE, recep, 0, ch, TO_VICT);
}

int Character::listUnrentables(Character *innkeeper)
{
	int i, found;
	for(i = 0, found = 0;i < NUM_WEARS;++i)
	{
		if(GET_EQ(this, i))
		{
			found += this->listUnrentables(innkeeper, GET_EQ(this, i));
		}
	}
	return (found + listUnrentables(innkeeper, this->carrying));
}

int Character::listUnrentables(Character *innkeeper, Object *container)
{
	Object *obj;
	int found;
	if(!container)
		return 0;
	for(found = 0, obj = container->contains;obj;obj = obj->next_content)
	{
		found += this->listUnrentables(innkeeper, obj);
	}
	if(OBJ_FLAGGED(container, ITEM_NORENT))
	{
		innkeeper->InterpCommand("tell %s You cannot store %s.", GET_NAME(this), container->short_description);
		++found;
	}
	return found;
}

void Character::extractUnrentables()
{
	Object *obj = 0, *next_obj = 0;

	for(obj = this->carrying;next_obj;obj = next_obj)
	{
		next_obj = obj->next_content;
		if(OBJ_FLAGGED(obj, ITEM_NORENT))
		{
			obj_from_char(obj);
			obj->Extract(true);
		}
	}
}

int genReceptionist(Character * ch, Character * recep, char *cmd, char *arg, int mode)
{
	int cost = 0;
	const char *action_table[] =
	    { "smile", "dance", "sigh", "blush", "burp",
	      "cough", "fart", "twiddle", "yawn"
	    };

	if (!ch->desc || IS_NPC(ch))
		return FALSE;

	if (!cmd && !MiscUtil::random(0, 5))
	{
		do_action(recep, NULL, FindCommand(action_table[MiscUtil::random(0, 8)]), 0);
		return FALSE;
	}

	if (!CMD_IS(ch, "offer") && !CMD_IS(ch, "rent"))
		return FALSE;

	if (!AWAKE(recep))
	{
		ch->send("She is unable to talk to you...\r\n");
		return TRUE;
	}

	if (!CAN_SEE(recep, ch))
	{
		Act("$n says, 'I don't deal with people I can't see!'", FALSE, recep, 0, 0, TO_ROOM);
		return TRUE;
	}
	if(AFF_FLAGGED(ch, AFF_NOQUIT))
	{
		ch->send("Can't you feel the speed of your heartbeat?\r\n");
		return TRUE;
	}

	if (GET_POS(ch) == POS_FIGHTING)
	{
		ch->send("No way!  You're fighting for your life!\r\n");
		return FALSE;
	}

	if(GET_RACE(ch) != GET_RACE(recep) && !PLR_FLAGGED(ch, PLR_DARKFRIEND))
		return FALSE;

	if(ch->listUnrentables(recep))
		return TRUE;
	if(!Conf->csd.free_rent)
		cost = crashOfferRent(ch, recep);
	if (CMD_IS(ch, "rent"))
	{
		if(!Conf->csd.free_rent)
		{
			if (cost > ch->points.gold)
			{
				Act("$n tells you, '...which I see you can't afford.'",
				    FALSE, recep, 0, ch, TO_VICT);
				return TRUE;
			}

			crashRentDeadline(ch, recep, cost);
		}
		Act("$n stores your belongings and helps you into your private chamber.",
		    FALSE, recep, 0, ch, TO_VICT);

		if(!Conf->csd.free_rent)
		{
			MudLog(NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE, "%s has rented (%s%s%d%s, %s%s%d%s, %s%d%s/day, %s%s%d%s, %s%s%d%s, %s%d%s total.)",
			       GET_NAME(ch),
			       COLOR_BOLD(ch, CL_COMPLETE), COLOR_YELLOW(ch, CL_COMPLETE), CalcGold(cost), COLOR_NORMAL(ch, CL_COMPLETE),
			       COLOR_BOLD(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE), CalcSilver(cost), COLOR_NORMAL(ch, CL_COMPLETE),
			       COLOR_YELLOW(ch, CL_COMPLETE), CalcCopper(cost), COLOR_NORMAL(ch, CL_COMPLETE),
			       COLOR_BOLD(ch, CL_COMPLETE), COLOR_YELLOW(ch, CL_COMPLETE), CalcGold(ch->points.gold), COLOR_NORMAL(ch, CL_COMPLETE),
			       COLOR_BOLD(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE), CalcSilver(ch->points.gold), COLOR_NORMAL(ch, CL_COMPLETE),
			       COLOR_YELLOW(ch, CL_COMPLETE), CalcCopper(ch->points.gold), COLOR_NORMAL(ch, CL_COMPLETE));
		}
		else
			MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s has rented(free).", GET_NAME(ch));
		Act("$n helps $N into $S private chamber.", FALSE, recep, 0, ch, TO_NOTVICT);
		ch->save();
		ch->rentSave();
		ch->Extract(UserLogoutType::rent);
	}

	else
	{
		crashRentDeadline(ch, recep, cost);
		Act("$N gives $n an offer.", FALSE, ch, 0, recep, TO_ROOM);
	}

	return TRUE;
}

void Character::saveAll()
{
	this->rentSave();
	this->save();
}

void Character::rentSave()
{
	int i = 0;
	Descriptor *d, *next_d;
	Object *obj, *next_obj;

	//Get rid of no-rent items first...
	this->extractUnrentables();
	//Then save the items to disk.
	this->itemSave();

	//Now extract all items from the game.
	// Serai - 09/11/05 - bad loop condition
	for(obj = this->carrying; obj; obj = next_obj)
	{
		next_obj = obj->next_content;
		obj_from_char(obj);
		obj->Extract();
	}

	for (i = 0; i < NUM_WEARS; ++i)
	{
		if (GET_EQ(this, i))
			unequip_char(this, i)->Extract();
	}

	for (d = descriptor_list; d; d = next_d)
	{
		next_d = d->next;

		if (d == this->desc)
			continue;
		if (d->character && (d->character->player.idnum == this->player.idnum))
			STATE(d) = CON_DISCONNECT;
	}
}


SPECIAL(receptionist)
{
	return (genReceptionist(ch, (Character *)me, cmd, argument, 0));
}

/*** Galnor 12/24/2009 - Place an item into another item / player's inventory, whether
   or not they are online.
***/
ACMD( do_insert )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	*arg1 = *arg2 = *arg3 = '\0';
	Object *obj;
	char holderType;
	boost::uuids::uuid tID;
	std::string holderId;
	Character *cHolder;
	Object *oHolder;
	bool found = false, needExtraction = false;

	OneArgument( TwoArguments( argument, arg1, arg2 ), arg3 );

	if( !*arg1 || !*arg2 || !*arg3 )
	{//Usage...
		ch->send("Usage: insert <objectName> <o | p> <targetID>\r\n");
		return;
	}

	if( !(obj = get_obj_in_list_vis(ch, arg1, ch->carrying)) )
	{
		ch->send("You don't seem to be carrying that item.\r\n");
		return;
	}
	//Item is found.
	holderType = toupper(arg2[0]);

	//Try to locate the holder in the game, first.
	if( holderType == 'P' )
	{
		if( (cHolder = CharacterUtil::getOnlineCharacterById( atoi(arg3) )) )
		{//Found.
			obj_from_char(obj);
			obj_to_char(obj, cHolder);
			found = true;
		}
		else
			holderId = ToString(arg3);
	}
	else if( holderType == 'O' )
	{
		boost::uuids::string_generator uuidGenerator;
		try {
			tID = uuidGenerator(arg3);
		}
		catch(...) {
			ch->send("Invalid object id supplied.\r\n");
			return;
		}
		if( (oHolder = get_obj_by_id( tID )) )
		{//Found
			obj_from_char(obj);
			obj_to_obj(obj, oHolder);
			found = true;
		}
		else
			holderId = ToString(tID);
	}
	else
	{//Invalid owner type.
		ch->send("Owner type must be 'O' or 'P'\r\n");
		return;
	}

	if( !found )
	{//Time to work some database magic...

		//Should we even bother checking to see if the owner exists?
		//They will get extracted next time we reboot if no owner is found...
		Object::saveItemToTopLevelHolder(holderType, holderId, obj);
		found = true;
		needExtraction = true;
	}
	ch->send("You create a tear in spacetime, and place %s inside.\r\n", obj->short_description);
	Act("$n creates a tear in spacetime, and places $p inside.", TRUE, ch, obj, NULL, TO_ROOM);

	if( needExtraction ) {
		obj->RemoveFromAll();
		obj->Extract();
	}
}
ACMD( do_retrieve )
{
	char type[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

	TwoArguments(argument, type, arg2);

	if( !*type || !*arg2 )
	{
		ch->send("Retrieve: [object <objID>]\r\n");
		return;
	}

	if( !strn_cmp(type, "object", strlen(type)) )
	{
		boost::uuids::string_generator uuidGenerator;
		boost::uuids::uuid objID;
		Object *obj;
		bool exception = false;
		try {
			objID = uuidGenerator(arg2);
		} catch( ... ) {
			exception = true;
		}

		if( exception ||
			(
				(obj = get_obj_by_id(objID)) == 0 && 
				(obj = Object::loadSingleItem(objID, true)) == 0
			))
		{
			ch->send("No item exists with that identification number.\r\n");
			return;
		}
		obj->RemoveFromAll();
		obj_to_char(obj, ch);

		ch->send("You create a tear in spacetime, reach in, and retrieve %s.\r\n", obj->short_description);
		Act("$n creates a tear in spacetime, reaches in, and retrieves $p.", TRUE, ch, obj, NULL, TO_ROOM);
		return;
	}
	else
	{
		ch->send("Retrieve what?\r\n");
	}
}

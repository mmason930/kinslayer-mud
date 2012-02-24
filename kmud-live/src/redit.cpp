/************************************************************************
 *  OasisOLC - redit.c						v1.5	*
 *  Copyright 1996 Harvey Gilpin.					*
 *  Original author: Levork						*
 ************************************************************************/

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "utils.h"
#include "db.h"
#include "olc.h"
#include "shop.h"
#include "screen.h"
#include "weaves.h"
#include "auction.h"
#include "js_trigger.h"
#include "zones.h"

#include "StringUtil.h"
#include "Descriptor.h"

/* List each room saved, was used for debugging. */
#if 0
const int REDIT_LIST = 1;
#endif

/*------------------------------------------------------------------------*/

/*
 * External data structures.
 */
extern const int	rev_dir[];
extern const char	*room_bits[];
extern const char	*sector_types[];
extern const char	*exit_bits[];
extern Object		*obj_proto;
extern Descriptor	*descriptor_list;
extern Track		*TopTrack, *BottomTrack;
extern Character	*character_list;

/*------------------------------------------------------------------------*/

/*
 * Function Prototypes
 */
//void redit_save_to_disk(int zone);
void redit_disp_extradesc_menu(Descriptor *d);
void redit_disp_exit_menu(Descriptor *d);
void redit_disp_exit_flag_menu(Descriptor *d);
void redit_disp_flag_menu(Descriptor *d);
void redit_disp_sector_menu(Descriptor *d);
void redit_disp_menu(Descriptor *d);
void redit_parse(Descriptor *d, char *arg);
void redit_setup_new(Descriptor *d);
void redit_setup_existing(Descriptor *d, int real_num);
void redit_save_internally(Descriptor *d);
void free_room(Room *room);
int can_edit_zone(Character *ch, int number);

/*------------------------------------------------------------------------*/

/***
class ReditGUIListener
{
	std::map< std::string, kuDescriptor* > sessions;
	kuListener *listener;

public:
	ReditGUIListener( kuListener *listener );

	void listen( kuListener *listener );

};
***/

#define  W_EXIT(room, num) (World[(room)]->dir_option[(num)])

/*------------------------------------------------------------------------*\
  Utils and exported functions.
\*------------------------------------------------------------------------*/

void AddRoomToWorld( Room *TheRoom )
{
	int room_num = 0, cmd_no, i;
	std::vector<Room*>::iterator rIter;
	for(rIter = World.begin();rIter != World.end();++rIter, ++room_num)
	{
		if( (*rIter)->vnum > TheRoom->vnum )
		{
			World.insert(rIter, TheRoom);
			break;
		}
	}
	if( room_num == World.size() )
	{
		World.push_back(TheRoom);
		return;
	}

	Zone *zone;
	for (i = 0;(zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;++i)
	{
		for (cmd_no = 0;cmd_no < zone->cmd.size(); ++cmd_no)
		{
			switch (zone->cmd[cmd_no]->command)
			{
			case 'M':
			case 'O':
				if (zone->cmd[cmd_no]->arg3 >= room_num)
					++zone->cmd[cmd_no]->arg3;
				break;
			case 'D':
			case 'R':
				if (zone->cmd[cmd_no]->arg1 >= room_num)
					++zone->cmd[cmd_no]->arg1;
			case 'G':
			case 'P':
			case 'E':
				break;
			}
		}
	}
}

int buildwalk(Character *ch, int dir)
{
	char buf[MAX_INPUT_LENGTH];
	room_vnum vnum;
	Room *room;
	Descriptor *d = ch->desc;

	if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_BUILDWALK) && GET_LEVEL(ch) >= LVL_BUILDER)
	{
		//get_char_colors(ch);
		if ( !ch->in_room->GetZone()->CanEdit(ch) )
		{
			ch->Send("You do not have build permissions in this zone.\r\n");
		}
		else if ((vnum = ch->in_room->GetZone()->FindUnusedRoomVnum()) == NOWHERE)
			ch->Send("No free vnums are available in this zone!\r\n");
		else
		{

			/*
			 * Give the descriptor an olc struct.
			 * This way we can let redit_save_internally handle the room adding.
			 */

			if (d->olc)
			{
				MudLog(BRF, LVL_IMMORT, TRUE, "SYSERR: buildwalk(): Player already had olc structure.");
				cleanup_olc(d, CLEANUP_ALL);
				d->olc = NULL;
			}

			d->olc = new OLC();
			OLC_ZNUM(d) = ch->in_room->zone;
			OLC_NUM(d) = vnum;
			OLC_ROOM(d) = new Room();

			OLC_ROOM(d)->name = str_dup("New BuildWalk Room");

			sprintf(buf, "This unfinished room was created by %s.\r\n", GET_NAME(ch));
			OLC_ROOM(d)->description = str_dup(buf);
			OLC_ROOM(d)->zone = OLC_ZNUM(d);
			OLC_ROOM(d)->vnum = NOWHERE;

			/*
			 * Save the new room to memory.
			 * redit_save_internally handles adding the room in the right place, etc.
			 */
			redit_save_internally(d);
			OLC_VAL(d) = 0;

			/* Link rooms */
			room = FindRoomByVnum(vnum);
			EXIT(ch, dir) = new Direction();
			EXIT(ch, dir)->to_room = room;
			room->dir_option[rev_dir[dir]] = new Direction();
			room->dir_option[rev_dir[dir]]->to_room = ch->in_room;

			/* Report room creation to user */
			ch->Send("%s%sRoom #%d created by BuildWalk.%s\r\n", COLOR_BOLD(ch, CL_COMPLETE),
			         COLOR_CYAN(ch, CL_COMPLETE), vnum, COLOR_NORMAL(ch, CL_COMPLETE));
			cleanup_olc(d, CLEANUP_STRUCTS);

			return (1);

		}
	}

	return(0);
}

void redit_setup_new(Descriptor *d)
{
	OLC_ROOM(d) = new Room();

	OLC_ROOM(d)->name = str_dup("An unfinished room");
	OLC_ROOM(d)->description = str_dup("You are in an unfinished room.\r\n");
	redit_disp_menu(d);
	OLC_VAL(d) = 0;
}

/*------------------------------------------------------------------------*/

void redit_setup_existing(Descriptor *d, int real_num)
{
	//Attach copy of room to player's descriptor.
	OLC_ROOM(d) =  new Room(World[real_num]);
	OLC_VAL(d) = 0;
	redit_disp_menu(d);
}

/*------------------------------------------------------------------------*/

#define ZCMD (zone_table[zone].cmd[cmd_no])

void redit_save_internally(Descriptor *d)
{
	unsigned int i;
	int j, room_num, cmd_no;
	Room *new_room;
	Character *temp_ch;
	Object *temp_obj;

	room_num = real_room(OLC_NUM(d));

	//Allocate & copy
	new_room					= new Room(OLC_ROOM(d));

	//Change a few variables from their standard copy values.
	new_room->vnum				= OLC_NUM(d);
	new_room->func				= NULL;

	//EXISTING ROOM
	if (room_num > 0)
	{
		new_room->contents = World[room_num]->contents;
		new_room->people   = World[room_num]->people;

		std::vector<Room *>::iterator old_room;
		old_room = World.begin()+room_num;

		for(temp_ch = character_list;temp_ch;temp_ch = temp_ch->next)
		{
			if(temp_ch->in_room == (*old_room))
				temp_ch->in_room = new_room;
			if(temp_ch->was_in_room == (*old_room))
				temp_ch->was_in_room = new_room;
			if( temp_ch->Eavesdropping == (*old_room) )
				temp_ch->Eavesdropping = new_room;
		}
		if( !(*old_room)->eavesdropping.empty() ) {
			new_room->eavesdropping = (*old_room)->eavesdropping;
		}
		for(temp_obj = (*old_room)->contents;temp_obj;temp_obj = temp_obj->next_content)
		{
			temp_obj->in_room = new_room;
		}
		std::list< Gate* > TempGateList = GateManager::GetManager().GetGatesInRoom( (*old_room) );
		for(std::list<Gate *>::iterator gi = TempGateList.begin();gi != TempGateList.end();++gi)
		{
			if((*gi)->roomA == (*old_room))
				(*gi)->roomA = new_room;
			else if( (*gi)->roomB == (*old_room) )
				(*gi)->roomB = new_room;
		}

		Room *oldRoomPtr = (*old_room);
		(*old_room)->FreeLiveRoom();//No longer considered a prototype
		delete ( (*old_room) );
		World.erase(old_room);

		V_InsertAfter(World, new_room, room_num - 1);
		Conf->RenumberRooms();

		for(i = 0;i < World.size();++i)
		{
			for(j = 0;j < NUM_OF_DIRS;++j)
			{
				if(World[i]->dir_option[j] && World[i]->dir_option[j]->to_room == oldRoomPtr)
				{
					World[i]->dir_option[j]->to_room = new_room;
				}
			}
		}
	}

	//NONEXISTING ROOM
	else
	{
		AddRoomToWorld( new_room );
		room_num = real_room(new_room->vnum);
		new_room->zone = OLC_ZNUM(d);
	}

	olc_add_to_save_list(World[room_num]->GetZone()->getVnum(), OLC_SAVE_ROOM);
}

/*------------------------------------------------------------------------*/
class JSTrigger;
void Room::AddToBatch( sql::BatchInsertStatement &roomInsert,
					   sql::BatchInsertStatement &exitInsert,
					   sql::BatchInsertStatement &jsInsert )
{
	roomInsert.beginEntry();
	Zone *myZone = this->GetZone();

	roomInsert.putLong(this->vnum);
	roomInsert.putLong(myZone ? myZone->getVnum() : -1);
	roomInsert.putString( this->name ? this->name : "" );
	roomInsert.putString( this->description ? this->description : "" );
	roomInsert.putInt( this->sector_type );
	roomInsert.putInt( this->room_flags );
	roomInsert.putInt( this->auction_vnum );
	roomInsert.putString( extra_descr_data::Serialize(this->ex_description) );

	roomInsert.endEntry();

	for(unsigned int dir = 0;dir < NUM_OF_DIRS;++dir)
	{
		if( this->dir_option[ dir ] == NULL || this->dir_option[ dir ]->to_room == NULL )
			continue;
		Direction *exit = this->dir_option[ dir ];
		exitInsert.beginEntry();

		exitInsert.putInt(this->vnum);
		exitInsert.putInt(exit->to_room->vnum);
		exitInsert.putString( exit->general_description ? exit->general_description : "" );
		exitInsert.putString( exit->keyword ? exit->keyword : "" );
		exitInsert.putInt( exit->exit_info );
		exitInsert.putInt( exit->hidden );
		exitInsert.putInt( exit->PickReq );
		exitInsert.putInt( exit->key );
		exitInsert.putInt( dir );

		exitInsert.endEntry();
	}

#ifdef KINSLAYER_JAVASCRIPT
	for(unsigned int i = 0;i < this->js_scripts->size();++i) {
		jsInsert.beginEntry();

		jsInsert.putString("R");
		jsInsert.putLong(this->vnum);
		jsInsert.putLong(this->js_scripts->at(i)->vnum);

		jsInsert.endEntry();
	}
#endif
}

void reditSaveFinalizationQueryProcessor( sql::Connection connection, std::string query1, std::string query2 )
{
	if( !query1.empty() )
		connection->sendRawQuery(query1);
	if( !query2.empty() )
		connection->sendRawQuery(query2);
}

void redit_save_to_disk( int lowVnum, int highVnum )
{
	sql::Connection localConnection;
	try {
		localConnection = dbContext->createConnection();
	} catch( sql::ConnectionException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Unable to establish connection with MUD database while saving rooms : %s",
			e.getMessage().c_str());
		return;
	}
	try {
		std::stringstream queryBuffer;

		std::string tempRoomTableName = "tempRoomSaveTable";
		std::string tempExitTableName = "tempRoomExitSaveTable";
		std::string tempJSAttachmentTable = "tempJSAttachmentSaveTable";

		localConnection->sendRawQuery("DROP TABLE IF EXISTS " + tempRoomTableName);
		localConnection->sendRawQuery("DROP TABLE IF EXISTS " + tempExitTableName);
		localConnection->sendRawQuery("DROP TABLE IF EXISTS " + tempJSAttachmentTable);
		localConnection->sendRawQuery("CREATE TABLE " + tempRoomTableName + " LIKE rooms");
		localConnection->sendRawQuery("CREATE TABLE " + tempExitTableName + " LIKE room_exits");
		localConnection->sendRawQuery("CREATE TABLE " + tempJSAttachmentTable + " LIKE js_attachments");

		sql::BatchInsertStatement roomInsert( localConnection, tempRoomTableName    , 1000 );
		sql::BatchInsertStatement exitInsert( localConnection, tempExitTableName    , 1000 );
		sql::BatchInsertStatement jsInsert(   localConnection, tempJSAttachmentTable, 1000 );

		roomInsert.addField("vnum");
		roomInsert.addField("znum");
		roomInsert.addField("name");
		roomInsert.addField("description");
		roomInsert.addField("sector");
		roomInsert.addField("room_flags");
		roomInsert.addField("auction_vnum");
		roomInsert.addField("edescription");

		exitInsert.addField("room_vnum");
		exitInsert.addField("to_room");
		exitInsert.addField("general_description");
		exitInsert.addField("keyword");
		exitInsert.addField("exit_info");
		exitInsert.addField("hidden_level");
		exitInsert.addField("pick_req");
		exitInsert.addField("key_vnum");
		exitInsert.addField("dir");

		jsInsert.addField("type");
		jsInsert.addField("target_vnum");
		jsInsert.addField("script_vnum");

		roomInsert.start();
		exitInsert.start();
		jsInsert.start();

		int bottomRnum = 0;
		while( World[ bottomRnum ]->vnum < lowVnum )
			bottomRnum++;

		while( bottomRnum < World.size() && World[ bottomRnum ]->vnum <= highVnum )
		{
			World[ bottomRnum++ ]->AddToBatch( roomInsert, exitInsert, jsInsert );
		}

		roomInsert.finish();
		exitInsert.finish();
		jsInsert.finish();

		std::string query1, query2, query3, query4, query5, query6;
		//The temporary tables have been created and populated by this point. We need to update the live tables now.

		///////// Insert the rooms /////////
		queryBuffer.str("");
		queryBuffer << "DELETE FROM rooms WHERE vnum BETWEEN " << lowVnum << " AND " << highVnum;
		query1 = queryBuffer.str();

		queryBuffer.str("");
		queryBuffer << "INSERT INTO rooms ("
					<< "vnum,znum,name,description,sector,room_flags,auction_vnum,edescription"
					<< ") SELECT "
					<< "vnum,znum,name,description,sector,room_flags,auction_vnum,edescription"
					<< " FROM " << tempRoomTableName;
		query2 = queryBuffer.str();

		///////// Insert the exits /////////
		queryBuffer.str("");
		queryBuffer << "DELETE FROM room_exits WHERE room_vnum BETWEEN " << lowVnum << " AND " << highVnum;
		query3 = queryBuffer.str();

		queryBuffer.str("");
		queryBuffer << "INSERT INTO room_exits ("
					<< "room_vnum,to_room,general_description,keyword,exit_info,keeper,hidden_level,pick_req,key_vnum,dir"
					<< ") SELECT "
					<< "room_vnum,to_room,general_description,keyword,exit_info,keeper,hidden_level,pick_req,key_vnum,dir"
					<< " FROM " << tempExitTableName;
		query4 = queryBuffer.str();

		///////// Insert the js attachments /////////
		queryBuffer.str("");
		queryBuffer << "DELETE FROM js_attachments WHERE type='R' AND target_vnum BETWEEN " << lowVnum << " AND " << highVnum;
		query5 = queryBuffer.str();

		queryBuffer.str("");
		queryBuffer << "INSERT INTO js_attachments ("
					<< "type,target_vnum,script_vnum"
					<< ") SELECT "
					<< "type,target_vnum,script_vnum"
					<< " FROM " << tempJSAttachmentTable;
		query6 = queryBuffer.str();
		
		//Distribute the workload.
		boost::thread thread1( boost::bind( reditSaveFinalizationQueryProcessor, dbContext->createConnection(), query1, query2 ) );
		boost::thread thread2( boost::bind( reditSaveFinalizationQueryProcessor, dbContext->createConnection(), query3, query4 ) );
		boost::thread thread3( boost::bind( reditSaveFinalizationQueryProcessor, dbContext->createConnection(), query5, query6 ) );

		//Resynchronize.
		thread1.join();
		thread2.join();
		thread3.join();

		boost::thread( boost::bind(reditSaveFinalizationQueryProcessor, dbContext->createConnection(), "DROP TABLE IF EXISTS " + tempRoomTableName, "") );
		boost::thread( boost::bind(reditSaveFinalizationQueryProcessor, dbContext->createConnection(), "DROP TABLE IF EXISTS " + tempExitTableName, "") );
		boost::thread( boost::bind(reditSaveFinalizationQueryProcessor, dbContext->createConnection(), "DROP TABLE IF EXISTS " + tempJSAttachmentTable, "") );

	} catch( sql::QueryException e ) {
		MudLog(BRF, TRUE, LVL_APPR, "Failed to save rooms : %s", e.getErrorMessage().c_str());
		return;
	}
}
void redit_save_to_disk(int zone_num)
{
	int counter, realcounter;
	Room *room;
	Zone *zone = ZoneManager::GetManager().GetZoneByRnum(zone_num);

	if( zone == NULL )
	{
		Log("SYSERR: redit_save_to_disk: Invalid real zone passed!");
		return;
	}
	redit_save_to_disk( zone->GetBottom(), zone->GetTop() );
	olc_remove_from_save_list(zone->getVnum(), OLC_SAVE_ROOM);
}

/*------------------------------------------------------------------------*/

void free_room(Room *room)
{
	int i;
	struct extra_descr_data *thist, *next;

	if (room->name)
		delete[] (room->name);
	if (room->description)
		delete[] (room->description);

	/*
	 * Free exits.
	 */
	for (i = 0; i < NUM_OF_DIRS; i++)
	{
		if (room->dir_option[i])
		{
			if (room->dir_option[i]->general_description)
				delete[] (room->dir_option[i]->general_description);

			if (room->dir_option[i]->keyword)
				delete[] (room->dir_option[i]->keyword);
		}

		delete (room->dir_option[i]);
	}

	/*
	 * Free extra descriptions.
	 */
	for (thist = room->ex_description; thist; thist = next)
	{
		next = thist->next;

		if (thist->keyword)
			delete[] (thist->keyword);

		if (thist->description)
			delete[] (thist->description);

		delete[] (thist);
	}
}

/**************************************************************************
 Menu functions
 **************************************************************************/

/*
 * For extra descriptions.
 */
void redit_disp_extradesc_menu(Descriptor *d)
{
	extra_descr_data *extra_desc = OLC_DESC(d);

	sprintf(buf,
#if defined(CLEAR_SCREEN)
	        "[H[J"
#endif
	        "%s1%s) Keyword: %s%s\r\n"
	        "%s2%s) Description:\r\n%s%s\r\n"
	        "%s3%s) Goto next description: ",

	        grn, nrm, yel, extra_desc->keyword ? extra_desc->keyword : "<NONE>",
	        grn, nrm, yel, extra_desc->description ? extra_desc->description : "<NONE>",
	        grn, nrm
	       );

	strcat(buf, !extra_desc->next ? "<NOT SET>\r\n" : "Set.\r\n");
	strcat(buf, "Enter choice (0 to quit) : ");
	d->Send(buf);
	OLC_MODE(d) = REDIT_EXTRADESC_MENU;
}

/*
 * For exits.
 */
void redit_disp_exit_menu(Descriptor *d)
{
	/*
	 * if exit doesn't exist, alloc/create it
	 */
	if (!OLC_EXIT(d))
		OLC_EXIT(d) = new Direction();

	/*
	 * Weird door handling!
	 */
	if (IS_SET(OLC_EXIT(d)->exit_info, EX_ISDOOR))
	{
		strcpy(buf2, "Is a door");
	}
	else
		strcpy(buf2, "No door");

	get_char_cols(d->character);
	d->Send(
#if defined(CLEAR_SCREEN)
	    "[H[J"
#endif
	    "%s1%s) Exit to     : %s%d\r\n"
	    "%s2%s) Description :-\r\n%s%s\r\n"
	    "%s3%s) Door name   : %s%s\r\n"
	    "%s4%s) Key         : %s%d\r\n"
	    "%s5%s) Door flags  : %s%s\r\n"
	    "%s6%s) Hidden level: %s%d\r\n"
	    "%s7%s) Pick Req    : %s%d\r\n"
	    "%s8%s) Purge exit.\r\n"
	    "Enter choice, 0 to quit : ",

	    grn, nrm, cyn, OLC_EXIT(d)->to_room ? OLC_EXIT(d)->to_room->vnum : -1,
	    grn, nrm, yel, OLC_EXIT(d)->general_description ? OLC_EXIT(d)->general_description : "<NONE>",
	    grn, nrm, yel, OLC_EXIT(d)->keyword ? OLC_EXIT(d)->keyword : "<NONE>",
	    grn, nrm, cyn, OLC_EXIT(d)->key,
	    grn, nrm, cyn, buf2,
	    grn, nrm, cyn, OLC_EXIT(d)->HiddenLevel(),
	    grn, nrm, cyn, OLC_EXIT(d)->PickReq,
	    grn, nrm
	);

	OLC_MODE(d) = REDIT_EXIT_MENU;
}

/*
 * For exit flags.
 */
void redit_disp_exit_flag_menu(Descriptor *d)
{
	get_char_cols(d->character);
	d->Send("%s1%s) Door Exists: %s\r\n"
			"%s2%s) Rammable: %s\r\n"
			"%s3%s) Exit\r\n"
			"	Enter choice : ", grn, nrm, StringUtil::yesNo(IS_SET(OLC_EXIT(d)->exit_info, EX_ISDOOR)).c_str(), grn, nrm,
			(OLC_EXIT(d)->IsRammable() ? "Yes" : "No"), grn, nrm);
}

/*
 * For room flags.
 */
void redit_disp_flag_menu(Descriptor *d)
{
	int counter, columns = 0;

	get_char_cols(d->character);
#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	for (counter = 0; counter < NUM_ROOM_FLAGS; ++counter)
	{
		d->Send("%s%2d%s) %-20.20s %s", grn, counter + 1, nrm,
		        room_bits[counter], !(++columns % 2) ? "\r\n" : "");
	}
	sprintbit(OLC_ROOM(d)->room_flags, (const char**)room_bits, buf1);
	d->Send("\r\nRoom flags: %s%s%s\r\n"
	        "Enter room flags, 0 to quit : ", cyn, buf1, nrm);
	OLC_MODE(d) = REDIT_FLAGS;
}

/*
 * For sector type.
 */
void redit_disp_sector_menu(Descriptor *d)
{
	int counter, columns = 0;

#if defined(CLEAR_SCREEN)

	d->Send("[H[J");
#endif

	for (counter = 0; counter < NUM_ROOM_SECTORS; ++counter)
	{
		d->Send("%s%2d%s) %-20.20s %s", grn, counter, nrm,
		        sector_types[counter], !(++columns % 2) ? "\r\n" : "");
	}
	d->Send("\r\nEnter sector type : ");
	OLC_MODE(d) = REDIT_SECTOR;
}

/*
 * The main menu.
 */
void redit_disp_menu(Descriptor *d)
{
	Room *room;
	Auction *a;
	std::string aBuffer;

	get_char_cols(d->character);
	room = OLC_ROOM(d);
	Zone *zone = ZoneManager::GetManager().GetZoneByRnum(OLC_ZNUM(d));

	sprintbit(room->room_flags, (const char**)room_bits, buf1);
	sprinttype(room->sector_type, (const char **) sector_types, buf2);

	if( d->olc->room->auction_vnum != -1
	&& (a = AuctionManager::GetManager().GetAuction(d->olc->room->auction_vnum)) != NULL )
		sprintf(aBuffer, "%s%s%s [ %s%d%s ]", cyn, a->getName().c_str(), nrm, cyn, a->getVnum(), nrm);
	else
		sprintf(aBuffer, "%s< NONE >%s", cyn, nrm);

	d->Send(

#if defined(CLEAR_SCREEN)
	    "[H[J"
#endif
	    "-- Room number : [%s%d%s]  	Room zone: [%s%d%s]\r\n"
	    "%s1%s) Name        : %s%s\r\n"
	    "%s2%s) Description :\r\n%s%s"
	    "%s3%s) Room flags  : %s%s\r\n"
	    "%s4%s) Sector type : %s%s\r\n"
	    "%s5%s) Exit north  : %s%d\r\n"
	    "%s6%s) Exit east   : %s%d\r\n"
	    "%s7%s) Exit south  : %s%d\r\n"
	    "%s8%s) Exit west   : %s%d\r\n"
	    "%s9%s) Exit up     : %s%d\r\n"
	    "%sA%s) Exit down   : %s%d\r\n"
		"%sB%s) Auction     : %s\r\n"
	    "%sC%s) Extra descriptions menu\r\n"
	    "%sD%s) Delete      : %s%s\r\n"
#ifdef KINSLAYER_JAVASCRIPT
		"%sJ%s) JavaScript  : %s%s\r\n"
#endif
	    "%sQ%s) Quit\r\n"
	    "Enter choice : ",

	    cyn, OLC_NUM(d), nrm,
	    cyn, zone->getVnum(), nrm,
	    grn, nrm, yel, room->name,
	    grn, nrm, yel, room->description,
	    grn, nrm, cyn, buf1,
	    grn, nrm, cyn, buf2,
	    grn, nrm, cyn,
	    room->dir_option[NORTH] && room->dir_option[NORTH]->to_room ?
	    room->dir_option[NORTH]->to_room->vnum : -1,
	    grn, nrm, cyn,
	    room->dir_option[EAST] && room->dir_option[EAST]->to_room ?
	    room->dir_option[EAST]->to_room->vnum : -1,
	    grn, nrm, cyn,
	    room->dir_option[SOUTH] && room->dir_option[SOUTH]->to_room ?
	    room->dir_option[SOUTH]->to_room->vnum : -1,
	    grn, nrm, cyn,
	    room->dir_option[WEST] && room->dir_option[WEST]->to_room ?
	    room->dir_option[WEST]->to_room->vnum : -1,
	    grn, nrm, cyn,
	    room->dir_option[UP] && room->dir_option[UP]->to_room ?
	    room->dir_option[UP]->to_room->vnum : -1,
	    grn, nrm, cyn,
	    room->dir_option[DOWN] && room->dir_option[DOWN]->to_room ?
	    room->dir_option[DOWN]->to_room->vnum : -1,
		grn, nrm, aBuffer.c_str(),
	    grn, nrm,
	    grn, nrm, cyn, room->deleted ? "Yes" : "No",
#ifdef KINSLAYER_JAVASCRIPT
		grn, nrm, cyn, (room->js_scripts->size() ? "Set." : "Not Set."),
#endif
	    grn, nrm
	);

	OLC_MODE(d) = REDIT_MAIN_MENU;
}

/**************************************************************************
  The main loop
 **************************************************************************/

void redit_parse(Descriptor *d, char *arg)
{
	int number = 0;

	switch (OLC_MODE(d))
	{
		case REDIT_CONFIRM_SAVESTRING:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					AddOlcLog( d->character, "room", d->olc->number );
					redit_save_internally(d);
					MudLog(CMP, MAX(LVL_BUILDER, GET_INVIS_LEV(d->character)), TRUE, "OLC: %s edits room %d.",
					       GET_NAME(d->character), OLC_NUM(d));
					/*
					 * Do NOT free strings! Just the room structure.
					 */
					cleanup_olc(d, CLEANUP_STRUCTS);
					d->Send("Room saved to memory.\r\n");
					break;
				case 'n':
				case 'N':
					/*
					 * Free everything up, including strings, etc.
					 */
					cleanup_olc(d, CLEANUP_ALL);
					break;
				default:
					d->Send("Invalid choice!\r\nDo you wish to save this room internally? : ");
					break;
			}
			return;

		case REDIT_MAIN_MENU:
			switch (*arg)
			{
				case 'q':
				case 'Q':
					if (OLC_VAL(d))
					{ /* Something has been modified. */
						d->Send("Do you wish to save this room internally? : ");
						OLC_MODE(d) = REDIT_CONFIRM_SAVESTRING;
					}
					else
						cleanup_olc(d, CLEANUP_ALL);
					return;
				case '1':
					d->Send("Enter room name:-\r\n] ");
					OLC_MODE(d) = REDIT_NAME;
					break;
				case '2':
					OLC_MODE(d) = REDIT_DESC;
#if defined(CLEAR_SCREEN)

					d->Send("\x1B[H\x1B[J");
#endif

					d->Send("Enter room description: (/s saves /h for help)\r\n\r\n");
					d->backstr = NULL;
					if (OLC_ROOM(d)->description)
					{
						d->Send(OLC_ROOM(d)->description);
						d->backstr = str_dup(OLC_ROOM(d)->description);
					}
					d->str = &OLC_ROOM(d)->description;
					d->max_str = MAX_ROOM_DESC;
					d->mail_to = 0;
					OLC_VAL(d) = 1;
					break;
				case '3':
					redit_disp_flag_menu(d);
					break;
				case '4':
					redit_disp_sector_menu(d);
					break;
				case '5':
					OLC_VAL(d) = NORTH;
					redit_disp_exit_menu(d);
					break;
				case '6':
					OLC_VAL(d) = EAST;
					redit_disp_exit_menu(d);
					break;
				case '7':
					OLC_VAL(d) = SOUTH;
					redit_disp_exit_menu(d);
					break;
				case '8':
					OLC_VAL(d) = WEST;
					redit_disp_exit_menu(d);
					break;
				case '9':
					OLC_VAL(d) = UP;
					redit_disp_exit_menu(d);
					break;
				case 'a':
				case 'A':
					OLC_VAL(d) = DOWN;
					redit_disp_exit_menu(d);
					break;
				case 'b':
				case 'B':
					d->Send("Enter the vnum of the auction to be used for this room : ");
					OLC_MODE(d) = REDIT_AUCTION_VNUM;
					break;
				case 'c':
				case 'C':
					/*
					 * If the extra description doesn't exist.
					 */
					if (!OLC_ROOM(d)->ex_description)
					{
						OLC_ROOM(d)->ex_description = new struct extra_descr_data;
						OLC_ROOM(d)->ex_description->keyword = 0;
						OLC_ROOM(d)->ex_description->description = 0;
						OLC_ROOM(d)->ex_description->next = 0;
					}
					OLC_DESC(d) = OLC_ROOM(d)->ex_description;
					redit_disp_extradesc_menu(d);
					break;
				case 'd':
				case 'D':
					if(OLC_ROOM(d)->deleted)
					{
						d->Send("This room will no longer be deleted.\r\n");
						OLC_ROOM(d)->deleted = false;
					}
					else
					{
						d->Send("This room will be deleted after the next reboot.\r\n");
						OLC_ROOM(d)->deleted = true;
					}
					OLC_VAL(d) = 1;
					redit_disp_menu(d);
					return;
#ifdef KINSLAYER_JAVASCRIPT
				case 'j':
				case 'J':
					OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_MAIN_MENU;
					OLC_MODE(d) = OLC_JSCRIPT_EDIT;
					d->olc->jsScripts = d->olc->room->js_scripts;
					JScriptDispMenu(d);
					return;
#endif
				default:
					d->Send("Invalid choice!");
					redit_disp_menu(d);
					break;
			}
			return;
#ifdef KINSLAYER_JAVASCRIPT
		case OLC_JSCRIPT_EDIT:
			if( JScriptParse(d, arg) )
				return;
			break;
#endif
		case REDIT_NAME:
			if (OLC_ROOM(d)->name)
				delete[] (OLC_ROOM(d)->name);
			if (strlen(arg) > MAX_ROOM_NAME)
				arg[MAX_ROOM_NAME - 1] = '\0';
			OLC_ROOM(d)->name = str_dup((arg && *arg) ? arg : "undefined");
			break;

		case REDIT_DESC:
			/*
			 * We will NEVER get here, we hope.
			 */
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: Reached REDIT_DESC case in parse_redit");
			break;

		case REDIT_FLAGS:
			number = atoi(arg);
			if ((number < 0) || (number > NUM_ROOM_FLAGS))
			{
				d->Send("That is not a valid choice!\r\n");
				redit_disp_flag_menu(d);
			}
			else if (number == 0)
				break;
			else
			{
				/*
				 * Toggle the bit.
				 */
				TOGGLE_BIT(OLC_ROOM(d)->room_flags, (1 << (number - 1)));
				redit_disp_flag_menu(d);
			}
			return;

		case REDIT_SECTOR:
			number = atoi(arg);
			if (number < 0 || number >= NUM_ROOM_SECTORS)
			{
				d->Send("Invalid choice!");
				redit_disp_sector_menu(d);
				return;
			}
			else
				OLC_ROOM(d)->sector_type = number;
			break;
		case REDIT_EXIT_MENU:
			switch (*arg)
			{
				case '0':
					break;
				case '1':
					OLC_MODE(d) = REDIT_EXIT_NUMBER;
					d->Send("Exit to room number : ");
					return;
				case '2':
					OLC_MODE(d) = REDIT_EXIT_DESCRIPTION;
					SEND_TO_Q("Enter exit description: (/s saves /h for help)\r\n\r\n", d);
					d->backstr = NULL;
					if (OLC_EXIT(d)->general_description)
					{
						SEND_TO_Q(OLC_EXIT(d)->general_description, d);
						d->backstr = str_dup(OLC_EXIT(d)->general_description);
					}
					d->str = &OLC_EXIT(d)->general_description;
					d->max_str = MAX_EXIT_DESC;
					d->mail_to = 0;
					return;
				case '3':
					OLC_MODE(d) = REDIT_EXIT_KEYWORD;
					d->Send("Enter keywords : ");
					return;
				case '4':
					OLC_MODE(d) = REDIT_EXIT_KEY;
					d->Send("Enter key number : ");
					return;
				case '5':
					redit_disp_exit_flag_menu(d);
					OLC_MODE(d) = REDIT_EXIT_DOORFLAGS;
					return;
				case '6':
					d->Send("Enter a hidden level for this door: ");
					OLC_MODE(d) = REDIT_HIDDEN;
					return;
				case '7':
					d->Send("Enter the pick requirement for this door: ");
					OLC_MODE(d) = REDIT_EXIT_PICKREQ;
					return;
				case '8':
					 //Delete an exit.
					delete OLC_EXIT(d);
					OLC_EXIT(d) = NULL;
					break;
				default:
					d->Send("Try again : ");
					return;
			}
			break;
		case REDIT_EXIT_NUMBER:
			if ((number = atoi(arg)) != -1)
				if ((number = real_room(number)) < 0)
				{
					d->Send("That room does not exist, try again : ");
					return;
				}
			OLC_EXIT(d)->to_room = FindRoomByVnum(atoi(arg));
			redit_disp_exit_menu(d);
			return;
		case REDIT_EXIT_DESCRIPTION:
			/*
			 * We should NEVER get here, hopefully.
			 */
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: Reached REDIT_EXIT_DESC case in parse_redit");
			break;
		case REDIT_EXIT_KEYWORD:
			if (OLC_EXIT(d)->keyword)
				delete[] (OLC_EXIT(d)->keyword);
			OLC_EXIT(d)->keyword = ((arg && *arg) ? str_dup(arg) : NULL);
			redit_disp_exit_menu(d);
			return;
		case REDIT_EXIT_KEY:
			OLC_EXIT(d)->key = atoi(arg);
			redit_disp_exit_menu(d);
			return;
		case REDIT_HIDDEN:
			number = atoi(arg);

			if(number < 0 || number > 99)
			{
				d->Send("Exit's hidden level must be between 0 and 99.\r\n");
				d->Send("Try again: ");
				return;
			}
			else
			{
				OLC_EXIT(d)->hidden = (char)number;
				redit_disp_exit_menu(d);
			}
			return;
		case REDIT_EXIT_PICKREQ:
			number = atoi(arg);

			if(number < -1 || number > 100)
			{
				d->Send("Pick requirement must be between 0 and 100, or -1 for pickproof.\r\n");
				d->Send("Try again: ");
				return;
			}
			else
			{
				OLC_EXIT(d)->PickReq = number;
				redit_disp_exit_menu(d);
			}
			return;
		case REDIT_EXIT_DOORFLAGS:
			number = atoi(arg);
			if( number == 1 )
			{
				TOGGLE_BIT(OLC_EXIT(d)->exit_info, EX_ISDOOR);
				redit_disp_exit_flag_menu(d);
			}
			else if( number == 2 )
			{
				OLC_EXIT(d)->ToggleRammable();
				redit_disp_exit_flag_menu(d);
			}
			else if( number == 3 )
			{
				redit_disp_exit_menu(d);
			}
			else
			{
				d->Send("That's not a valid choice!\r\n");
				redit_disp_exit_flag_menu(d);
			}
			return;
		case REDIT_AUCTION_VNUM:
			if( (AuctionManager::GetManager().GetAuction( atoi(arg) )) != NULL )
				d->olc->room->auction_vnum = atoi(arg);
			else
				d->olc->room->auction_vnum = (-1);
			redit_disp_menu(d);
			break;
		case REDIT_EXTRADESC_KEY:
			OLC_DESC(d)->keyword = ((arg && *arg) ? str_dup(arg) : NULL);
			redit_disp_extradesc_menu(d);
			return;
		case REDIT_EXTRADESC_MENU:
			switch ((number = atoi(arg)))
			{
				case 0:
					{
						/*
						 * If something got left out, delete the extra description
						 * when backing out to the menu.
						 */
						if (!OLC_DESC(d)->keyword || !OLC_DESC(d)->description)
						{
							extra_descr_data **tmp_desc;

							/*
							 * Clean up pointers.
							 */
							for (tmp_desc = &(OLC_ROOM(d)->ex_description); *tmp_desc;tmp_desc = &((*tmp_desc)->next))
							{
								if (*tmp_desc == OLC_DESC(d))
								{
									*tmp_desc = NULL;
									break;
								}
							}
							delete (OLC_DESC(d));
						}
					}
					break;
				case 1:
					OLC_MODE(d) = REDIT_EXTRADESC_KEY;
					d->Send("Enter keywords, separated by spaces : ");
					return;
				case 2:
					OLC_MODE(d) = REDIT_EXTRADESC_DESCRIPTION;
					d->Send("Enter extra description: (/s saves /h for help)\r\n\r\n");
					d->backstr = NULL;
					if (OLC_DESC(d)->description)
					{
						SEND_TO_Q(OLC_DESC(d)->description, d);
						d->backstr = str_dup(OLC_DESC(d)->description);
					}
					d->str = &OLC_DESC(d)->description;
					d->max_str = MAX_MESSAGE_LENGTH;
					d->mail_to = 0;
					return;

				case 3:
					if (!OLC_DESC(d)->keyword || !OLC_DESC(d)->description)
					{
						d->Send("You can't edit the next extra desc without completing this one.\r\n");
						redit_disp_extradesc_menu(d);
					}
					else
					{
						extra_descr_data *new_extra;

						if (OLC_DESC(d)->next)
							OLC_DESC(d) = OLC_DESC(d)->next;
						else
						{
							/*
							 * Make new extra description and attach at end.
							 */
							new_extra = new extra_descr_data;
							memset(new_extra, 0, sizeof(extra_descr_data));
							OLC_DESC(d)->next = new_extra;
							OLC_DESC(d) = new_extra;
						}
						redit_disp_extradesc_menu(d);
					}
					return;
			}
			break;
		default:
			/*
			 * We should never get here.
			 */
			MudLog(BRF, LVL_BUILDER, TRUE, "SYSERR: Reached default case in parse_redit");
			break;
	}
	/*
	 * If we get this far, something has been changed.
	 */
	OLC_VAL(d) = 1;
	redit_disp_menu(d);
}

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "db.h"
#include "utils.h"
#include "mobs.h"
#include "zones.h"
#include "threaded_jobs.h"
#include "handler.h"
#include "weather.h"
#include "js.h"

boost::recursive_mutex ZoneManager::SingletonMutex;
extern Index *obj_index;
void add_follower(Character * ch, Character * leader);
int perform_group( Character *ch, Character *vict );

unsigned int Zone::CountObjects( const int obj_nr )
{
	unsigned int count = 0, room;

	for (room = GetBottom();room < GetTop() && room < World.size();++room)
	{
		int rroom = real_room(room);
		Object *obj;
		if( rroom == NOWHERE ) continue;
		for( obj = World[ rroom ]->contents;obj;obj = obj->next_content )
		{
			if( obj->item_number == obj_nr ) ++count;
		}
	}
	return count;
}
unsigned int Zone::CountMobs( const int mob_nr )
{
	static unsigned int room, i;
	static Character *ch;

	for (i = 0, room = GetBottom();room < GetTop() && (unsigned int)room < World.size();++room)
	{
		for(ch = World[room]->people;ch;ch = ch->next_in_room)
		{
			if(GET_MOB_RNUM(ch) == mob_nr)
				++i;
		}
	}
	return i;
}

Zone::Zone( Zone *Source )
{
	CopyFrom(Source);
}
void Zone::CopyFrom( Zone *Source )
{
	setName(Source->getName());
	SetLifespan(Source->GetLifespan());
	SetAge(Source->GetAge());
	SetX(Source->GetX());
	SetY(Source->GetY());
	SetBottom(Source->GetBottom());
	SetTop(Source->GetTop());
	SetResetMode(Source->GetResetMode());
	setVnum(Source->getVnum());
	SetRnum(Source->GetRnum());
	this->weather = NULL;

	SetSunrise(Source->GetSunrise());
	SetSunset(Source->GetSunset());
	SetBuilders(Source->GetBuilders());
	deleted = Source->IsDeleted();
	inDB = Source->IsInDB();
	closed = Source->IsClosed();
}

Zone::Zone( const unsigned int vnum )
{
	this->lifespan	= 30;
	this->age		= 0;
	this->x			= -100;
	this->y			= -100;
	this->bot		= 0;
	this->top		= 0;
	this->reset_mode= 2;
	this->vnum		= vnum;
	this->rnum		= 0;
	this->deleted	= false;
	this->inDB		= false;
	this->closed	= true;
	this->weather = new Weather(vnum);

	this->bot = (vnum * 100);
	this->top = (vnum * 100) + 99;//Default. This can be changed in zedit.
}

/* Galnor 04-01-2005 Zone constructor. */
Zone::Zone()
{
	this->lifespan	= 30;
	this->age		= 0;
	this->x			= -100;
	this->y			= -100;
	this->bot		= 0;
	this->top		= 0;
	this->reset_mode= 2;
	this->vnum		= 0;
	this->rnum		= 0;
	this->inDB		= false;
	this->deleted	= false;
	this->closed	= true;
	this->weather = new Weather(this->vnum);
}

/* Galnor 04-01-2005 Zone destructor. */
Zone::~Zone()
{
	while( !cmd.empty() )
	{
		delete cmd.back();
		cmd.pop_back();
	}
	if( this->weather )
		delete (this->weather);
}

bool Zone::AddCommand( const unsigned int pos )
{
	return AddCommand(new ResetCommand(), pos);
}
bool Zone::AddCommand( ResetCommand *RC, const unsigned int pos )
{
	if( (pos >= 0) && (pos <= cmd.size()) )
		V_InsertBefore(cmd, RC, pos);
	else	return false;
	return true;
}
bool Zone::RemoveCommand( const unsigned int pos, bool purge )
{
	if ( (pos >= 0) && (pos < cmd.size()) )
	{
		if( purge ) delete cmd[pos];
		cmd.erase( cmd.begin() + pos );
		return true;
	}
	return false;
}

bool Zone::RemoveCommand( ResetCommand *RC, bool purge )
{
	for(std::vector<ResetCommand *>::iterator rIter = cmd.begin();rIter != cmd.end();++rIter)
	{
		if( (*rIter) == RC )
		{
			if( purge ) delete (*rIter);
			cmd.erase(rIter);
			return true;
		}
	}
	return false;
}

/* execute the reset command table of a given zone */
void Zone::Reset()
{
	int last_cmd = 0;
	unsigned int cmd_no;
	Character *mob = NULL, *fol = NULL;
	Object *obj = NULL, *obj_to;
	unsigned int room_vnum;
	int room_rnum;
	int mob_load = FALSE; /* ### */
	int obj_load = FALSE; /* ### */

	for (cmd_no = 0;cmd_no < cmd.size(); ++cmd_no)
	{
		if(obj)
			sprintf(obj->creator, "Loaded by normal reset in zone %d.", getVnum());
		if (this->cmd[cmd_no]->if_flag && !last_cmd && !mob_load && !obj_load)
			continue;

		if (!this->cmd[cmd_no]->if_flag)
		{ /* ### */
			mob_load = FALSE;
			obj_load = FALSE;
		}
		if( fol && fol->IsPurged() )
			fol = 0;
		switch (this->cmd[cmd_no]->command)
		{
			case 'M':			// read a mobile
				if (this->cmd[cmd_no]->arg2 >= MiscUtil::random(1, 100) &&
					(this->cmd[cmd_no]->arg4 == -1 || this->cmd[cmd_no]->arg4 > (int)CountMobsRoom(this->cmd[cmd_no]->arg1, World[this->cmd[cmd_no]->arg3])) &&
					(this->cmd[cmd_no]->arg5 == -1 || this->cmd[cmd_no]->arg5 > (int)CountMobs(this->cmd[cmd_no]->arg1)) &&
					(this->cmd[cmd_no]->arg6 || !this->cmd[cmd_no]->mob))
				{
					// Serai - Sets the previously loaded mob to be the leader
					//  And when loading a mob with the defualt arg7 (-1) it kills fol.
					if (this->cmd[cmd_no]->arg7 == 1 && fol == NULL)
						fol = mob;
					else if (this->cmd[cmd_no]->arg7 != 1)
						fol = NULL;

				//MobClock.On();
					mob = new Character(this->cmd[cmd_no]->arg1, REAL, false);
				//MobClock.Off();
					mob->MoveToRoom(World[this->cmd[cmd_no]->arg3]);
					this->cmd[cmd_no]->mob = mob;
					last_cmd = 1;
					mob_load = TRUE;

					// It's not possible to get a loop.
					if (this->cmd[cmd_no]->arg7 == 1 && fol && !fol->IsPurged())
					{
						add_follower(mob, fol);
						perform_group(fol, mob);
					}
#ifdef KINSLAYER_JAVASCRIPT
					if( !mob->IsPurged() ) {
						js_load_triggers(mob);
					}
#endif
				}
				else
				{
					if (this->cmd[cmd_no]->mob)
						mob = this->cmd[cmd_no]->mob;
					else
						last_cmd = 0;
				}
				break;

			case 'O':			/* read an object */
				if (this->cmd[cmd_no]->arg2 >= MiscUtil::random(1, 100) &&
				(CountObjectsRoom(cmd[cmd_no]->arg1, cmd[cmd_no]->arg3) < this->cmd[cmd_no]->arg4 || cmd[cmd_no]->arg4 == -1)&&
				(this->cmd[cmd_no]->arg5 == -1 || CountObjects(this->cmd[cmd_no]->arg1) < (unsigned int)cmd[cmd_no]->arg5) &&
				(this->cmd[cmd_no]->arg6 || !cmd[cmd_no]->obj) && (obj_proto[this->cmd[cmd_no]->arg1]->Max == -1 ||
				ItemCount[this->cmd[cmd_no]->arg1] < obj_proto[this->cmd[cmd_no]->arg1]->Max))
				{
					if (cmd[cmd_no]->arg3 >= 0)
					{
						obj = read_object(cmd[cmd_no]->arg1, REAL, true);
						sprintf(obj->creator, "zone load -ground- to room %d", World[cmd[cmd_no]->arg3]->vnum);
						obj->MoveToRoom(cmd[cmd_no]->arg3);

						if(IS_OBJ_STAT(obj, ITEM_CHEST))
							obj->loadItems();
#ifdef KINSLAYER_JAVASCRIPT
					if( !obj->IsPurged() ) {
						js_load_triggers(obj);
					}
#endif
						last_cmd = 1;
						obj_load = TRUE;
					}

					else
					{
						obj = read_object(cmd[cmd_no]->arg1, REAL, true);
						obj->in_room = 0;
						last_cmd = 1;
						obj_load = TRUE;
					}
					cmd[cmd_no]->obj = obj;
				}

				else
					last_cmd = 0;
				break;

			case 'P':			// object to object
				if (cmd[cmd_no]->arg2 >= MiscUtil::random(1, 100) && (obj_proto[cmd[cmd_no]->arg1]->Max == -1 ||
				ItemCount[cmd[cmd_no]->arg1] < obj_proto[cmd[cmd_no]->arg1]->Max))
				{
					if( cmd[cmd_no]->arg4 < 0 || cmd[cmd_no]->arg4 >= World.size() )
						break;

					//Scan the current room to find the container.
					Room *room = World[cmd[cmd_no]->arg4];
					for(obj_to = room->contents;obj_to && obj_to->item_number != cmd[cmd_no]->arg3;obj_to = obj_to->next_content);

					if( !obj_to )
						break;

					// If weight in container + weight of item to put in > max weight, we terminate this operation.
					if( (obj_to->Weight() - obj_to->GetTotalWeight() + (obj_proto[cmd[cmd_no]->arg1])->GetTotalWeight())
						> obj_to->GetTotalVal0())
					{
						break;
					}
					obj = read_object(cmd[cmd_no]->arg1, REAL, true);

					obj_to_obj(obj, obj_to);
#ifdef KINSLAYER_JAVASCRIPT
					if( !obj->IsPurged() ) {
						js_load_triggers(obj);
					}
#endif
					last_cmd = 1;
				}
				else
					last_cmd = 0;
				break;
			case 'G':			/* obj_to_char ### */
				if (!mob)
				{
					if (cmd[cmd_no]->if_flag)
						this->LogError(cmd_no, "attempt to give obj to non-existant mob");
					/*
					 *  FYI: don't set the if-flag on 'G' commands if you want the command to execute even
					 *   if the mob wasn't loaded by a preceding 'M' ZCMD.  It can take it as long as the mob
					 *   doesn't die, or if it is purged in any way it is reloaded before here.
					 *     - Serai
					 */
					break;
				}

				// Serai - This added for more control on loading obj's to mobs
				if (cmd[cmd_no]->arg2 >= MiscUtil::random(1, 100)
				&& CountObjectsInv(cmd[cmd_no]->arg1, mob) < cmd[cmd_no]->arg3 && (obj_proto[cmd[cmd_no]->arg1]->Max == -1
				|| ItemCount[cmd[cmd_no]->arg1] < obj_proto[cmd[cmd_no]->arg1]->Max))
				{
					obj = read_object(cmd[cmd_no]->arg1, REAL, true);
					sprintf(obj->creator, "zone load -inventory- on mob %d", mob->nr);
					obj_to_char(obj, mob);
#ifdef KINSLAYER_JAVASCRIPT
					if( !obj->IsPurged() ) {
						js_load_triggers(obj);
					}
#endif
					last_cmd = 1;
				}
				else
					last_cmd = 0;

				break;

			case 'E':			/* object to equipment list ### */
				if (!mob)
				{
					this->LogError(cmd_no, "trying to equip non-existant mob");
					break;
				}

				if (cmd[cmd_no]->arg3 < 0 || cmd[cmd_no]->arg3 >= NUM_WEARS)
				{
					this->LogError(cmd_no, "invalid equipment pos number");
				}

				else
				{
					if(MiscUtil::random(1, 100) <= cmd[cmd_no]->arg2 && (obj_proto[cmd[cmd_no]->arg1]->Max == -1
					|| ItemCount[cmd[cmd_no]->arg1] < obj_proto[cmd[cmd_no]->arg1]->Max))
					{
						obj = read_object(cmd[cmd_no]->arg1, REAL, true);

						if(!GET_EQ(mob, cmd[cmd_no]->arg3))
						{
							sprintf(obj->creator, "zone load -equip- on mob %d", mob->nr);
							equip_char(mob, obj, cmd[cmd_no]->arg3);
#ifdef KINSLAYER_JAVASCRIPT
							if( !obj->IsPurged() ) {
								js_load_triggers(obj);
							}
#endif
							last_cmd = 1;
						}
					}
				}
				break;

			case 'R': /* rem obj from room */
				if ((obj = get_obj_in_list_num(cmd[cmd_no]->arg2, World[cmd[cmd_no]->arg1]->contents)) != NULL)
				{
					obj->RemoveFromRoom();
					obj->Extract(true);
				}
				obj = 0;

				last_cmd = 1;
				break;
			case 'D':			/* set state of door */
				if (cmd[cmd_no]->arg2 < 0 || cmd[cmd_no]->arg2 >= NUM_OF_DIRS ||
				(World[cmd[cmd_no]->arg1]->dir_option[cmd[cmd_no]->arg2] == NULL))
				{
					this->LogError(cmd_no, "door does not exist");
				}

				else
				{
					switch (cmd[cmd_no]->arg3)
					{
						case 0:
							World[cmd[cmd_no]->arg1]->RemoveDoorBit(cmd[cmd_no]->arg2, EX_LOCKED);
							World[cmd[cmd_no]->arg1]->RemoveDoorBit(cmd[cmd_no]->arg2, EX_CLOSED);
							break;
						case 1:
							World[cmd[cmd_no]->arg1]->SetDoorBit(cmd[cmd_no]->arg2, EX_CLOSED);
							World[cmd[cmd_no]->arg1]->RemoveDoorBit(cmd[cmd_no]->arg2, EX_LOCKED);
							break;
						case 2:
							World[cmd[cmd_no]->arg1]->SetDoorBit(cmd[cmd_no]->arg2, EX_LOCKED);
							World[cmd[cmd_no]->arg1]->SetDoorBit(cmd[cmd_no]->arg2, EX_CLOSED);
							break;
					}
				}

				last_cmd = 1;
				break;
			default:
				this->LogError(cmd_no, "unknown cmd in reset table; cmd disabled");
				//cmd[cmd_no]->command = '*';
				break;
		}
	}

	SetAge(0);

	// handle reset triggers
	room_vnum = GetBottom();
	while (room_vnum <= GetTop())
	{
		room_rnum = real_room(room_vnum);

		if (room_rnum != NOWHERE)
		{
#ifdef KINSLAYER_JAVASCRIPT
			js_zone_reset(World[room_rnum]);
#endif
		}
		++room_vnum;
	}
}

void Zone::Boot( const sql::Row &ZoneRow, std::list< sql::Row > &RowList )
{
	setName( ZoneRow["name"] );
	SetLifespan( atoi(ZoneRow["lifespan"].c_str()) );
	SetX( atoi(ZoneRow["x"].c_str()) );
	SetY( atoi(ZoneRow["y"].c_str()) );
	SetBottom( atoi(ZoneRow["bot"].c_str()) );
	SetTop( atoi(ZoneRow["top"].c_str()) );
	SetResetMode( atoi(ZoneRow["reset_mode"].c_str()) );
	SetSunrise( ZoneRow["sunrise"] );
	SetSunset( ZoneRow["sunset"] );
	SetBuilders( ZoneRow["builders"] );
	SetClosed( (bool)atoi(ZoneRow["closed"].c_str()) );

	this->cmd.resize(RowList.size());
	int i = 0;
	for(std::list<sql::Row>::iterator RowIter = RowList.begin();RowIter != RowList.end();++RowIter, ++i)
	{
		cmd[i] = new ResetCommand();
		cmd[i]->Boot( (*RowIter) );
	}
	this->inDB = true;
}
void ResetCommand::Boot( sql::Row &CmdRow )
{
	this->command = ( (CmdRow)["cmd"] )[0];
	this->dbID = atoi( (CmdRow)["id"].c_str() );
	this->if_flag = (bool)atoi( (CmdRow)["if_flag"].c_str() );
	this->arg1 = atoi( (CmdRow)["arg1"].c_str() );
	this->arg2 = atoi( (CmdRow)["arg2"].c_str() );
	this->arg3 = atoi( (CmdRow)["arg3"].c_str() );
	this->arg4 = atoi( (CmdRow)["arg4"].c_str() );
	this->arg5 = atoi( (CmdRow)["arg5"].c_str() );
	this->arg6 = atoi( (CmdRow)["arg6"].c_str() );
	this->arg7 = atoi( (CmdRow)["arg7"].c_str() );
}
void ZoneManager::SaveZones()
{
	for(unsigned int i = 0;i < ZoneList.size();++i)
	{
		ZoneList[i]->Save();
	}
}
void ZoneManager::Free()
{
	if( Self != NULL )
		delete Self;
}

void Zone::Save()
{
	std::stringstream Query, DeleteQuery;

	if( inDB )
		Query << "UPDATE zoneIndex SET name='" << sql::escapeString(Name) << "',lifespan='" << lifespan <<
		"',x='" << x << "',y='" << y << "',bot='" << bot << "',top='" << top << "',reset_mode='" << reset_mode <<
		"',sunrise='" << sql::escapeString(Sunrise) << "',sunset='" << sql::escapeString(Sunset) <<
		"',builders='" << sql::escapeString(Builders) << "',closed='" << (closed?1:0) << "' WHERE id='" << getVnum() << "';";
	else
		Query << "INSERT INTO zoneIndex (id,name,lifespan,x,y,bot,top,reset_mode,sunrise,sunset,builders,closed) VALUES("
			<< SQLVal(getVnum()) << SQLVal(sql::escapeString(Name)) << SQLVal(lifespan) << SQLVal(x) << SQLVal(y)
			<< SQLVal(bot) << SQLVal(top) << SQLVal(reset_mode) << SQLVal(sql::escapeString(Sunrise))
			<< SQLVal(sql::escapeString(Sunset)) << SQLVal(sql::escapeString(Builders)) << SQLVal(closed,true) << ");";
	try {
		gameDatabase->sendRawQuery(Query.str());
	} catch(sql::QueryException e) {
		MudLog(BRF, LVL_APPR, TRUE, "Unable to save zone #%d <%s>", getVnum(), e.message.c_str());
		Log("Errant query: %s", Query.str().c_str());
		return;
	}
	inDB = true;

	Query.str("");
	Query << "DELETE FROM zoneCommand WHERE zone_id='" << this->vnum << "';";
	gameDatabase->sendRawQuery(Query.str());

	/*********** Save the Zone Commands **************/
	for(unsigned int cmd_no=0;cmd_no < cmd.size();++cmd_no)
	{
		Query.str("");
		if( this->cmd[cmd_no]->command == '*' ) continue;
/*
		if( this->cmd[cmd_no]->altered && cmd[cmd_no]->dbID != -1 )//In the database, and altered, needs an UPDATE
		{
			Query << "UPDATE zoneCommand SET cmd='" << cmd[cmd_no]->command << "',if_flag='" << cmd[cmd_no]->if_flag
				<< "',arg1='" << cmd[cmd_no]->GetRealArg1() << "',arg2='" << cmd[cmd_no]->GetRealArg2() << "',arg3='"
				<< cmd[cmd_no]->GetRealArg3() << "',arg4='" << cmd[cmd_no]->GetRealArg4() << "',arg5='"
				<< cmd[cmd_no]->GetRealArg5() << "',arg6='" << cmd[cmd_no]->GetRealArg6() << "',arg7='"
				<< cmd[cmd_no]->GetRealArg7() << "' WHERE id='" << cmd[cmd_no]->dbID <<"';" << std::endl;
		}
		else if(this->cmd[cmd_no]->dbID == -1 )//Not in DB. Needs to be INSERTED
		{
*/
			Query << "INSERT INTO zoneCommand(zone_id,cmd,if_flag,arg1,arg2,arg3,arg4,arg5,arg6,arg7) VALUES("
				<< SQLVal(getVnum()) << SQLVal(cmd[cmd_no]->command) << SQLVal(cmd[cmd_no]->if_flag)
				<< SQLVal(cmd[cmd_no]->GetRealArg1()) << SQLVal(cmd[cmd_no]->GetRealArg2()) << SQLVal(cmd[cmd_no]->GetRealArg3())
				<< SQLVal(cmd[cmd_no]->GetRealArg4()) << SQLVal(cmd[cmd_no]->GetRealArg5()) << SQLVal(cmd[cmd_no]->GetRealArg6())
				<< SQLVal(cmd[cmd_no]->GetRealArg7(),true) << ");" << std::endl;
//		}
//		else if(this->cmd[cmd_no]->deleted == true && cmd[cmd_no]->dbID != -1)//Deletion set. Needs to be REMOVED
//		{
//			Query << "DELETE FROM zoneCommand WHERE id='" << cmd[cmd_no]->dbID << "';" << std::endl;
//		}
//		else
//			continue;
		try {
			gameDatabase->sendRawQuery(Query.str());
		} catch(sql::QueryException e) {
			MudLog(BRF, LVL_APPR, TRUE, "Unable to save zone #%d (COMMANDS) <%s>", getVnum(), e.message.c_str());
			Log("Errant query: %s", Query.str().c_str());
			continue;
		}
		cmd[cmd_no]->dbID = (int)gameDatabase->lastInsertID();
		cmd[cmd_no]->altered = false;
		if( cmd[cmd_no]->deleted ) cmd[cmd_no]->dbID = -1;
	}

	//Perform deletions for commands that no longer exist.
	for(unsigned int cmd_no = 0;cmd_no < cmd.size();++cmd_no)
	{
		if( !cmd_no )
			DeleteQuery << "DELETE FROM zoneCommand WHERE ";
		else
			DeleteQuery << " AND ";
		DeleteQuery << "id!='" << cmd[cmd_no]->dbID << "'";
	}
	if( DeleteQuery.str().size() )
	{
		DeleteQuery << " AND zone_id = '" << this->getVnum() << "';";
		try {
			gameDatabase->sendRawQuery(DeleteQuery.str());
		} catch(sql::QueryException e) {
			MudLog(BRF, LVL_APPR, TRUE, "Unable to save zone #%d (DELETING COMMANDS) <%s>", getVnum(), e.message.c_str());
			Log("Errant query: %s", Query.str().c_str());
			return;
		}
	}

	olc_remove_from_save_list(getVnum(), OLC_SAVE_ZONE);
}

void Zone::PrintToBuffer( std::string &Buffer )
{
	std::string TBuff;
	sprintf(TBuff, "%3d %-30.30s Age: %3d; Reset: %3d (%1d); Bot: %5d; Top: %5d; State: %s\r\n",
		getVnum(), getName().c_str(), GetAge(), GetLifespan(), GetResetMode(), GetBottom(), GetTop(),
		(IsClosed() ? "Closed" : "Open"));
	Buffer += TBuff;
}

ZoneManager *ZoneManager::Self = NULL;
ZoneManager::~ZoneManager()
{
	{
		boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
		for( unsigned int i = 0;i < ZoneList.size();++i )
		{
			delete ZoneList[i];
		}
	}
	{
		boost::recursive_mutex::scoped_lock lock( ZoneManager::SingletonMutex );
		Self = NULL;
	}
}
ZoneManager &ZoneManager::GetManager()
{
	boost::recursive_mutex::scoped_lock lock( ZoneManager::SingletonMutex );
	if( !Self ) Self = new ZoneManager();
	return (*Self);
}


Zone *ZoneManager::AddNewZone( const unsigned int vnum )
{
	boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
	std::vector< Zone * >::iterator zIter;
	for( zIter = ZoneList.begin();zIter != ZoneList.end();++zIter )
	{
		if( (*zIter)->getVnum() == vnum )
			return NULL;
		else if( (*zIter)->getVnum() > vnum ) //insert before...
			break;
	}
	//If we have made it this far, we have found an iterator after which point the zone must go.
	Zone *NewZone = new Zone( vnum );
	ZoneList.insert( zIter, NewZone );

	RenumberZones();//This will reset all of the rnums of the zones to their proper value.

	return NewZone;
}

Zone *ZoneManager::GetZoneByVnum( const unsigned int vnum )
{
	boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
	for(unsigned int i = 0;i < ZoneList.size();++i)
	{
		if( ZoneList[i]->getVnum() == vnum )
			return ZoneList[i];
	}
	return (0);
}
Zone *ZoneManager::GetZoneByRnum( const unsigned int rnum )
{
	boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
	return (rnum >= 0 && rnum < ZoneList.size()) ? (ZoneList[rnum]) : (NULL);
}
Zone *ZoneManager::GetZoneByRoomVnum( const unsigned int rvnum )
{
	boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
	for( unsigned int i = 0;i < ZoneList.size();++i )
	{
		if( rvnum >= ZoneList[i]->GetBottom() && rvnum <= ZoneList[i]->GetTop() )
			return ZoneList[i];
	}
	return NULL;
}
const size_t ZoneManager::NumZones()
{
	boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
	return ZoneList.size();
}
void ZoneManager::RenumberZones()
{
	boost::recursive_mutex::scoped_lock lock( this->ZoneListMutex );
	for(unsigned int i = 0;i < NumZones();++i)
		ZoneList[i]->SetRnum(i);
}
class ZonePassiveQueryReleaseJob : public Job
{
	sql::Query MyQuery, SecondQuery;
public:
	ZonePassiveQueryReleaseJob( sql::Query MyQuery, sql::Query SecondQuery )
	{
		this->MyQuery = MyQuery;
		this->SecondQuery = SecondQuery;
	}
	virtual void performRoutine()
	{
		this->MyQuery.reset();
		this->SecondQuery.reset();
	}
	virtual void performPostJobRoutine()
	{
	}
};
void ZoneManager::LoadThreadedZoneBatch( sql::Connection connection, const int zoneIndexOffset, const int zoneIndexFetchSize )
{
//	Clock ZoneBootClock;
//	ZoneBootClock.Reset(true);
	std::stringstream Query;
	sql::Query MyQuery, SecondQuery;

	try
	{
		sql::Row ZoneRow, CmdRow;
		Query << "SELECT * FROM zoneIndex ORDER BY id ASC LIMIT " << zoneIndexOffset << "," << zoneIndexFetchSize;
		MyQuery = connection->sendQuery(Query.str());

		if( MyQuery->numRows() > 0 )
		{
			int lowZoneID = atoi(MyQuery->peekRow()["id"].c_str());
			MyQuery->reverseRows();
			int highZoneID = atoi(MyQuery->peekRow()["id"].c_str());
			MyQuery->reverseRows();

			//Using this range, obtain the matching zone commands.
			Query.str("");
			Query << "SELECT * FROM zoneCommand WHERE zone_id BETWEEN " << lowZoneID << " AND " << highZoneID << " ORDER BY zone_id, id ASC";
			SecondQuery = connection->sendQuery(Query.str());

			std::list<sql::Row>::iterator rIter;
			while( MyQuery->hasNextRow() )
			{
				ZoneRow = MyQuery->getRow();

				std::list< sql::Row > RowList;
				while( SecondQuery->hasNextRow() )
				{
					CmdRow = SecondQuery->peekRow();
					if( ZoneRow["id"] == CmdRow["zone_id"] )//Command is for this zone.
					{
						RowList.push_back(CmdRow);
						SecondQuery->skipRow();//If and only if we find a match do we iterate.
					}
					else
						break;
				}

				//Now that we have a list of related commands, we just send them off the Zone::Boot
				//along with the zoneIndex row.
				Zone *zone = this->AddNewZone( atoi(ZoneRow["id"].c_str()) );
				zone->Boot( ZoneRow, RowList );
			}
		}
	} catch( sql::QueryException e ) {
		Log("Could not boot zones : %s", e.getMessage().c_str());
	}

	assert( MyQuery.use_count() == 1 );
	assert( SecondQuery.use_count() == 1 );

	Job *cleanupJob = new ZonePassiveQueryReleaseJob( MyQuery, SecondQuery );
	MyQuery.reset();
	SecondQuery.reset();
	ThreadedJobManager::get().addJob( cleanupJob );
	
//	ZoneBootClock.Off();
//	ZoneBootClock.Print();
}
void ZoneManager::BootZones()
{
	const int SIZE_OF_THREAD_POOL = 4;
	int zoneIndexTableSize = 0;
	sql::Query query;
	std::list< boost::thread* > threadPool;

	query = gameDatabase->sendQuery("SELECT COUNT(*) AS size FROM zoneIndex;");
	zoneIndexTableSize = atoi(query->getRow()[ "size" ].c_str());

	int zoneIndexOffset = 0;
	int zoneIndexFetchSize = zoneIndexTableSize / SIZE_OF_THREAD_POOL + 1;

	//We need to divide the work up.
	for(int threadNumber = 0;threadNumber < SIZE_OF_THREAD_POOL;++threadNumber)
	{
		sql::Connection connection = dbContext->createConnection();
		threadPool.push_back( new boost::thread( boost::bind( &ZoneManager::LoadThreadedZoneBatch, this, connection, zoneIndexOffset, zoneIndexFetchSize ) ) );

		zoneIndexOffset += zoneIndexFetchSize;
	}
	while( threadPool.empty() == false )
	{
		threadPool.front()->join();
		delete threadPool.front();
		threadPool.pop_front();
	}

	Zone *zoneBefore = NULL;
	Zone *zone = NULL;
	//Validate the zone table.
	for(size_t zoneIndex = 1;zoneIndex < ZoneManager::GetManager().NumZones();++zoneIndex)
	{
		if( zoneBefore == NULL )
			zoneBefore = ZoneManager::GetManager().GetZoneByRnum( zoneIndex - 1 );
		else
			zoneBefore = zone;
		zone = ZoneManager::GetManager().GetZoneByRnum( zoneIndex );
		assert( zoneBefore->getVnum() < zone->getVnum() );
	}
}

void Zone::LogError( int cmd_no, const char *message )
{
    int *room_var = 0;

    switch(this->cmd[cmd_no]->command)
    {
        case 'M':
        case 'O':
            room_var = &this->cmd[cmd_no]->arg3;
            break;
        case 'D':
            room_var = &this->cmd[cmd_no]->arg1;
            break;
    }

    std::stringstream RoomStream;

    if(room_var && (*room_var) != -1)
        RoomStream << ", room " << World[*room_var]->vnum;

	MudLog(NRM, LVL_GOD, TRUE, "SYSERR: zone file: %s", message);
	MudLog(NRM, LVL_GOD, TRUE,
	       "SYSERR: ...offending cmd: '%c' cmd in zone #%d, line %d, room %s",
	       this->cmd[cmd_no]->command, this->getVnum(), this->cmd[cmd_no]->line,
	       RoomStream.str().c_str());
}


room_vnum Zone::FindUnusedRoomVnum()
{
	room_vnum vnum = GetBottom();
	room_rnum rnum = real_room(vnum);

	if (rnum == NOWHERE)
		return vnum;
	for(;;)
	{
		if (vnum > (room_rnum)GetTop())
			return(NOWHERE);
		if (rnum >= (room_rnum)World.size() || World[rnum]->vnum > vnum)
			break;
		++rnum;
		++vnum;
	}

	return(vnum);
}
void Zone::Extract()
{
	if(weather)
		delete weather;
}

bool Zone::CanEdit( std::string &Name )
{
	return ( is_name( Name.c_str(), Builders.c_str() ) );
}
bool Zone::CanEdit( Character *Editor )
{
	return (GET_LEVEL(Editor) >= LVL_APPR || CanEdit( Editor->player.name ));
}

ResetCommand::ResetCommand()
{
	this->arg1 = 0;
	this->arg2 = 0;
	this->arg3 = 0;
	this->arg4 = 0;
	this->arg5 = 0;
	this->arg6 = 0;
	this->arg7 = 0;
	this->command = '\0';
	this->if_flag = 0;
	this->line    = 0;
	this->mob     = 0;
	this->obj	  = 0;
	this->dbID = -1;
	this->altered = false;
	this->deleted = false;
}
ResetCommand::ResetCommand(ResetCommand *Source)
{
	this->arg1 = Source->arg1;
	this->arg2 = Source->arg2;
	this->arg3 = Source->arg3;
	this->arg4 = Source->arg4;
	this->arg5 = Source->arg5;
	this->arg6 = Source->arg6;
	this->arg7 = Source->arg7;
	this->command = Source->command;
	this->if_flag = Source->if_flag;
	this->line    = Source->line;
	this->mob     = Source->mob;
	this->obj	  = Source->obj;
	this->dbID = Source->dbID;
	this->altered = Source->altered;
	this->deleted = Source->deleted;
}
int ResetCommand::GetRealArg1()
{
	if( command == 'M' ) return MobManager::GetManager().GetIndex(arg1)->vnum;
	if( command == 'O' ) return obj_index[arg1].vnum;
	if( command == 'G' ) return obj_index[arg1].vnum;
	if( command == 'E' ) return obj_index[arg1].vnum;
	if( command == 'P' ) return obj_index[arg1].vnum;
	if( command == 'D' ) return World[arg1]->vnum;
	if( command == 'R' ) return World[arg1]->vnum;
	return arg1;
}
int ResetCommand::GetRealArg2()
{
	if( command == 'R' ) return obj_index[arg2].vnum;
	return arg2;
}
int ResetCommand::GetRealArg3()
{
	if( command == 'P' ) return obj_index[arg3].vnum;
	if( command == 'O' ) return World[arg3]->vnum;
	if( command == 'M' ) return World[arg3]->vnum;
	return arg3;
}
int ResetCommand::GetRealArg4()
{
	if( command == 'P' ) return World[arg4]->vnum;
	return arg4;
}
int ResetCommand::GetRealArg5()
{
	return arg5;
}
int ResetCommand::GetRealArg6()
{
	return arg6;
}
int ResetCommand::GetRealArg7()
{
	if( command == 'M' ) return (arg7);
	return arg7;
}

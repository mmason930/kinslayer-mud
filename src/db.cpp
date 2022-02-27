/*
************************************************************************
*   File: db.c                                          Part of CircleMUD *
*  Usage: Loading/saving chars, booting/resetting world, internal funcs   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on Diku, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __DB_C__

#include "conf.h"


#include "structs.h"
#include "olc/olc.h"
#include "weather.h"
#include "stats.h"
#include "weaves.h"
#include "poker.h"
#include "mobs.h"
#include "auction.h"
#include "dg_event.h"
#include "md5.h"
#include "threaded_jobs.h"
#include "DatabaseUtil.h"
#include "Descriptor.h"
#include "UserDisabledCommand.h"
#include "CharacterUtil.h"
#include "ClanUtil.h"
#include "MailUtil.h"
#include "EntityType.h"
#include "ObjectMoveLogger.h"
#include "MobLoadLogger.h"
#include "Game.h"
#include "rooms/Room.h"
#include "rooms/Exit.h"

#include "js/js.h"

#include "StringUtil.h"

#include "guilds/GuildUtil.h"


boost::uuids::uuid u;

std::list< std::pair< Character*, event_info* > * > BashStandQueue;
std::list< Character * > CharPurgeList;
std::list< Object *    > ObjPurgeList;
std::thread *objectMoveLoggerThread;
ObjectMoveLogger objectMoveLogger;


/**************************************************************************
*  declarations of most of the 'global' variables                         *
**************************************************************************/

int top_of_objt = 0;		/* top of object index table	 */
int circle_restrict = 0;	/* level of game restriction	 */
int top_of_helpt = 0;		/* top of help index table	 */
long long item_loads = 0;	/* Number of items loaded this reboot. Used with ID Keys. */
int Character::top_mob_id = -2; /* First MOB id(counts downward) */

extern int gamePort;

Clock TheClock, TheClock2, TheClock3, TheClock4, TheClock5, TheClock6, TheClock7, TheClock8, TheClock9;

class Wizard *wizlist =	NULL;
std::list<Warrant *>		Warrants;
std::list<Legend *>			Legends;
std::list<std::string>	   *Tips;
std::vector<Room *>			World;
std::vector<Character *>	MobProto;
std::vector<Index *>		MobIndex;
Character *character_list = NULL;				// global linked list of chars
Object *object_list = NULL;						// global linked list of objs
Index *obj_index;								// index table for object file
std::vector<Object*> obj_proto;				//Object prototypes
MeleeMessageList fight_messages[MAX_MESSAGES];	// fighting messages
std::list<PlayerIndex *> PlayerTable;
GameTime time_info;								// the infomation about the time
reset_q_type reset_q;							// queue of zones to be reset
GameTime *mud_time_passed(time_t t2, time_t t1);
Object *chest_head = NULL;					// Chest Log List
std::vector<int> ItemCount;
std::vector<std::string> MySQLTables;
std::shared_ptr<std::vector<JSTrigger*> > globalJS_Scripts;
class Config *Conf;
void BootKits();
boost::uuids::random_generator Object::uuidGenerator = boost::uuids::random_generator();

std::string DebugFileName;

int Character::nr_alloc = 0;
int Character::nr_dealloc = 0;
int Object::nr_alloc = 0;
int Object::nr_dealloc = 0;
int Room::nr_alloc = 0;
int Room::nr_dealloc = 0;

sql::Context dbContext;
sql::Connection gameDatabase;

/***** Configuration Combat *****/
Bash	BashData;
Charge	ChargeData;
Melee	MeleeData;
Taint	TaintData;
Flee    FleeData;
PreciseStrike PreciseStrikeData;

/********************************/

time_t boot_time = 0;			/* time of mud boot					*/

char *credits		= NULL;		/* game credits						*/
char *motd			= NULL;		/* message of the day - mortals		*/
char *imotd			= NULL;		/* message of the day - immorts		*/
char *background	= NULL;		/* background story					*/
char *startup		= NULL;		/* startup screen					*/

std::vector<int> BodyPercents;

/* local functions */
int file_to_string(const char *name, char *buf);
int is_zone_empty(int zone_nr);
void assign_mobiles(void);
void assign_objects(void);
void assign_rooms(void);
void assign_the_shopkeepers(void);
void buildPlayerIndex(void);
void destroyPlayerIndex(void);
void bootWorld(void);
void renumberRoomExits(const std::map<Room *, std::map<int, int>> &roomToExitToVnumMap);
void renum_zone_table(void);
void add_follower(Character * ch, Character * leader);
int perform_group( Character *ch, Character *vict );
void reset_time(void);
void SetupItemCount();
void PlayerFileCycle();
int BootObjects();
void startGameSession();

// external functions
extern void load_messages(void);
extern int level_exp(int level);
void boot_the_shops();

// external vars
extern Descriptor *descriptor_list;

const int READ_SIZE = 256;

/*************************************************************************
*  routines for booting the system                                       *
*************************************************************************/

//Miscellaneous code to be executed right after the MySQL connection is established.
void miscBootMaintenance() {

	std::string sql = DatabaseUtil::getSingleResultFromQuery(gameDatabase, "SELECT value FROM config WHERE name='Boot_SQL'");

	if(sql.empty() == false) {

		std::vector<std::string> queries = StringUtil::splitToVector(sql, ';');

		for(std::string::size_type index = 0;index < queries.size();++index) {

			try {

				gameDatabase->sendRawQuery(queries[index]);
			}
			catch(sql::QueryException &e) {

				Log("miscBootMaintenance() : Could not execute query : %s", e.getMessage().c_str());
				exit(1);
			}
		}
	}

	gameDatabase->sendRawQuery("UPDATE config SET value='' WHERE name='Boot_SQL'");

	//Misc code below.
}

class LiveObjectMaintenanceJob : public Job
{
	sql::Connection connection;
	Clock MyClock;
	int numberOfItemsDeleted;
public:
	LiveObjectMaintenanceJob( sql::Connection connection )
	{
		this->numberOfItemsDeleted = 0;
		this->connection = connection;
	}
	void performRoutine()
	{
		MyClock.turnOn();
		std::stringstream sBuffer;
		try {
			connection->sendRawQuery("DROP TABLE IF EXISTS `tempObjectsPurgeQueue`");

			//Create the temporary table to store all items that will be deleted.
			sBuffer << "CREATE TABLE `tempObjectsPurgeQueue` ("
				<< "`object_id` VARCHAR( 36 ) NOT NULL ,"
				<< " PRIMARY KEY ( `object_id` )"
				<< ") ENGINE = MYISAM ;";
			connection->sendRawQuery( sBuffer.str() );

			//Toss in the items no longer held by their former owner.
			sBuffer.str("");
			sBuffer << "INSERT INTO `tempObjectsPurgeQueue`"
				<< " (`object_id`)"
				<< " SELECT id FROM objects "
				<< " WHERE holder_type='P'"
				<< " AND holder_id NOT IN(SELECT user_id FROM users);";
			connection->sendRawQuery( sBuffer.str() );

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//TODO: Redo this.
			sql::Query aQuery = connection->sendQuery("SELECT ai.* FROM auctionItem ai JOIN objects o ON o.id=ai.object_id WHERE ai.owner_id NOT IN(SELECT user_id FROM users);");

			while( aQuery->hasNextRow() )
			{
				sql::Row MyRow = aQuery->getRow();
				sBuffer << "DELETE FROM objects WHERE id='" << MyRow["object_id"] << "';";

				connection->sendRawQuery( sBuffer.str() );

				sBuffer.str("");
				sBuffer << "UPDATE auctionItem SET active='0' WHERE id='" << MyRow["id"] << "';";
				connection->sendRawQuery( sBuffer.str() );
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool firstPass = true;
			int objectsPurgeQueueSizeBefore = 0, objectsPurgeQueueSizeAfter = 0;
			do
			{
				objectsPurgeQueueSizeBefore = objectsPurgeQueueSizeAfter;
				sBuffer.str("");
				sBuffer		<< "INSERT INTO `tempObjectsPurgeQueue` (`object_id`)"
							<< " SELECT object.id"
							<< " FROM objects object";
				if( firstPass == true ) {
					sBuffer	<< " LEFT JOIN objects containerInObjects ON object.holder_id = containerInObjects.id";
				}
					sBuffer	<< " LEFT JOIN tempObjectsPurgeQueue containerInTemp ON object.holder_id = containerInTemp.object_id"
							<< " LEFT JOIN tempObjectsPurgeQueue objectInTemp ON object.id = objectInTemp.object_id"
							<< " WHERE object.holder_type = 'O' AND";
				if( firstPass == true ) {
					sBuffer	<< " containerInObjects.id IS NULL OR";
				}
					sBuffer	<< "("
							<< "        containerInTemp.object_id IS NOT NULL"
							<< "    AND objectInTemp.object_id IS NULL"
							<< ")";

				connection->sendRawQuery( sBuffer.str() );
				firstPass = false;

				sBuffer.str("");
				sBuffer << "SELECT COUNT(*) AS size FROM `tempObjectsPurgeQueue`";

				sql::Query tableSizeQuery = connection->sendQuery( sBuffer.str() );

				objectsPurgeQueueSizeAfter = atoi(tableSizeQuery->getRow()["size"].c_str());

			} while( objectsPurgeQueueSizeBefore != objectsPurgeQueueSizeAfter );//If deleted, run again.

			connection->sendRawQuery("DELETE FROM objects WHERE id IN(SELECT object_id FROM tempObjectsPurgeQueue);");
			connection->sendRawQuery("DELETE FROM object_specials WHERE id NOT IN(SELECT id FROM objects);");
			connection->sendRawQuery("DELETE FROM object_retools WHERE id NOT IN(SELECT id FROM objects);");
			this->numberOfItemsDeleted = objectsPurgeQueueSizeAfter;
		} catch( sql::QueryException &e ) {
			e.report();
		}

		try {
			connection->sendRawQuery("DROP TABLE IF EXISTS `tempObjectsPurgeQueue`");
		}
		catch( sql::QueryException &e ) {
			e.report();
		}
		MyClock.turnOff();
	}
	void performPostJobRoutine()
	{
		Log("Live object maintenance queries have run to completion. Seconds elapsed: %f. Items deleted: %d.", MyClock.getSeconds(), numberOfItemsDeleted);
	}
};

CommandHandler do_reboot = DEFINE_COMMAND
{
	int i;
	char arg[MAX_INPUT_LENGTH];

	OneArgument(argument, arg);

	if (!str_cmp(arg, "all") || *arg == '*')
	{
		BanManager::GetManager().Reload();

		MobManager::GetManager().Reload();
		
		destroyPlayerIndex();
		buildPlayerIndex();
	}
	else if (!str_cmp(arg, "mobs"))
		MobManager::GetManager().Reload();
	else if (!str_cmp(arg, "playerindex"))
	{
		destroyPlayerIndex();
		buildPlayerIndex();
	}
	else if (!str_cmp(arg, "bans"))
		BanManager::GetManager().Reload();
	else if(!str_cmp(arg, "screentexts"))
		loadScreenText();
	else
	{
		ch->send("Unknown reload option. Available options: `all`, `mobs`, `playerindex`, `bans`, `screentexts`\r\n");
		return;
	}

	ch->send(OK);
};

/*** Galnor 01/24/2010 - Load all global JavaScripts into the MUD ***/
void BootGlobalScripts()
{
	globalJS_Scripts = std::shared_ptr< std::vector<JSTrigger*> >(new std::vector<JSTrigger*>);
	sql::Query MyQuery;

	try {
		MyQuery = gameDatabase->sendQuery("SELECT * FROM js_attachments WHERE type='G';");
	} catch( sql::QueryException &e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error booting global JavaScripts : %s", e.getMessage().c_str());
		return;
	}

	while( MyQuery->hasNextRow() )
	{
		sql::Row MyRow = MyQuery->getRow();
		int vnum = atoi(MyRow["script_vnum"].c_str());
		if( JSManager::get()->triggerExists( vnum ) ) {
			JSTrigger *t = JSManager::get()->getTrigger( vnum );
			if( t ) {
				globalJS_Scripts->push_back(t);
			}
		}
	}
}
/*** Galnor 01/24/2010 - Save all global JavaScripts to the MUD database ***/
void SaveGlobalScripts()
{
	try
	{
		//Clear out all of the JS attachments that are on the global scope.
		gameDatabase->sendRawQuery("DELETE FROM js_attachments WHERE type='G';");
		//Now we will cycle through and add back those that still remain global.
		for(unsigned int i = 0;i < globalJS_Scripts->size();++i)
		{
			gameDatabase->sendRawQuery("INSERT INTO js_attachments(`type`,`script_vnum`) VALUES('G','"
				+ MiscUtil::convert<std::string>(globalJS_Scripts->at(i)->vnum) + "');");
		}
	} catch( sql::QueryException &e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error saving global JavaScripts : %s", e.getMessage().c_str());
	}
}

void bootWorld(void)
{
	std::map<Room *, std::map<int, int>> roomToExitToVnumMap;

	Log("Loading zone table.");
	ZoneManager::GetManager().BootZones();

	Log("Loading objects...");
	BootObjects();

	Log("Loading rooms.");
	Room::bootWorld(roomToExitToVnumMap);

	Log("Loading global scripts.");
	BootGlobalScripts();

	objectMoveLoggerThread = new std::thread(&ObjectMoveLogger::threadHandler, &objectMoveLogger);
	mobLoadLoggerThread = new std::thread(&MobLoadLogger::threadHandler, &mobLoadLogger);
	
	Log("Renumbering rooms.");
	Room::renumberRoomExits(roomToExitToVnumMap);

	Log("Loading kits and generating index.");
	BootKits();
	
	Log("Loading mobs and generating index.");
	MobManager::GetManager().BootPrototypes();

	Log("Loading auctions houses from database.");
	AuctionManager::GetManager().Boot();

	Log("Renumbering zone table.");
	renum_zone_table();

	Log("Loading shops.");
	boot_the_shops();
}

void SetupMySQL( bool crash_on_failure )
{
	std::string username, password, dbname, hostname;

	username = game->getBasicConfigValue("MySQL Username");
	password = game->getBasicConfigValue("MySQL Password");
	dbname = game->getBasicConfigValue("MySQL Database");
	hostname = game->getBasicConfigValue("MySQL Hostname");

	try
	{
		dbContext = sql::createContext(hostname, username, password, dbname);
		gameDatabase = dbContext->createConnection();

		game->setContext(dbContext);
		game->setConnection(gameDatabase);
	}
	catch (sql::ConnectionException &e) {
		e.report();
		if( crash_on_failure == true )
			exit(1);
	}
}

void loadScreenText()
{
	sql::Query query = gameDatabase->sendQuery("SELECT * FROM screenText");

	while(query->hasNextRow()) {

		sql::Row row = query->getRow();

		int screenTextId = row.getInt("id");
		std::string text = row.getString("text");

		if(screenTextId == 1) {

			if(motd)
				delete[] motd;
			motd = str_dup(text.c_str());
		}
		else if(screenTextId == 2) {

			if(imotd)
				delete[] imotd;
			imotd = str_dup(text.c_str());
		}
		else if(screenTextId == 3) {

			if(startup)
				delete startup;
			startup = str_dup(text.c_str());
		}
		else if(screenTextId == 4) {

			if(credits)
				delete[] credits;
			credits = str_dup(text.c_str());
		}
	}
}

Clock MobClock, ObjClock, EqClock;
/* body of the booting system */
void boot_db(void)
{
    Clock BootClock;
    BootClock.reset( true );
	int i;

	Poker::Setup();

	Log("Boot db -- BEGIN.");

	Log("Establishing MySQL Connection.");
	SetupMySQL( true );

	Log("Setting Up Editor Interfaces.");
	game->setupEditorInterfaces();
	
	Log("Booting the Configuration.");
	Conf = new Config();
	Conf->Load();
	Conf->save(); //Need to update the reboot count.

	Log("Running Boot Maintenance Queries...");
	miscBootMaintenance();

	Log("Starting game session...");
	startGameSession();

    Log("Booting JS Triggers");
    // get() forces the ctoring
    JSManager* temp = JSManager::get();

	temp->loadScriptsFromFile(std::string("scripts/lib/util/LoDash-2.4.1.js"));

	Log("Monitoring file modifications...");
	temp->monitorFileModifications(false);

	Log("Loading scripts from filesystem...");
	temp->loadScriptsFromFilesystem("scripts", false);
	
	Log("Processing Script imports...");
	temp->processScriptImports();
	
	Log("Setting up monitoring threads...");
	temp->setupMonitoringThreads();

	Log("Loading triggers...");
	temp->loadTriggers();

	Log("Loading script map...");
	temp->loadScriptMap();

	JSManager::get()->executeExpression("initGlobals();");
	JSManager::get()->executeExpression("bootProcs();");

	Log("Booting Warrants.");
	BootWarrants();

	Log("Booting Clans.");
	BootClans();

	Log("Loading Email.");
	MailUtil::loadConfig(game);

	Log("Booting the Weaves.");
	WeaveManager::GetManager().LoadWeaves();

	Log("Resetting the game time.");
	reset_time();

	Log("Booting StatGroups.");
	StatManager::GetManager().Boot();

	Log("Reading screen text.");
	loadScreenText();
		
	Log("Loading guilds.");
	GuildUtil::get()->loadGuildsFromDatabase(game->getConnection());
	GuildUtil::get()->loadGuildApplicationsFromDatabase(game->getConnection());
	GuildUtil::get()->loadGuildApplicationSignaturesFromDatabase(game->getConnection());
	GuildUtil::get()->loadUserGuildsFromDatabase(game->getConnection());
	GuildUtil::get()->loadGuildJoinApplicationsFromDatabase(game->getConnection());

	bootWorld();

	Log("Renumbering start rooms.");
	Conf->RenumberRooms();

	Log("Generating player index.");
	buildPlayerIndex();

	Log("Cycling through player files.");
	PlayerFileCycle();

	Log("Loading fight messages.");
	load_messages();

	Log("Loading social messages.");
	CommandUtil::get()->bootSocials(game->getConnection());
	CommandUtil::get()->createCommandList();
	CommandUtil::get()->sortCommands();

	Log("Assigning function pointers:");

	Log("   Mobiles.");
	assign_mobiles();
	Log("   Shopkeepers.");
	assign_the_shopkeepers();
	Log("   Objects.");
	assign_objects();
	Log("   Rooms.");
	assign_rooms();

	Log("Sorting command list and spells.");
	CommandUtil::get()->sortCommands();

	Log("Reading banned site and invalid name list.");
	BanManager::GetManager().Boot();

	Log("Counting file-stored items.");
	SetupItemCount();

	//TheClock.Reset(true);
	Zone *zone;
	if (Conf->empty_world == false && game->getBasicConfigValue("Dev Mode") == "0")
	{
		for (i = 0; (zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL; ++i)
		{
			Log("Resetting %s (rooms %d-%d).", zone->getName().c_str(), zone->GetBottom(), zone->GetTop());
			zone->Reset();
		}
	}
	reset_q.head = reset_q.tail = NULL;
	boot_time = time(0);

	BootClock.turnOff();
	Log("Boot db complete ( %.3f seconds ).", BootClock.getSeconds());
}


/* reset the time in the game from file */
void reset_time(void)
{
	Weather * weather_info = 0;
#if defined(CIRCLE_MACINTOSH)
	long beginning_of_time = -1561789232;
#else
	long beginning_of_time = 1074420331;
#endif

	time_info = *mud_time_passed(time(0), beginning_of_time);

	if (time_info.hours <= 4)
		Weather::setSun(SUN_DARK);
	else if (time_info.hours == 5)
		Weather::setSun(SUN_RISE);
	else if (time_info.hours <= 19)
		Weather::setSun(SUN_LIGHT);
	else if (time_info.hours == 20)
		Weather::setSun(SUN_SET);
	else
		Weather::setSun(SUN_DARK);

	Log("   Current Gametime: %dH %dD %dM %dY.", time_info.hours,
	    time_info.day, time_info.month, time_info.year);
}

void Character::createPlayerIndex()
{
	PlayerIndex *entry = new PlayerIndex();
	entry->name = this->player.name;
	entry->id   = this->player.idnum;
	PlayerTable.push_back(entry);
}

bool Character::ProperNameFormat( const std::string &Name )
{
	for(unsigned int i = 0;i < Name.size();++i)
	{
		if(!isalpha( Name[i] ))
			return false;
	}
	return true;
}

void buildPlayerIndex(void)
{
	PlayerIndex *entry;
	sql::Query Query;
	std::stringstream strQuery;

	try {Query = gameDatabase->sendQuery("SELECT username, user_id FROM users");}
	catch( sql::QueryException &e )
	{
		e.report();
		exit(1);
	}
	for(unsigned int i = 0;i < Query->numRows();++i)
	{
		sql::Row Row = Query->getRow();
		if( !Character::ProperNameFormat( Row["username"] ) )
		{
			MySQLDeleteAll( Row["username"] );
			continue;
		}
		entry = new PlayerIndex();
		entry->name = Row["username"];
		entry->id = atoi(Row["user_id"].c_str());

		PlayerTable.push_back(entry);
	}
}

void destroyPlayerIndex()
{
	for( std::list<PlayerIndex*>::iterator pIter = PlayerTable.begin();pIter != PlayerTable.end();++pIter )
	{
		delete (*pIter);
	}
	PlayerTable.clear();
}

//Seperate the filename and its extention name into two different strings.
void FileSlice(std::string Filename, std::string &Name, std::string &Extention)
{
	int i = 0, ReadPoint = 0;

	Name.erase();
	Extention.erase();

	//Find the last slash
	for(std::string::size_type i = 0;i < Filename.size();++i)
	{
		if(Filename[i] == *SLASH)
			ReadPoint = i + 1;
	}

	for(std::string::size_type i = ReadPoint;i < Filename.size();++i)
	{
		if(Filename[i] == '.')
			break;
		Name += Filename[i];
	}

	Extention.assign(Filename, ++i, Filename.size());
}

long asciiflag_conv(char *flag)
{
	long flags = 0;
	int IsNumber = 1;
	char *p;

	for (p = flag; *p; ++p)
	{
		if (islower(*p))
			flags |= 1 << (*p - 'a');

		else if (isupper(*p))
			flags |= 1 << (26 + (*p - 'A'));

		if (!isdigit(*p))
			IsNumber = 0;
	}

	if (IsNumber)
		flags = atol(flag);
	return flags;
}

char fread_letter(FILE *fp)
{
	char c;

	do
	{
		c = getc(fp);
	}

	while (isspace(c));

	return c;
}


#define ZCMD zone_table[zone].cmd[cmd_no]

/* resolve vnums into rnums in the zone reset tables */
void renum_zone_table(void)
{
	int a, b, c, olda, oldb, oldc;
	unsigned int cmd_no;
	char buf[128];

	Zone *zone;
	for(unsigned int i = 0;(zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;++i)
	{
		for (cmd_no = 0;cmd_no < zone->cmd.size(); cmd_no++)
		{
			a = b = c = 0;
			olda = zone->cmd[cmd_no]->arg1;
			oldb = zone->cmd[cmd_no]->arg2;
			oldc = zone->cmd[cmd_no]->arg3;

			switch (zone->cmd[cmd_no]->command)
			{
			case 'M':
				a = zone->cmd[cmd_no]->arg1 = MobManager::GetManager().RealMobile(zone->cmd[cmd_no]->arg1);
				c = zone->cmd[cmd_no]->arg3 = real_room(zone->cmd[cmd_no]->arg3);
				break;
			case 'O':
				a = zone->cmd[cmd_no]->arg1 = real_object(zone->cmd[cmd_no]->arg1);
				if (zone->cmd[cmd_no]->arg3 != NOWHERE)
					c = zone->cmd[cmd_no]->arg3 = real_room(zone->cmd[cmd_no]->arg3);
				break;
			case 'G':
				a = zone->cmd[cmd_no]->arg1 = real_object(zone->cmd[cmd_no]->arg1);
				break;
			case 'E':
				a = zone->cmd[cmd_no]->arg1 = real_object(zone->cmd[cmd_no]->arg1);
				break;
			case 'P':
				a = zone->cmd[cmd_no]->arg1 = real_object(zone->cmd[cmd_no]->arg1);
				c = zone->cmd[cmd_no]->arg3 = real_object(zone->cmd[cmd_no]->arg3);
				b = zone->cmd[cmd_no]->arg4 = real_room(zone->cmd[cmd_no]->arg4);
				break;
			case 'D':
				a = zone->cmd[cmd_no]->arg1 = real_room(zone->cmd[cmd_no]->arg1);
				break;
			case 'R': /* rem obj from room */
				a = zone->cmd[cmd_no]->arg1 = real_room(zone->cmd[cmd_no]->arg1);
				b = zone->cmd[cmd_no]->arg2 = real_object(zone->cmd[cmd_no]->arg2);
				break;
			}
			if (a < 0 || b < 0 || c < 0)
			{
				sprintf(buf,  "Invalid vnum %d, cmd disabled",
					(a < 0) ? olda : ((b < 0) ? oldb : oldc));
				zone->LogError(cmd_no, buf);
				zone->cmd.erase( zone->cmd.begin() + cmd_no );
				cmd_no--;

			//	zone->cmd[cmd_no]->command = '*';
			}

			else if (zone->cmd[cmd_no]->arg3 < 0)
				zone->cmd[cmd_no]->arg3 = 0;
		}
	}
}

/* Serai - added for the 'G' ZCMD in loading obj's to mobs */
int CountObjectsInv(int number, Character *target)
{
	Object *obj;
	int counter = 0;

	for(obj = target->carrying; obj; obj = obj->next_content)
	{
		if (GET_OBJ_RNUM(obj) == number)
			++counter; /* the prefix ++ is 'faster' than the suffix ++ :p */
	}

	return (counter);
}

/* Added by Galnor - Count objects inside of a room */
int CountObjectsRoom(int number, int room)
{
	Object *obj;
	int counter = 0;

	for(obj = World[room]->contents; obj;obj = obj->next_content)
	{
		if(GET_OBJ_RNUM(obj) == number)
			++counter;
	}

	return counter;
}

/* Added by Galnor - Count objects inside of the entire MUD. */
int CountMobsTotal(int mob_no)
{
	Character *mob;
	int counter = 0;

	for(mob = character_list; mob;mob = mob->next)
	{
		if(!mob->IsPurged() && GET_MOB_RNUM(mob) == mob_no)
			++counter;
	}

	return counter;
}

/* Added by Galnor - Count mobs inside of a room */
int CountMobsRoom(int mob_no, Room *room)
{
	static Character *mob;
	static int i;

	for(mob = room->people, i = 0; mob;mob = mob->next_in_room)
	{
		if(GET_MOB_RNUM(mob) == mob_no)
			++i;
	}

	return i;
}

std::string ConvertNumberToKey(const int number, const unsigned int min_digits)
{
	std::string str;
	for(int i = 0;pow((float)26, (float)i) - 1 <= number;++i)
	{
		str += (char) 'A' + ((number / (int)pow((float)26, (float)i)) % 26);
	}
	while(str.size() < min_digits)
	{
		str += 'A';
	}
	return str;
}

void SetupItemCount()
{
	sql::Query MyQuery;
	try {
		MyQuery = gameDatabase->sendQuery("SELECT vnum,COUNT(*) AS num FROM objects WHERE vnum >= 0 GROUP BY vnum;");
	} catch( sql::QueryException &e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to send query for item count(SetupItemCount()): %s",
			e.getMessage().c_str());
		return;
	}
	for(unsigned int i = 0;i < ItemCount.size();++i)
		ItemCount[i] = (0);
	while( MyQuery->hasNextRow() )
	{
		sql::Row MyRow = MyQuery->getRow();
		int count = atoi(MyRow["num"].c_str());
		if( count > 0 )
		{
			int rnum = real_object(atoi(MyRow["vnum"].c_str()));
			if( rnum > -1 )
				ItemCount[rnum] = (count);
		}
	}
}

#define Z	zone_table[zone]

/* Galnor 4-1-2005 Kit constructor. */
Kit::Kit()
{
	this->prev		= NULL;
	this->next		= NULL;
	this->vnum		= -1;
	this->deleted	= false;
	this->in_db		= false;
	this->AppendBlankKit();
}
void Kit::AppendBlankKit()
{
	this->KitItems.resize(this->KitItems.size()+1);
	this->KitItems[this->KitItems.size()-1].resize(NUM_WEARS);
}

#undef Z

int Character::VnumMobile(std::string SearchName)
{
	int found = 0, jsScriptVnum = 0;
	Character *Mob;

	if( HasParameter(SearchName, "js") )
	{
		jsScriptVnum = atoi(ParamValue(SearchName, "js").c_str());
	}

	for(unsigned int i = 0;(Mob = MobManager::GetManager().GetPrototype(i)) != NULL;++i)
	{
		if(jsScriptVnum != 0)
		{
			bool scriptFound = false;
			for(auto iter = Mob->js_scripts->begin();iter != Mob->js_scripts->end();++iter)
			{
				if((*iter)->vnum == jsScriptVnum)
				{
					scriptFound = true;
					break;
				}
			}

			if(!scriptFound)
				continue;
		}
		else if( !isname(SearchName, Mob->player.name) )
			continue;
		send("%3d. [%5d] %s\r\n", found++, MobManager::GetManager().GetIndex(i)->vnum, Mob->player.short_descr);
	}
	return (found);
}

int Character::VnumObject(std::string SearchName)
{
	int nr, found = 0;

	for (nr = 0; nr <= top_of_objt; ++nr)
	{
		if( !isname(SearchName, obj_proto[nr]->name) )
			continue;
		this->send("%3d. [%5d] %s\r\n", ++found, obj_index[nr].vnum, obj_proto[nr]->short_description);
	}
	return (found);
}
int Character::VnumKit(std::string SearchName)
{
	int nr, found = 0;
	std::list<Kit *> KitList = KitManager::GetManager().GetAllKits();
	for(std::list<Kit *>::iterator kIter = KitList.begin();kIter != KitList.end();++kIter) {
		if( isname(SearchName, (*kIter)->Name) )
			this->send("%3d. [%5d] %s\r\n", ++found, (*kIter)->vnum, (*kIter)->Name.c_str());
	}
	return (found);
}
int Character::VnumZone(std::string SearchName)
{
	int nr, found = 0;
	for(unsigned int i = 0;i < ZoneManager::GetManager().NumZones();++i) {
		Zone *z = ZoneManager::GetManager().GetZoneByRnum(i);
		if( isname(SearchName, (z)->getName()) )
			this->send("%3d. [%5d] %s\r\n", ++found, (z)->getVnum(), z->getName().c_str());
	}
	return (found);
}
int Character::VnumRoom(std::string SearchName)
{
	int found=0, exitVnum=-1;
	unsigned int nr;
	
	if( HasParameter(SearchName, "exit") )
	{
		exitVnum = atoi(ParamValue(SearchName,"exit").c_str());
	}
	for(nr = 0;nr < World.size();++nr)
	{
		if( (exitVnum != -1) )
		{
			/******* EXIT ********/
			bool exitFound = false;
			for(int i = 0;i < NUM_OF_DIRS;++i)
			{
				if( World[nr]->dir_option[i] && World[nr]->dir_option[i]->getToRoom() && World[nr]->dir_option[i]->getToRoom()->getVnum() == exitVnum)
				{
					exitFound=true;
					break;
				}
			}
			if( !exitFound ) continue;
			/************************/
		}
		else if( !isname(SearchName, World[nr]->getName()) )
			continue;

		this->send("%3d. [%5d] %s\r\n", ++found, World[nr]->getVnum(), World[nr]->getName());
	}
	return (found);
}


/* create an object, and add it to the object list */
Object *create_obj(bool incID)
{
	Object *obj;

	obj = new Object();
	obj->next = object_list;
	object_list = obj;

	if( incID ) {
		obj->objID = Object::uuidGenerator();
	}

	return obj;
}


/* create a new object from a prototype */
Object *read_object(int nr, int type, bool new_instance, bool increment_top_id)
{	
	Object *obj;
	int i;

	if (nr < 0)
	{
		Log("SYSERR: Trying to create obj with negative (%d) num!", nr);
		return NULL;
	}

	if (type == VIRTUAL)
	{
		if ((i = real_object(nr)) < 0)
		{
			Log("Object (V) %d does not exist in database.", nr);
			return NULL;
		}
	}
	else
		i = nr;

	obj = new Object();
	DateTime createdDatetime = obj->createdDatetime;

	*obj = *obj_proto[i];
	obj->proto = false;
	obj->createdDatetime = createdDatetime;
	obj->obj_flags.mModifiers = new std::map< sbyte,long double >();
	if( !obj_proto[i]->action_description || strlen(obj_proto[i]->action_description) == 0 )
		obj->action_description = NULL;

	obj->next = object_list;
	object_list = obj;

	++obj_index[i].number;

	// Serai - 09/17/05 - Needs to be [i], not [nr]
	if (new_instance)
		++ItemCount[i];
	if( increment_top_id )
		obj->objID = Object::uuidGenerator();		

	return obj;
}

const int ZO_DEAD = 999;

/* update zone ages, queue for reset if necessary, and dequeue when possible */
void zone_update(void)
{
	int i;
	struct reset_q_element *update_u, *temp;
	static int timer = 0;
	Zone *zone;

	/* jelson 10/22/92 */

	if (((++timer * PULSE_ZONE) / PASSES_PER_SEC) >= 60)
	{
		/* one minute has passed */
		/*
		* NOT accurate unless PULSE_ZONE is a multiple of PASSES_PER_SEC or a
		* factor of 60
		*/

		timer = 0;

		/* since one minute has passed, increment zone ages */
		for (i = 0; (zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL; ++i)
		{
			if (zone->GetAge() < zone->GetLifespan() && zone->GetResetMode())
				zone->SetAge( zone->GetAge() + 1 );

			if (zone->GetAge() >= zone->GetLifespan() && zone->GetAge() < ZO_DEAD && zone->GetResetMode())
			{
				/* enqueue zone */
				update_u = new struct reset_q_element;

				update_u->zone_to_reset = i;
				update_u->next = 0;

				if (!reset_q.head)
					reset_q.head = reset_q.tail = update_u;
				else
				{
					reset_q.tail->next = update_u;
					reset_q.tail = update_u;
				}

				zone->SetAge(ZO_DEAD);
			}
		}
	}	/* end - one minute has passed */

	/* dequeue zones (if possible) and reset */
	/* this code is executed every 10 seconds (i.e. PULSE_ZONE) */
	for (update_u = reset_q.head; update_u; update_u = update_u->next)
	{
		Zone *zone = ZoneManager::GetManager().GetZoneByRnum(update_u->zone_to_reset);
		if (zone->GetResetMode() == 2 || is_zone_empty(update_u->zone_to_reset))
		{
			zone->Reset();
			/* dequeue */

			if (update_u == reset_q.head)
				reset_q.head = reset_q.head->next;
			else
			{
				for (temp = reset_q.head; temp->next != update_u; temp = temp->next);
				if (!update_u->next)
					reset_q.tail = temp;

				temp->next = update_u->next;
			}

			delete(update_u);
			break;
		}
	}
}

/* for use in reset_zone; return TRUE if zone 'nr' is free of PC's  */
int is_zone_empty(int zone_nr)
{
	Descriptor *i;

	for (i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) == CON_PLAYING)
		{
			if (i->character->in_room->getZoneNumber() == zone_nr)
			{
				return 0;
			}
		}
	}
	return 1;
}

/*************************************************************************
*  stuff related to the save/load player system				 *
*************************************************************************/

long GetPlayerIdByName(const std::string &name)
{
	PlayerIndex *playerIndex = CharacterUtil::getPlayerIndexByUserName(name);

	if(playerIndex)
		return playerIndex->id;

	return -1;
}

std::string getNameById(const long id)
{
	PlayerIndex *playerIndex = CharacterUtil::getPlayerIndexByUserId(id);

	if(playerIndex)
		return playerIndex->name;

	return "";
}

void Character::AddLogin(const std::string &host, const DateTime &loginDatetime, GatewayDescriptorType *gatewayDescriptorType)
{
	std::stringstream Buffer;

	Buffer
		<< " INSERT INTO userLogin("
		<< "   `user_id`,"
		<< "   `login_datetime`,"
		<< "   `host`,"
		<< "   `gateway_descriptor_type`"
		<< " ) VALUES ("
		<< this->player.idnum << ","
		<< sql::encodeQuoteDate(loginDatetime.getTime()) << ","
		<< sql::escapeQuoteString(host) << ","
		<< gatewayDescriptorType->getValue() << ")";

	try {gameDatabase->sendRawQuery(Buffer.str());}
	catch (sql::QueryException &e)
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
	}
}
/* Static method */
bool Character::LoadLogins(std::list<pLogin> &Logins, std::string &Name,
						   const int iLimit, time_t tLow, time_t tHigh)
{
	std::stringstream Query;
	sql::Query MyQuery;

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(Name);

	Query << "SELECT userLogin.* "
		  << (Name.empty()==false?"":",users.username ")
		  << "FROM userLogin "
		  << (Name.empty()==false?"":",users ")
	      << "WHERE ";
	std::string::size_type stPreSize = Query.str().size();
	bool hasCond = false;
	if( !(Name.empty()) ) {
		Query << "user_id=" << index->id << " ";
		hasCond=true;
	}
	if( tLow != -1 ) {
		Query << (hasCond ? " AND" : "");
		Query << " login_datetime >= FROM_UNIXTIME(" << tLow << ") ";
		hasCond = true;
	}
	if( tHigh!= -1 ) {
		Query << (hasCond ? " AND" : "");
		Query << " login_datetime <= FROM_UNIXTIME(" << tHigh<< ") ";
		hasCond = true;
	}
	if(Name.empty()) {
		Query << (hasCond ? " AND" : "");
		Query << " userLogin.user_id=users.user_id ";
		hasCond = true;
	}

	if( stPreSize == Query.str().size() ) Query << "1 ";

	Query << "ORDER BY login_datetime DESC";
	if( iLimit!=-1 ) Query << " LIMIT " << iLimit;
	Query << ";";

	try {MyQuery = gameDatabase->sendQuery(Query.str());}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return false;
	}
	for(unsigned int i = 0;i < MyQuery->numRows();++i)
	{
		sql::Row Row = MyQuery->getRow();

		pLogin login;
		login.Host = Row["host"];
		login.time = Row.getTimestamp("login_datetime");
		login.PlayerName = Name.empty() ? Row["username"] : StringUtil::cap(StringUtil::allLower(Name));
		login.gatewayDescriptorType = (GatewayDescriptorType*)GatewayDescriptorType::getEnumByValue(Row.getInt("gateway_descriptor_type"));

		Logins.push_back(login);
	}

	return true;
}

pLogin::pLogin( const std::string &h, time_t t, GatewayDescriptorType *gatewayDescriptorType )
{
	this->Host = h;
	this->time = t;
	this->gatewayDescriptorType = gatewayDescriptorType;
}

std::string pLogin::Print()
{
	std::stringstream buffer;
	buffer << time.toString() << "     "
		   << std::setw(15) << std::left << this->Host
		   << std::setw(12) << std::left << this->gatewayDescriptorType->getStandardName()
		   << "   " << PlayerName.c_str() << std::endl;

	return buffer.str();
}

/* Written by Galnor in 2005. Grab a list of files in a given directory. FileType may be specified
 * to filter specific file extensions. All filenames will be dumped in the 'Files' list.
 */
void GetDirectoryList(const std::string &Directory, std::list<std::string> &Files, const std::string &FileType)
{
	std::string FileName;

	if(Directory.empty())
		return;

#if defined (WIN32)

	WIN32_FIND_DATA data;
	HANDLE hfind = INVALID_HANDLE_VALUE;

	if(!(hfind = FindFirstFile(std::string(Directory + "\\*").c_str(), &data)))
		return;

	while(FindNextFile(hfind, &data))
	{
		FileName = data.cFileName;
#else

	DIR		*dir = NULL;
	dirent	*de = NULL;

	if(!(dir = opendir(Directory.c_str())))
		return;

	while((de = readdir(dir)))
	{
		FileName = de->d_name;
#endif

		if((FileType.empty() || FileName.find(FileType) != std::string::npos) && str_cmp(FileName, ".."))
		{
			Files.insert(Files.end(), Directory + FileName);
		}
	}
#ifndef WIN32
	free(dir);
#endif
}

UserDisabledCommand *Character::loadUserDisabledCommand(const sql::Row &row)
{
	UserDisabledCommand *userDisabledCommand = new UserDisabledCommand();

	userDisabledCommand->setId(row.getInt("id"));
	userDisabledCommand->setUserId(row.getInt("user_id"));
	userDisabledCommand->setDisabledByUserId(row.getInt("disabled_by_user_id"));
	userDisabledCommand->setCommand(row.getString("command"));
	userDisabledCommand->setCreatedDatetime(row.getTimestamp("created_datetime"));

	return userDisabledCommand;
}

std::list<UserDisabledCommand *> Character::loadUserDisabledCommands(const int user_id)
{
	std::stringstream sql;
	sql::Query query;
	sql::Row row;
	std::list<UserDisabledCommand *> userDisabledCommands;

	try {
		sql << "SELECT *"
			<< " FROM userDisabledCommand"
			<< " WHERE user_id=" << user_id;

		query = gameDatabase->sendQuery(sql.str());

		while(query->hasNextRow())
		{
			sql::Row row = query->getRow();
			UserDisabledCommand *userDisabledCommand = loadUserDisabledCommand(row);

			userDisabledCommands.push_back(userDisabledCommand);
		}
	}
	catch(sql::QueryException &e)
	{
		e.report();
		MudLog(BRF, LVL_APPR, TRUE, "Could not load user disabled commands for character #%d", user_id);
		return userDisabledCommands;
	}

	return userDisabledCommands;
}

void Character::deleteUserDisabledCommands(int user_id)
{
	std::stringstream sql;
	sql << "DELETE FROM userDisabledCommand"
		<< " WHERE user_id=" << user_id;

	gameDatabase->sendRawQuery(sql.str());
}

void Character::saveUserDisabledCommands(int user_id, std::list<UserDisabledCommand *> &userDisabledCommands)
{
	for(std::list<UserDisabledCommand *>::iterator iter = userDisabledCommands.begin();iter != userDisabledCommands.end();++iter)
	{
		putUserDisabledCommand( (*iter) );
	}
}

void Character::putUserDisabledCommand(UserDisabledCommand *userDisabledCommand)
{
	std::stringstream sql;

	if(userDisabledCommand->isNew())
	{
		sql << "INSERT INTO userDisabledCommand("
			<< " `user_id`,"
			<< " `command`,"
			<< " `disabled_by_user_id`,"
			<< " `created_datetime`"
			<< ") VALUES("
			<< userDisabledCommand->getUserId() << ","
			<< "'" << sql::escapeString(userDisabledCommand->getCommand()) << "',"
			<< userDisabledCommand->getDisabledByUserId() << ","
			<< sql::encodeQuoteDate((time_t)userDisabledCommand->getCreatedDatetime().getTime())
			<< ")";

		gameDatabase->sendRawQuery(sql.str());

		userDisabledCommand->setId(gameDatabase->lastInsertID());
	}
	else
	{
		sql << "UPDATE userDisabledCommand SET"
			<< " user_id=" << userDisabledCommand->getUserId() << ","
			<< " command=" << sql::escapeString(userDisabledCommand->getCommand()) << ","
			<< " disabled_by_user_id=" << userDisabledCommand->getDisabledByUserId() << ","
			<< " created_datetime='" << sql::encodeDate(userDisabledCommand->getCreatedDatetime().getTime())
			<< " WHERE id=" << userDisabledCommand->getId();

		gameDatabase->sendRawQuery(sql.str());
	}
}

bool Character::mysqlInsertQuery()
{
	std::string Query;
	sql::Row Row;
	sql::Query MyQuery;

	Query = "INSERT INTO users (username) VALUES ('" + sql::escapeString(this->player.name) + "')";

	try {gameDatabase->sendQuery(Query);}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return false;
	}
	this->player.idnum = gameDatabase->lastInsertID();
	return true;
}

bool Character::basicSave()
{
	std::stringstream Query;
	if(IS_NPC(this))
		return false;

	int save_room;

	if(!in_room)//Player has no room. Most likely they were loaded and altered while offline.
		save_room = PlayerData->load_room;
	else if (player.timer >= CONFIG_IDLE_VOID)//In the void.
		save_room = was_in_room ? was_in_room->getVnum() : StartRoom()->getVnum();
	else
		save_room = this->in_room->getVnum();

	int mount = (MOUNT(this) ? MOUNT(this)->getVnum() : 0);

	LAST_LOGOUT(this) = time(0);
	this->player.time.logout = DateTime();
	time_t played = this->player.time.played + (LAST_LOGOUT(this) - LAST_LOGON(this));

	Query << "UPDATE users SET ";
	Query << "user_password = '" << sql::escapeString(this->player.passwd)				<< "',";
	Query << "poofin = '" << sql::escapeString(this->points.poofin)			<< "',";
	Query << "poofout = '" << sql::escapeString(this->points.poofout)			<< "',";
	Query << "level = " << (int)this->player.level			<< ",";
	Query << "maxhit = " << this->points.max_hit			<< ",";
	Query << "hit = " << this->points.hit			<< ",";
	Query << "maxmove = " << this->BaseMoves()			<< ",";
	Query << "move = " << this->points.move			<< ",";
	Query << "maxmana = " << this->points.max_mana			<< ",";
	Query << "sex = " << (int)this->player.sex << ",";
	Query << "mana = " << this->points.mana			<< ",";
	Query << "shadow = " << this->points.shadow_points			<< ",";
	Query << "maxshadow = " << this->points.max_shadow_points			<< ",";
	Query << "race = " << (int)this->player.race			<< ",";
	Query << "chclass = " << (int)this->player.chclass			<< ",";
	Query << "birth = " << this->player.time.birth.getTime()			<< ",";
	Query << "weight = " << this->player.weight			<< ",";
	Query << "height = " << this->player.height			<< ",";
	Query << "invis = " << (int)this->points.invis			<< ",";
	Query << "weave = " << this->points.weave			<< ",";
	Query << "taint = " << this->points.taint			<< ",";
	Query << "death_wait = " << (int)this->points.death_wait			<< ",";
	Query << "warnings = " << (int)this->points.warning			<< ",";
	Query << "master_weapon = " << this->points.master_weapon			<< ",";
	Query << "strain = " << this->points.strain			<< ",";
	Query << "slew = '" << sql::escapeString(this->points.slew)			<< "',";
	Query << "title = '" << sql::escapeString(this->player.title)			<< "',";
	Query << "whois_extra = '" << sql::escapeString(this->points.whois_extra)			<< "',";

	Query << "description = '" << sql::escapeString(this->player.description == NULL ? "" : this->player.description)			<< "',";

	if(this->player.time.logon.after(DateTime(0))) {
		Query << "last_logon = FROM_UNIXTIME("  << this->player.time.logon.getTime()		<< "),";
	}
	if(this->player.time.logout.after(DateTime(0))) {
		Query << "last_logout = FROM_UNIXTIME(" << this->player.time.logout.getTime()		<< "),";
	}
	Query << "warrants = '" << this->points.warrants[0] << " " << this->points.warrants[1] << " "
		  << this->points.warrants[2] << " " << this->points.warrants[3] << "',";

	Query << "gold = " << this->points.gold										<< ",";
	Query << "bank_gold = " << this->points.bank_gold							<< ",";
	Query << "str = " << ((int)this->real_abils.str)							<< ",";
	Query << "dex = " << ((int)this->real_abils.dex)							<< ",";
	Query << "intel = " << ((int)this->real_abils.intel)						<< ",";
	Query << "wis = " << ((int)this->real_abils.wis)							<< ",";
	Query << "con = " << ((int)this->real_abils.con)							<< ",";
	Query << "exp = " << this->points.exp										<< ",";
	Query << "played = " << played												<< ",";
	Query << "restat = " << this->restat_time.getTime()							<< ",";
	Query << "pk_deaths = " << this->pk_deaths									<< ",";
	Query << "mob_deaths = " << this->mob_deaths								<< ",";
	Query << "pks = " << this->pks												<< ",";
	Query << "pw_updated = '" << (this->PasswordUpdated() ? (1) : (0))			<< "',";
	Query << "mount = " << mount												<< ",";
	Query << "loadroom = " << MAX(0, save_room)									<< ",";
	Query << "pracs = " << this->PlayerData->skillpracs							<< ",";
	Query << "spell_pracs = " << this->PlayerData->spellpracs					<< ",";
	Query << "trade_pracs = " << this->PlayerData->tradepracs					<< ",";
	Query << "freeze = " << MAX(0,(int)this->PlayerData->freeze_level)			<< ",";
	Query << "bad_passwords = " << (int)this->PlayerData->bad_pws				<< ",";
	Query << "hunger = " << (int)this->PlayerData->conditions[ FULL ]			<< ",";
	Query << "thirst = " << (int)this->PlayerData->conditions[ THIRST ]			<< ",";
	Query << "drunk = " << (int)this->PlayerData->conditions[ DRUNK ]			<< ",";
	Query << "plr = " << (int)PLR_FLAGS(this) << ",";
	Query << "prf = '" << PRF_FLAGS(this)[0] << " " << PRF_FLAGS(this)[1] << " "
		 << PRF_FLAGS(this)[2] << " " << PRF_FLAGS(this)[3] <<" "<< PRF_FLAGS(this)[4] << "',";
	Query << "newb = '" << NEWB_FLAGS(this)[0] << " " << NEWB_FLAGS(this)[1] << " "
		 << NEWB_FLAGS(this)[2] << " " << NEWB_FLAGS(this)[3] << "',";
	Query << "reset = " << this->reset_time.getTime() << ",";
	Query << "account_id = " << this->GetAccount();
	Query << " WHERE username = '" << this->player.name << "'";

	try {gameDatabase->sendRawQuery(Query.str());}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return false;
	}
	return true;
}

//Galnor: 6-16-2006: MySQL Save
bool Character::save()
{
	if( IS_NPC(this) )
		return true;
	bool retval = this->basicSave();
	this->saveSkills();
	this->saveClans();
	this->saveTrophies();
	this->saveHitRolls();
	this->saveManaRolls();

	Character::deleteUserDisabledCommands(this->player.idnum);
	Character::saveUserDisabledCommands(this->player.idnum, this->PlayerData->userDisabledCommands);

	return retval;
}

bool Character::loadSkills()
{
	std::string queryStr = "SELECT percent, skill FROM skills WHERE user_id=" + MiscUtil::convert<std::string>(this->player.idnum);
	sql::Query query;
	sql::Row row;

	try {query = gameDatabase->sendQuery(queryStr);}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return false;
	}

	for(unsigned int i = 0;i < query->numRows();++i)
	{
		row = query->getRow();
		this->setSkill(row.getInt("skill"), row.getInt("percent"));
	}
	return true;
}

bool Character::loadIgnores()
{
	std::string query = "SELECT * FROM userIgnore WHERE user_id=" + MiscUtil::convert<std::string>(this->player.idnum);
	sql::Query MyQuery;
	sql::Row MyRow;

	try {MyQuery = gameDatabase->sendQuery(query);}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return false;
	}
	for(unsigned int i = 0; i < MyQuery->numRows();++i)
	{
		MyRow = MyQuery->getRow();
		this->ignores.push_back(MyRow["victim"]);
	}

	return true;
}

void MySQLSaveAlias(const std::string &playername, std::map<std::string,std::string>::iterator a, bool update)
{
	std::stringstream Query;

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(playername);

	if(index == NULL)
		return;

	if(update)
		Query	<< "UPDATE userAlias SET replacement='" << sql::escapeString((*a).second) << "'";
	else
		Query	<< "INSERT INTO userAlias (user_id,command,replacement) VALUES(" << index->id << ",'"
				<< sql::escapeString((*a).first) << "','" << sql::escapeString((*a).second) << "')";

	try { gameDatabase->sendRawQuery(Query.str()); }
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "MySQLSaveAlias : %s", e.getMessage().c_str());
	}
}
void MySQLDeleteAlias(const std::string &playername, const std::string &command)
{
	std::stringstream Query;

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(playername);

	if(index == NULL)
		return;

	Query	<< " DELETE FROM userAlias"
			<< " WHERE user_id=" << index->id
			<< " AND command = " << sql::escapeQuoteString(command);

	try{ gameDatabase->sendRawQuery(Query.str()); }
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "MySQLDeleteAlias : %s", e.getMessage().c_str());
	}
}

bool Character::loadAliases()
{
	std::stringstream Query;
	sql::Query MyQuery;
	sql::Row MyRow;

	Query << "SELECT command,replacement FROM userAlias WHERE user_id=" << this->player.idnum;

	try { MyQuery = gameDatabase->sendQuery(Query.str()); }
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Character::loadAliases : %s", e.getMessage().c_str());
		return false;
	}
	for(unsigned int i = 0;i < MyQuery->numRows();++i)
	{
		MyRow = MyQuery->getRow();
		this->aliases[ MyRow["command"] ] = MyRow["replacement"];
	}
	return true;
}
bool Character::loadClans()
{
	std::stringstream sql;
	
	sql << " SELECT *"
		<< " FROM userClan"
		<< " WHERE user_id = " << this->getUserId();

	sql::Query query;
	sql::Row row;
	UserClan *userClan;

	try
	{
		query = gameDatabase->sendQuery(sql.str());
	}
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Character::loadClans : %s", e.getMessage().c_str());
		return false;
	}
	
	while(query->hasNextRow())
	{
		row = query->getRow();
		userClan = ClanUtil::getUserClan(row);
		userClans.push_back(userClan);
	}
	return true;
}
bool Character::loadTrophies()
{
	std::string query = "SELECT * FROM trophies WHERE user_id=" + MiscUtil::convert<std::string>(this->player.idnum);
	sql::Query MyQuery;
	sql::Row MyRow;

	if(!this->LoadData)
		return false;

	try { MyQuery = gameDatabase->sendQuery(query); }
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Character::loadTrophies : %s", e.getMessage().c_str());
		return false;
	}
	for(unsigned int i = 0;i < MyQuery->numRows();++i)
	{
		MyRow = MyQuery->getRow();
		this->kill_list[MyRow[1]] = atoi(MyRow[2].c_str());
	}
	this->LoadData->Trophies.clear();
	this->LoadData->Trophies.insert(this->kill_list.begin(), this->kill_list.end());
	return true;
}

//This function can be used for either hitrolls or manarolls, just supply the correct table name.
bool MySQLSaveRolls(const std::string &playername, const std::string &TableName,
					std::vector<int> &StoredRolls, std::vector<int> &Rolls)
{
	std::string Query;
	sql::Query MyQuery;
	sql::Row MyRow;
	char roll[16], level[16];
	unsigned int i;

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(playername);

	if( index == NULL )
		return false;

	for(i = 0;i < StoredRolls.size() && i < Rolls.size();++i)
	{
		//Roll needs to be updated.
		if(StoredRolls[i] != Rolls[i])
		{
			sprintf(roll , "%d", Rolls[i]);
			Query = "UPDATE " + TableName + " SET roll = "
				+ roll + " WHERE user_id=" + MiscUtil::convert<std::string>(index->id);

			try { MyQuery = gameDatabase->sendQuery( Query ); }
			catch( sql::QueryException &e )
			{
				MudLog(BRF, LVL_APPR, TRUE, "MySQLSaveRolls : %s", e.getMessage().c_str());
				return false;
			}
		}
	}
	//No more or less rolls. Most likely scenerio.
	if(Rolls.size() == StoredRolls.size())
		return true;
	//Need to insert new rolls.
	else if(Rolls.size() > StoredRolls.size())
	{
		for(;i < Rolls.size();++i)
		{
			sprintf(level, "%d", (i + 1) );
			sprintf(roll , "%d", Rolls[i]);
			Query = "INSERT INTO " + TableName + " (user_id, roll, level) VALUES ("
				+ MiscUtil::convert<std::string>(index->id) + "," + roll + "," + level + ")";
			try {
				MyQuery = gameDatabase->sendQuery(Query);
			}
			catch(sql::QueryException &e) {
				MudLog(BRF, LVL_APPR, TRUE, "MySQLSaveRolls : %s", e.getMessage().c_str());
				return false;
			}
		}
	}
	//Need to delete rolls which no longer exist.
	else if(StoredRolls.size() > Rolls.size())
	{
		sprintf(level, "%d", (i + 1) );
		Query = "DELETE FROM " + TableName + " WHERE user_id=" + MiscUtil::convert<std::string>(index->id) +
			" AND level >= " + level;
		try {
			MyQuery = gameDatabase->sendQuery(Query);
		}
		catch(sql::QueryException &e) {
			MudLog(BRF, LVL_APPR, TRUE, "MySQLSaveRolls : %s", e.getMessage().c_str());
			return false;
		}
	}
	StoredRolls.clear();
	StoredRolls.insert(StoredRolls.end(), Rolls.begin(), Rolls.end());
	return true;
}

//This function can be used for either hitrolls or manarolls, just supply the correct table name.
bool MySQLLoadRolls(const std::string &playername, const std::string TableName, std::vector<int> &Rolls)
{
	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(playername);
	if(index == NULL)
		return false;
	std::string query = "SELECT level, roll FROM " + TableName + " WHERE user_id="
		+ MiscUtil::convert<std::string>(index->id) + " ORDER BY level DESC";
	sql::Query MyQuery;
	sql::Row MyRow;

	try
	{
		MyQuery = gameDatabase->sendQuery(query);
		int level = 0, roll = 0, max = 0;
		for( unsigned int i = 0;i < MyQuery->numRows();++i )
		{
			MyRow = MyQuery->getRow();
			level = atoi(MyRow[0].c_str());
			roll = atoi(MyRow[1].c_str());
			if(!i)
			{
				max = level;
				Rolls.resize(max);
			}
			else if(level > max)
			{
				MudLog(BRF, LVL_GRGOD, TRUE,
					"Out of order hit for player %s. Level: %d, Max: %d.",
					playername.c_str(), level, max);
				continue;
			}
			Rolls[level - 1] = roll;
		}
	}
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "MySQLLoadRolls : %s", e.getMessage().c_str());
		return false;
	}
	return true;
}

bool Character::loadHitRolls()
{
	bool result = MySQLLoadRolls(this->player.name, "userHitRoll", this->LoadData->HitRolls);
	this->points.HitRolls.insert(this->points.HitRolls.end(),
		this->LoadData->HitRolls.begin(), this->LoadData->HitRolls.end());
	return result;
}
bool Character::loadManaRolls()
{
	bool result = MySQLLoadRolls(this->player.name, "userManaRoll", this->LoadData->ManaRolls);
	this->points.ManaRolls.insert(this->points.ManaRolls.end(),
		this->LoadData->ManaRolls.begin(), this->LoadData->ManaRolls.end());
	return result;
}
bool Character::loadQuests()
{
	std::string query = "SELECT quest_name, value FROM quests WHERE user_id=" + MiscUtil::convert<std::string>(this->player.idnum);
	sql::Query MyQuery;
	sql::Row MyRow;

	try
	{
		MyQuery = gameDatabase->sendQuery(query);
		for( unsigned int i = 0;i < MyQuery->numRows();++i )
		{
			MyRow = MyQuery->getRow();
			new Quest(this, MyRow[0], atoi(MyRow[1].c_str()));
		}
	}
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Character::loadQuests : %s", e.getMessage().c_str());
		return false;
	}
	return true;
}

bool Character::saveSkills()
{
	std::stringstream queryBuffer;

	queryBuffer << " DELETE FROM skills"
				<< " WHERE user_id=" << this->player.idnum;

	gameDatabase->sendRawQuery(queryBuffer.str());

	queryBuffer.str("");

	sql::BatchInsertStatement batchInsertStatement(gameDatabase, "skills", 1000);

	batchInsertStatement.addField("user_id");
	batchInsertStatement.addField("skill");
	batchInsertStatement.addField("percent");

	batchInsertStatement.start();

	for(auto skillIter = skills.begin();skillIter != skills.end();++skillIter)
	{
		batchInsertStatement.beginEntry();

		batchInsertStatement.putInt( this->player.idnum );
		batchInsertStatement.putInt( (*skillIter).second.getSkillId() );
		batchInsertStatement.putInt( (*skillIter).second.getPercent() );

		batchInsertStatement.endEntry();
	}

	batchInsertStatement.finish();

	return true;
}

bool Character::saveClans()
{
	for(auto iter = userClans.begin();iter != userClans.end();++iter)
	{
		ClanUtil::putUserClan( gameDatabase, (*iter) );
	}
	return true;
}

bool Character::saveTrophies()
{
	std::stringstream queryBuffer;
	std::map<std::string, int>::iterator old_trophy, new_trophy;

	if(!this->LoadData)
		return false;//wtf?

	for(new_trophy = this->kill_list.begin();new_trophy != this->kill_list.end();++new_trophy)
	{
		queryBuffer.str("");
		if( (old_trophy = this->LoadData->Trophies.find( (*new_trophy).first))
			!= this->LoadData->Trophies.end() )
		{
			//Kill value has changed. Update...
			if( (*new_trophy).second != (*old_trophy).second)
			{
				queryBuffer << " UPDATE trophies SET"
							<< "   kill_count = " << (*new_trophy).second
							<< " WHERE user_id = " << this->player.idnum
							<< " AND victim = " << sql::escapeQuoteString((*new_trophy).first);
				try {
					gameDatabase->sendRawQuery(queryBuffer.str());
				}
				catch( sql::QueryException &e )
				{
					MudLog(BRF, LVL_APPR, TRUE, "Character::saveTrophies : %s", e.getMessage().c_str());
					return false;
				}
			}
		}
		else
		{
			queryBuffer << " INSERT INTO trophies ("
						<< "   `user_id`,"
						<< "   `victim`,"
						<< "   `kill_count`"
						<< ") VALUES ("
						<< this->player.idnum << ","
						<< sql::escapeQuoteString((*new_trophy).first) << ","
						<< (*new_trophy).second
						<< ")";
			try {
				gameDatabase->sendRawQuery(queryBuffer.str());
			}
			catch( sql::QueryException &e ) {
				MudLog(BRF, LVL_APPR, TRUE, "Character::saveTrophies : %s", e.getMessage().c_str());
				return false;
			}
		}
	}

	this->LoadData->Trophies.clear();
	this->LoadData->Trophies.insert(this->kill_list.begin(), this->kill_list.end());
	return true;
}
bool Character::saveHitRolls()
{
	return (MySQLSaveRolls(this->player.name, "userHitRoll", this->LoadData->HitRolls, this->points.HitRolls));
}
bool Character::saveManaRolls()
{
	return (MySQLSaveRolls(this->player.name, "userManaRoll", this->LoadData->ManaRolls, this->points.ManaRolls));
}

bool playerExists(const std::string &name)
{
	if(CharacterUtil::getPlayerIndexByUserName(name))
		return true;
	return false;
}

bool Character::CreateDatabaseEntry()
{
	static char query[512];
	sprintf(query, "INSERT INTO users (name) VALUES ('%s')", this->player.name.c_str());

	try {gameDatabase->sendRawQuery(query);}
	catch( sql::QueryException &e )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Character::CreateDatabaseEntry : %s", e.getMessage().c_str());
		return false;
	}
	return true;
}

void Object::UnsetPrototypeSpecifics()
{
	this->name = NULL;
	this->description = NULL;
	this->ex_description = NULL;
	this->action_description = NULL;
	this->short_description = NULL;
}

//Object Destructor
Object::~Object()
{
	++Object::nr_dealloc;
	int i = 0;
	unsigned int s = 0;

	for(Descriptor *d = descriptor_list;d;d=d->next)
	{
		if(d->olc && d->olc->auction_data && d->olc->auction_data->GetItemToSell() == this)
			d->olc->auction_data->SetItemToSell( 0 );
	}
	//Anyone targeting this object must have their otarget set to NULL.
	for( Character* ch = character_list;ch;ch = ch->next )
	{
		if( ch->player.otarget == this )
			ch->player.otarget = NULL;
	}
	Zone *zone;
	for(i = 0;(zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;++i)
	{
		for(s = 0;s < zone->cmd.size();++s)
		{
			if(zone->cmd[s]->obj == this)
				zone->cmd[s]->obj = NULL;
		}
	}

	//We may have an immortal in the zone editor with a reference to this object. Get rid of those as well!
	//Galnor 2014-07-05
	for(Descriptor *descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
	{
		if(STATE(descriptor) == CON_ZEDIT)
		{
			Zone *olcZone = descriptor->olc->zone;
			for(auto resetCommandIterator = olcZone->cmd.begin();resetCommandIterator != olcZone->cmd.end();++resetCommandIterator)
			{
				ResetCommand *resetCommand = (*resetCommandIterator);
				if(resetCommand->obj == this)
				{
					resetCommand->obj = NULL;
				}
			}
		}
	}

	if( this->SatOnBy != NULL )
	{
		this->SatOnBy->player.sitting_on = NULL;
		this->SatOnBy = NULL;
	}

	if(this->scalp)
	{
		if(this->scalp->Food)
		{
			delete this->scalp->Food;
		}
		if(this->scalp->Skin)
		{
			delete this->scalp->Skin;
		}
		delete this->scalp;
	}

	JSManager::get()->handleExtraction( this );
	if( (this->item_number > -1 && this == obj_proto[this->item_number]) || item_number == -1 )
	{
		if( this->name )
			delete[] this->name;
		if( this->description )
			delete[] this->description;
		ExtraDescription *exDesc, *exDescNext;
		for( ExtraDescription *exDesc = this->ex_description;exDesc;exDesc = exDescNext )
		{
			exDescNext = exDesc->next;
			delete (exDesc);
		}
		if( this->action_description )
			delete[] this->action_description;
		if( this->short_description )
			delete[] this->short_description;
	}
	if( this->retool_desc )
		delete[] retool_desc;
	if( this->retool_sdesc )
		delete[] retool_sdesc;
	if( this->retool_name )
		delete[] retool_name;
	if( this->retool_ex_desc )
		delete retool_ex_desc;
	if( obj_flags.mModifiers )
		delete obj_flags.mModifiers;
}

//Galnor - Object constructor
Object::Object()
{
	++Object::nr_alloc;
	memset(&this->obj_flags,	0, sizeof(this->obj_flags));
	memset(&this->affected,		0, sizeof(this->affected));
	this->proto				= false;
	this->hidden			= false;
	this->worn_on			= 0;
	this->item_number		= -1;
	this->Max				= -1;
	this->in_room			= 0;
	this->name				= NULL;
	this->description		= NULL;
	this->short_description	= NULL;
	this->action_description= NULL;
	this->ex_description	= NULL;
	this->carried_by		= NULL;
	this->worn_by			= NULL;
	this->SatOnBy			= NULL;
	this->in_obj			= NULL;
	this->contains			= NULL;
	this->next_content		= NULL;
	this->next				= NULL;
	this->scalp				= NULL;
	this->retool_desc		= NULL;
	this->retool_name		= NULL;
	this->retool_sdesc		= NULL;
	this->retool_ex_desc	= NULL;
	this->deleted			= false;
	this->needs_save		= false;
	this->purged			= false;
	this->Money				= 0;
	this->createdDatetime	= DateTime();
	this->objID				= boost::uuids::nil_generator()();
	this->decayType			= -1;
	this->decayTimer		= -1;
	this->decayTimerType	= -1;
	this->js_scripts		= std::shared_ptr<std::vector<JSTrigger*> >( new std::vector< JSTrigger* > );
}

bool Character::IsProto()
{
	return proto;
}
bool Object::IsProto()
{
	return proto;
}

//Galnor - 01/16/2008 - Boot all the object prototypes from the database.
int BootObjects()
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery, jsAttachmentQuery;
	sql::Row MyRow, jsAttachmentRow;
	std::list<int> jsAttachmentList;
	int i;

	QueryBuffer << "SELECT * FROM obj_protos ORDER BY vnum ASC";
	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
		jsAttachmentQuery = gameDatabase->sendQuery("SELECT * FROM js_attachments WHERE type='O' ORDER BY target_vnum ASC");
	}
	catch(sql::QueryException &e)
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		exit(1);
	}
	top_of_objt = MyQuery->numRows() - 1;

	obj_proto.resize( top_of_objt + 1);
	obj_index = new Index[top_of_objt + 1];
	ItemCount.resize(top_of_objt + 1);
	memset(obj_index, 0, sizeof(Index) * (top_of_objt + 1));

	for(i = 0;i <= top_of_objt;++i)
	{
		MyRow = MyQuery->getRow();
		jsAttachmentList.clear();
		int objectPrototypeVnum = atoi(MyRow["vnum"].c_str());

		while(jsAttachmentQuery->hasNextRow() && jsAttachmentQuery->peekRow().getInt("target_vnum") < objectPrototypeVnum)
			jsAttachmentQuery->skipRow();

		while(jsAttachmentQuery->hasNextRow() && jsAttachmentQuery->peekRow().getInt("target_vnum") == objectPrototypeVnum)
		{
			int scriptVnum = jsAttachmentQuery->getRow().getInt("script_vnum");

			jsAttachmentList.push_back(scriptVnum);
		}

		obj_proto[i] = new Object();
		obj_proto[i]->ProtoBoot(MyRow, i, jsAttachmentList);
	}
	return top_of_objt;
}
void Object::protoSave()
{
	if( deleted ) return;

	std::stringstream Buffer;
	std::stringstream EDescBuffer(ExtraDescription::Serialize(this->ex_description));
	Buffer << "REPLACE DELAYED INTO obj_protos SET "
		<< "aliases='" << (name ? sql::escapeString(this->name) : "") << "',"
		<< "sdesc='" << (short_description ? sql::escapeString(this->short_description) : "") << "',"
		<< "ldesc='" << (description ? sql::escapeString(this->description) : "") << "',"
		<< "adescription='" << (action_description ? sql::escapeString(this->action_description) : "") << "',"
		<< "type_flag=" << ((int)this->obj_flags.type_flag) << ","
		<< "extra_flags=" << this->obj_flags.extra_flags << ","
		<< "obj_flag0="   << this->obj_flags.value[0] << ","
		<< "obj_flag1="   << this->obj_flags.value[1] << ","
		<< "obj_flag2="   << this->obj_flags.value[2] << ","
		<< "obj_flag3="   << this->obj_flags.value[3] << ","
		<< "obj_flag4="   << this->obj_flags.value[4] << ","
		<< "obj_flag5="   << this->obj_flags.value[5] << ","
		<< "obj_flag6="   << this->obj_flags.value[6] << ","
		<< "obj_flag7="   << this->obj_flags.value[7] << ","
		<< "obj_flag8="   << this->obj_flags.value[8] << ","
		<< "obj_flag9="   << this->obj_flags.value[9] << ","
		<< "obj_flag10="  << this->obj_flags.value[10] << ","
		<< "obj_flag11="  << this->obj_flags.value[11] << ","
		<< "weight=" << this->obj_flags.weight << ","
		<< "cost=" << this->obj_flags.cost << ","
		<< "cost_per_day=" << this->obj_flags.cost_per_day << ","
		<< "offensive=" << this->obj_flags.offensive << ","
		<< "parry=" << this->obj_flags.parry << ","
		<< "dodge=" << this->obj_flags.dodge << ","
		<< "absorb=" << this->obj_flags.absorb << ","
		<< "clan="	<< this->obj_flags.clan << ","
		<< "clan_weight=" << this->obj_flags.clan_weight << ","
		<< "clan_offensive=" << this->obj_flags.clan_offensive << ","
		<< "clan_parry=" << this->obj_flags.clan_parry << ","
		<< "clan_dodge=" << this->obj_flags.clan_dodge << ","
		<< "clan_absorb=" << this->obj_flags.clan_absorb << ","
		<< "clan_moves=" << this->obj_flags.clan_moves << ","
		<< "clan_hit=" << this->obj_flags.clan_hit << ","
		<< "clan_dmg1=" << this->obj_flags.clan_dmg1 << ","
		<< "clan_dmg2=" << this->obj_flags.clan_dmg2 << ","
		<< "max=" << this->Max << ","
		<< "decay_type="  << ((int)this->decayType) << ","
		<< "decay_timer=" << ((int)this->decayTimer) << ","
		<< "decay_timer_type=" << ((int)this->decayTimerType) << ","
		<< "affectsL0=" << ((int)this->affected[0].location) << ","
		<< "affectsL1=" << ((int)this->affected[1].location) << ","
		<< "affectsL2=" << ((int)this->affected[2].location) << ","
		<< "affectsL3=" << ((int)this->affected[3].location) << ","
		<< "affectsL4=" << ((int)this->affected[4].location) << ","
		<< "affectsL5=" << ((int)this->affected[5].location) << ","
		<< "affectsM0=" << ((int)this->affected[0].modifier) << ","
		<< "affectsM1=" << ((int)this->affected[1].modifier) << ","
		<< "affectsM2=" << ((int)this->affected[2].modifier) << ","
		<< "affectsM3=" << ((int)this->affected[3].modifier) << ","
		<< "affectsM4=" << ((int)this->affected[4].modifier) << ","
		<< "affectsM5=" << ((int)this->affected[5].modifier) << ","
		<< "wear=" << ((int)this->obj_flags.wear_flags) << ","
		<< "edescription='" << sql::escapeString(EDescBuffer.str().c_str()) << "',"
		<< "vnum='" << obj_index[this->item_number].vnum << "';";
	
	sql::Query MyQuery;
	try {MyQuery = gameDatabase->sendQuery(Buffer.str());}
	catch( sql::QueryException &e )
	{
		MudLog(BRF,LVL_APPR, TRUE, "Error saving object #%d: %s",
			obj_index[this->item_number].vnum, e.getMessage().c_str());
		return;
	}

	Buffer.str("");
	Buffer << "DELETE FROM js_attachments WHERE type='O' AND target_vnum='"
	<<  obj_index[this->item_number].vnum << "';" << std::endl;
	try {
		gameDatabase->sendRawQuery(Buffer.str());
	} catch( sql::QueryException &e ) {
		MudLog(BRF,LVL_APPR, TRUE, "Error saving object #%d: %s",
			obj_index[this->item_number].vnum, e.getMessage().c_str());
		return;
	}

	for(unsigned int i = 0;i < this->js_scripts->size();++i) {
		Buffer.str("");
		Buffer << "INSERT INTO js_attachments(type,target_vnum,script_vnum) VALUES('O','"
			<< this->getVnum() << "','" << this->js_scripts->at(i)->vnum << "');";
		try {
			gameDatabase->sendRawQuery(Buffer.str());
		} catch( sql::QueryException &e ) {
			MudLog(BRF,LVL_APPR, TRUE, "Error saving object #%d: %s",
				obj_index[this->item_number].vnum, e.getMessage().c_str());
			return;
		}
	}
	this->needs_save = false;
}
void Object::ProtoDelete()
{
	std::stringstream Buffer;
	sql::Query MyQuery;
	Buffer << "DELETE FROM obj_protos WHERE vnum=" << obj_index[this->item_number].vnum;
	try
	{
		MyQuery = gameDatabase->sendQuery(Buffer.str());
	}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_APPR, TRUE, "Object::ProtoDelete : %s", e.getMessage().c_str());
		return;
	}
}
void Object::ProtoBoot( sql::Row &MyRow, const int rnum, const std::list<int> &jsAttachmentList )
{
	this->proto = true;
	obj_index[rnum].number = rnum;
	this->item_number = rnum;
	obj_index[rnum].vnum = atoi(MyRow[0].c_str());
	this->name = str_dup(MyRow[1].c_str());
	this->short_description = str_dup(MyRow[2].c_str());
	this->description = str_dup(MyRow[3].c_str());
	this->action_description = str_dup(MyRow[4].c_str());
	this->obj_flags.type_flag = atoi((MyRow[5].c_str()));
	this->obj_flags.extra_flags = atoi(MyRow[6].c_str());
	this->obj_flags.value[0] = atoi(MyRow[7].c_str());
	this->obj_flags.value[1] = atoi(MyRow[8].c_str());
	this->obj_flags.value[2] = atoi(MyRow[9].c_str());
	this->obj_flags.value[3] = atoi(MyRow[10].c_str());
	this->obj_flags.value[4] = atoi(MyRow[11].c_str());
	this->obj_flags.value[5] = atoi(MyRow[12].c_str());
	this->obj_flags.value[6] = atoi(MyRow[13].c_str());
	this->obj_flags.value[7] = atoi(MyRow[14].c_str());
	this->obj_flags.value[8] = atoi(MyRow[15].c_str());
	this->obj_flags.value[9] = atoi(MyRow[16].c_str());
	this->obj_flags.value[10] = atoi(MyRow[17].c_str());
	this->obj_flags.weight = atof(MyRow[19].c_str());
	this->obj_flags.cost = atoi(MyRow[20].c_str());
	this->obj_flags.cost_per_day = atoi(MyRow[21].c_str());
	this->obj_flags.offensive = atoi(MyRow[22].c_str());
	this->obj_flags.parry = atoi(MyRow[23].c_str());
	this->obj_flags.dodge = atoi(MyRow[24].c_str());
	this->obj_flags.absorb = atoi(MyRow[25].c_str());
	this->obj_flags.clan = atoi(MyRow[26].c_str());
	this->obj_flags.clan_weight = atoi(MyRow[27].c_str());
	this->obj_flags.clan_offensive = atoi(MyRow[28].c_str());
	this->obj_flags.clan_parry = atoi(MyRow[29].c_str());
	this->obj_flags.clan_dodge = atoi(MyRow[30].c_str());
	this->obj_flags.clan_absorb = atoi(MyRow[31].c_str());
	this->obj_flags.clan_moves = atoi(MyRow[32].c_str());
	this->obj_flags.clan_hit = atoi(MyRow[33].c_str());
	this->obj_flags.clan_dmg1 = atoi(MyRow[34].c_str());
	this->obj_flags.clan_dmg2 = atoi(MyRow[35].c_str());
	this->Max = atoi(MyRow[36].c_str());
	this->decayType = atoi(MyRow["decay_type"].c_str());
	this->decayTimer = atoi(MyRow["decay_timer"].c_str());
	this->decayTimerType = atoi(MyRow["decay_timer_type"].c_str());
	this->affected[0].location = atoi(MyRow[40].c_str());
	this->affected[1].location = atoi(MyRow[41].c_str());
	this->affected[2].location = atoi(MyRow[42].c_str());
	this->affected[3].location = atoi(MyRow[43].c_str());
	this->affected[4].location = atoi(MyRow[44].c_str());
	this->affected[5].location = atoi(MyRow[45].c_str());
	this->affected[0].modifier = atoi(MyRow[46].c_str());
	this->affected[1].modifier = atoi(MyRow[47].c_str());
	this->affected[2].modifier = atoi(MyRow[48].c_str());
	this->affected[3].modifier = atoi(MyRow[49].c_str());
	this->affected[4].modifier = atoi(MyRow[50].c_str());
	this->affected[5].modifier = atoi(MyRow[51].c_str());
	this->obj_flags.wear_flags = atoi(MyRow["wear"].c_str());
	this->ex_description = ExtraDescription::Parse( MyRow["edescription"] );

	// Find any js scripts that are listed as attached to this mob in the db, and attach them to it
	this->js_scripts = std::shared_ptr<std::vector<JSTrigger*> >(new std::vector<JSTrigger*>());
	for(auto scriptVnumIter = jsAttachmentList.begin();scriptVnumIter != jsAttachmentList.end();++scriptVnumIter)
	{
		JSTrigger* t = JSManager::get()->getTrigger((*scriptVnumIter));
		this->js_scripts->push_back(t);
	}
}

/* Return a list of ExtraDescription, parsed from the string that is passed */
ExtraDescription *ExtraDescription::Parse( const std::string &eDescStr )
{
	std::vector< std::string > vExDescParts = StringUtil::splitToVector(eDescStr, '~');
	ExtraDescription *eDescList = (NULL);

	while( !vExDescParts.empty() )
	{
		if( vExDescParts.size() == 1 )
			break;//Incomplete
		ExtraDescription* ExDesc = new ExtraDescription;
		ExDesc->keyword = str_dup( vExDescParts[0].c_str() );
		ExDesc->description = str_dup( vExDescParts[1].c_str() );

		if( !eDescList )
			eDescList = ExDesc;
		else {
			ExtraDescription *temp;
			for( temp = eDescList;temp->next;temp=temp->next );
			temp->next = ExDesc;
		}
		vExDescParts.erase(vExDescParts.begin(), vExDescParts.begin()+2);
	}
	return eDescList;
}

std::string ExtraDescription::Serialize(ExtraDescription *eDescList)
{
	std::stringstream eDescBuffer;
	for( ExtraDescription *e = eDescList;e;e = e->next )
		eDescBuffer << e->keyword << "~" << std::endl << e->description << "~" << std::endl;
	return eDescBuffer.str();
}

KitItem::KitItem()
{
	this->dbID = -1;
	this->item_vnum = -1;
	this->body_pos = -1;
	this->percent = 0;
	this->deleted = false;
}
KitItem::KitItem( const int bp, const int iv, const int p, const int id )
{
	this->body_pos = bp;
	this->item_vnum = iv;
	this->percent = p;
	this->dbID = id;
	this->deleted = false;
}
KitItem::~KitItem()
{
}

void KitItem::SetBodyPos( const int bp )
{
	this->body_pos = bp;
}
void KitItem::SetItemVnum( const int iv )
{
	this->item_vnum = iv;
}
void KitItem::SetPercent( const int perc )
{
	this->percent = perc;
}

int KitItem::GetBodyPos()
{
	return this->body_pos;
}
int KitItem::GetItemVnum()
{
	return this->item_vnum;
}
/*** Galnor, 04/27/2009 - Return the item's REAL MiscUtil::random(obj_index subscript) ***/
int KitItem::GetItemRnum()
{
	if( this->item_vnum >= 0 )
		return real_object(this->item_vnum);
	return -1;//Invalid vnum...
}
int KitItem::GetPercent()
{
	return this->percent;
}

bool KitItem::IsDeleted()
{
	return this->deleted;
}
void KitItem::SetDeletion()
{
	this->deleted = true;
}
void KitItem::UnsetDeletion()
{
	this->deleted = false;
}
bool KitItem::IsInDB()
{
	return (this->dbID != -1);
}
int KitItem::GetDBID()
{
	return this->dbID;
}
void KitItem::SetDBID( const int dbid )
{
	this->dbID = dbid;
}

void Kit::CopyFrom( Kit *Source, bool full )
{
	this->Name = Source->Name;
	this->KitItems = Source->KitItems;
	this->KitInventory = Source->KitInventory;

	if( full == true )
	{//Full copy
		this->vnum = Source->vnum;
		this->deleted = Source->deleted;
		this->in_db = Source->in_db;
	}
}
void Kit::Boot(sql::Row *MyRow, std::list<sql::Row> MyItems)
{
	this->Name = (*MyRow)["name"];
	this->vnum = atoi( (*MyRow)["vnum"].c_str() );

	//Now boot all of the kit items.
	for(std::list<sql::Row>::iterator kiIter = MyItems.begin();kiIter != MyItems.end();++kiIter)
	{
		unsigned int hierarchy = atoi( (*kiIter)["hierarchy"].c_str() );
		if( (hierarchy+1) > this->KitItems.size() )
		{
			unsigned int prev_size = this->KitItems.size();
			this->KitItems.resize( (hierarchy+1) );

			//Resize all the newly added kits.
			for(;prev_size < this->KitItems.size();++prev_size)
				this->KitItems[prev_size].resize(NUM_WEARS);
		}
		int pos  = atoi((*kiIter)["body_position"].c_str());
		int iv   = atoi((*kiIter)["obj_vnum"].c_str());
		int prob = atoi((*kiIter)["probability"].c_str());
		int id   = atoi((*kiIter)["id"].c_str());
		if( pos == -1 ) {//Inventory
			this->KitInventory.push_back( KitItem(-1, iv, prob, id) );
		}
		else {//Equipment
			this->KitItems[hierarchy][pos] = KitItem(pos, (real_object(iv) > -1 ? iv : -1), prob, id);
		}
	}
	in_db = true;
	KitManager::GetManager().AddKit(this);
}

void BootKits()
{
	std::stringstream Query;
	sql::Query MyQuery, SecondQuery;
	std::list< sql::Row > KitPositions;
	sql::Row MyRow, KR;

	try {
		gameDatabase->sendRawQuery("DELETE FROM kitItem WHERE kit_vnum NOT IN(SELECT vnum FROM kits);");
	} catch( sql::QueryException &e ) {
		Log("Could not delete kit item from non existent kit : %s", e.getErrorMessage().c_str());
	}

	Query << "SELECT * FROM kits WHERE 1 ORDER BY vnum ASC;";
	try {
		MyQuery = gameDatabase->sendQuery(Query.str());
		Query.str("");//Clear the old query text.

		Query << "SELECT * FROM kitItem WHERE 1 ORDER BY kit_vnum ASC;";
		SecondQuery = gameDatabase->sendQuery(Query.str());
	}
	catch (sql::QueryException &e) {
		e.report();
		exit(1);
	}

	while( MyQuery->hasNextRow() )
	{
		MyRow = MyQuery->getRow();
		KitPositions.clear();

		while( SecondQuery->hasNextRow() )
		{
			KR = SecondQuery->peekRow();
			if( KR["kit_vnum"] == MyRow["vnum"] )
				KitPositions.push_back(KR);
			else
				break;//Theses are ordered, so once we find a non-match, we know we need to break out.
			SecondQuery->skipRow();//Need to skip.
		}

		Kit *MyKit = new Kit();
		MyKit->Boot(&MyRow, KitPositions);
	}
}

//Galnor - Boot the warrants of the game from the warrant file.
void BootWarrants()
{
	sql::Query resultSet =
		gameDatabase->sendQuery
		(
			"SELECT"
			" `id`,"
			" `warrant_name` "
			"FROM warrant"
		);

	while(resultSet->hasNextRow())
	{
		sql::Row row = resultSet->getRow();

		int id = atoi( row["id"].c_str() );
		std::string warrantName = row["warrant_name"];
		new Warrant(id, warrantName);
	}

	Log("Booted %lld warrants.", (__int64)Warrants.size());
}

Warrant::Warrant()
{
	this->vnum = 0;
	this->Name.erase();
}

Warrant::Warrant(int v, const std::string &n)
{
	this->vnum = v;
	this->Name = n;
	Warrants.insert(Warrants.end(), this);
}

Warrant::~Warrant()
{
	Warrants.remove(this);
}

void Character::PasswordUpdated( bool val )
{
	this->pw_updated = val;
}
bool Character::PasswordUpdated()
{
	return (this->pw_updated);
}

int Character::getVnumOrID() const
{
	return (IS_NPC(this) ? getVnum() : player.idnum);
}
char Character::GetType() const
{
	return (IS_NPC(this) ? 'M' : 'C');
}

void Character::zero()
{
	this->js_scripts.reset();
	this->proto						=	false;
	this->PermissionToSnoop			=	false;
	this->purged					=	false;
	this->ignoreCommandTrigger		=	false;
	this->pw_updated				=	false;
	this->dizzy_time				=	0;
	this->Restat					=	false;
	this->CannotFinishCharge		=	false;
	this->ShieldBlock				=	false;
	this->InvertNextWeave			=	false;
	this->SlowedBy					=	0;
	this->DecayedBy					=	0;
	this->BurnedBy					=	0;
	this->PlaguedBy					=	0;
	this->Eavesdropping				=	0;
	this->points.temp_taint			=	0;
	this->ps_tgt					=	-1;
	this->nr						=	NOBODY;
	this->LoadData					=	0;
	this->in_room 					=	0;
	this->was_in_room				=	0;
	this->Stats						=	0;
	//Set these as the defaults. They'll otherwise be correctly set in Character::load()
	this->MobData					=	0;
	this->PlayerData				=	0;
	this->wait						=	0;
	this->pulverizeCooldown			=	0;

	// struct CharAbilityData real_abils
	memset(&(this->real_abils), 0, sizeof(this->real_abils));
	//struct CharAbilityData aff_abils
	memset(&(this->aff_abils), 0, sizeof(this->aff_abils));
	// struct CharPointData points

	this->affected					=	NULL;
	this->affection_list			=	NULL;
	this->carrying					=	NULL;
	this->desc						=	NULL;
	this->next_in_room				=	NULL;
	this->next						=	NULL;
	this->next_fighting				=	NULL;
	this->PokerData					=	NULL;
	this->PokerTable				=	NULL;
	this->NextSeated				=	NULL;
	this->followers					=	NULL;
	this->master					=	NULL;
	this->timer						=	0.0f;
	this->command_ready				=	false;
	this->pks						=	0;
	this->pk_deaths					=	0;
	this->mob_deaths				=	0;
	this->cmd_no					=	0;
	this->stance					=	0;
	this->delayed_command.erase();
	this->body_structure			=	STRUCT_LIGHT;
	this->last_tell					=	NOBODY;
	this->js_scripts				=	std::shared_ptr<std::vector<JSTrigger*> >( new std::vector< JSTrigger* > );

	for (int i = 0; i < NUM_WEARS; ++i)
		this->equipment[i]			=	NULL;

	this->last_tell					=	0;
}

void CharPointData::operator =(CharPointData &source)
{
	mana = source.mana;
	max_mana = source.max_mana;
	hit = source.hit;
	max_hit = source.max_hit;
	move = source.move;
	max_move = source.max_move;
	offensive = source.offensive;
	parry = source.parry;
	dodge = source.dodge;
	absorb = source.absorb;
	weave = source.weave;
	invis = source.invis;
	taint = source.taint;
	death_wait = source.death_wait;
	warning = source.warning;
	shadow_points = source.shadow_points;
	max_shadow_points = source.max_shadow_points;
	master_weapon = source.master_weapon;
	is_bashed = source.is_bashed;
	strain = source.strain;
	last_logon = source.last_logon;
	last_logout = source.last_logout;
	gold = source.gold;
	bank_gold = source.bank_gold;
	exp = source.exp;
	damroll = source.damroll;
	memcpy( &warrants, &source.warrants, sizeof( warrants ) );

	HitRolls.insert(this->HitRolls.end(), source.HitRolls.begin(), source.HitRolls.end());
	ManaRolls.insert(this->ManaRolls.end(), source.ManaRolls.begin(), source.ManaRolls.end());
}

Character::Character(const int nr, const int type, bool full_copy)
{
	++Character::nr_alloc;
	int i, p, ovnum = -1;
	Character *source;
	Object *obj;
	std::list<int>::iterator ai;

	if (type == VIRTUAL)
	{
		if ((i = MobManager::GetManager().RealMobile(nr)) < 0)
		{
			Log("Mobile (V) %d does not exist in database.", nr);
			this->zero();
			this->player.idnum = Character::top_mob_id--;
			return;
		}
	}
	else
		i = nr;
	source = (MobManager::GetManager().GetPrototype(i));

	this->MobData = source->MobData ? (new MobOnlyData(source->MobData)) : (0);
	this->PlayerData = source->PlayerData ? (new PlayerOnlyData(source->PlayerData)) : (0);

	if(IS_NPC(source))
		SET_BITK(MOB_FLAGS(source), Q_BIT(MOB_ISNPC));

	this->proto = false;
	this->ignoreCommandTrigger = false;
	this->pw_updated = false;
	this->userClans = ClanUtil::cloneUserClansFromMobPrototype(source);
	this->points = source->points;
	this->real_abils = source->real_abils;
	this->aff_abils = source->aff_abils;
	this->player.CopyFrom( &source->player, false );
	this->next_in_room = 0;
	this->next_fighting = 0;
	this->DecayedBy = 0;
	this->SlowedBy = 0;
	this->BurnedBy = 0;
	this->PlaguedBy = 0;
	this->LoadData = 0;
	this->PokerData = 0;
	this->PokerTable = 0;
	this->NextSeated = 0;
	this->followers = 0;
	this->master = 0;
	this->timer = source->timer;
	this->command_ready = false;
	this->Restat = false;
	this->purged = false;
	this->PermissionToSnoop = false;
	this->InvertNextWeave = false;
	this->dizzy_time = 0;
	this->stance = 0;
	this->Stats = 0;
	this->pk_deaths = 0;
	this->mob_deaths = 0;
	this->pks = 0;
	this->body_structure = source->body_structure;
	this->cmd_no = 0;
	this->last_tell = 0;
	this->desc = 0;
	this->nr = source->nr;
	this->was_in_room = 0;
	this->carrying = 0;
	this->affected = source->affected;
	this->affection_list = source->affection_list;
	this->ShieldBlock = false;
	this->CannotFinishCharge = false;
	this->ps_tgt = -1;
	this->Eavesdropping = 0;
	this->in_room = 0;
	this->wait = 0;
	this->player.idnum = -1;
	this->pulverizeCooldown = 0;
	memset(&this->equipment, 0, sizeof(this->equipment));
	//memcpy(&this->skills, &source->skills, sizeof(this->skills));

	this->next = character_list;
	character_list = this;

    this->js_scripts = source->js_scripts;

	this->points.max_hit = MiscUtil::dice(this->points.hit, this->points.mana) + this->points.move;

	this->points.hit = this->points.max_hit;
	this->points.mana = this->points.max_mana;
	this->points.move = this->points.max_move;

	this->player.time.birth.setTime(time(0));
	this->player.time.logon.setTime(time(0));
	this->player.time.played = 0;

	if(MOB_FLAGGED(this, MOB_INVIS))
	{
		GET_INVIS_LEV(this) = 100;
		SET_BIT_AR(AFF_FLAGS(this), AFF_INVISIBLE);
	}

	for(p = 0;p < MAX_SKILLS;++p)
		SET_SKILL(this, p, 99);
	this->SetSkillDefaults();

	/* Serai - go through kit and equip mobs on creation */
	if (source->MobData && source->MobData->primary_kit)
	{
		Kit *k = source->MobData->primary_kit;
		//Load the equipment items.
		for(unsigned int tt = 0; tt < (unsigned int)NUM_WEARS; ++tt)
		{
			if (source->MobData->primary_kit->KitItems[0][tt].GetItemVnum() != NOTHING)
			{
				for(unsigned int kit = 0; kit < source->MobData->primary_kit->KitItems.size(); ++kit)
				{
					if (source->MobData->primary_kit->KitItems[kit][tt].GetItemVnum() == NOTHING ||
					MiscUtil::random(0, 100) > source->MobData->primary_kit->KitItems[kit][tt].GetPercent())
						continue;

					ovnum = real_object(source->MobData->primary_kit->KitItems[kit][tt].GetItemVnum());

					if (ovnum < 0 || (obj_proto[ovnum]->Max != -1 && ItemCount[ovnum] >= obj_proto[ovnum]->Max))
						continue;

//					if(mob->CanWear((obj = read_object(ovnum, REAL, true)), tt))
//					{
					//FOGELMARKER
						obj = read_object(ovnum, REAL, true);
						equip_char(this, obj, tt);
						obj->creator = "kit for " + this->player.name;

						if( !obj->IsPurged() ) {
							js_load_triggers(obj);
						}
//					}
//					else
//						delete obj;
					break;
				}
			}
		}
		//Load the inventory items.
		for(unsigned int i = 0;i < k->KitInventory.size();++i)
		{
			if (k->KitInventory[i].GetItemVnum() == NOTHING || MiscUtil::random(0, 100) > k->KitInventory[i].GetPercent())
				continue;

			ovnum = real_object(k->KitInventory[i].GetItemVnum());

			if (ovnum < 0 || (obj_proto[ovnum]->Max != -1 && ItemCount[ovnum] >= obj_proto[ovnum]->Max))
				continue;
			obj = read_object(ovnum, REAL, true);
			obj_to_char(obj, this);
			obj->creator = "kit for " + this->player.name;

			if( !obj->IsPurged() ) {
				js_load_triggers(obj);
			}
		}
	}
	++MobManager::GetManager().GetIndex((unsigned int)i)->number;
	this->player.idnum = Character::top_mob_id--;
}

Character::Character(eCharType MyType)
{
	++Character::nr_alloc;
	this->zero();
	switch(MyType)
	{
	case CharMob:
		this->MobData = new MobOnlyData();
		SET_BITK(MOB_FLAGS(this), Q_BIT(MOB_ISNPC));
		for(int i = 0;i < MAX_SKILLS;++i)
			SET_SKILL(this, i, 99);
		this->nr = -1;
		this->player.idnum = Character::top_mob_id--;
		break;
	case CharPlayer:
		this->LoadData = new LoadOnlyData();
		this->PlayerData = new PlayerOnlyData();
		break;
	case CharNone:
		break;
	default:
		break;
	}
}

Character::Character()
{
	++Character::nr_alloc;
	this->zero();
}

void Character::DeleteAliases()
{
	//No longer needed since the aliases are a map.
}

void Character::DeleteQuests()
{
	for(std::list<Quest *>::iterator quest = this->quests.begin();quest != this->quests.end();)
	{
		delete (*quest);
		quest = this->quests.erase(quest);
	}
}

void Character::DeleteClans()
{
	ClanUtil::freeUserClans(userClans);
}

void Character::DeleteAffections()
{
	while(affection_list)
	{
		remove_affection_list(this, affection_list, TRUE);
	}
}

MobOnlyData::~MobOnlyData()
{
	if(this->Food)
		delete this->Food;
	if(this->Skin)
		delete this->Skin;
}

void Character::UnsetMobPrototypeSpecifics()
{
	this->wards.clear();
	this->quests.clear();
	this->ignores.clear();
	this->aliases.clear();
}

/* release memory allocated for a char struct */
Character::~Character()
{
	++Character::nr_dealloc;
	int i = 0;
	unsigned int s;

	for(std::list< Character* >::iterator ch = WaitingList.begin();ch != WaitingList.end();++ch)
	{
		if( (*ch) == this)
		{
			WaitingList.erase( ch );
			break;
		}
	}
	std::list< std::pair< Character *, event_info * > *>::iterator eIter;
	for(eIter = BashStandQueue.begin();eIter != BashStandQueue.end();)
	{
		if( (*eIter)->first == this ) {
			remove_event( (*eIter)->second );
			delete (*eIter);
			eIter = BashStandQueue.erase( eIter );

		}
		else ++eIter;
	}
	if( this->player.sitting_on != NULL )
	{
		this->player.sitting_on->SatOnBy = NULL;
		this->player.sitting_on = NULL;
	}

	if (this->PlayerData)
		delete this->PlayerData;
	if(this->in_room)
		this->RemoveFromRoom();

	JSManager::get()->handleExtraction( this );

	//Delete all zone command references to this mob.
	Zone *zone;
	for(i = 0;(zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;++i)
	{
		for(s = 0;s < zone->cmd.size();++s)
		{
			if(zone->cmd[s]->mob == this)
				zone->cmd[s]->mob = NULL;
		}
	}

	//We may have an immortal in the zone editor with a reference to this mob. Get rid of those as well!
	//Galnor 2014-07-05
	for(Descriptor *descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
	{
		if(STATE(descriptor) == CON_ZEDIT)
		{
			Zone *olcZone = descriptor->olc->zone;
			for(auto resetCommandIterator = olcZone->cmd.begin();resetCommandIterator != olcZone->cmd.end();++resetCommandIterator)
			{
				ResetCommand *resetCommand = (*resetCommandIterator);
				if(resetCommand->mob == this)
				{
					resetCommand->mob = NULL;
				}
			}
		}
	}

	DeleteAffections();

	if (PokerData)
	{
		if(PokerData->Table)
			PokerData->Table->RemovePlayer( this );
		else
		{
			delete PokerData;
		}
	}
	if( PokerTable != NULL )
	{
		PokerTable->IsWatching( this );
		PokerTable->RemoveWatcher(this);
	}

	if(MOUNT(this))
		this->Dismount();
	if(RIDDEN_BY(this))
		RIDDEN_BY(this)->Dismount();

	remove_affection_list(this, affection_list, TRUE);

	/*** Galnor 11/24/2009 - Eavesdropper needs to get removed, otherwise the ptr is invalid... ***/
	std::vector< Character* >::iterator cIter;
	if( this->Eavesdropping &&
(cIter = std::find(Eavesdropping->eavesdropping.begin(), Eavesdropping->eavesdropping.end(), this)) != Eavesdropping->eavesdropping.end() )
	{
		this->Eavesdropping->eavesdropping.erase( cIter );
	};

	/* if this is a player, or a prototyped NPC, free all */
	if(player.short_descr && (nr == -1 || (!IS_NPC(this) || this->GetPrototype()->player.short_descr != this->player.short_descr
	|| this->GetPrototype() == this)))
		delete[] player.short_descr;
	if(player.long_descr && (nr == -1 || (!IS_NPC(this) || this->GetPrototype()->player.long_descr != this->player.long_descr
	|| this->GetPrototype() == this)))
		delete[] player.long_descr;
	if(player.ArriveMessage && (nr == -1 || (!IS_NPC(this) || this->GetPrototype()->player.ArriveMessage != this->player.ArriveMessage
	|| this->GetPrototype() == this)))
		delete[] player.ArriveMessage;
	if(player.LeaveMessage && (nr == -1 || (!IS_NPC(this) || this->GetPrototype()->player.LeaveMessage != this->player.LeaveMessage
	|| this->GetPrototype() == this)))
		delete[] player.LeaveMessage;
	if(player.description && (nr == -1 || (!IS_NPC(this) || this->GetPrototype()->player.description != this->player.description
	|| this->GetPrototype() == this)))
		delete[] (player.description);
	DeleteQuests();
	DeleteClans();
	if(this->LoadData)
		delete this->LoadData;
	if(this->MobData)
		delete this->MobData;
	while (affected)
		affect_remove(this, affected);

	if (desc)
		desc->character = NULL;
}

/* release memory allocated for an obj struct */
void free_obj(Object * obj)
{
	delete (obj);
}
//Live objects may point to prototype-sensitive material. We need to reserve that.
void Object::FreeLiveObject()
{
	delete (this);
}

/* read contents of a text file, and place in buf */
int file_to_string(const char *name, char *buf)
{
	FILE *fl;
	char tmp[READ_SIZE+3];

	*buf = '\0';

	if (!(fl = fopen(name, "r")))
	{
		sprintf(tmp, "SYSERR: reading %s", name);
		perror(tmp);
		return (-1);
	}

	do
	{
		char *resultValue = fgets(tmp, READ_SIZE, fl);
		tmp[strlen(tmp) - 1] = '\0'; /* take off the trailing \n */
		strcat(tmp, "\r\n");

		if(resultValue == NULL)
		{
			sprintf(tmp, "SYSERR: Error while reading file %s.", name);
			perror(tmp);
			return (-1);
		}

		if (!feof(fl))
		{
			if (strlen(buf) + strlen(tmp) + 1 > MAX_STRING_LENGTH)
			{
				Log("SYSERR: %s: std::string too big (%lld max)", name,
				    (__int64)MAX_STRING_LENGTH);
				*buf = '\0';
				return -1;
			}

			strcat(buf, tmp);
		}
	}

	while (!feof(fl));
	fclose(fl);
	return (0);
}

int topUserId() {

	sql::Query MyQuery;
	try {MyQuery = gameDatabase->sendQuery("SELECT MAX(user_id) FROM users");}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return -1;
	}
	int i = (MyQuery->numRows() ? atoi(MyQuery->getRow()[0].c_str()) : 0);
	return i;
}

int PlayerBaseCount()
{
	sql::Query MyQuery;
	try {MyQuery = gameDatabase->sendQuery("SELECT COUNT(user_id) FROM users");}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return -1;
	}
	int i = (MyQuery->numRows() ? atoi(MyQuery->getRow()[0].c_str()) : 0);
	return i;
}

/* initialize a new character only if class is set */
void Character::Init()
{
	int i, taeller;

	this->body_structure = MiscUtil::random(STRUCT_LIGHT, STRUCT_HEAVY);
	this->RollWeightAndHeight();
	this->RollMoves();

	//this->char_specials.saved.affected_by = 0;
	for(taeller=0; taeller < AF_ARRAY_MAX; taeller++)
		this->player.affected_by[taeller] = 0;

	this->real_abils.intel = this->StatMinimum(STAT_INT);
	this->real_abils.wis = this->StatMinimum(STAT_WIS);
	this->real_abils.dex = this->StatMinimum(STAT_DEX);
	this->real_abils.str = this->StatMinimum(STAT_STR);
	this->real_abils.con = this->StatMinimum(STAT_CON);

	for (i = 0; i < 3; ++i)
		this->PlayerData->conditions[ i ] = (GET_LEVEL(this) == LVL_IMPL ? -1 : 24);

	GET_LEVEL(this)		= 1;
	GET_EXP(this)		= 1;
	this->GainLevel( TRUE );
	this->ResetHitRolls(true);
	this->ResetManaRolls(true);

	this->points.mana	= this->points.max_mana;
	this->points.hit	= this->points.max_hit;
	this->points.move	= this->points.max_move;

	this->PlayerData->conditions[ THIRST ] = 24;
	this->PlayerData->conditions[ FULL ] = 24;
	this->PlayerData->conditions[ DRUNK ] = 0;

	SET_BITK( PLR_FLAGS( this ), Q_BIT(PLR_SITEOK) );

	if( this->player.time.birth.getTime() != 0 )
	{
		for(i = 0;i < NB_ARRAY_MAX;++i)
			SET_BIT_AR(NEWB_FLAGS(this), i);
	}//If this player is just restatting, we'd like to keep the flags as they were.
	else
	{//Things we want to happen only if this is the first initialization
		this->player.time.birth.setTime(time(0));
		this->player.time.logon.setTime(time(0));
		this->PlayerData->load_room = NOWHERE;
	}
	this->ResetAllSkills();
}

Room *FindRoomByVnum(unsigned int vnum)
{
	int top, bot, mid;

	bot = 0;
	top = World.size() - 1;

	for(;;)
	{
		mid = (bot + top) / 2;
		if ((unsigned int)World[mid]->getVnum() == vnum)
			return World[mid];
		if(bot >= top)
			return 0;
		if ((unsigned int)World[mid]->getVnum() > vnum)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}

/* returns the real number of the room with given virtual number */
int real_room(unsigned int vnum)
{
	unsigned int bot, top, mid;
	if( World.size() == 0 )
		return NOWHERE;

	bot = 0;
	top = World.size() - 1;

	/* perform binary search on world-table */
	for (;;)
	{
		mid = (bot + top) / 2;
		if ((unsigned int)World[mid]->getVnum() == vnum)
			return (int)(mid);
		if (bot >= top)
			return NOWHERE;
		if ((unsigned int)World[mid]->getVnum() > vnum)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}

/* returns the real number of the object with given virtual number */
int real_object(int vnum)
{
	int bot, top, mid;

	bot = 0;
	top = top_of_objt;

	/* perform binary search on obj-table */
	for (;;)
	{
		mid = (bot + top) / 2;

		if ((obj_index + mid)->vnum == vnum)
			return (mid);
		if (bot >= top)
			return (-1);
		if ((obj_index + mid)->vnum > vnum)
			top = mid - 1;
		else
			bot = mid + 1;
	}
}

void eatwhite(std::istream &fin)
{
	while(isspace(fin.peek()) || fin.peek() == '\n')
		fin.ignore();
}

void EatUntil(std::istream &fin, char c, bool EatMarker)
{
	while(fin.peek() != c)
		fin.ignore();

	if(EatMarker)
		fin.ignore();
}

bool Character::TimedOut(const int days)
{
	return (GET_LEVEL(this) <= 5 && days >= (7 * 2));
}

void MySQLDeleteAll(const std::string &playername)
{
	return;//Temporarily disabled.
	std::string Query;
	std::list<std::string> TableList = gameDatabase->getTableList();
	long id=-1;

	sql::Query MyQuery;

	try {
		MyQuery = gameDatabase->sendQuery("SELECT user_id FROM users WHERE username='" + sql::escapeString(playername) + "';");
	} catch( sql::QueryException &e ) {
		e.report();
		return;
	}

	if( MyQuery->numRows() ) {
		id = atoi(MyQuery->getRow()["user_id"].c_str());
	}

	for(std::list<std::string>::iterator sIter = TableList.begin();sIter != TableList.end();++sIter)
	{
		if( (*sIter) == "users" )
			Query = "DELETE FROM " + (*sIter) + " WHERE username = '" + playername + "'";
		else
			Query = "DELETE FROM " + (*sIter) + " WHERE name = '" + playername + "'";
		try {
			gameDatabase->sendRawQuery(Query);
		}
		catch( sql::QueryException &e )
		{
			//...Not a problem
		}
	}

	//We need to get rid of some of the rows in the forum databases.
	try {
		gameDatabase->sendRawQuery("DELETE FROM forums_banlist WHERE ban_userid='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("DELETE FROM forums_sessions WHERE session_user_id='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("DELETE FROM forums_user_group WHERE id='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("DELETE FROM forums_topics_watch WHERE id='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("UPDATE forums_vote_voters SET vote_user_id='-1' WHERE vote_user_id='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("UPDATE forums_privmsgs SET privmsgs_to_userid='-1' WHERE privmsgs_to_userid='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("UPDATE forums_posts SET poster_id='-1' WHERE poster_id='"
			+ MiscUtil::convert<std::string>(id) + "';");
		gameDatabase->sendRawQuery("UPDATE forums_topics SET topic_poster='-1' WHERE topic_poster='"
			+ MiscUtil::convert<std::string>(id) + "';");
	} catch( sql::QueryException &e ) {
		e.report();
	}
}

void Character::CheckWiz()
{
	class Wizard *wiz;

	if(GET_LEVEL(this) >= LVL_IMMORT)
	{
		wiz			= new Wizard;
		wiz->name	= GET_NAME(this);
		wiz->level	= GET_LEVEL(this);
		wiz->next	= wizlist;
		wizlist		= wiz;
	}
}

//Cycle the playerfile one time and do different boot processes all in one go.
void PlayerFileCycle(void)
{
	Character *ch = new Character(CharPlayer);
	sql::Query MyQuery;
	sql::Row MyRow;

	try {
		MyQuery = gameDatabase->sendQuery(
			"SELECT"
			" `username`,"
			" `weave`,"
			" `user_id`,"
			" `last_logon`,"
			" `level`,"
			" `plr`,"
			" `prf` "
			"FROM users "
			"ORDER BY weave DESC"
		);
	}
	catch( sql::QueryException &e )
	{
		e.report();
		MudLog(BRF, LVL_IMPL, TRUE, e.message.c_str());
		return;
	}

	std::list<int> userIdDeletionList;
	for(unsigned int i = 0;i < MyQuery->numRows();++i)
	{
		MyRow = MyQuery->getRow();
		ch->player.name = MyRow["username"];
		ch->points.weave = atoi(MyRow["weave"].c_str());
		LAST_LOGON(ch) = MyRow.getTimestamp("last_logon");
		ch->player.level = atoi(MyRow["level"].c_str());
		ch->player.idnum = atoi(MyRow["user_id"].c_str());

		PLR_FLAGS(ch) = atoi(MyRow["plr"].c_str());
		ConvertBitvectorFromString(MyRow["prf"].c_str(), PRF_FLAGS(ch), sizeof(PRF_FLAGS(ch)));

		if( (PLR_FLAGGED(ch, PLR_DELETED)) ||
		(ch->TimedOut(((time(0) - LAST_LOGON(ch)) / SECS_PER_REAL_DAY)) ))
		{
			if(!PLR_FLAGGED(ch, PLR_NODELETE)) {
				userIdDeletionList.push_back(ch->player.idnum);
				continue;
			}
		}

		ch->CheckWiz();
		ch->UpdateLegendEntry();
	}

	if(userIdDeletionList.empty() == false)
	{
		try
		{
			std::stringstream queryBuffer;
			
			queryBuffer << "DELETE userArchive.* "
						<< "FROM userArchive, users "
						<< "WHERE userArchive.user_id=users.user_id";

			gameDatabase->sendRawQuery(queryBuffer.str());

			queryBuffer.str("");

			queryBuffer << "INSERT INTO userArchive "
						<< "("
						<< " SELECT * "
						<< " FROM users "
						<< " WHERE user_id "
						<< " IN(" << StringUtil::implode(userIdDeletionList, ",") << ")"
						<< ");";

			gameDatabase->sendRawQuery(queryBuffer.str());

			queryBuffer.str("");

			sql::BatchInsertStatement batchInsertStatement(gameDatabase, "userDeletion", 1000);

			batchInsertStatement.addField("user_id");

			batchInsertStatement.addField("created_datetime");

			batchInsertStatement.start();
			std::string nowTimestamp = DateTime::parse("%Y-%m-%d %H:%M:%S", DateTime());

			for(std::list<int>::iterator iter = userIdDeletionList.begin();iter != userIdDeletionList.end();++iter) {

				batchInsertStatement.beginEntry();

				batchInsertStatement.putInt( (*iter) );

				batchInsertStatement.putString(nowTimestamp);

				batchInsertStatement.endEntry();
			}

			batchInsertStatement.finish();

			queryBuffer << "DELETE FROM users "
						<< "WHERE user_id IN(" << StringUtil::implode(userIdDeletionList, ",") << ")";

			gameDatabase->sendRawQuery(queryBuffer.str());
		}
		catch(sql::QueryException &e)
		{
			MudLog(BRF, LVL_APPR, TRUE, "PlayerFileCycle() : Unable to archive deleted users : %s", e.getMessage().c_str());
			exit(1);
		}
	}

	delete ch;
}

UserType *Character::getUserType()
{
	if(IS_NPC(this))
		return UserType::mob;
	return UserType::player;
}

int Character::getUserId()
{
	UserType *userType = getUserType();
	if(userType == UserType::mob)
		return this->getVnum();
	return this->player.idnum;
}

bool Character::passwordMatches(const std::string &passwordInput)
{
	bool passwordUpdated = this->PasswordUpdated();

	return !((!passwordUpdated && strncmp( CRYPT(passwordInput.c_str(), this->player.passwd.c_str()), GET_PASSWD(this).c_str(), 10 )) ||
	(passwordUpdated &&	GET_PASSWD(this).compare( MD5::getHashFromString(passwordInput.c_str())) ));
}

const char *Character::getDisplayName()
{
	return GET_NAME(this);
}

const char *Room::getDisplayName()
{
	return name;
}

const char *Object::getDisplayName()
{
	return this->GetSDesc();
}

std::string Object::getDisplayableId()
{
	return MiscUtil::toString(objID);
}
Room *Object::getRoom()
{
	Character * ch;

	if (this->worn_by)
		return this->worn_by->in_room;
	else if (this->carried_by)
		return this->carried_by->in_room;
	else if ((ch = this->FindHolder()))
		return ch->in_room;
	else if (this->in_obj)
		return this->in_obj->getRoom();
	else
		return this->in_room;
}

class EntityType *Object::getEntityType()
{
	return EntityType::object;
}

std::string Character::getDisplayableId()
{
	return MiscUtil::toString(this->player.idnum);
}

Room *Character::getRoom()
{
	return in_room;
}

class EntityType *Character::getEntityType()
{
	if (getUserType() == UserType::player)
		return EntityType::player;
	return EntityType::mob;
}

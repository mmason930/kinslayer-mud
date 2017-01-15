/* ************************************************************************
*   File: comm.c                                        Part of CircleMUD *
*  Usage: Communication, socket handling, main(), central game loop       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __COMM_C__

#include "conf.h"
#include "sysdep.h"
#include "screen.h"
#include "constants.h"


#ifdef CIRCLE_MACINTOSH /* Includes for the Macintosh */
# define SIGPIPE 13
# define SIGALRM 14
/* GUSI headers */
# include <sys/ioctl.h>
/* Codewarrior dependant */
# include <SIOUX.h>
# include <console.h>
#endif

#ifdef CIRCLE_WINDOWS
# include <mmsystem.h>
#endif

#ifdef CIRCLE_AMIGA /* Includes for the Amiga */
# include <sys/ioctl.h>
# include <clib/socket_protos.h>
#endif /* CIRCLE_AMIGA */

#ifdef CIRCLE_ACORN /* Includes for the Acorn (RiscOS) */
# include <socklib.h>
# include <inetlib.h>
# include <sys/ioctl.h>
#endif

/*
* Note, most includes for all platforms are in sysdep.h.  The list of
* files that is included is controlled by conf.h for that platform.
*/

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "olc.h"
#include "poker.h"
#include "clans.h"
#include "shop.h"
#include "weaves.h"
#include "dg_event.h"
#include "stats.h"
#include "mobs.h"
#include "js.h"
#include "auction.h"
#include "threaded_jobs.h"
#include "JSRow.h"
#include "accounts.h"
#include "StringUtil.h"
#include "MiscUtil.h"
#include "ForumUtil.h"
#include "UserLogoutType.h"
#include "GatewayServer.h"
#include "Descriptor.h"
#include "rooms/Room.h"
#include "GatewayDescriptorType.h"
#include "UserType.h"
#include "rooms/RoomSector.h"
#include "PlayerPortalDescriptorStatus.h"
#include "OlcEditType.h"
#include "EntityType.h"
#include "Game.h"
#include "SystemUtil.h"

#include "ObjectMoveLogger.h"
#include "MobLoadLogger.h"
#include "editor-interface/EditorInterfaceInstance.h"
#include <boost/filesystem.hpp>

#include "commands/infrastructure/CommandUtil.h"
#include "commands/infrastructure/CommandInfo.h"
#include "commands/infrastructure/Social.h"

#ifdef HAVE_ARPA_TELNET_H
#include <arpa/telnet.h>
#else
#include "telnet.h"
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

bool rebootNotified15=false, rebootNotified10=false, rebootNotified5=false, rebootNotified1=false;

/* externs */
extern struct legend_data legend[ 8 ];
extern char *startup;
extern int circle_restrict;
extern int MAX_PLAYERS;
extern Object *object_list;
extern Character *character_list;
extern struct Index *obj_index;
extern class event_info *event_list;
extern time_t time_of_boot_high;
extern int top_of_objt;
extern reset_q_type reset_q;
kuDescriptor *gatewayConnection;
std::string subroutine;

extern char *credits;
extern char *motd;
extern char *imotd;
extern char *background;
extern char *startup;

extern std::thread *objectMoveLoggerThread;
extern ObjectMoveLogger objectMoveLogger;

struct PendingSession
{
public:
	std::string sessionKey;
	DateTime createdDatetime;
	std::string host;
	GatewayDescriptorType *gatewayDescriptorType;
};

extern std::list<PendingSession> pendingSessions;

int GLOBAL_RESET = 0;
SwitchManager* SwitchManager::Self = NULL;
FILE *logfile = NULL; /* Where to send the log messages. */

void SetupMySQL( bool crash_on_failure );

/* In db.c */
extern struct GameTime time_info; /* In db.c */
extern const char *save_info_msg[];   /* In olc.c */
extern std::list<Track *> TrackList;
extern class Wizard *wizlist;
extern std::list<Warrant *>	Warrants;
extern Clan *ClanList;
extern Clan *ClanEnd;
extern Shop *shop_index;

int gameSessionId;

/* local globals */
kuListener *listener;
Descriptor *descriptor_list = NULL; /* master desc list */
std::list< Character* > WaitingList;
int circle_shutdown = 0; /* clean shutdown */
int no_specials = 0; /* Suppress ass. of special routines */
int max_players = 0; /* max descriptors available */
int tics = 0; /* for extern checkpointing */
int Seconds = 0;
DateTime rebootTime;
unsigned int tip_on = 1;
unsigned long int bandwidth = 0;
long int LongestPulse = 0, AveragePulese = 0, NumPulses = 0;
clock_t GlobalClock = 0;
void check_fighting( void );
void CheckWaitState( void );
struct timeval null_time; /* zero-valued time structure */
void *Pointer = 0;
int gamePort;

time_t bootTime;
int pulse;

/* functions in this file */
RETSIGTYPE unrestrict_game( int sig );
RETSIGTYPE reap( int sig );
RETSIGTYPE checkpointing( int sig );
RETSIGTYPE hupsig( int sig );

int performAlias( Descriptor *d, char *orig );
void checkIdlePasswords( void );
void heartbeat( int pulse );
void checkTimers( void );
void recordUsage( void );
void initiateGame( int port );
void signal_setup( void );
void gameLoop();
void performFlee( Character *ch );
void destroyPlayerIndex();
void startGameSession();
void endGameSession();

struct timeval *timediff( struct timeval a, struct timeval b );
struct timeval *timeadd( struct timeval a, struct timeval b );
#if defined(POSIX)
sigfunc *my_signal( int signo, sigfunc * func );
#endif

/* extern fcnts */
extern void bootWorld( void );
extern void affect_update( void ); /* In spells.c */
extern void mobileActivity( void );
extern void string_add( Descriptor *d, char *str );
extern void perform_violence( void );
extern void show_string( Descriptor *d, char *input );
extern void weather_and_time( int mode );
extern void sedit_save_to_disk();
extern void rebootCountdown( void );
extern const char *pc_class_types[];
extern MeleeMessageList fight_messages[];


extern sql::Connection gameDatabase;

#ifdef __CXREF__
#undef FD_ZERO
#undef FD_SET
#undef FD_ISSET
#undef FD_CLR
#define FD_ZERO(x)
#define FD_SET(x, y) 0
#define FD_ISSET(x, y) 0
#define FD_CLR(x, y)
#endif

/***********************************************************************
*  main game loop and related stuff                                    *
***********************************************************************/


#if defined(CIRCLE_WINDOWS) || defined(CIRCLE_MACINTOSH)

/* Windows doesn't have gettimeofday, so we'll simulate it. */
/* The Mac doesn't have gettimeofday either. */
void gettimeofday( struct timeval *t, struct timezone *dummy )
{

#if defined(CIRCLE_WINDOWS)
	DWORD millisec = GetTickCount();

#elif defined(CIRCLE_MACINTOSH)

	unsigned long int millisec;
	millisec = ( int ) ( ( float ) TickCount() * 1000.0 / 60.0 );

#endif

	t->tv_sec = ( int ) ( millisec / 1000 );
	t->tv_usec = ( millisec % 1000 ) * 1000;
}

#endif /* CIRCLE_WINDOWS || CIRCLE_MACINTOSH */



/********** INT MAIN() *************/

void exportScripts()
{
	std::stringstream queryBuffer;

	queryBuffer << " SELECT REPLACE(text, '\r', '') AS text, vnum, name"
				<< " FROM js_scripts";

	sql::Query query = gameDatabase->sendQuery(queryBuffer.str());
	sql::Row row;

	while(query->hasNextRow())
	{
		row = query->getRow();

		int triggerVnum = row.getInt("vnum");
		std::string triggerName = row.getString("name");
		std::string body = row.getString("text");

		std::string scriptBuffer;

		if(triggerVnum >= 0)
		{
			scriptBuffer = (std::string("var script") + MiscUtil::convert<std::string>(triggerVnum) + std::string(" = function(self, actor, here, args, extra)\n{\n\t") + body + "\n};\n\n");
		}
		else
			scriptBuffer = body;
		
		StringUtil::replace(triggerName, " ", "");
		StringUtil::replace(triggerName, "/", "");
		StringUtil::replace(triggerName, "[", "");
		StringUtil::replace(triggerName, "]", "");
		StringUtil::replace(triggerName, ".", "_");
		StringUtil::replace(triggerName, "(", "");
		StringUtil::replace(triggerName, ")", "");

		std::string fileName = MiscUtil::toString(triggerVnum) + std::string("_") + triggerName + std::string(".js");

		Log("Exporting `%s`...", fileName.c_str());

		std::ofstream outFile(std::string("scripts/") + fileName);

		outFile << scriptBuffer;

		outFile.close();
	}
}

int main( int argc, char **argv )
{
#if (defined WIN32 && defined _DEBUG && defined _MEM_LEAKS )
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	game = new Game();
	int port = 0;
	int pos = 1;
	std::string dir = game->getDefaultDirectory();

	// It would be nice to make this a command line option but the parser uses
	// the log() function, maybe later. -gg
	logfile = fdopen( STDERR_FILENO, "w" );
	if ( logfile == NULL )
	{
		std::cout << "error opening log file stderr: " << strerror(errno) << std::endl;
		exit(1);
	}

	Log("%s", game->getVersion().c_str());

	// Serai - 06/18/04 - copyover
	game->setName(argv[ 0 ]);

	while ( ( pos < argc ) && ( *( argv[ pos ] ) == '-' ) )
	{

		switch ( *( argv[ pos ] + 1 ) )
		{
		case '-':
		{//Sub Routine.

			subroutine = argv[pos] + 2;
			break;
		}
		case 'd':
			if ( *( argv[ pos ] + 2 ) )
				dir = std::string(argv[ pos ] + 2);

			else if ( ++pos < argc )
				dir = std::string(argv[ pos ]);

			else
			{
				Log( "SYSERR: Directory arg expected after option -d." );
				exit( 1 );
			}

			break;

		case 'r':
			circle_restrict = 1;
			Log( "Restricting game -- no new players allowed." );
			break;

		default:
			Log( "SYSERR: Unknown option -%c in argument string.", *( argv[ pos ] + 1 ) );
			break;
		}
		pos++;
	}

	if ( pos < argc )
	{
		if ( !isdigit( *argv[ pos ] ) )
		{
			Log( "Usage: %s [-r] [-d pathname] [port #]\n", argv[ 0 ] );
			exit( 1 );
		}

		else if ( ( port = atoi( argv[ pos ] ) ) <= 1024 )
		{
			Log( "SYSERR: Illegal port number %d.", port );
			exit( 1 );
		}
	}

	Log("Using %s as data directory.", dir.c_str());
	if ( chdir( dir.c_str() ) < 0 )
	{
		int retval = chdir( "../" );
		if( chdir( dir.c_str() ) < 0 )
		{
			perror( "SYSERR: Fatal error changing to data directory" );
			exit( 1 );
		}
	}

	game->setupFilesystem();
	game->loadBasicConfig();
	game->setupPlayerPortalServer(atoi(game->getBasicConfigValue("Player Portal Server Port").c_str()));

	if(game->monitorRepo())
		game->loadSubversionInfo();

	if(!port && game->hasBasicConfiguration("MUD Port"))
		port = atoi(game->getBasicConfigValue("MUD Port").c_str());
	else
	{
		Log("SYSERR: No port specified using either -q switch or in BasicConfig file.");
		exit(1);
	}

	//Check to see if we need to perform a subroutine.
	if(subroutine.empty() == false)
	{
		Log("Establishing database connection...");
		SetupMySQL(true);

		if(!strcmp(subroutine, "ImportPlayerLogs"))
		{
			try
			{
				MudLog(BRF, 100, TRUE, "Importing player logs.");
				DateTime currentDatetime;
				std::stringstream sqlBuffer;
				
				boost::filesystem::path playerLogDirectoryPath(game->getPlayerLogsDirectory());
				boost::filesystem::directory_iterator end;
				for( boost::filesystem::directory_iterator iter(playerLogDirectoryPath) ; iter != end ; ++iter )
				{
					std::string fileName = (*iter).path().filename().string();
					std::string::size_type pos;

					pos = fileName.find('_');

					int userId, year, month, day;

					userId = atoi(fileName.substr(0, pos).c_str());
					year = atoi(fileName.substr(pos+1, 4).c_str());
					month = atoi(fileName.substr(pos+5, 2).c_str());
					day = atoi(fileName.substr(pos+7, 2).c_str());

					DateTime fileDatetime;
				
					fileDatetime.setYear(year);
					fileDatetime.setMonth(month);
					fileDatetime.setDay(day);

					if(!currentDatetime.sameDay(fileDatetime))
					{
						try {
							MudLog(BRF, 100, TRUE, "Processing `%s`...", (*iter).path().string().c_str());
							std::ifstream fileStream((*iter).path().string());
							std::string logBuffer;

							fileStream.seekg(0, std::ios::end);   
							logBuffer.reserve(fileStream.tellg());
							fileStream.seekg(0, std::ios::beg);

							logBuffer.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

							fileStream.close();
							sqlBuffer.str("");
							sqlBuffer
								<< " INSERT INTO userLog("
								<< "   `user_id`,"
								<< "   `console_output`,"
								<< "   `record_date`"
								<< " ) VALUES ("
								<< userId << ","
								<< sql::escapeQuoteString(logBuffer) << ","
								<< sql::encodeQuoteDate(fileDatetime.getTime()) << ")";

							gameDatabase->sendRawQuery(sqlBuffer.str());

							remove((*iter).path().string().c_str());
						}
						catch(sql::QueryException exception)
						{
							MudLog(BRF, 100, TRUE, "Could not store log. Error: %s", exception.getMessage().c_str());
						}
					}
					else
					{
						MudLog(BRF, 100, TRUE, "Skipping `%s`.", (*iter).path().string().c_str());
					}
				}
			}
			catch(std::exception e)
			{
				MudLog(BRF, 100, TRUE, "Error while importing: %s", e.what());
			}

			MudLog(BRF, 100, TRUE, "Finished processing player logs.");
		}
		else if(!strcmp(subroutine, "ExportScripts"))
		{
			exportScripts();
		}
		else
		{
			Log("Unknown subroutine `%s`.", subroutine.c_str());
		}

		return 0;
	}

	else
	{
		Log( "Running game on port %d.", port );
		initiateGame( port );
	}

	return 0;
}

void onDescriptorOpen(void *data, kuListener *listener, kuDescriptor *descriptor)
{
	if(gatewayConnection == NULL) {

		return;
	}
	static int last_desc = 0;

	Descriptor *newd, *d;

	newd = new Descriptor();

	newd->descriptor = descriptor;

	strcpy(newd->host, descriptor->getHost().c_str());

	newd->setGatewayDescriptorType(GatewayDescriptorType::unknown);
	newd->connected = CON_GATEWAY;
	newd->idle_tics = 0;
	newd->wait = 0;
	newd->loginTime = DateTime();
	newd->hadInput = false;
	newd->hadOutput = false;

	newd->desc_num = last_desc++;

	// prepend to list
	newd->next = descriptor_list;
	descriptor_list = newd;
}

void onDescriptorClose(void *data, kuListener *listener, kuDescriptor *descriptor)
{
	if(descriptor == gatewayConnection) {

		Log("Gateway connection has been closed.");

		gatewayConnection = NULL;
	}

	for(Descriptor *d = descriptor_list;d;d = d->next) {

		if(d->descriptor == descriptor) {

			d->descriptor = NULL;
			d->disconnect();
			break;
		}
	}
}

void waitForGatewayConnection() {

	while(gatewayConnection == NULL) {

		listener->acceptNewHosts();

		listener->pulse();

		std::list<kuDescriptor*> descriptors = listener->getDescriptors();

		for(std::list<kuDescriptor*>::iterator iter = descriptors.begin();iter != descriptors.end();++iter) {

			kuDescriptor *desc = (*iter);

			if(desc->hasCommand() == false) {

				continue;
			}

			std::string input = desc->getCommand();

			std::vector<std::string> vArgs = StringUtil::getArgVector(input);

			std::string command = vArgs.at(0);

			if(command == "Validate")
			{
				std::string passwordExpected = "78fd516c2825e7f463f045e609a8523e";

				if(vArgs.at(1) == passwordExpected) {

					gatewayConnection = desc;
					std::stringstream processIdMessage;
					processIdMessage << "ProcessID " << SystemUtil::getProcessId() << std::endl;
					desc->socketWriteInstant(processIdMessage.str());
					break;
				}
			}
			else {

				desc->socketClose();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	//Kick out all connections except for our gateway.

	for (kuDescriptor *desc : listener->getDescriptors())
	{
		if(desc != gatewayConnection) {

			desc->socketClose();
		}
	}
}

void processGatewayCommand(const std::string &input)
{
	try {
		std::vector<std::string> vArgs = StringUtil::getArgVector(input);

		std::string command = vArgs.at(0);

		if(command == "Host")
		{
			std::string clientId = vArgs.at(1);
			std::string host = vArgs.at(2);
			GatewayDescriptorType *gatewayDescriptorType = (GatewayDescriptorType*)GatewayDescriptorType::getEnumByValue(atoi(vArgs.at(3).c_str()));

			PendingSession pendingSession;

			pendingSession.createdDatetime = DateTime();
			pendingSession.host = host;
			pendingSession.sessionKey = StringUtil::getRandomString(40);
			pendingSession.gatewayDescriptorType = gatewayDescriptorType;

			pendingSessions.push_back(pendingSession);

			std::stringstream responseBuffer;

			responseBuffer << "Session " << clientId << " " << pendingSession.sessionKey << "\r\n";

			gatewayConnection->send(responseBuffer.str());
		}
		else if(command == "Ping") {

			gatewayConnection->send("PingResponse " + DateTime::parse("%Y-%m-%d %H:%M:%S", DateTime()) + "\n");
		}
	}
	catch(std::out_of_range e) {

		Log("Invalid input received from gateway: %s", input.c_str());
	}
}

void onBeforeSocketWrite(void *data, kuListener *listener, kuDescriptor *descriptor)
{
}

void onAfterSocketWrite(void *data, kuListener *listener, kuDescriptor *descriptor, const std::string &output)
{
	Descriptor *d;

	for(d = descriptor_list;d;d = d->next)
	{

		if(d->descriptor == descriptor) {

			break;
		}
	}

	if(d == NULL) {
		//Descriptor we are writing to, for some reason, was not found.

		return;
	}

	if(output.empty() == false) {

		d->hadOutput = true;

		//if(d->snoop_by && d->snoop_by->descriptor && d->snoop_by->character && d->snoop_by->character->hasPermissionToSnoop()) {

		//	d->snoop_by->send((std::string("% ") + output).c_str());
		//}
	}
}

void onSocketRead(void *data, kuDescriptor *descriptor, const std::string &input)
{
/**
	Descriptor *d;

	for(d = descriptor_list;d;d = d->next)
	{
		if(d->descriptor == descriptor)
			break;
	}

	if(d == NULL) {

		return;
	}

	if(d->snoop_by) {

		d->snoop_by->send(" % %d", input.c_str());
	}
**/
}

/* Init sockets, run game, and cleanup sockets */
void initiateGame( int port )
{
	gamePort = port;

	srand( time( 0 ) );

	listener = new kuListener(port, TCP);

	if( listener->isListening() == false ) {

		Log("Could not bind to port %d.", port);

		exit(1);
	}

	listener->setCloseDescriptorCallback(&onDescriptorClose);

	listener->setOpenDescriptorCallback(&onDescriptorOpen);

	listener->setBeforeSocketWriteCallback(&onBeforeSocketWrite);
	
	listener->setAfterSocketWriteCallback(&onAfterSocketWrite);

	if(!(listener->enableKeepAlive()))
	{
		Log("Could not enable keepalive on socket.");
	}

//	listener->setSocketReadCallback(&onSocketRead);

#ifdef WIN32
	Log("Boosting thread priority.");
	HANDLE pid = GetCurrentProcess();
	DWORD priority = GetPriorityClass(pid);
//	SetPriorityClass(pid, NORMAL_PRIORITY_CLASS);
	SetPriorityClass(pid, REALTIME_PRIORITY_CLASS);

#endif

	Log("Waiting for gateway to establish connection.");
	waitForGatewayConnection();

	//TODO:
	//	1) Obtain a list of players to load back into the game.
	//	2) 

	try
	{
		boot_db();
	}
	catch(sql::QueryException queryException)
	{
		MudLog(BRF, 0, TRUE, "Could not boot game. SQL Exception: %s", queryException.getMessage().c_str());

		throw queryException;
	}

	Log("Reboot set for %s.", rebootTime.toString().c_str());

#ifdef CIRCLE_UNIX

	Log( "Signal trapping." );
	signal_setup();
#endif

	UpdateBootHigh(0, true);

	if(gatewayConnection != NULL) {

		gatewayConnection->socketWriteInstant("FinishedBooting\n");
	}

	Log( "Entering game loop." );
	gameLoop();

	listener->pulse();

	//Tell the gateway server that we're rebooting.
	if(gatewayConnection != NULL) {

		gatewayConnection->socketWriteInstant("Reboot\n");
	}

	sedit_save_to_disk();

	// Save OLC before freeing everything..
	if ( olc_save_list )
	{ /* Don't save zones. */
		struct olc_save_info * entry, *next_entry;
		Zone *zone;

		for ( entry = olc_save_list; entry; entry = next_entry )
		{
			next_entry = entry->next;

			if ( entry->type < 0 || entry->type > 4 )
			{
				sprintf( buf, "OLC: Illegal save type %d!", entry->type );
				Log( "%s", buf );
			}
			else if ( !(zone = ZoneManager::GetManager().GetZoneByVnum(entry->zone)) )
			{
				sprintf( buf, "OLC: Illegal save zone %d!", entry->zone );
				Log( "%s", buf );
			}
			else
			{
				sprintf( buf, "OLC: Reboot saving %s for zone %d.",
					save_info_msg[ ( int ) entry->type ], zone->getVnum() );
				Log( "%s", buf );

				switch ( entry->type )
				{
				case OLC_SAVE_ROOM:
					redit_save_to_disk( zone->GetRnum() );
					break;
				case OLC_SAVE_OBJ:
					oedit_save_to_disk( zone->GetRnum() );
					break;
				case OLC_SAVE_MOB:
					MobManager::GetManager().SavePrototypes(zone->getVnum());
					break;
				case OLC_SAVE_ZONE:
					zone->save();
					break;
				default:
					Log( "Unexpected olc_save_list->type" );
					break;
				}
			}
		}
	}

	// Serai - lets free as much allocated memory as we possibly can.  06/11/04

	for(int i = 0;i < MAX_MESSAGES;++i)
		delete fight_messages[i].msg;
	while(wizlist) {
		Wizard *Next = wizlist->next;
		delete wizlist;
		wizlist=Next;
	}
	while( !Warrants.empty() )
		delete (Warrants.front());
	while( ClanList ) {
		Clan* Next = ClanList->Next;
		delete (ClanList);
		ClanList = Next;
	}
	Poker::Cleanup();

	Log( "Closing all sockets." );
	while ( descriptor_list )
	{
		if ( (descriptor_list->character) )
		{
			Character *ch = descriptor_list->character;
			//Regular saving occurs inside of Character::Extract()
			ch->rentSave();
			ch->Extract(UserLogoutType::reboot, true);
		}
		descriptor_list->persistentDisconnect();
	}

	listener->close();

	Log( "Freeing PCs and NPCs..." );

	while ( character_list )
	{
		Character *temp, *ch = character_list;
		REMOVE_FROM_LIST(ch, character_list, next);
		ch->Extract(UserLogoutType::reboot);
		delete ch;
	}

	Log( "Freeing items..." );
	while ( object_list )
	{
		Object *temp, *obj = object_list;
		//Before extraction, which removes & vault saves, we need to remove & prevent vault saving.
		if( obj->in_room ) {
			obj->RemoveFromRoom(false);
		}
		obj ->Extract();
		REMOVE_FROM_LIST(obj, object_list, next);
		delete obj;
	}

	MobManager::GetManager().Free();
	delete (Tips);

	for(auto objectProtoIter = obj_proto.begin();objectProtoIter != obj_proto.end();++objectProtoIter)
	{
		delete (*objectProtoIter);
	}
	obj_proto.clear();
	delete[] ( obj_index );

	Log("Freeing Legends list...");
	for( std::list<Legend*>::iterator lIter = Legends.begin();lIter != Legends.end();++lIter )
	{
		delete (*lIter);
	}
	Log("Freeing Switch list...");
	SwitchManager::GetManager().Free();

	Log("Freeing Auctions...");
	AuctionManager::GetManager().Free();

	Log("Freeing Ban list...");
	BanManager::GetManager().Free();

	Log("Freeing Player Index...");
	destroyPlayerIndex();

	Log("Freeing Shop Index...");
	delete[] (shop_index);

	Log("Freeing Configuration...");
	delete (Conf);

	Log("Freeing Kits...");
	KitManager::GetManager().Free();

	Log("Freeing Events...");
	for(event_info *e = event_list;e;e=e->next)
	{
		delete (e);
	}

	Log( "Freeing the world..." );
	for ( unsigned int i2 = 0; i2 < World.size(); ++i2 )
	{
		delete World[i2];
	}
	World.clear();

	reset_q_element *r, *r_next;
	for( r = reset_q.head;r;r = r_next ) {
		r_next = r->next;
		delete r;
	}
	reset_q.head = reset_q.tail = (0);

	Log("Freeing Shields...");
	ShieldManager::GetManager().Free();

	Log("Freeing Gates...");
	GateManager::GetManager().Free();

	Log("Freeing Weaves...");
	WeaveManager::GetManager().Free();

	Log("Freeing Stat Manager...");
	StatManager::GetManager().Free();

	Log("Freeing Comm Manager...");
	CommManager::GetManager().Free();

	Log("Freeing JSManager...");
	delete JSManager::get();

	Log( "Freeing menus/guides..." );
	delete[] ( credits );
	delete[] ( motd );
	delete[] ( imotd );
	delete[] ( background );
	delete[] ( startup );
	CommandUtil::get()->destroy();

	Log( "Freeing zone table..." );
	ZoneManager::GetManager().Free();

	Log("Ending Game Session...");
	endGameSession();

	Log("Closing main socket...");
	delete listener;

	dbContext.reset();

	Log("Closing object movement log thread...");
	objectMoveLogger.kill();
	objectMoveLoggerThread->join();
	delete objectMoveLoggerThread;

	Log("Closing mob load log thread...");
	mobLoadLogger.kill();
	mobLoadLoggerThread->join();
	delete mobLoadLoggerThread;

	UserLogoutType::cleanup();
	MudStatus::cleanup();
	UserType::cleanup();
	GatewayDescriptorStatus::cleanup();
	RoomSector::cleanup();
	PlayerPortalDescriptorStatus::cleanup();
	OlcEditType::cleanup();
	ScriptImportOperation::cleanup();
	EntityType::cleanup();

	Log("Deleting game object...");
	delete game;

	Log("~~~Summary~~~");
	Log("Rooms       : Total Alloc: %d, Dealloc: %d, Rem: %d",
		Room::nr_alloc, Room::nr_dealloc, (Room::nr_alloc-Room::nr_dealloc));
	Log("Objects     : Total Alloc: %d, Dealloc: %d, Rem: %d",
		Object::nr_alloc, Object::nr_dealloc, (Object::nr_alloc-Object::nr_dealloc));
	Log("Characters  : Total Alloc: %d, Dealloc: %d, Rem: %d",
		Character::nr_alloc, Character::nr_dealloc, (Character::nr_alloc-Character::nr_dealloc));
	Log("JSCharacter : Total Alloc: %d, Dealloc: %d, Rem: %d",
		JSCharacter::numberAllocated, JSCharacter::numberDeallocated, (JSCharacter::numberAllocated-JSCharacter::numberDeallocated));
	Log("JSRow       : Total Alloc: %d, Dealloc: %d, Rem: %d",
		sqlJSRow::numberAllocated, sqlJSRow::numberDeallocated, (sqlJSRow::numberAllocated-sqlJSRow::numberDeallocated));
	Log("sql::Query    : Total Alloc: %d, Dealloc: %d, Rem: %d",
		sql::_Query::getNumberOfAllocations(), sql::_Query::getNumberOfDeallocations(), sql::_Query::getRemainder());
	Log("sql::Row      : Total Alloc: %d, Dealloc: %d, Rem: %d",
		sql::Row::getNumberOfAllocations(), sql::Row::getNumberOfDeallocations(), sql::Row::getRemainder());

	Log( "Normal termination of game." );
#if (defined WIN32 && defined _DEBUG && defined _MEM_LEAKS )
	_CrtDumpMemoryLeaks();
#endif
}

void startGameSession()
{
	std::stringstream queryBuffer;

	queryBuffer << "INSERT INTO gameSession("
				<< "`start_datetime`"
				<< ") VALUES ("
				<< "NOW()"
				<< ");";

	gameDatabase->sendRawQuery(queryBuffer.str());

	gameSessionId = gameDatabase->lastInsertID();
}
void endGameSession()
{
	std::stringstream queryBuffer;

	queryBuffer << "UPDATE gameSession SET"
				<< " end_datetime=NOW() "
				<< "WHERE id=" << gameSessionId << ";";

	gameDatabase->sendRawQuery(queryBuffer.str());
}

void gameLoop()
{
	int missed_pulses = 0, aliased;
	time_t last_db_alert_time = 0;
	Descriptor *d, *next_d;
	
	Clock heartbeatClock;
	Clock pulseTimer;

	rebootNotified15 = rebootNotified10 = rebootNotified5 = rebootNotified1 = false;

	pulseTimer.reset(false);

	bootTime = time( 0 );
	pulse = 0;

	while ( !circle_shutdown )
	{
		std::this_thread::sleep_for( std::chrono::microseconds( OPT_USEC ) );

		//Make sure we're still connected to the database...
		if( !gameDatabase->isConnected() && (time(0) - last_db_alert_time) > 15 )
		{
			MudLog(BRF, LVL_APPR, TRUE, "Disconnected from database... Attempting reconnection.");
			SetupMySQL( false );
			last_db_alert_time = time(0);
		}

		pulseTimer.reset(true);

		if(gatewayConnection == NULL) {

			waitForGatewayConnection();
			gatewayConnection->socketWriteInstant("FinishedBooting\n");
		}

		while(true) {

			std::string input = gatewayConnection->getCommand();

			if(input.empty()) {

				break;
			}

			processGatewayCommand(input);
		}

		//Process Web Socket Commands & prepare input
		for(d = descriptor_list; d; d = next_d)
		{
			next_d = d->next;
			if(d->getGatewayDescriptorType() == GatewayDescriptorType::websocket)
			{
				d->processWebSocketCommands();
			}
			else
			{
				while(d->descriptor->hasCommand())
					d->commandQueue.push_back(d->descriptor->getCommand());
			}
		}

		// Process commands.
		for ( d = descriptor_list; d; d = next_d )
		{
			next_d = d->next;

			try {
				d->processInput();
			}
			catch( sql::Exception e ) {

				MudLog(BRF, LVL_APPR, TRUE, "Generic sql::exception uncaught while processing user input: %s", e.getMessage().c_str());
			}
			catch( flusspferd::exception e) {

				MudLog(BRF, LVL_APPR, TRUE, "Generic flusspferd::exception uncaught while processing user input: %s", e.what());
			}
			catch( std::exception e ) {

				MudLog(BRF, LVL_APPR, TRUE, "Generic std::exception uncaught while processing user input: %s", e.what());
			}
		}

		++missed_pulses;

		if ( missed_pulses <= 0 )
		{
			Log( "SYSERR: **BAD** MISSED_PULSES NONPOSITIVE (%d), TIME GOING BACKWARDS!!", missed_pulses );
			missed_pulses = 1;
		}

		/* If we missed more than 30 seconds worth of pulses, just do 30 secs */
		if ( missed_pulses > ( 2 * PASSES_PER_SEC ) )
		{
			Log( "SYSERR: Missed %d seconds worth of pulses. Heartbeat ran for %.3f seconds.", missed_pulses / PASSES_PER_SEC, heartbeatClock.getSeconds() );
			missed_pulses = 2 * PASSES_PER_SEC;
		}

		for(d = descriptor_list;d;d = d->next)
		{
			d->flushOutputBuffer();
		}

		listener->pulse();

		listener->acceptNewHosts();

		for(d = descriptor_list;d;d = d->next) {

			if(d->hadOutput) {

				d->sendInstant("\r\n");
			}
			if( (d->hadInput || d->hadOutput) && d->shouldMakePrompt() ) {

				d->sendInstant(d->makePrompt());
			}

			d->hadInput = false;
			d->hadOutput = false;
		}


		/* Kick out folks in the CON_CLOSE or CON_DISCONNECT state */
		for ( d = descriptor_list; d; d = next_d )
		{
			next_d = d->next;

			if ( STATE( d ) == CON_CLOSE || STATE( d ) == CON_DISCONNECT )
				d->disconnect();
		}

		heartbeatClock.reset(false);
		heartbeatClock.turnOn();
		/* Now execute the heartbeat functions */
		while ( missed_pulses-- ) {

			try {
				heartbeat( ++pulse );
			}
			catch( sql::Exception e ) {

				MudLog(BRF, LVL_APPR, TRUE, "Generic sql::exception uncaught while processing heartbeat(): %s", e.getMessage().c_str());
			}
			catch( flusspferd::exception e) {

				MudLog(BRF, LVL_APPR, TRUE, "Generic flusspferd::exception uncaught while heartbeat(): %s", e.what());
			}
			catch( std::exception e ) {

				MudLog(BRF, LVL_APPR, TRUE, "Generic std::exception uncaught while processing heartbeat(): %s", e.what());
			}
		}
		heartbeatClock.turnOff();

		/* Roll pulse over after 10 hours */
		if ( pulse >= ( 600 * 60 * PASSES_PER_SEC ) )
			pulse = 0;

		pulseTimer.turnOff();

		missed_pulses = pulseTimer.getClocks() / (1000 / PASSES_PER_SEC);

#ifdef CIRCLE_UNIX
		/* Update tics for deadlock protection (UNIX only) */
		++tics;
#endif

	}
}

/*This function updates wait states and performs lagged actions if the lag is over. */
void CheckWaitState( void )
{
	std::list< Character* >::iterator ch;

	for ( ch = WaitingList.begin();ch != WaitingList.end();++ch )
	{
		if( (*ch)->WaitUpdate() )
		{
			ch = WaitingList.erase( ch );
			if(ch == WaitingList.end())
				break;
		}
	}
}

void Character::WaitState( int pulses )
{
	if(pulses <= 0)
	{
		WaitingList.remove( this );
		this->WaitEvents();
	}
	else
	{
		if( !(this->wait) ) //They were not on the list before...
			WaitingList.push_back( this );
	}
	this->wait = pulses;
}

//Returns true if wait period is up. False otherwise.
bool Character::WaitUpdate()
{
	this->wait -= 1;

	if( !this->wait )
	{
		this->WaitEvents();
		return true;
	}
	return false;
}

void Character::WaitEvents()
{
	if ( FLEE_GO( this ) )
		performFlee( this );
}

void autoSave( void )
{
	sendToAll("Beginning autosave. Please wait...\r\n", true);
	Clock clock1;

	std::map<std::pair<char, std::string>, std::list<Object *>> holderIdAndTypeToContentsMap;
	Object *object;

	for(Character *ch = character_list;ch;ch = ch->next)
	{
		if( !ch->IsPurged() && !IS_NPC(ch) )
		{
			std::list<Object*> characterContents;
			for(object = ch->carrying;object;object = object->next_content)
			{
				characterContents.push_back(object);
			}
			for(int bodyPosition = 0;bodyPosition < NUM_WEARS;++bodyPosition)
			{
				if(GET_EQ(ch, bodyPosition))
					characterContents.push_back(GET_EQ(ch, bodyPosition));
			}
			holderIdAndTypeToContentsMap[ std::pair<char, std::string>('P', MiscUtil::convert<std::string>(ch->player.idnum)) ] = characterContents;

			ch->save();
		}
	}

	//Build a list of all rooms that have at least one corpse.
	std::set<Room*> corpseRooms;
	for(auto corpseRoomVnumIter = Room::corpseRooms.begin();corpseRoomVnumIter != Room::corpseRooms.end();)
	{
		int rnum = real_room((*corpseRoomVnumIter));
		if( rnum != -1 )
		{
			corpseRooms.insert( World[ rnum ] );
			
			bool hasCorpse = false;
			//Ensure that the room has at least one corpse in it still.
			for(object = World[ rnum ]->contents;object != NULL;object = object->next_content)
			{
				if(IS_CORPSE(object))
				{
					hasCorpse = true;
					break;
				}
			}

			if(!hasCorpse)
					corpseRoomVnumIter = Room::corpseRooms.erase(corpseRoomVnumIter);
			else
				++corpseRoomVnumIter;
		}
	}

	//Now loop through each room and save its corpses.
	for(auto corpseRoomIter = corpseRooms.begin();corpseRoomIter != corpseRooms.end();++corpseRoomIter)
	{
		Room *corpseRoom = (*corpseRoomIter);
		std::list<Object*> roomContents;
		for(object = corpseRoom->contents;object;object = object->next_content)
		{
			if(IS_CORPSE(object))
				roomContents.push_back(object);
		}
		holderIdAndTypeToContentsMap[std::pair<char, std::string>('R', MiscUtil::convert<std::string>(corpseRoom->getVnum()))] = roomContents;
	}

	Object::saveMultipleHolderItems(holderIdAndTypeToContentsMap, true);

	sendToAll("\r\nAutosave complete.\r\n", true);
	
	for( Descriptor *d = descriptor_list;d;d=d->next )
	{
		if( d->shouldMakePrompt() )
		{
			d->sendInstant(d->makePrompt());
		}
	}
}

void NewbieTips()
{
	Descriptor * d;

	if ( !Tips->size() )
		return ;

	for ( d = descriptor_list;d;d = d->next )
	{
		if ( d->character && d->character->PlayerData &&
		NEWB_FLAGGED( d->character, NEW_TIPS ) && STATE( d ) == CON_PLAYING )
			d->character->NewbieTip( GetTip( tip_on ).c_str() );
	}

	if ( ++tip_on > Tips->size() )
		tip_on = 1;
}

class PassickTrackDeletionJob : public Job
{
	std::list<Track*> *tracksToDelete;
public:
	PassickTrackDeletionJob( std::list<Track*> *tracksToDelete )
	{
		this->tracksToDelete = tracksToDelete;
	}
	virtual void performRoutine()
	{
		for(auto iter = tracksToDelete->begin();iter != tracksToDelete->end();++iter)
		{
			delete (*iter);
		}

		delete tracksToDelete;
	}
	virtual void performPostJobRoutine()
	{
		//...
	}
};

void DeleteOldTracks()
{
	//Tracks should be sorted newest to oldest. We will scan the back of the track list until
	//We find the first track that has not yet expired.
	time_t timeContext = time(0);
	Track *track;
	std::list<Track *> *tracksToDelete = new std::list<Track *>();
	while(!TrackList.empty())
	{
		track = TrackList.back();
		if(track->Age(timeContext) >= 50)
		{
			//delete track;
			if(track->room)
				track->room->tracks.remove(track);
			TrackList.pop_back();

			tracksToDelete->push_back(track);
		}
		else
			break;
	}

	//Free up the tracks in a separate thread.
	Job *job = new PassickTrackDeletionJob(tracksToDelete);
	ThreadedJobManager::get().addJob( job );
}

void TaintEvents( int pulse )
{
	Character * ch;

	for ( ch = character_list;ch;ch = ch->next )
	{
		if( ch->IsPurged() ) continue;
		if ( !( pulse % ( 10 * PASSES_PER_SEC ) ) )
		{
			if ( ch->GetTotalTaint() >= TaintData.TaintForRandomSocial && TAINT_CALC( ch ) )
				ch->RandomTaintSocial();
			if ( ch->GetTotalTaint() >= TaintData.TaintForRandomVoice && TAINT_CALC( ch ) )
				ch->RandomTaintVoices();
		}
		else if ( !( pulse % PASSES_PER_SEC ) )
		{
			if ( !( ch->dizzy_time - 1 ) )
				ch->RemoveTaintDizzy();
			ch->dizzy_time = MAX( 0, ch->dizzy_time - 1 );
		}
	}
}
void PurgeQueue()
{
	for( std::list< Character* >::iterator cIter = CharPurgeList.begin();cIter != CharPurgeList.end();++cIter )
	{
		Character *temp;
		REMOVE_FROM_LIST( (*cIter), character_list, next);
		delete (*cIter);
	}
	for( std::list< Object* >::iterator oIter = ObjPurgeList.begin();oIter != ObjPurgeList.end();++oIter )
	{
		Object *temp;
		REMOVE_FROM_LIST( (*oIter), object_list, next);
		delete (*oIter);
	}

	CharPurgeList.clear();
	ObjPurgeList.clear();
}

double LagRoutineEntry::avgRunTime()
{
	if( !numberOfTimesRun )
		return 0.0;
	return runTime / numberOfTimesRun;
}

LagMonitor::LagMonitor()
{
}
LagMonitor::~LagMonitor()
{
}

void LagMonitor::startClock()
{
	timer.turnOff();
	timer.reset(false);
	timer.turnOn();
}
void LagMonitor::stopClock( const std::string &sRoutineName )
{
	timer.turnOff();

	double runTime = (double)timer.getClocks() / (double)1000;

	if( mRoutineTimeCard.count( sRoutineName ) == 0 ) {
		mRoutineTimeCard[ sRoutineName ].numberOfTimesRun = 1;
		mRoutineTimeCard[ sRoutineName ].runTime = runTime;
	}
	else {
		++mRoutineTimeCard[ sRoutineName ].numberOfTimesRun;
		mRoutineTimeCard[ sRoutineName ].runTime += runTime;
	}
}
LagRoutineEntry LagMonitor::getRoutineEntry( const std::string &sRoutineName )
{
	return mRoutineTimeCard[ sRoutineName ];
}
LagMonitor lagMonitor;

void invisiblePing()
{
	for(Descriptor *descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
	{
		std::string pingMessage = std::string("\r\n");

		if(descriptor->shouldMakePrompt())
			pingMessage += descriptor->makePrompt();

		descriptor->sendInstant(pingMessage);
//		descriptor->sendRaw("\0");
	}
}

void heartbeat( int pulse )
{
	lagMonitor.startClock();
	process_events();
	lagMonitor.stopClock( LAG_MONITOR_PROCESS_EVENTS );

	lagMonitor.startClock();
	CheckWaitState();
	lagMonitor.stopClock( LAG_MONITOR_CHECK_WAIT_STATE );

	lagMonitor.startClock();
	checkTimers();
	lagMonitor.stopClock( LAG_MONITOR_CHECK_TIMERS );

	lagMonitor.startClock();
	TaintEvents( pulse );
	lagMonitor.stopClock( LAG_MONITOR_TAINT_EVENTS );

	lagMonitor.startClock();
	ThreadedJobManager::get().cycleThroughFinishedJobs();
	lagMonitor.stopClock( LAG_MONITOR_THREADED_JOBS );

	lagMonitor.startClock();
	JSManager::get()->heartbeat();
	lagMonitor.stopClock( LAG_MONITOR_KJS_HEARTBEAT );

	//Check to see if a new day has passed. If so, we need to update the boot high to reflect this.
	if( !Time::SameDay(time_of_boot_high, time(0)) ) {
		
		lagMonitor.startClock();
		UpdateBootHigh(NumPlayers(true,false));
		lagMonitor.stopClock( LAG_MONITOR_UPDATE_BOOT_HIGH );
	}
	if ( !( pulse % (13 RL_SEC) ) )
    {
		lagMonitor.startClock();
        js_random_triggers();
		lagMonitor.stopClock( LAG_MONITOR_KJS_RANDOM_TRIGGERS );
    }

	if( !( pulse % (3600 RL_SEC)) )
	{
		lagMonitor.startClock();
		ForumUtil::archiveAndRemoveDeletedForumUsers(gameDatabase);
		lagMonitor.stopClock( LAG_MONITOR_ARCHIVE_FORUM_USERS );

		lagMonitor.startClock();
		ForumUtil::addUsersToForum(gameDatabase);
		lagMonitor.stopClock( LAG_MONITOR_ADD_FORUM_USERS );
	}

	if ( !( pulse % PULSE_ZONE ) ) {
		lagMonitor.startClock();
		zone_update();
		lagMonitor.stopClock( LAG_MONITOR_ZONE_UPDATE );
	}

	if ( !( pulse % ( PASSES_PER_SEC ) ) )
	{
		lagMonitor.startClock();
		check_fighting();
		lagMonitor.stopClock( LAG_MONITOR_CHECK_FIGHTING );
	}
	if ( !( pulse % ( (60 RL_SEC) * (CONFIG_AUTOSAVE_TIME) ) ) ) {
		lagMonitor.startClock();
		autoSave();
		lagMonitor.stopClock( LAG_MONITOR_AUTO_SAVE );
	}
	if ( !( pulse % ( 60 * PASSES_PER_SEC * 5 ) ) ) {
		lagMonitor.startClock();
		checkIdlePasswords();
		lagMonitor.stopClock( LAG_MONITOR_CHECK_IDLE_PASSWORDS );
		
		lagMonitor.startClock();
		invisiblePing();
		lagMonitor.stopClock( LAG_MONITOR_INVISIBLE_PING );
	}

	if ( !( pulse % PULSE_MOBILE ) ) {
		lagMonitor.startClock();
		mobileActivity();
		lagMonitor.stopClock( LAG_MONITOR_MOBILE_ACTIVITY );
	}

	if ( !( pulse % PULSE_VIOLENCE ) )
	{
		lagMonitor.startClock();
		GateManager::GetManager().UpdateGates();
		lagMonitor.stopClock( LAG_MONITOR_UPDATE_GATES );

		lagMonitor.startClock();
		perform_violence();
		lagMonitor.stopClock( LAG_MONITOR_PERFORM_VIOLENCE );
	}

	if ( !( pulse % PASSES_PER_SEC ) )
	{
		++Seconds;
		lagMonitor.startClock();
		PurgeQueue();
		lagMonitor.stopClock( LAG_MONITOR_PURGE_QUEUE );
	}

	if ( !( pulse % ( SECS_PER_MUD_HOUR * PASSES_PER_SEC ) ) )
	{
		lagMonitor.startClock();
		weather_and_time( 1 );
		lagMonitor.stopClock( LAG_MONITOR_WEATHER_AND_TIME );

		lagMonitor.startClock();
		affect_update();
		lagMonitor.stopClock( LAG_MONITOR_AFFECT_UPDATE );
		
		lagMonitor.startClock();
		point_update();
		lagMonitor.stopClock( LAG_MONITOR_POINT_UPDATE );

		lagMonitor.startClock();
		js_time_triggers();
		lagMonitor.stopClock( LAG_MONITOR_KJS_TIME_TRIGGERS );

		lagMonitor.startClock();
		DeleteOldTracks();
		lagMonitor.stopClock( LAG_MONITOR_DELETE_OLD_TRACKS );

		lagMonitor.startClock();
		AuctionManager::GetManager().UpdateAuctions();
		lagMonitor.stopClock( LAG_MONITOR_AUCTION_UPDATES );

//		lagMonitor.startClock();
//		Room::saveCorpseRooms();
//		lagMonitor.stopClock( LAG_MONITOR_SAVE_CORPSE_ROOMS );
	}


	lagMonitor.startClock();
	Descriptor::sendWebSocketCommands(pulse);
	lagMonitor.stopClock( LAG_MONITOR_WEBSOCKET_COMMANDS );
	
	lagMonitor.startClock();
	rebootCountdown();
	lagMonitor.stopClock( LAG_MONITOR_REBOOT_COUNTDOWN );

	lagMonitor.startClock();
	game->processPlayerPortalServer();
	lagMonitor.stopClock(LAG_MONITOR_PLAYER_PORTAL_SERVER);

	if ( !( pulse % ( SECS_PER_MUD_HOUR * PASSES_PER_SEC * 5 ) ) ) {
		lagMonitor.startClock();
		NewbieTips();
		lagMonitor.stopClock( LAG_MONITOR_NEWBIE_TIPS );
	}

	if ( !( pulse % ( 5 * 60 * PASSES_PER_SEC ) ) )  /* 5 minutes */
	{
		recordUsage();
	}
}

/* ******************************************************************
*  general utility stuff (for local use)                            *
****************************************************************** */

/*
*  new code to calculate time differences, which works on systems
*  for which tv_usec is unsigned (and thus comparisons for something
*  being < 0 fail).  Based on code submitted by ss@sirocco.cup.hp.com.
*/

/*
* code to return the time difference between a and b (a-b).
* always returns a nonnegative value (floors at 0).
*
* Fixed the 'aggregate return' warning.  Now it's not thread-safe.
* -gg 6/18/98
*/

struct timeval *timediff( struct timeval a, struct timeval b )
{
	static struct timeval rslt;

	if ( a.tv_sec < b.tv_sec )
		return &null_time;

	else if ( a.tv_sec == b.tv_sec )
	{
		if ( a.tv_usec < b.tv_usec )
			return & null_time;

		else
		{
			rslt.tv_sec = 0;
			rslt.tv_usec = a.tv_usec - b.tv_usec;
			return &rslt;
		}
	}

	else
	{ /* a->tv_sec > b->tv_sec */
		rslt.tv_sec = a.tv_sec - b.tv_sec;

		if ( a.tv_usec < b.tv_usec )
		{
			rslt.tv_usec = a.tv_usec + 1000000 - b.tv_usec;
			rslt.tv_sec--;
		}

		else
			rslt.tv_usec = a.tv_usec - b.tv_usec;

		return &rslt;
	}
}

/*
* add 2 timevals
*
* Fixed the 'aggregate return' warning. Not thread-safe now.
* -gg 6/18/98
*/
struct timeval *timeadd( struct timeval a, struct timeval b )
{
	static struct timeval rslt;

	rslt.tv_sec = a.tv_sec + b.tv_sec;
	rslt.tv_usec = a.tv_usec + b.tv_usec;

	while ( rslt.tv_usec >= 1000000 )
	{
		rslt.tv_usec -= 1000000;
		rslt.tv_sec++;
	}

	return &rslt;
}

void recordUsage( void )
{
	int sockets_connected = 0, sockets_playing = 0;
	Descriptor *d;

	for ( d = descriptor_list; d; d = d->next )
	{
		++sockets_connected;

		if ( STATE( d ) == CON_PLAYING )
			++sockets_playing;
	}

	Log( "nusage: %-3d sockets connected, %-3d sockets playing", sockets_connected, sockets_playing );
}

const char *Character::Health()
{
	int percent = ( GET_MAX_HIT( this ) == 0 ) ? 0 : ( GET_HIT( this ) * 100 ) / GET_MAX_HIT( this );

	if ( GET_POS( this ) == POS_DEAD )
		return "Dead";
	if ( GET_POS( this ) == POS_MORTALLYW )
		return "Mortally Wounded";
	else if ( GET_POS( this ) == POS_INCAP )
		return "Incapacitated";
	else if ( GET_POS( this ) == POS_STUNNED )
		return "Stunned";
	else
	{
		if ( percent >= 100 )
			return "Healthy";
		else if ( percent >= 90 )
			return "Scratched";
		else if ( percent >= 75 )
			return "Hurt";
		else if ( percent >= 50 )
			return "Wounded";
		else if ( percent >= 30 )
			return "Battered";
		else if ( percent >= 15 )
			return "Beaten";
		else
			return "Critical";
	}
	return "";
}

const char *mana( int percent )
{
	if ( percent >= 100 )
		return "Full";
	else if ( percent >= 90 )
		return "Excellent";
	else if ( percent >= 75 )
		return "Strong";
	else if ( percent >= 50 )
		return "Good";
	else if ( percent >= 30 )
		return "Weakening";
	else if ( percent >= 15 )
		return "Flickering";
	else if ( percent >= 1 )
		return "Flickering";
	else
		return "Wasted";
}

const char *moves( int percent )
{
	if ( percent >= 100 )
		return "Fresh";
	else if ( percent >= 90 )
		return "Excellent";
	else if ( percent >= 75 )
		return "Strong";
	else if ( percent >= 50 )
		return "Good";
	else if ( percent >= 30 )
		return "Draining";
	else if ( percent >= 15 )
		return "Faint";
	else
		return "Burning";
}

/********************************************************************
 *  Socket Handling                                                 *
 ********************************************************************/

void Character::ShowTimer()
{
	int timer = ( int ) ( this->timer * 10 );

	if ( !desc )
		return ;

	if ( !( timer % 40 ) )
		desc->sendInstant("\r\n * ");
	else if ( !( timer % 35 ) )
		desc->sendInstant(" + ");
	else if ( !( timer % 30 ) )
		desc->sendInstant(" - ");
	else if ( !( timer % 25 ) )
		desc->sendInstant(" = ");
	else if ( !( timer % 20 ) )
		desc->sendInstant(" % ");
	else if ( !( timer % 15 ) )
		desc->sendInstant(" = ");
	else if ( !( timer % 10 ) )
		desc->sendInstant(" - ");
	else if ( !( timer % 5 ) )
		desc->sendInstant(" + ");
}

void checkTimers()
{
	Character * ch;

	for ( ch = character_list; ch; ch = ch->next )
	{
		if( ch->IsPurged() )
			continue;
		if( ch->pulverizeCooldown > 0 )
			ch->pulverizeCooldown -= ( ( float ) 1 / PASSES_PER_SEC );
		//No Delayed Command.
		if ( ch->delayed_command.empty() )
			continue;

		ch->ShowTimer();
		ch->timer -= ( ( float ) 1 / PASSES_PER_SEC );

		//If timer is still running, we move on to next person.
		if ( ch->timer > 0.0f || !ch->command_ready )
			continue;

		// timer reached end
		if (ch->delayed_command == "delayed_javascript") // special value to indicate it was caused by a script
		{
			if( ch->delayed_script.use_count() == 0 )
			{//Galnor 06/06/2010 - It seems that this delayed_script is getting reset somewhere that it should not.
			 //We'll print out an error in case this happens so that we do not crash somewhere down the line.
				MudLog(BRF, MAX(LVL_APPR,GET_INVIS_LEV(ch)), TRUE, "%s is trying to run a timer script with an invalid script set.", GET_NAME(ch));
			}
			else {
				JSManager::get()->execute_timer(ch->delayed_script, true);
				if( ch->timer <= 0 ) {// 08/17/2010 - We do this because the script, after resuming, may
					//set up another timer. If this is the case, then we do not want to clean the timer up.
					ch->delayed_script.reset();
				}
			}
		}
		else
		{
			char dCommand[MAX_INPUT_LENGTH];
			strcpy(dCommand, ch->delayed_command.c_str());
			CommandUtil::get()->interpretCommand( ch, dCommand );
		}
		if( !(ch->timer > 0) ) {
			ch->CancelTimer( false );
		}
	}
}

void checkIdlePasswords( void )
{
	Descriptor *d;
	int requirement;

	for ( d = descriptor_list; d; d = d->next )
	{

		if ( STATE( d ) == CON_GATEWAY || STATE( d ) == CON_PASSWORD || STATE( d ) == CON_GET_NAME )
			requirement = 5;
		else if( d->loggedIn == false )
			requirement = 15;
		else
			continue;
		if (d->idle_tics != requirement)
			d->idle_tics++;
		else
		{
			d->echoOn();
			d->sendRaw("\r\nTimed out... goodbye.\r\n");
			STATE( d ) = CON_CLOSE;
		}
	}
}

/********************************************************************
*  signal-handling functions (formerly signals.c).  UNIX only.      *
*********************************************************************/

#if defined(CIRCLE_UNIX) || defined(CIRCLE_MACINTOSH)

RETSIGTYPE unrestrict_game( int sig )
{
	MudLog( BRF, LVL_IMMORT, TRUE, "Received SIGUSR2 - completely unrestricting game (emergent)" );
	circle_restrict = 0;
}

#ifdef CIRCLE_UNIX

/* clean up our zombie kids to avoid defunct processes */
RETSIGTYPE reap( int sig )
{
	while ( waitpid( -1, NULL, WNOHANG ) > 0 )
		;

	my_signal( SIGCHLD, reap );
}

RETSIGTYPE checkpointing( int sig )
{
	if ( !tics )
	{
		Log( "SYSERR: CHECKPOINT shutdown: tics not updated. (Infinite loop suspected)" );
		abort();
	}

	else
		tics = 0;
}

RETSIGTYPE hupsig( int sig )
{
	Log( "SYSERR: Received SIGHUP, SIGINT, or SIGTERM.  Shutting down..." );
	exit( 1 ); /* perhaps something more elegant should
			* substituted */
}


/*
* This is an implementation of signal() using sigaction() for portability.
* (sigaction() is POSIX; signal() is not.)  Taken from Stevens' _Advanced
* Programming in the UNIX Environment_.  We are specifying that all system
* calls _not_ be automatically restarted for uniformity, because BSD systems
* do not restart select(), even if SA_RESTART is used.
*
* Note that NeXT 2.x is not POSIX and does not have sigaction; therefore,
* I just define it to be the old signal.  If your system doesn't have
* sigaction either, you can use the same fix.
*
* SunOS Release 4.0.2 (sun386) needs this too, according to Tim Aldric.
*/

#ifndef POSIX
#define my_signal(signo, func) signal(signo, func)
#else
sigfunc *my_signal( int signo, sigfunc * func )
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset( &act.sa_mask );
	act.sa_flags = 0;

#ifdef SA_INTERRUPT

	act.sa_flags |= SA_INTERRUPT; /* SunOS */
#endif

	if ( sigaction( signo, &act, &oact ) < 0 )
		return SIG_ERR;

	return oact.sa_handler;
}
#endif /* POSIX */


void signal_setup( void )
{
#ifndef CIRCLE_MACINTOSH
	struct itimerval itime;
	struct timeval interval;

	/*
	 * user signal 2: unrestrict game.  Used for emergencies if you lock
	 * yourself out of the MUD somehow.  (Duh...)
	 */
	my_signal( SIGUSR2, unrestrict_game );

	/*
	 * set up the deadlock-protection so that the MUD aborts itself if it gets
	 * caught in an infinite loop for more than 3 minutes.
	 */

	interval.tv_sec = 300;
	interval.tv_usec = 0;
	itime.it_interval = interval;
	itime.it_value = interval;
	setitimer( ITIMER_VIRTUAL, &itime, NULL );
	my_signal( SIGVTALRM, checkpointing );

	/* just to be on the safe side: */
	my_signal( SIGHUP, hupsig );
	my_signal( SIGCHLD, reap );

#endif /* CIRCLE_MACINTOSH */

	my_signal( SIGINT, hupsig );
	my_signal( SIGTERM, hupsig );
	my_signal( SIGPIPE, SIG_IGN );
	my_signal( SIGALRM, SIG_IGN );
}
#endif /* CIRCLE_UNIX */
#endif /* CIRCLE_UNIX || CIRCLE_MACINTOSH */

void SetClock()
{
	GlobalClock = clock();
}

clock_t GetClockDifference()
{
	return ( clock() - GlobalClock );
}

/* ****************************************************************
*       Public routines for system-to-player-communication        *
**************************************************************** */

void Character::LogOutput( const std::string &buffer )
{
	char timestamp[16];
	time_t timeNow = time(0);
	tm *tmNow = localtime(&timeNow);
	std::stringstream filePathBuffer;
	std::string filePath;

	strftime(timestamp, sizeof(timestamp) - 1, "%Y%m%d", tmNow);
	filePathBuffer << game->getPlayerLogsDirectory() << this->player.idnum << "_" << timestamp << ".plog";
	filePath = filePathBuffer.str();
	
	FILE *outfile;

	if ( !( outfile = fopen( filePath.c_str(), "a+" ) ) )
	{
		Log("Unable to open log file for %s: %s: %s", GET_NAME(this), filePathBuffer.str().c_str(), strerror(errno));
		return;
	}

	fputs(buffer.data(), outfile);
	fclose( outfile );
}

void Character::send( const char *messg, va_list args )
{
	if ( this->desc && messg && *messg )
		this->desc->writeToOutput(true, messg, args );
}

void Character::send( const char *messg, ... )
{
	va_list args;

	va_start( args, messg );
	send(messg, args);
	va_end(args);
}

void Character::send( const std::string &s)
{
    send(s.c_str());
}

void Character::sendDisorientableMessage( const char *message, ... )
{
	if(!message)
		return;

	va_list args;
	va_start(args, message);

	send(message, args);

	if(AFF_FLAGGED(this, AFF_DISORIENT) && MiscUtil::random(1, 2) == 1)
		send(message, args);

	va_end(args);
}

void Character::sendDisorientableMessage( const std::string &s )
{
	sendDisorientableMessage(s.c_str());
}

// Character operator<< overloading
Character& Character::operator<< ( const std::string &s)
{
	this->send( s.c_str() );
	return *this;
}

Character& Character::operator<< ( const char * s )
{
	this->send( s );
	return *this;
}

Character& Character::operator<< ( const char s )
{
	std::string str;
	std::stringstream strstrm;
	strstrm << s;
	strstrm >> str;
	this->send( str.c_str() );
	return *this;

}

Character& Character::operator<< ( const int s )
{
	this->send( ToString(s).c_str() );
	return *this;
}
Character& Character::operator<< ( const float s )
{
	this->send( ToString(s).c_str() );
	return *this;
}

Character& Character::operator<< ( const double s )
{
	this->send( ToString(s).c_str() );
	return *this;
}

Character& Character::operator<< ( const bool s )
{
	this->send( ToString(s).c_str() );
	return *this;
}

void sendToZone(const char *messg, int zone_rnum)
{
	Descriptor *i;

	if (!messg || !*messg)
		return;

	for (i = descriptor_list; i; i = i->next)
	{
		if (!i->connected && i->character && AWAKE(i->character) &&
		        (i->character->in_room->getZoneNumber() == zone_rnum))
		{
			i->sendRaw(messg);
		}
	}
}
void sendToAll( const char* messg, bool instant )
{
	Descriptor *i;

	if ( messg == NULL )
		return ;

	for ( i = descriptor_list; i; i = i->next )
	{
		if ( STATE( i ) == CON_PLAYING )
		{
			if( instant == true )
				i->sendInstant(messg);
			else
				i->sendRaw(messg);
		}
	}
}
void sendToOutdoor( const char* messg )
{
	Descriptor *i;

	if ( !messg || !*messg )
		return ;

	for ( i = descriptor_list; i; i = i->next )
	{
		if ( STATE( i ) != CON_PLAYING || i->character == NULL )
			continue;

		if ( !AWAKE( i->character ) || !OUTSIDE( i->character ) )
			continue;

		i->sendRaw(messg);
	}
}

void sendToRoom( const std::string &s, Room *room )
{
	Character *i;

	if ( s.empty() )
		return ;

	for ( i = room->people; i; i = i->next_in_room )
		if ( i->desc )
			i->desc->sendRaw(s.c_str());
}

void sendToRoom( const char* messg, Room *room )
{
	Character *i;

	if ( messg == NULL )
		return ;

	for ( i = room->people; i; i = i->next_in_room )
		if ( i->desc )
			i->desc->sendRaw(messg);
}



const char *ACTNULL = "<NULL>";

#define CHECK_NULL(pointer, expression) \
 if ((pointer) == NULL) i = ACTNULL; else i = (expression);

/* higher-level communication: the Act() function */
void PerformAct( const char *orig, Character *ch, Object *obj, const void *vict_obj, Character *to, const int type, const char *bgColor, bool disorientable )
{
	const char * i = NULL;
	char lbuf[ MAX_STRING_LENGTH ], *buf;

	buf = lbuf;

	for ( ;; )
	{
		if ( *orig == '$' )
		{
			switch ( *( ++orig ) )
			{

			case 'n':
				i = PERS( ch, to, bgColor );
				break;

			case 'N':
				CHECK_NULL( vict_obj, PERS( ( Character * ) vict_obj, to, bgColor ) );
				break;

			case 'm':
				i = HMHR( ch );
				break;

			case 'M':
				CHECK_NULL( vict_obj, HMHR( ( Character * ) vict_obj ) );
				break;

			case 's':
				i = HSHR( ch );
				break;

			case 'S':
				CHECK_NULL( vict_obj, HSHR( ( Character * ) vict_obj ) );
				break;

			case 'e':
				i = HSSH( ch );
				break;

			case 'E':
				CHECK_NULL( vict_obj, HSSH( ( Character * ) vict_obj ) );
				break;

			case 'o':
				CHECK_NULL( obj, OBJN( obj, to ) );
				break;

			case 'O':
				CHECK_NULL( vict_obj, OBJN( ( Object * ) vict_obj, to ) );
				break;

			case 'p':
				CHECK_NULL( obj, OBJS( obj, to ) );
				break;

			case 'P':
				CHECK_NULL( vict_obj, OBJS( ( Object * ) vict_obj, to ) );
				break;

			case 'a':
				CHECK_NULL( obj, SANA( obj ) );
				break;

			case 'A':
				CHECK_NULL( vict_obj, SANA( ( Object * ) vict_obj ) );
				break;

			case 'T':
				CHECK_NULL( vict_obj, ( char * ) vict_obj );
				break;

			case 't':
				CHECK_NULL( obj, ( char * ) obj );
				break;

			case 'F':
				CHECK_NULL( vict_obj, fname( ( char * ) vict_obj ) );
				break;

			case '$':
				i = "$";
				break;

			default:
				Log( "SYSERR: Illegal $-code to Act(): %c", *orig );
				Log( "SYSERR: %s", orig );
				break;
			}

			while ( ( *buf = *( i++ ) ) )
				++buf;

			orig++;
		}

		else if ( !( *( buf++ ) = *( orig++ ) ) )
			break;
	}

	*( --buf ) = '\r';
	*( ++buf ) = '\n';
	*( ++buf ) = '\0';

	if ( to->desc )
	{
		to->desc->sendRaw(StringUtil::cap(lbuf));

		if(disorientable && to->disorientRoll())
			to->desc->sendRaw(StringUtil::cap(lbuf));
	}
}


/* moved this to utils.h --- mah */
#ifndef SENDOK
	#define SENDOK(ch) ((ch)->desc && (to_sleeping || AWAKE(ch)) && \
		!PLR_FLAGGED((ch), PLR_WRITING))
	#endif

void Act( const char *str, int hide_invisible, Character *ch, Object *obj, const void *vict_obj, int type, const char *bgColor, bool disorientable )
{

	Character * to = NULL;
	int to_sleeping;

	if ( !str || !*str )
		return ;

	/*
	* Warning: the following TO_SLEEP code is a hack.
	*
	* I wanted to be able to tell Act to deliver a message regardless of sleep
	* without adding an additional argument.  TO_SLEEP is 128 (a single bit
	* high up).  It's ONLY legal to combine TO_SLEEP with one other TO_x
	* command.  It's not legal to combine TO_x's with each other otherwise.
	* TO_SLEEP only works because its value "happens to be" a single bit;
	* do not change it to something else.  In short, it is a hack.
	*/

	/* check if TO_SLEEP is there, and remove it if it is. */
	if ( ( to_sleeping = ( type & TO_SLEEP ) ) )
		type &= ~TO_SLEEP;

	if ( type == TO_CHAR )
	{
		if ( ch && ( SENDOK( ch ) || IS_NPC( ch ) ) )
			PerformAct( str, ch, obj, vict_obj, ch, type, bgColor, disorientable );
		return ;
	}

	if ( type == TO_VICT )
	{
		if ( ( to = ( Character * ) vict_obj ) && SENDOK( to ) )
			PerformAct( str, ch, obj, vict_obj, to, type, bgColor, disorientable );
		return ;
	}

	/* ASSUMPTION: at this point we know type must be TO_NOTVICT or TO_ROOM */

	if ( ch && ch->in_room )
		to = ch->in_room->people;
	else if ( obj && obj->in_room )
		to = obj->in_room->people;
	else
	{
		Log( "SYSERR: no valid target to Act()!" );
		return ;
	}

	for ( ; to; to = to->next_in_room )
	{
		if ( !SENDOK( to ) || ( to == ch ) )
			continue;
		if ( hide_invisible && ch && !CAN_SEE( to, ch ) )
			continue;
		if ( AFF_FLAGGED(to, AFF_BLIND) )
			continue;
		if ( type != TO_ROOM && to == vict_obj )
			continue;
		PerformAct( str, ch, obj, vict_obj, to, type, bgColor, disorientable );
	}
}

bool Character::LoggedIn()
{
	return( this->desc && this->desc->loggedIn );
}

void rebootCountdown( void )
{
	DateTime timeNow = DateTime();

	__int64 ticksBetween = (__int64)ceil(((double)rebootTime.getTime()-(double)timeNow.getTime()) / 60);

	if (	(ticksBetween == 15 && !rebootNotified15)
		||	(ticksBetween == 10 && !rebootNotified10)
		||	(ticksBetween ==  5 && !rebootNotified5)
		||  (ticksBetween ==  1 && !rebootNotified1)
	   )
	{
		if(ticksBetween == 15)
			rebootNotified15 = true;
		else if(ticksBetween == 10)
			rebootNotified10 = true;
		else if(ticksBetween == 5)
			rebootNotified5 = true;
		else if(ticksBetween == 1)
			rebootNotified1 = true;
		std::stringstream msg;

		msg << "Reboot in " << ticksBetween << " minute" << (ticksBetween > 1 ? "s" : "") << "\007\r\n";
		sendToAll( msg.str().c_str() );
	}

	if ( timeNow.after(rebootTime) )
	{

		sendToAll( "\r\n\nThe game is rebooting. Come back in a few moments.\007\r\n\n", true );
		crashsaveAll();
		circle_shutdown = 1;
	}
}



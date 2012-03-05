/* ************************************************************************
*   File: comm.h                                        Part of CircleMUD *
*  Usage: header file: prototypes of public communication functions       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef CIRCLE_COMM_H
#define CIRCLE_COMM_H

#include "structs.h"

const int NUM_RESERVED_DESCS = 8;

/* comm.c */
void	sendToAll(const char *messg, bool instant=false);
void	sendToRoom(const char *messg, Room *room);
void	sendToOutdoor(const char *messg);
void	perform_to_all(const char *messg, Character *ch);

void	perform_act(const char *orig, Character *ch,
                 Object *obj, const void *vict_obj, Character *to);

void	Act(const char *str, int hide_invisible, Character *ch,
         Object *obj, const void *vict_obj, int type, const char *bgColor=0);

const int TO_ROOM = 1;
const int TO_VICT = 2;
const int TO_NOTVICT = 3;
const int TO_CHAR = 4;
const int TO_SLEEP = 128; // to char, even if sleeping

int	WriteToDescriptor(Descriptor * , const char *txt);
void write_to_output(Descriptor *t, const char *txt, ...);
void page_string(Descriptor *d, char *str, int keep_internal);

const char *health(int percent);
const char *mana(int percent);
const char *moves(int percent);
void SetClock();
clock_t GetClockDifference();
extern clock_t GlobalClock;
#define SEND_TO_Q(messg, desc)  write_to_output(desc, (messg))

typedef RETSIGTYPE sigfunc(int);

extern void *Pointer;
extern std::list< Character* > WaitingList;



#define LAG_MONITOR_KJS_HEARTBEAT			 "KJS_HEARTBEAT"
#define LAG_MONITOR_CHECK_WAIT_STATE		 "CHECK_WAIT_STATE"
#define LAG_MONITOR_PROCESS_EVENTS			 "PROCESS_EVENTS"
#define LAG_MONITOR_CHECK_TIMERS			 "CHECK_TIMERS"
#define LAG_MONITOR_TAINT_EVENTS			 "TAINT_EVENTS"
#define LAG_MONITOR_THREADED_JOBS			 "THREADED_JOBS"
#define LAG_MONITOR_KJS_SOCKET_EVENTS		 "KJS_SOCKET_EVENTS"
#define LAG_MONITOR_UPDATE_BOOT_HIGH		 "UPDATE_BOOT_HIGH"
#define LAG_MONITOR_KJS_RANDOM_TRIGGERS		 "KJS_RANDOM_TRIGGERS"
#define LAG_MONITOR_ZONE_UPDATE				 "ZONE_UPDATE"
#define LAG_MONITOR_CHECK_FIGHTING			 "CHECK_FIGHTING"
#define LAG_MONITOR_AUTO_SAVE				 "AUTO_SAVE"
#define LAG_MONITOR_CHECK_IDLE_PASSWORDS	 "CHECK_IDLE_PASSWORDS"
#define LAG_MONITOR_MOBILE_ACTIVITY			 "MOBILE_ACTIVITY"
#define LAG_MONITOR_UPDATE_GATES			 "UPDATE_GATES"
#define LAG_MONITOR_PERFORM_VIOLENCE		 "PERFORM_VIOLENCE"
#define LAG_MONITOR_PURGE_QUEUE				 "PURGE_QUEUE"
#define LAG_MONITOR_WEATHER_AND_TIME		 "WEATHER_AND_TIME"
#define LAG_MONITOR_AFFECT_UPDATE			 "AFFECT_UPDATE"
#define LAG_MONITOR_POINT_UPDATE			 "POINT_UPDATE"
#define LAG_MONITOR_KJS_TIME_TRIGGERS		 "KJS_TIME_TRIGGERS"
#define LAG_MONITOR_REBOOT_COUNTDOWN		 "REBOOT_COUNTDOWN"
#define LAG_MONITOR_DELETE_OLD_TRACKS		 "DELETE_OLD_TRACKS"
#define LAG_MONITOR_AUCTION_UPDATES			 "AUCTION_UPDATES"
#define LAG_MONITOR_SAVE_CORPSE_ROOMS		 "SAVE_CORPSE_ROOMS"
#define LAG_MONITOR_NEWBIE_TIPS				 "NEWBIE_TIPS"
#define LAG_MONITOR_ADD_FORUM_USERS			 "ADD_FORUM_USERS"
#define LAG_MONITOR_ARCHIVE_FORUM_USERS		 "LAG_MONITOR_ARCHIVE_FORUM_USERS"

struct LagRoutineEntry
{
	__int64 numberOfTimesRun;
	double runTime;

	double avgRunTime();
};

class LagMonitor
{
	Clock timer;

public:
	std::map< std::string, LagRoutineEntry > mRoutineTimeCard;

	LagMonitor();
	~LagMonitor();

	void startClock();
	void stopClock( const std::string &sRoutineName );
	LagRoutineEntry getRoutineEntry( const std::string &sRoutineName );


};

extern class LagMonitor lagMonitor;




#endif





/* ************************************************************************
*   File: Act.wizard.c                                  Part of CircleMUD *
*  Usage: Player-level god commands and other goodies                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "constants.h"
#include "olc.h"
#include "clans.h"
#include "shop.h"
#include "stats.h"
#include "customcmds.h"
#include "weaves.h"
#include "mobs.h"
#include "md5.h"
#include "js.h"
#include "js_trigger.h"
#include "js_functions.h"
#include "accounts.h"
#include "Descriptor.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"
#include "zones.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "MiscUtil.h"
#include "CharacterUtil.h"
#include "StringUtil.h"
#include "ForumUtil.h"
#include "ClanUtil.h"
#include "MailUtil.h"
#include "SQLUtil.h"
#include "SystemUtil.h"

#include "kuSockets.h"
#include "kuListener.h"
#include "kuDescriptor.h"
#include "threaded_jobs.h"
#include "kuClient.h"
#include "UserLogoutType.h"
#include "UserDisabledCommand.h"
#include "ClanQuestPointTransaction.h"
#include "dg_event.h"
#include "MobLoadLogger.h"

#include "Exception.h"
#include "Game.h"

/*   external vars  */
extern GameTime time_info;
extern Character *character_list;
extern Object *object_list;
extern Descriptor *descriptor_list;
extern Index *obj_index;
extern attack_hit_type attack_hit_text[];
extern const char *class_abbrevs[];
extern char *race_abbrevs[];
extern time_t boot_time;
extern int circle_shutdown;
extern int circle_restrict;
extern int top_of_mobt;
extern int top_of_objt;
extern DateTime rebootTime;
extern int rank_req[14];
extern int Seconds;
extern std::list<Track *> TrackList;
extern void check_autowiz(Character *ch);
extern char mudname[256];
extern int mother_desc;
extern int spell_sort_info[MAX_SKILLS+1];
extern Social *soc_mess_list;
extern int top_of_socialt;		
extern time_t bootTime;
extern int pulse;
extern bool rebootNotified15, rebootNotified10, rebootNotified5, rebootNotified1;

/* for chars */
extern char *credits;
extern char *motd;
extern char *imotd;
extern char *background;
extern char *startup;
extern const char *class_types[];
extern const char *pc_class_types[];
extern const char *pc_race_types[];

/* extern functions */
void AddRoomToWorld( Room *TheRoom );
int getCommandIndex(const std::string &commandText);
extern void show_shops(Character * ch, char *value);
extern void do_start(Character *ch);
extern void update_legend(Character *ch);
extern void save_legends(void);
extern void save_player_clan(Character *ch, int number);
extern int level_exp(int level);
extern int parse_class(char arg);
extern int parse_race(char arg);
extern Character *find_char(long n);
extern unsigned long int bandwidth;
extern std::tr1::unordered_map<void*, pair<std::string, flusspferd::value> > mapper;

/* local functions */
ACMD(do_advance);
ACMD(do_at);
ACMD(do_award);
ACMD(do_clan);
ACMD(do_council);
ACMD(do_countdown);
ACMD(do_copy);
ACMD(do_copyover);
ACMD(do_date);
ACMD(do_dc);
ACMD(do_declan);
ACMD(do_delete);
ACMD(do_demote);
ACMD(do_dig);
ACMD(do_disable);
ACMD(do_echo);
ACMD(do_enable);
ACMD(do_find);
ACMD(do_force);
ACMD(do_gcomm);
ACMD(do_gecho);
ACMD(do_goto);
ACMD(do_invis);
ACMD(do_ipfind);
ACMD(do_lag);
ACMD(do_last);
ACMD(do_load);
ACMD(do_memory);
ACMD(do_noreply);
ACMD(do_override);
ACMD(do_pardon);
ACMD(do_poofset);
ACMD(do_purge);
ACMD(do_qval);
ACMD(do_rank);
ACMD(do_reset);
ACMD(do_restore);
ACMD(do_retool);
ACMD(do_return);
ACMD(do_saveall);
ACMD(do_send);
ACMD(do_set);
ACMD(do_wshow);
ACMD(do_shutdown);
ACMD(do_snoop);
ACMD(do_statfind);
ACMD(do_switch);
ACMD(do_syslog);
ACMD(do_teleport);
ACMD(do_test_roll);
ACMD(do_trans);
ACMD(do_vnum);
ACMD(do_vstat);
ACMD(do_warrant);
ACMD(do_wizlock);
ACMD(do_wiznet);
ACMD(do_wizutil);
ACMD(do_wotmud);
ACMD(do_zap);
ACMD(do_zreset);

Room *FindTargetRoom(Character * ch, char *rawroomstr);
int perform_set(Character *ch, Character *vict, int mode, char *val_arg, int file);
void perform_immort_invis(Character *ch, int level);
void do_stat_room(Character * ch);
void do_stat_object(Character * ch, Object * j);
void do_stat_character(Character * ch, Character * k);
void print_zone_to_buf(char *bufptr, int zone);
void perform_immort_vis(Character *ch);
void perform_warrant(Character *ch, Character *victim, int clan);
void redit_save_internally(Descriptor *d);
int topUserId();

void redit_save_to_disk( int lowVnum, int highVnum );

ACMD(do_saveall)
{
	char arg1[MAX_INPUT_LENGTH];
	unsigned int i = 0;

	OneArgument(argument, arg1);

	if(!argument || !*arg1)
	{
		ch->send("Syntax: saveall <TYPE>. Possible Types: Zones, Mobs, Rooms, Objects, OLC.\r\n");
		return;
	}

	if(!strn_cmp(arg1, "Zones", strlen(arg1)))
	{
		Zone *zone;
		for(i = 0;(zone=ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;++i)
			zone->save();
		ch->send("All zones have been saved.\r\n");
		MudLog(CMP, LVL_GOD, TRUE, "%s saved all zones.", GET_NAME(ch));
	}

	else if(!strn_cmp(arg1, "Mobs", strlen(arg1)))
	{
		for(i = 0;i < ZoneManager::GetManager().NumZones();++i)
		{
			Log("Saving mobs for zone: %d\r\n", ZoneManager::GetManager().GetZoneByRnum(i)->getVnum());
			MobManager::GetManager().SavePrototypes(ZoneManager::GetManager().GetZoneByRnum(i)->getVnum());
		}
		ch->send("All mobs have been saved.\r\n");
		MudLog(CMP, LVL_GOD, TRUE, "%s saved all mobiles.", GET_NAME(ch));
	}

	else if(!strn_cmp(arg1, "Objects", strlen(arg1)))
	{
		for(i = 0;i < ZoneManager::GetManager().NumZones();++i)
			oedit_save_to_disk(i);
		ch->send("All objects have been saved.\r\n");
		MudLog(CMP, LVL_GOD, TRUE, "%s saved all objects.", GET_NAME(ch));
	}

	else if(!strn_cmp(arg1, "Rooms", strlen(arg1)))
	{
		Clock ReditSaveClock;
		ReditSaveClock.reset(true);

		if( World.empty() == false ) {
			int lowVnum = World.front()->getVnum();
			int highVnum = World.back()->getVnum();
			redit_save_to_disk( lowVnum, highVnum );
		}
		ReditSaveClock.turnOff();
		ReditSaveClock.print();
		ch->send("All rooms have been saved.\r\n");
		MudLog(CMP, LVL_GOD, TRUE, "%s saved all rooms.",  GET_NAME(ch));
	}

	else if(!strn_cmp(arg1, "OLC", strlen(arg1)))
	{
		if( World.empty() == false ) {
			int lowVnum = World.front()->getVnum();
			int highVnum = World.back()->getVnum();
			redit_save_to_disk( lowVnum, highVnum );
		}
		for(i = 0;i < ZoneManager::GetManager().NumZones();++i)
		{
			Zone *zone = ZoneManager::GetManager().GetZoneByRnum(i);
			oedit_save_to_disk(i);
			MobManager::GetManager().SavePrototypes(ZoneManager::GetManager().GetZoneByRnum(i)->getVnum());
			zone->save();
		}
		ch->send("All of OLC has been saved.\r\n");
		MudLog(CMP, LVL_GOD, TRUE, "%s saved all OLC.",  GET_NAME(ch));
	}
	else
	{
		ch->send("Invalid option. Possible Types: Zones, Mobs, Rooms, Objects, OLC.\r\n");
		return;
	}
}

/***
  * Galnor 10/07/2010 - Remove the reply target of anyone whose last
  *  tell received originated from this player.
  *
  ***/
ACMD(do_noreply)
{
	for(Character *teller = character_list;teller != NULL;teller = teller->next)
	{
		if( teller->last_tell == ch->player.idnum && GET_LEVEL(teller) < GET_LEVEL(ch) )
			teller->last_tell = 0;
	}
	ch->send("You are no longer set as anyone's reply target.\r\n");
}

ACMD(do_test_roll)
{
	char Type[MAX_INPUT_LENGTH], VictName[MAX_INPUT_LENGTH], Number[MAX_INPUT_LENGTH], Show[MAX_INPUT_LENGTH],
	CharName[MAX_INPUT_LENGTH];
    //General variable names.
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH], arg4[MAX_INPUT_LENGTH],
	arg5[MAX_INPUT_LENGTH];

	if( GET_LEVEL(ch) < LVL_GRGOD && !PLR_FLAGGED(ch, PLR_STAT_EDITOR) )
	{
		ch->send("You do not have permission to use that command.\r\n");
		return;
	}

	Character *victim, *test;
	int offense = 0, defense = 0, n = 0, i = 0, missed = 0, landed = 0;
	float perc = 0.0f;
	bool show = false;

    strcpy(argument, OneArgument(TwoArguments(TwoArguments(argument, arg1, arg2), arg3, arg4), arg5));
	strcpy(Type, arg1); strcpy(CharName, arg2); strcpy(VictName, arg3);
	strcpy(Number, arg4); strcpy(Show, arg5);

	if(!strn_cmp(Type, "stats", strlen(Type)))
	{
	    Character *Test = new Character();
	    char arg6[MAX_INPUT_LENGTH];

		OneArgument(argument, arg6);

		n = atoi(arg2);
        if(!*arg2 || !*arg3 || !*arg4 || !*arg5)
        {
            ch->send("Syntax: Testroll stats <Number of Rolls> <Gender> <Race> <Class> <Show?>\r\n");
        }
	    if( (GET_SEX(Test) = SexByString(arg3)) == SEX_UNDEFINED)
        {
            ch->send("Invalid gender.\r\n");
            delete Test;
            return;
        }
	    if( (GET_RACE(Test) = RaceByString(arg4)) == RACE_UNDEFINED)
        {
            ch->send("Invalid race.\r\n");
            delete Test;
            return;
        }
	    if( (GET_CLASS(Test) = ClassByString(arg5)) == CLASS_UNDEFINED)
        {
            ch->send("Invalid class.\r\n");
            delete Test;
            return;
        }

        bool showStats = (!str_cmp(arg6, "yes"));

	    for(int i = 0;i < n;++i)
	    {
	        if(!(StatManager::GetManager().RollStats(Test)))
	        {
	        	delete Test;
	        	return;
	        }

	        if(Test->GetStr() + Test->GetDex() + Test->GetCon() >= 53)
                ++landed;
            else
                ++missed;
            if(showStats)
				ch->send("Roll #%d: Str:%d, Int:%d, Wis:%d, Dex:%d, Con:%d\r\n", (i+1),
                Test->GetStr(), Test->GetInt(), Test->GetWis(), Test->GetDex(), Test->GetCon());
	    }
	    delete Test;
		perc = (n != 0) ? ( (float)landed / n * 100) : (0);
		ch->send("Superstat %%: %s%.2f%s%%, Total Superstats: %s%d%s. Total Non-Superstats: %s%d%s\r\n",
				 COLOR_CYAN(ch, CL_COMPLETE), perc, COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE),
				 landed, COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE), missed, COLOR_NORMAL(ch, CL_COMPLETE));
	    return;
	}


	if(!argument || !*Type || !*VictName)
	{
		ch->send("Format: Testroll <Test Type> <Char Name> <Victim Name> <Number of Rolls><Show (yes/no)\r\n"
		         "*Note: Command shows 100 rolls max.\r\n");
		return;
	}

	if(!*Number)
	{
		n = 1;
	}
	else
		n = atoi(Number);
	if(n <= 0 || n > 100000)
	{
		ch->send("Your range must be from 1 to 100000.\r\n");
		return;
	}

	n = atoi(Number);

	if(*Show && !strn_cmp(Show, "Yes", strlen(Show)))
		show = true;
	else if(!strn_cmp(Show, "No", strlen(Show)))
		show = false;
	else if(*Show)
	{
		ch->send("You must either choose Yes or No, or leave this field blank.\r\n");
		return;
	}
	if(!(test = get_char_vis(ch, CharName)))
	{
		ch->send("Unable to find test character '%s'.\r\n", CharName);
		return;
	}
	if(!(victim = get_char_vis(ch, VictName)))
	{
		ch->send("Unable to find victim '%s'.\r\n", VictName);
		return;
	}

	if(!strn_cmp(Type, "bash", strlen(Type)))
	{
		for(i = 0;i < n;++i)
		{
			bool roll;

			if( (roll = test->rollBash( victim, offense, defense )) )
				++landed;
			else
				++missed;

			if(show && i < 100)
			{
				ch->send("Bash Roll: Tester's offense: %d, Victim's defense: %d, outcome: %s\r\n",
					offense, defense, (roll ? "Bashed" : "Missed") );
			}
		}
	}
	else if(!strn_cmp(Type, "charge", strlen(Type)))
	{
		for(i = 0;i < n;++i)
		{
			offense = test->ChargeOffenseRoll(IS_TROLLOC(test) ? SKILL_SKEWER : SKILL_CHARGE);
			defense = victim->ChargeDefenseRoll(IS_TROLLOC(test) ? SKILL_SKEWER : SKILL_CHARGE);

			if(offense > defense)
				++landed;
			else
				++missed;

			if(show && i < 100)
			{
				ch->send("Charge Roll: Tester's offense: %d, Victim's defense: %d, outcome: %s\r\n",
				         offense, defense, offense > defense ? "Charged" : "Missed");
			}
		}
	}
	else if(!strn_cmp(Type, "melee", strlen(Type)))
	{
		for(i = 0;i < n;++i)
		{
			offense = test->OffenseMeleeRoll();
			defense = victim->DefenseMeleeRoll();
			int roll = MiscUtil::random(1, 20);

			if ( roll != 1 && ( (roll == 20 && !IS_NPC(test)) || offense > defense ) )
				++landed;
			else
				++missed;

			if(show && i < 100)
			{
				ch->send("Melee Roll: Tester's Offense: %d, Victim's Defense: %d, outcome: %s\r\n",
				         offense, defense, offense < defense ? "Missed" : "Hit");
			}
		}
	}
	else if(!strn_cmp(Type, "backstab", strlen(Type)))
	{
		for(i = 0;i < n;++i)
		{
			if(test->BackstabLanded(victim))
				++landed;
			else
				++missed;

			if(show && i < 100)
			{
				ch->send("Stab Roll: outcome: %s\r\n",
				         offense < defense ? "Missed" : "Hit");
			}
		}
	}
	else if(!strn_cmp(Type, "precisestrike", strlen(Type)) || !strn_cmp(Type, "strike", strlen(Type)))
	{
		for(i = 0;i < n;++i)
		{
			bool roll;

			if( (roll = test->rollPrecStrike( victim, offense, defense )) )
				++landed;
			else
				++missed;

			if(show && i < 100)
			{
				ch->send("Strike Roll: Tester's offense: %d, Victim's defense: %d, outcome: %s\r\n",
					offense, defense, (roll ? "Landed" : "Missed") );
			}
		}
	}
	else
	{
		ch->send("Invalid option.\r\n");
		return;
	}

	if(!n)
		perc = 0;
	else
		perc = (float)((float)landed / n) * 100;
	ch->send("Land %%: %s%.1f%s%%, Total Landed: %s%d%s. Total Missed: %s%d%s\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), perc, COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE),
	         landed, COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE), missed, COLOR_NORMAL(ch, CL_COMPLETE));
}

/*****
	* Coded by Galnor on 04/21/2009
	*	- This will allow one to copy one of the desired types(kit,obj,mob,room,trig)
	*	  of a pre-existing prototype to a destination prototype(existing or not).
	*
******/

ACMD(do_copy)
{
	char type[MAX_INPUT_LENGTH], sVnum1[MAX_INPUT_LENGTH], sVnum2[MAX_INPUT_LENGTH];
	int vnum1, vnum2, real;

	OneArgument(TwoArguments(argument, type, sVnum1), sVnum2);

	if( !*type || !*sVnum1 || !*sVnum2 )
	{
		ch->send("USAGE: copy [room|mob|kit] [source vnum] [target vnum]\r\n");
		return;
	}
	vnum1 = atoi(sVnum1);
	vnum2 = atoi(sVnum2);

	if( !strn_cmp(type, "kit", strlen(type)) )
	{
		Kit *Source, *Destination;

		if( !(Source = KitManager::GetManager().GetKitByVnum(vnum1)) )
		{
			ch->send("The source kit(#%d) does not exist.\r\n", vnum1);
			return;
		}
		if( !(Destination = KitManager::GetManager().GetKitByVnum(vnum2)) )
		{//Make a new kit.
			Destination = new Kit();
		}
		Destination->CopyFrom(Source, false);
		unsigned int index;
		for(index = 0;index < Destination->KitItems.size();++index)
		{
			for(unsigned int innerIndex = 0;innerIndex < Destination->KitItems[ index ].size();++innerIndex)
			{
				Destination->KitItems[ index ][ innerIndex ].SetDBID(-1);
				Destination->KitItems[ index ][ innerIndex ].UnsetDeletion();
			}
		}
		for(index = 0;index < Destination->KitInventory.size();++index)
		{
			Destination->KitInventory[ index ].SetDBID(-1);
			Destination->KitInventory[ index ].UnsetDeletion();
		}
		Destination->vnum = vnum2;
		Destination->save();
		KitManager::GetManager().AddKit(Destination);
	}
	else if( !strn_cmp(type, "mob", strlen(type)) )
	{
		Zone *zone;
		if( (zone = ZoneManager::GetManager().GetZoneByRoomVnum( vnum2 )) == NULL )
		{
			ch->send("There is no zone which can contain that room!\r\n");
			return;
		}
		Character *Source, *Destination;
		if( !(Source = MobManager::GetManager().GetPrototypeByVnum(vnum1)) )
		{
			ch->send("The source MOB(#%d) does not exist.\r\n",vnum1);
			return;
		}
		if( !(Destination = MobManager::GetManager().GetPrototypeByVnum(vnum2)) )
			MobManager::GetManager().AddPrototype((Destination=MobManager::GetManager().AllocateRawMobile()), vnum2);
		sh_int prev_rnum = Destination->nr;
		MobManager::GetManager().CopyPrototype(Destination,Source);
		Destination->nr=prev_rnum;
		MobManager::GetManager().UpdateLiveMobilesFromPrototype(Destination);

		//Add to save list.
		Zone *z = ZoneManager::GetManager().GetZoneByRoomVnum(vnum2);
		if( z != NULL )
			olc_add_to_save_list( z->getVnum(), OLC_SAVE_ROOM );
	}
	else if( !strn_cmp(type, "room", strlen(type)) )
	{
		Zone *zone;
		if( (zone = ZoneManager::GetManager().GetZoneByRoomVnum( vnum2 )) == NULL )
		{
			ch->send("There is no zone which can contain that room!\r\n");
			return;
		}
		if( !zone->CanEdit(ch) )
		{
			ch->send("You do not have permission to copy a room into that zone.\r\n");
			return;
		}

		Room *Source, *Destination;

		if( (real = real_room(vnum1)) == NOWHERE )
		{
			ch->send("The source room(#%d) does not exist.\r\n", vnum1);
			return;
		}
		//Room exists.
		Source = World[real];
		if( (real = real_room(vnum2)) == NOWHERE )
			Destination = new Room();
		else
			Destination = World[real];
		Destination->copy(Source,false);
		Destination->setVnum(vnum2);
		Destination->setZoneNumber(zone->GetRnum());
		if( real == NOWHERE )//New room. Add to world.
			AddRoomToWorld( Destination );
		olc_add_to_save_list( Destination->getZone()->getVnum(), OLC_SAVE_ROOM );
	}
	ch->send("Copy was successful.\r\n");
}

// Serai - 06/18/04 - Copyover for Kinslayer.
/***
ACMD(do_copyover)
{

	skip_spaces(&argument);

	if (str_cmp(argument, "now"))
	{
		ch->send("To start a copyover, type \"copyover now\".  There is no timer.\r\n");
		return;
	}

	sendToAll("Warning:  Copyover started.  You will not be disconnected.\r\n");

	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "Copyover started by %s.", GET_NAME(ch));

	// Serai - chdir(..) because CircleMUD does a chdir() into their data directory
	//         which is by default lib/ ... If you specify another directory copyover
	//         will probably fail.
	if (chdir("..") == -1)
	{
		sprintf(buf, "Copyover failed:  chdir(..) with error code %d.", errno);
	}
		else
		{
			execl(mudname, mudname, NULL);
			sprintf(buf, "Copyover failed:  execl(%s, %s, NULL) with error code %d.", mudname, mudname, errno);
		}

		if (chdir("lib") == -1)
		{
			Log("execl() failed, and we can't find your data directory again...  Quitting.");
			circle_shutdown = 1;

			return;
		}

		MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, buf);
	}

	if (remove(".copyover") != 0)
		Log("SYSERR: Could not remove(.copyover)!  Remove it manually!");

	sendToAll("Copyover failed.\r\n");

	return;
}
***/

int Character::PracticeGain(int level)
{
	if(level <= 15)
	{
		if( IS_DREADGUARD(this) )
			return 4;
		return 5;
	}
	else if(level <= 30)
		return 3;
	else if(level <= 45)
		return 1;
	else
		return 2;
	return 0;
}

int Character::SPracticeGain(int level)
{
	if( level <= 30 )
		return 7;
	return 2;
}
int Character::TPracticeGain(int level)
{
	if( level == 15 ) return (1);
	if( level == 30 ) return (1);
	return (0);
}

void Character::ResetAllSkills()
{
	this->ResetSkills();
	this->ResetTrades();
	this->ResetSpells();
}

void Character::ResetSkills()
{
	int i = 0;

	if( !IS_NPC(this) )
	{
		this->PlayerData->skillpracs = 2;
		while( i <= GET_LEVEL(this) )
			this->PlayerData->skillpracs += this->PracticeGain(i++);
	}
	for(i = 0; i < MAX_SKILLS; ++i)
	{
		Weave *skill = WeaveManager::GetManager().GetWeave(i);
		if( skill && !skill->classSet("Trade") && !skill->classSet("Channeler") && !skill->classSet("Dreadlord") )
			SET_SKILL(this, i, 0);
	}
	this->SetSkillDefaults();
}

void Character::ResetTrades()
{
	int i = 1;

	if( !IS_NPC(this) )
	{
		this->PlayerData->tradepracs = 0;
		while( i <= GET_LEVEL(this) )
			this->PlayerData->tradepracs += this->TPracticeGain(i++);
	}
	for(i = 0; i < MAX_SKILLS; ++i)
	{
		Weave *skill = WeaveManager::GetManager().GetWeave(i);
		if( skill && skill->classSet("Trade") )
			SET_SKILL(this, i, 0);
	}
	this->SetSkillDefaults();
}

void Character::ResetSpells()
{
	int i = 0;

	if( !IS_NPC(this) )
	{
		this->PlayerData->spellpracs = 0;
		while( i <= GET_LEVEL(this) )
			this->PlayerData->spellpracs += this->SPracticeGain(i++);
	}
	for(i = 0; i < MAX_SKILLS; ++i)
	{
		Weave *skill = WeaveManager::GetManager().GetWeave(i);
		if( skill && ( skill->classSet("Channeler") || skill->classSet("Dreadlord") ) )
			SET_SKILL(this, i, 0);
	}
	this->SetSkillDefaults();
}

ACMD(do_qval)
{
	//Variabals
	char playerName[MAX_INPUT_LENGTH], val1[MAX_INPUT_LENGTH], val2[MAX_INPUT_LENGTH];
	std::string questName;
	Quest *q;
	Character *victim;
	bool nquest = false;

	//Get one word from argument and put it onto playerName.
	HalfChop(argument, playerName, argument);

	//Check to see if user input anything for the player name.
	if(!*playerName)
	{
		ch->send("Syntax: qval <Player Name> \'<Quest Name>\' <Value>\r\n");
		return;
	}

	//Skip spaces in argument string.
	skip_spaces(&argument);

	//Make sure the next character in argument is a quote
	if(*argument != '\'')
	{
		ch->send("Quest name must be surrounded by \'.\r\n", *argument);
		return;
	}

	++argument;

	//Print everything inside of the quotes to questName.
	for(;*argument != '\'';++argument)
	{

		if(!*argument)
		{
			ch->send("Quest name must be surrounded by \'\r\n");
			return;
		}

		questName += *argument;
	}

	++argument;

	//This checks to see if ch can see victim, based on his name.
	if(!(victim = get_char_vis(ch, playerName)))
	{
		ch->send(NOPERSON);
		return;
	}
	//Skip spaces
	skip_spaces(&argument);

	//This will take the first word from argument and copy it to val1, and copy the second word to val2.
	TwoArguments(argument, val1, val2);

	//If there was nothing for val1, we send ch the syntax code.
	if(!*val1)
	{
		ch->send("Syntax: qval <Player Name> \'<Quest Name>\' <Value> OR\r\n qval <Player Name> SET <Value>\r\n");
		return;
	}

	//Create a new quest if this one does not exist...
	if(!(q = victim->GetQuest(questName)))
	{
		//Allocate the quest pointer
		q = new Quest(victim, questName, 0);
		nquest = true;
	}

	if(!str_cmp(val1, "set"))
	{
		if(!*val2)
		{
			victim->quests.remove(q);
			if(nquest)
				delete q;

			ch->send("You must specify what value you are setting this quest variable to.\r\n");
			return;
		}
		q->var = atoi(val2);
	}
	else
		q->var += atoi(val1);

	ch->send("Variable for \"%s\" set to %d for player %s.\r\n", q->name.c_str(), q->var, GET_NAME(victim));
	MySQLSaveQuest(victim->player.name, q, !nquest);
	return;

}

//Added by Galnor on 08/02/2009 - this command will allow an immortal to submit a command without possibility
//of interruption by a script.
ACMD(do_override)
{
	//What we're doing here is very simple. Grab the appended text and submit it back to the interpreter...
	ch->ignoreCommandTrigger = (true);
	skip_spaces(&argument);
	ch->InterpCommand(argument);
	ch->ignoreCommandTrigger = (false);
}

//Coded by Galnor on February 6th, 2004. Calculates variouse memory types in the program.//
// Minor changes by Serai...
ACMD(do_memory)
{
	int number;
	size_t total = 0;
	Character *vict;
	Object *obj;
	get_char_cols( ch );

	ch->send("These are the different memory sizes for the game structures:\r\n\n", ch);

	for(number = 0, vict = character_list;vict;vict = vict->next)
	{
//		if( vict->IsPurged() ) continue;
		number++;
	}

	ch->send("Bandidth used:    [%s%f%s] GB.\r\n", COLOR_GREEN(ch, CL_NORMAL), GB(bandwidth), COLOR_NORMAL(ch, CL_NORMAL));
	/*CHARACTERS*/
	ch->send("Character:        Single: [%s%5i%s] bytes, Number: [%s%5d%s], Total: [%s%10f%s] MB.\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), sizeof(Character), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), number, COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), MB(number * sizeof(Character)), COLOR_NORMAL(ch, CL_NORMAL));

	total += sizeof(Character) * number;


	/*OBJECTS*/
	for(number = 0, obj = object_list;obj;obj = obj->next)
		number++;

	ch->send("Object:           Single: [%s%5i%s] bytes, Number: [%s%5d%s], Total: [%s%10f%s] MB.\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), sizeof(Object), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), number, COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), MB(number * sizeof(Object)), COLOR_NORMAL(ch, CL_NORMAL));

	total += sizeof(Object) * number;


	/*ROOMS*/
	ch->send("Room:             Single: [%s%5i%s] bytes, Number: [%s%5d%s], Total: [%s%10f%s] MB.\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), sizeof( Room), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), World.size(), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), MB(World.size() * sizeof( Room)),
	         COLOR_NORMAL(ch, CL_NORMAL));

	total += sizeof(Room) * World.size();

	ch->send("Track:            Single: [%s%5i%s] bytes, Number: [%s%5d%s], Total: [%s%10f%s] MB.\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), sizeof(Track), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), TrackList.size(), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), MB(TrackList.size() * sizeof(Track)), COLOR_NORMAL(ch, CL_NORMAL));

	total += sizeof(Track) *number;

	ch->send("Gates:            Single: [%s%5i%s] bytes, Number: [%s%5d%s], Total: [%s%10f%s] MB.\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), sizeof(Gate), COLOR_NORMAL(ch, CL_NORMAL),
			 COLOR_GREEN(ch, CL_NORMAL), GateManager::GetManager().NumberOfGates(), COLOR_NORMAL(ch, CL_NORMAL),
			 COLOR_GREEN(ch, CL_NORMAL), MB(GateManager::GetManager().NumberOfGates() * sizeof(Gate)), COLOR_NORMAL(ch, CL_NORMAL));

	total += sizeof(Gate) * GateManager::GetManager().NumberOfGates();

	number = 0;
	for(int i = 0;i < (int)World.size();++i)
	{
		for(int d = 0;d < NUM_OF_DIRS;++d)
		{
			if( World[i]->dir_option[ d ] ) ++number;
		}
	}

	ch->send("Directions:       Single: [%s%5i%s] bytes, Number: [%s%5d%s], Total: [%s%10f%s] MB.\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), sizeof(Exit), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), number, COLOR_NORMAL(ch, CL_NORMAL),
			 COLOR_GREEN(ch, CL_NORMAL), MB(number * sizeof(Exit)), COLOR_NORMAL(ch, CL_NORMAL));

	total += sizeof(Exit) * number;

	std::tr1::unordered_map<void*, pair<std::string, flusspferd::value> >::iterator iter;
	int jsValues = mapper.size();
	int jsCharacters = 0, jsObjects = 0, jsRooms = 0;
	for(iter = mapper.begin();iter != mapper.end();++iter) {
		flusspferd::object obj = (*iter).second.second.to_object();
		if( flusspferd::is_native<JSCharacter>( obj ) )
			++jsCharacters;
		if( flusspferd::is_native<JSObject>( obj ) )
			++jsObjects;
		if( flusspferd::is_native<JSRoom>( obj ) )
			++jsRooms;
	}
	ch->send("JS Values:                               Number: [%s%5d%s]\r\n", grn, jsValues, nrm);
	ch->send("JS Characters:                           Number: [%s%5d%s]\r\n", grn, jsCharacters, nrm);
	ch->send("JS Objects:                              Number: [%s%5d%s]\r\n", grn, jsObjects, nrm);
	ch->send("JS Rooms:                                Number: [%s%5d%s]\r\n", grn, jsRooms, nrm);

	/* TOTAL */
	ch->send("\r\nTotal:                    [%s%9d%s] bytes, [%s%13f%s] KB, [%s%10f%s] MB.\r\n\n",
	         COLOR_GREEN(ch, CL_NORMAL), total, COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), KB(total), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), MB(total), COLOR_NORMAL(ch, CL_NORMAL));

	//	ch->send("Other memory:\r\n");

}

ACMD(do_ipfind)
{
	char nameString[MAX_INPUT_LENGTH];
	std::stringstream sqlBuffer;
	sql::Query query;
	sql::Row row;
	std::list<std::string> ipAddressList;
	
	OneArgument(argument, nameString);

	if(!*nameString)
	{
		ch->send("Usage: ipfind <player name>\r\n");
		return;
	}

	PlayerIndex *index = CharacterUtil::getPlayerIndexByUserName(nameString);

	if(index == NULL)
	{
		ch->send(NOPERSON);
		return;
	}

	sqlBuffer	<< " SELECT DISTINCT(host) AS IpAddress"
				<< " FROM userLogin"
				<< " WHERE user_id = " << index->id;

	try {
		query = gameDatabase->sendQuery(sqlBuffer.str());
	}
	catch( sql::QueryException e )
	{
		MudLog(BRF, MAX(GET_LEVEL(ch), LVL_APPR), TRUE, "IPFind: %s", e.getMessage().c_str());
		return;
	}

	while(query->hasNextRow())
	{
		row = query->getRow();
		ipAddressList.push_back(row.getString("IpAddress"));
	}

	sqlBuffer.str("");

	sqlBuffer	<< " SELECT"
				<< "   users.username AS Username,"
				<< "   COUNT(*) AS Logins,"
				<< "   users.prf AS PreferenceFlags"
				<< " FROM users, userLogin"
				<< " WHERE users.user_id = userLogin.user_id"
				<< " AND userLogin.host IN " << SQLUtil::buildListSQL(ipAddressList.begin(), ipAddressList.end(), true, true)
				<< " AND users.level <= " << ((int)GET_LEVEL(ch))
				<< " GROUP BY users.user_id"
				<< " ORDER BY COUNT(*) DESC";

	try {
		query = gameDatabase->sendQuery(sqlBuffer.str());
	}
	catch(sql::QueryException e)
	{
		MudLog(BRF, MAX(GET_LEVEL(ch), LVL_APPR), TRUE, "IPFind: %s", e.getErrorMessage().c_str());
		return;
	}

	while(query->hasNextRow())
	{
		row = query->getRow();

		std::string username = row.getString("Username");
		int logins = row.getInt("Logins");
		std::string bitvectorString = row.getString("PreferenceFlags");

		long bits[PM_ARRAY_MAX];
		ConvertBitvectorFromString(bitvectorString.c_str(), bits, PM_ARRAY_MAX);

		if(IS_SET_AR(bits, PLR_NO_TRACE) && GET_LEVEL(ch) < LVL_IMPL)
			continue;

		ch->send("%5d: %s\r\n", logins, username.c_str());
	}
}

extern int top_shop;
extern class Shop *shop_index;

void rebootCountdown();

void SendInvisCharacter( void* data )
{
	Character *ch = (Character*)data;
	ch->send("\0");
}
void autoSave( void );




flusspferd::object JS_parseJson(std::string jsonText);
std::string JS_stringifyJson(flusspferd::object obj);
void DeleteOldTracks();

/*** Extra immortal command to perform various, usually one-time test/maintanence routines ***/
ACMD(do_extra)
{
	std::vector< std::string > vArgs;
	if( !ch->in_room )
		return;
	bool bCorrectArgument=true;
	vArgs = StringUtil::SplitToVector<std::string>( argument, ' ' );

	try {
		if(false);
		else if( !str_cmp(vArgs.at(0), "trackgen") )
		{
			Character *target = get_char_vis(ch, vArgs.at(2).c_str());
			int nrOfTracks = 0;
			int tracksPerRoom = 12;
			for(int i = 0;i < World.size();++i)
			{
				for(int trackNumber = 0;trackNumber < tracksPerRoom;++trackNumber)
				{
					target->LayTrack(World[ i ], 0);
					TrackList.back()->laytime = time(0) - (60 * 100);
					++nrOfTracks;
				}

				if(nrOfTracks >= 100000)
					break;
			}
		}
		else if(!str_cmp(vArgs.at(0), "trackdel"))
		{
			Clock MyClock;

			MyClock.reset(true);
			DeleteOldTracks();
			MyClock.turnOff();

			ch->send("Total Clocks: %d, Seconds: %f\r\n", MyClock.getClocks(), MyClock.getSeconds());
		}
		else if( !str_cmp(vArgs.at(0), "eval") )
		{
			std::vector<std::string> vArgs = StringUtil::SplitToVector<std::string>(argument, ' ');
			vArgs.erase(vArgs.begin(),vArgs.begin()+1);
			std::string expression = StringUtil::implode(vArgs, " ");
			flusspferd::value val = JSManager::get()->executeExpression(expression);
		}
		else if( !str_cmp(vArgs.at(0), "jsonparse") )
		{
			std::string jsonText = "{\"g\":173}";
			flusspferd::object jsonObject = JS_parseJson(jsonText);

			flusspferd::value gVal = jsonObject.get_property("g");
			std::cout << "Value of 'g': " << gVal.to_number() << std::endl;
		}
		else if( !str_cmp(vArgs.at(0), "jsonstringify") )
		{
			flusspferd::object obj = flusspferd::object(global());
			obj.set_property("g", 173);

			std::string jsonStr = JS_stringifyJson(obj);

			std::cout << "JSON String: " << jsonStr << std::endl;
		}
		else if( !str_cmp(vArgs.at(0), "invischar"))
		{
			add_event( 26, SendInvisCharacter, (void*)ch );
			ch->send("\0");
		}
		else if( !str_cmp(vArgs.at(0), "email"))
		{
			MailUtil::sendEmail("admin@kinslayermud.org", "KinslayerMUD Staff", "mikemason930@gmail.com", "Test Email!", "This is a test. Hope you got it!");
		}
		else if( !str_cmp(vArgs.at(0), "forums") )
		{
			ForumUtil::archiveAndRemoveDeletedForumUsers(gameDatabase);
			ForumUtil::addUsersToForum(gameDatabase);
		}
		else if( !str_cmp(vArgs.at(0), "autosave") )
		{
			autoSave();
		}
		else if( !str_cmp(vArgs.at(0), "zcmd") )
		{
			Zone *zone = ZoneManager::GetManager().GetZoneByRoomVnum(ch->in_room->getVnum());

			for(ResetCommand *resetCommand : zone->cmd)
			{

				if(resetCommand->command == 'M' && resetCommand->arg3 == real_room(ch->in_room->getVnum()))
				{
					ch->send("ID: %d, Mob Vnum: %d, Ptr: %p, Ptr Vnum: %s\r\n", resetCommand->dbID,
						MobManager::GetManager().GetPrototype(resetCommand->arg1)->getVnum(), resetCommand->mob,
						(resetCommand->mob ? MiscUtil::toString(resetCommand->mob->getVnum()).c_str() : "<NONE>"));
				}
			}
		}
		else if( !str_cmp(vArgs.at(0), "corpses") )
		{
			int numberOfCorpses = MIN(500, atoi(vArgs.at(1).c_str()));
			while(numberOfCorpses-- > 0)
			{
				ch->InterpCommand("at guard k guard");
				Log("Number of corpses: %d", numberOfCorpses);
			}
			ch->send(OK);
			return;
		}
		else if( !str_cmp(vArgs.at(0), "switch") )
		{
			if( !str_cmp(vArgs.at(1), "add") )
			{
				std::string name = vArgs.at(2);
				std::string host = vArgs.at(3);
				SwitchManager::GetManager().AddSwitch(host, name, time(0), -1);
				ch->send("Switch added.");
			}
		}
		else if( !str_cmp(vArgs.at(0), "tic") ) {
			//Subtract the progress to the next tic.
			pulse -= pulse % (SECS_PER_MUD_HOUR * PASSES_PER_SEC);
			//Move to the next tic.
			pulse += (SECS_PER_MUD_HOUR * PASSES_PER_SEC);
			//Move back 1 pulse.
			pulse -= 1;
			Seconds = (pulse / PASSES_PER_SEC);
			//Now we are tic - 1 pulse.
		}
		else if(!str_cmp(vArgs.at(0), "js"))
		{
			std::string methodName = "script9";

			flusspferd::object globalObject = flusspferd::global();

			ch->send("Has Property: %s", StringUtil::yesNo(globalObject.has_property(methodName)).c_str());
			ch->send("Has Own Property: %s", StringUtil::yesNo(globalObject.has_property(methodName)).c_str());

				/**
			for(flusspferd::value propertyValue : globalObject)
			{
				ch->send("Property `%s`", propertyValue.)
			}
			**/
		}
		else if(!str_cmp(vArgs.at(0), "sys"))
		{
			char arg1[MAX_INPUT_LENGTH];
			HalfChop(argument, arg1, argument);
			skip_spaces(&argument);

			Log("Argument: %s", argument);

			std::string commandResult = SystemUtil::processCommand(argument);

			ch->send("`%s`", commandResult.c_str());
		}
		else if(!str_cmp(vArgs.at(0), "svn"))
		{
			std::map<std::string, std::string> svnMap = SystemUtil::getSubversionInfoMap("../");
		}
		else bCorrectArgument=false;
	} catch( std::out_of_range ) {
		bCorrectArgument=false;
	}
	if( !bCorrectArgument )
	{
		ch->send("Invalid argument.\r\n");
		return;
	}
	return;


#ifndef WIN32
	ch->send("Memory remaining: %lld\r\n", AvailableSystemMemory());
#endif

}


ACMD(do_lag)
{

	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	Character *victim;
	int number = 0;

	TwoArguments(argument, arg1, arg2);

	if(!argument || !*arg1 || !*arg2 || atoi(arg2) <= 0)
	{
		ch->send("Hmm...\r\n");
		return;
	}

	number = atoi(arg2);

	if(!(victim = get_char_vis(ch, arg1)))
	{
		ch->send(NOPERSON);
		return;
	}

	ch->send("%s lagged for %d seconds.\r\n", GET_NAME(victim), number);

	WAIT_STATE(victim, number * PASSES_PER_SEC);
}

ACMD(do_disable)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH], arg4[MAX_INPUT_LENGTH];
	std::map<int, std::pair<std::string,int> >::iterator Dis;
	Character *target;
	bool fileLoad = false;

	*arg1 = *arg2 = *arg3 = *arg4 = '\0';
	skip_spaces(&argument);
	TwoArguments(TwoArguments(argument, arg1, arg2), arg3, arg4);

	try {

		if(!str_cmp(arg1, "command"))
		{
			if(!str_cmp(arg2, "global"))
			{
				if(!*arg3)
				{
					int count=0;
					ch->send(	" Num    Command             Disabled By\r\n"
								"---------------------------------------\r\n\n");

					for(count = 0, Dis = DisabledCommands.begin();Dis != DisabledCommands.end();++Dis)
					{
						ch->send("%s%3d%s)    %s%-15s%s     %s%s%s%s\r\n",
						COLOR_YELLOW(ch, CL_COMPLETE), ++count, COLOR_NORMAL(ch, CL_COMPLETE),
						COLOR_CYAN(ch, CL_COMPLETE), complete_cmd_info[(*Dis).first].command.c_str(), COLOR_NORMAL(ch, CL_COMPLETE),
						COLOR_GREEN(ch, CL_COMPLETE), COLOR_BOLD(ch, CL_COMPLETE),
						((*Dis).second.first).c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
					}
				}
				else
				{
					std::string commandText = std::string(arg3);
					int commandIndex = getCommandIndex(commandText);
				
					if(commandIndex == -1)
					{
						ch->send("That command does not exist.\r\n");
						return;
					}

					if( (Dis = DisabledCommands.find(commandIndex)) == DisabledCommands.end() )
					{
						ch->send("Command %s is now disabled.\r\n", complete_cmd_info[commandIndex].command.c_str());
						DisabledCommands[commandIndex] = std::pair<std::string,int>(ch->player.name,static_cast<int>(ch->player.level));
					}
					else
					{
						ch->send("That command is already disabled.\r\n");
					}
				}
			}
			else if(!str_cmp(arg2, "user"))
			{

				if(!*arg3 || !*arg4)
				{
					ch->send("Usage: disable command user <username> <commandText>\r\n");
					return;
				}

				std::string username = StringUtil::cap(StringUtil::allLower(std::string(arg3)));
				std::string commandText = std::string(arg4);
				std::list<UserDisabledCommand*> userDisabledCommands;
				std::list<UserDisabledCommand*> *userDisabledCommandsPtr = NULL;
				PlayerIndex *playerIndex;
				std::list<UserDisabledCommand*>::iterator iter;
				UserDisabledCommand *userDisabledCommand;
				int userId;

				if(!(target = get_char_vis(ch, username.c_str()))) {

					playerIndex = CharacterUtil::getPlayerIndexByUserName(username);

					if(playerIndex == NULL)
					{
						ch->send(NOPERSON);
						return;
					}

					userId = playerIndex->id;
					userDisabledCommands = Character::loadUserDisabledCommands(userId);
					userDisabledCommandsPtr = &userDisabledCommands;
				}
				else
				{
					userDisabledCommandsPtr = &target->PlayerData->userDisabledCommands;
					userId = target->player.idnum;
				}

				for(iter = userDisabledCommandsPtr->begin();iter != userDisabledCommandsPtr->end();++iter)
				{
					userDisabledCommand = (*iter);

					if(!str_cmp(userDisabledCommand->getCommand(), commandText))
					{
						ch->send("This command has already been disabled for %s\r\n", username.c_str());
						return;
					}
				}

				userDisabledCommand = new UserDisabledCommand();
				userDisabledCommand->setUserId(userId);
				userDisabledCommand->setCommand(commandText);
				userDisabledCommand->setCreatedDatetime(DateTime());
				userDisabledCommand->setDisabledByUserId(ch->player.idnum);
			
				userDisabledCommandsPtr->push_back(userDisabledCommand);

				Character::deleteUserDisabledCommands(userId);
				Character::saveUserDisabledCommands(userId, *userDisabledCommandsPtr);

				ch->send("The command '%s' has been disabled for %s.\r\n", commandText.c_str(), username.c_str());
			}
			else
			{
				ch->send("For commands, you can disable: global OR user\r\n");
				return;
			}
		}
		else
		{
			ch->send("You can disable the following: command\r\n");
		}
	}
	catch(sql::QueryException e) {
		MudLog(BRF, LVL_GOD, TRUE, "Failed while processing command `disable` : %s", e.getMessage().c_str());
	}
}

ACMD(do_enable)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH], arg4[MAX_INPUT_LENGTH];
	std::map<int, std::pair<std::string,int> >::iterator Dis;
	Character *target;
	bool fileLoad = false;

	*arg1 = *arg2 = *arg3 = *arg4 = '\0';
	skip_spaces(&argument);
	TwoArguments(TwoArguments(argument, arg1, arg2), arg3, arg4);

	if(!str_cmp(arg1, "command"))
	{
		if(!str_cmp(arg2, "global"))
		{
			if(!*arg3)
			{
				ch->send("Usage: enable command global <command text>\r\n");
				return;
			}
			else
			{
				std::string commandText = std::string(arg3);
				int commandIndex = getCommandIndex(commandText);
				
				if(commandIndex == -1)
				{
					ch->send("That command does not exist.\r\n");
					return;
				}

				if( (Dis = DisabledCommands.find(commandIndex)) == DisabledCommands.end() )
				{
					ch->send("That command is not disabled.\r\n");
				}
				else
				{
					ch->send("Command %s is now enabled.\r\n", complete_cmd_info[commandIndex].command.c_str());
					DisabledCommands.erase(Dis);
				}
			}
		}
		else if(!str_cmp(arg2, "user"))
		{

			if(!*arg3 || !*arg4)
			{
				ch->send("Usage: enable command user <username> <commandText>\r\n");
				return;
			}

			std::string username = StringUtil::cap(StringUtil::allLower(std::string(arg3)));
			std::string commandText = std::string(arg4);
			std::list<UserDisabledCommand*> userDisabledCommands;
			std::list<UserDisabledCommand*> *userDisabledCommandsPtr = NULL;
			PlayerIndex *playerIndex;
			std::list<UserDisabledCommand*>::iterator iter;
			UserDisabledCommand *userDisabledCommand;
			int userId;

			if(!(target = get_char_vis(ch, username.c_str()))) {

				playerIndex = CharacterUtil::getPlayerIndexByUserName(username);

				if(playerIndex == NULL)
				{
					ch->send(NOPERSON);
					return;
				}

				userId = playerIndex->id;
				userDisabledCommands = Character::loadUserDisabledCommands(userId);
				userDisabledCommandsPtr = &userDisabledCommands;
			}
			else
			{
				userDisabledCommandsPtr = &target->PlayerData->userDisabledCommands;
				userId = target->player.idnum;
			}

			for(iter = userDisabledCommandsPtr->begin();iter != userDisabledCommandsPtr->end();++iter)
			{
				userDisabledCommand = (*iter);

				if(!str_cmp(userDisabledCommand->getCommand(), commandText))
				{
					ch->send("The command '%s' is now enabled for %s.\r\n", commandText.c_str(), username.c_str());

					userDisabledCommandsPtr->erase(iter);
					Character::deleteUserDisabledCommands(userId);
					Character::saveUserDisabledCommands(userId, *userDisabledCommandsPtr);
					return;
				}
			}

			ch->send("This command is not disabled for %s.\r\n", username.c_str());
		}
		else
		{
			ch->send("For commands, you can enable: global OR user\r\n");
			return;
		}
	}
	else
	{
		ch->send("You can enable the following: command\r\n");
	}

/***
	char com[MAX_INPUT_LENGTH];
	std::map<int, std::pair<std::string,int> >::iterator Dis;

	skip_spaces(&argument);
	strcpy(com,argument);

	if( !(*com) )
	{
		ch->send("Syntax: Disable <Command>\r\n");
		return;
	}

	for(int number = 0;complete_cmd_info[number].command[0] != '\n';++number)
	{
		if(!str_cmp(complete_cmd_info[number].command, com))
		{
			if((Dis = DisabledCommands.find(number)) != DisabledCommands.end())
			{
				if( (*Dis).second.second > GET_LEVEL(ch) )
				{
					ch->send("That command was disabled by someone higher level than you.\r\n");
					return;
				}
				else
				{
					ch->send("Command %s is now enabled.\r\n", complete_cmd_info[number].command.c_str());
					DisabledCommands.erase(Dis);
				}
			}
			else
			{
				ch->send("That command is already avaliable.\r\n");
			}
			return;
		}
	}
	ch->send("%s is not a valid command!\r\n", com);
***/
}

ACMD(do_pardon)
{
	char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	Character *victim;
	Clan *clan;
	UserClan *chUserClan, *victimUserClan;
	Warrant *warrant;
	bool load = false;

	TwoArguments(OneArgument(argument, arg), arg2, arg3);
	if(!argument || !*arg || !*arg2)
	{
		ch->send("Syntax: Pardon <Player Name> <Clan Name Or Number>\r\n");
		return;
	}

	if(!(victim = get_char_vis(ch, arg)))
	{
		if( (victim = CharacterUtil::loadCharacter(arg)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
			load = true;
	}

	std::unique_ptr<Character> victimUniquePointer;

	if(load)
		victimUniquePointer = std::unique_ptr<Character>(victim);

	int clanId = GetClanByString(arg2);

	if(!clanId)
	{
		ch->send("Invalid clan. Enter a valid clan name or clan number. You can see all clans by using the VIEW CLANS command.\r\n");
		return;
	}

	clan = ClanUtil::getClan(clanId);
	chUserClan = ch->getUserClan(clanId);
	
	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan)
	{
		ch->send("You are not a member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan->getIsCouncil())
	{
		ch->send("You are not a member of the %s council.\r\n", clan->Name.c_str());
		return;
	}

	warrant = clan->GetWarrant();

	if(!warrant)
	{
		ch->send("Oddly, the %s clan cannot issue any pardons!\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_RACE(ch) != GET_RACE(victim))
	{
		ch->send("%s is not of the same race as you!\r\n", GET_NAME(victim));
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_LEVEL(victim) >= LVL_IMMORT)
	{
		ch->send("Only the gods themselves are mighty enough to pardon %s of %s crimes!\r\n", GET_NAME(victim), HSHR(victim));
		return;
	}

	if(!IS_SET_AR(GET_WARRANTS(victim), warrant->vnum))
	{
		ch->send("%s is not wanted by the %s clan.\r\n", GET_NAME(victim), clan->Name.c_str());
		return;
	}
	
	victim->RemoveWarrant(ch, clanId);

	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE,
		"%s pardoned %s for clan %s(%s).",
		GET_NAME(ch), GET_NAME(victim), clan->Name.c_str(), clan->GetWarrant()->Name.c_str());

	if(load)
	{
		victim->save();
	}
	else
	{//We want all MOBs in the clan to forget the player if they previously were set to aggro them.
		for(Character *mob = character_list;mob;mob = mob->next)
		{
			if(IS_NPC(mob) && mob->isInClan(clan->vnum))
			{
				mob->Forget(victim);
			}
		}
	}
}

ACMD(do_warrant)
{
	char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
	Character *victim;
	Clan *clan;
	UserClan *chUserClan, *victimUserClan;
	Warrant *warrant;
	bool load = false;

	TwoArguments(OneArgument(argument, arg), arg2, arg3);
	if(!argument || !*arg || !*arg2)
	{
		ch->send("Syntax: Warrant <Player Name> <Clan Name Or Number>\r\n");
		return;
	}

	if(!(victim = get_char_vis(ch, arg)))
	{
		if( (victim = CharacterUtil::loadCharacter(arg)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
			load = true;
	}

	std::unique_ptr<Character> victimUniquePointer;

	if(load)
		victimUniquePointer = std::unique_ptr<Character>(victim);

	int clanId = GetClanByString(arg2);

	if(!clanId)
	{
		ch->send("Invalid clan. Enter a valid clan name or clan number. You can see all clans by using the VIEW CLANS command.\r\n");
		return;
	}

	clan = ClanUtil::getClan(clanId);
	chUserClan = ch->getUserClan(clanId);
	
	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan)
	{
		ch->send("You are not a member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan->getIsCouncil())
	{
		ch->send("You are not a council member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	warrant = clan->GetWarrant();

	if(!warrant)
	{
		ch->send("Oddly, the %s clan cannot issue any warrants!\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_RACE(ch) != GET_RACE(victim))
	{
		ch->send("%s is not of the same race as you!\r\n", GET_NAME(victim));
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_LEVEL(victim) >= LVL_IMMORT)
	{
		ch->send("You think you're godly enough to warrant %s? Hah!\r\n", GET_NAME(victim));
		return;
	}

	if(IS_SET_AR(GET_WARRANTS(victim), warrant->vnum))
	{
		ch->send("%s is already wanted by the %s clan.\r\n", GET_NAME(victim), clan->Name.c_str());
		return;
	}
	
	victim->SetWarrant(ch, clanId, false);

	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE,
		"%s warranted %s for clan %s(%s).",
		GET_NAME(ch), GET_NAME(victim), clan->Name.c_str(), clan->GetWarrant()->Name.c_str());

	if(load)
	{
		victim->save();
	}
}

ACMD(do_zap)
{
	Character *victim;
	int amount = 0, i;
	bool load = false;
	char arg[MAX_INPUT_LENGTH], level[MAX_INPUT_LENGTH];

	if(!*argument)
	{
		ch->send("Usage: Delevel player level\r\n");
		return;
	}

	TwoArguments(argument, arg, level);

	if(!*level)
	{
		ch->send("You must input a level\r\n");
		return;
	}

	if(!(victim = get_char_vis(ch, arg)))
	{
		if( (victim = CharacterUtil::loadCharacter(arg)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
		{
			victim->loadHitRolls();
			victim->loadManaRolls();
			load = true;
		}
	}

	amount = atoi(level);

	if(amount >= GET_LEVEL(victim))
	{
		Act("Your argument MUST be lower than $S level.", FALSE, ch, NULL, victim, TO_CHAR);
		CLEANUP(victim, load);
		return;
	}

	if(amount <= 0)
	{
		ch->send("Level drop cannot be below 1.\r\n");
		CLEANUP(victim, load);
		return;
	}

	if(GET_LEVEL(victim) > GET_LEVEL(ch))
	{
		ch->send("You'd be better off not doing that...\r\n");
		CLEANUP(victim, load);
		return;
	}

	if(IS_NPC(victim))
	{
		ch->send("Players only!!!\r\n");
		CLEANUP(victim, load);
		return;
	}

	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s lowered %s's level from %d to %d.%s",
	       GET_NAME(ch), GET_NAME(victim), GET_LEVEL(victim), amount, load ? " (Offline)" : "");

	for(i = GET_LEVEL(victim);i > amount;--i)
		victim->DropLevel();

	GET_EXP(victim) = level_exp(GET_LEVEL(victim));


	ch->send("%s is now level %d.%s\r\n", GET_NAME(victim), GET_LEVEL(victim), load ? " (Offline)" : "");

	victim->send("ZAAAAP! You have been lowered to level %d!\r\n", GET_LEVEL(victim));
	victim->save();

	CLEANUP(victim, load);
}

ACMD(do_warn)
{
	Character *victim;
	char arg[MAX_INPUT_LENGTH];

	OneArgument(argument, arg);

	if(!*arg)
	{
		ch->send("Who are you trying to warn");
		return;
	}

	if(!(victim = get_char_vis(ch, arg)))
	{
		ch->send("Player '%s' was not found'\r\n", arg);
		return;
	}

	if(GET_LEVEL(victim) >= GET_LEVEL(ch))
	{
		ch->send("You can't do that to %s! Might be risky!\r\n", GET_NAME(victim));
		return;
	}

	++GET_WARNINGS(victim);
	ch->send("%s now has %d warnings.\r\n", GET_NAME(victim), GET_WARNINGS(victim));
	victim->send("You have been warned! You now have %d warning%s!\r\n",
	             GET_WARNINGS(victim), GET_WARNINGS(victim) == 1 ? "" : "s");

	MudLog(NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE, "%s has warned %s --Warning Number %d--",
	       GET_NAME(ch), GET_NAME(victim), GET_WARNINGS(victim));
}

ACMD(do_find)
{
	std::stringstream QueryBuffer, OutputBuffer;
	sql::Query MyQuery;
	unsigned int i = 0;
	int nr_arg;
	char name[MAX_INPUT_LENGTH];

	*buf = '\0';
	OneArgument(argument, name);

	if( !*name )
	{
		ch->send("Find what?\r\n");
		return;
	}
	Clock TheClock, TheClock2;
	nr_arg = atoi(name);

	QueryBuffer << "SELECT o.*,r.retool_name,r.retool_sdesc,r.retool_ldesc,r.retool_exdesc "
		<< " FROM objects o "
		<< " LEFT JOIN object_retools r ON r.id=o.id"
		<< " WHERE vnum IN (";
	if( MiscUtil::isNumber(name) )//Searching for single vnum(provided by user)
		QueryBuffer << nr_arg;
	else
	{//Searching for item by name(possible multiple different vnums)
		QueryBuffer << "-1";
		for(i = 0;i < top_of_objt;++i)
		{
			if( isname(name, obj_proto[i]->name) )
				QueryBuffer << "," << obj_index[obj_proto[i]->item_number].vnum;
		}
	}
	QueryBuffer << ");";

	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, TRUE, MAX(LVL_APPR, GET_INVIS_LEV(ch)), "do_find : Failed to send object query: %s",
			e.getMessage().c_str());
		return;
	}

	get_char_cols(ch);
	for(unsigned int i = 0;MyQuery->hasNextRow();)
	{
		sql::Row MyRow = MyQuery->getRow();
		Object *obj = Object::bootLiveObject( MyRow );

		if( obj == NULL )
			continue;
		if( MyRow["special_type"] != "0" && !isname(name,obj->getName()) )
			continue;
		if( i++ == 0 )
		{//This is the first item. Output the header.
			OutputBuffer << "  Nr    Vnum               UUID                                 Name                                   Held By" << std::endl;
			OutputBuffer << "-----------------------------------------------------------------------------------------------------------------" << std::endl;
//<<                       "[   1] [  121] [993a3700-0000-0000-0000-000000000000] a set of earthen sleeves                       Player Mordain
		}
		OutputBuffer << "[" << cyn << std::setw(4) << std::right << i << nrm << "] "
			<< "[" << cyn << std::setw(5) << std::right << MyRow["vnum"] << nrm << "] "
			<< "[" << cyn << std::setw(8) << std::right << MyRow["id"]   << nrm << "] "
			<< grn << bld << std::setw(47)<< std::left << obj->GetSDesc() << nrm;

		std::stack<sql::Query> MyQueryStack;
		while( MyRow["holder_type"] == "O" )
		{//Recurse upward until ultimate holder is found.
			try {
				QueryBuffer.str("");
				QueryBuffer << "SELECT * FROM objects WHERE id='" << MyRow["holder_id"] << "';";
				MyQueryStack.push(gameDatabase->sendQuery(QueryBuffer.str()));
			} catch( sql::QueryException e ) {
				MudLog(BRF, TRUE, MAX(LVL_APPR, GET_INVIS_LEV(ch)), "do_find : Failed to send object query: %s",
					e.getMessage().c_str());
				return;
			}
			if( !MyQueryStack.top()->numRows() )
				break;
			MyRow = MyQueryStack.top()->getRow();
		}

		if( MyRow["holder_type"] == "C" )
			OutputBuffer << "Chest " << cyn << MyRow["holder_id"] << nrm;
		else if( MyRow["holder_type"] == "P" )
			OutputBuffer << "Player " << cyn << getNameById(MiscUtil::Convert<int>(MyRow["holder_id"])) << nrm;
		else if( MyRow["holder_type"] == "R" )
			OutputBuffer << "Room " << cyn << MyRow["holder_id"] << nrm;
		OutputBuffer << std::endl;

		obj->Extract();
		/* This must come last */
		while( !MyQueryStack.empty() )
		{
			MyQueryStack.pop();
		}
	}

	if( QueryBuffer.str().size() > 0 )
	{
		/* Create dynamic C buffer for output */
		char *cBuffer;
		cBuffer = new char[OutputBuffer.str().size()+1];
		strcpy(cBuffer,OutputBuffer.str().c_str());
		page_string(ch->desc, cBuffer, TRUE);
		delete[](cBuffer);
	}
	else
		ch->send("No items found.\r\n");
}

ACMD(do_countdown)
{
	int count;
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

	if( GET_LEVEL(ch) < LVL_GRGOD )
	{
		ch->send("Access denied!\r\n");
		return;
	}

	TwoArguments(argument, arg1, arg2);

	if(!str_cmp(arg1, "shutdown"))
	{
		ch->send("The Gateway is now set to shut down when the MUD next boots.\r\n");
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "Reboot mode to `shutdown` by %s.", GET_NAME(ch));

		gatewayConnection->send("ShutdownOnReboot\n");
		
	}
	else if(!str_cmp(arg1, "restart"))
	{
		ch->send("The Gateway is now set to restart when the MUD next boots.\r\n");
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "Reboot mode to `restart` by %s.", GET_NAME(ch));

		gatewayConnection->send("RestartOnReboot\n");
	}
	else if(MiscUtil::isInt(arg1) && atoi(arg1) > 0)
	{
		DateTime timeNow;
		count = atoi(arg1);
		rebootTime.setTime( timeNow.getTime() + count*60 );
		int ticksBetween = (int)(rebootTime.getTime()-timeNow.getTime())/60;

		sprintf(buf, "Reboot in %d minute%s.\r\n", ticksBetween, (ticksBetween > 1) ? "s" : "");
		rebootNotified15 = rebootNotified10 = rebootNotified5 = rebootNotified1 = false;
		sendToAll(buf);

		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "Reboot set to %s by %s.", rebootTime.toString().c_str(), GET_NAME(ch));
	}
	else
	{
		ch->send("Valid arguments:\r\n"
				 "`shutdown` : Setting this option will result in the MUD shutting down and requiring a manual startup upon reboot.\r\n"
				 "`restart` : Setting this option will result in the MUD attempting to automatically restart upon reboot.\r\n"
				 "A positive integer: Supplying a positive integer will tell the MUD to reboot in the specified number of minutes.\r\n"
				);
		return;
	}
}

ACMD(do_rank)
{
	Character *victim;
	UserClan *victimUserClan, *chUserClan;
	Clan *clan;
	int i = 0;
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	bool load = false;
	std::string syntax = "Syntax: Rank <Player Name> <Clan Name or Number>";
	TwoArguments(argument, arg1, arg2);

	if(!*arg1)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}

	if(!*arg2)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}

	if(!(victim = get_char_vis(ch, arg1)))
	{
		if( (victim = CharacterUtil::loadCharacter(arg1)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
		{
			victim->loadClans();
			load = true;
		}
	}

	std::unique_ptr<Character> victimUniquePointer;

	if(load)
		victimUniquePointer = std::unique_ptr<Character>(victim);

	if(!(i = GetClanByString(arg2)) || !ch->CanViewClan(i))
	{
		ch->send("Invalid clan. Enter a valid clan name or clan number. You can see all clans by using the VIEW CLANS command.\r\n");
		return;
	}

	clan = ClanUtil::getClan(i);
	chUserClan = ch->getUserClan(i);

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan)
	{
		ch->send("You are not a member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan->getIsCouncil())
	{
		ch->send("You are not a council member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	if(!(victimUserClan = victim->getUserClan(i)))
	{
		ch->send("%s is not a member of the %s clan.\r\n", GET_NAME(victim), clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && ch->getUserId() == victim->getUserId())
	{
		ch->send("You cannot rank yourself!\r\n");
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_LEVEL(victim) >= LVL_IMMORT)
	{
		ch->send("%s might appreciate what you're doing, but you'd better let %s take care of this %sself.\r\n", StringUtil::cap(std::string(HSSH(victim))).c_str(), HMHR(victim), HMHR(victim));
		return;
	}

	if(victimUserClan->getQuestPoints() < rank_req[victimUserClan->getRank()])
	{
		ch->send("%s does not have enough quest points to rank.\r\n", GET_NAME(victim));
		return;
	}

	victimUserClan->setRank(victimUserClan->getRank() + 1);
	MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, "%s raised %s's rank to %d in the %s clan.%s", GET_NAME(ch), GET_NAME(victim), (int)victimUserClan->getRank(), clan->Name.c_str(), load ? " (Offline)" : "");

	ch->send("You raise %s's rank to %d in the %s clan.\r\n", GET_NAME(victim), (int)victimUserClan->getRank(), clan->Name.c_str());
	ClanUtil::putUserClan(gameDatabase, victimUserClan);

	std::string rankName = clan->GetRankName(victimUserClan->getRank());

	if(!rankName.empty() && !clan->secret)
	{
		game->sendToAll([&](Character *target) {

			std::stringstream messageStringStream;

			messageStringStream
				<< COLOR_BOLD(target, CL_NORMAL) << COLOR_GREEN(target, CL_NORMAL)
				<< "**" << GET_NAME(victim) << " has been promoted to " << rankName << "!**" << COLOR_NORMAL(victim, CL_NORMAL) << "\r\n";

			return messageStringStream.str();
		});
	}

	if(victimUserClan->getRank() == 6)
	{
		victim->send("You are now able to select a special skill at your clan leader.\r\n"
					 "Type %s%sSHOW SKILLS%s at your clan leader to view the skills available.\r\n",
					 COLOR_BOLD(victim, CL_NORMAL), COLOR_CYAN(victim, CL_NORMAL), COLOR_NORMAL(victim, CL_NORMAL));
	}
}

ACMD(do_demote)
// demote function, imped by Stark, rewritten by Galnor: 11-25-2004, rewritten again July 3rd 2014.
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	int i = 0;
	Character *victim;
	bool load = false;
	std::string syntax = "Syntax: Demote <Player Name> <Clan Name or Number>";

	TwoArguments(argument, arg1, arg2);

	if(!*arg1)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}

	if(!*arg2)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}

	if(!(victim = get_char_vis(ch, arg1)))
	{
		if( (victim = CharacterUtil::loadCharacter(arg1)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
		{
			victim->loadClans();
			load = true;
		}
	}

	std::unique_ptr<Character> victimUniquePointer;

	if(load)
		victimUniquePointer = std::unique_ptr<Character>(victim);

	if(!(i = GetClanByString(arg2)) || !ch->CanViewClan(i))
	{
		ch->send("There is no such clan.\r\n");
		return;
	}

	Clan *clan = ClanUtil::getClan(i);
	UserClan *chUserClan = ch->getUserClan(i);

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan)
	{
		ch->send("You are not a member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan->getIsCouncil())
	{
		ch->send("You are not a member of the %s council.\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && ch->getUserId() == victim->getUserId())
	{
		ch->send("You cannot demote yourself!\r\n");
		return;
	}

	UserClan *victimUserClan = victim->getUserClan(i);

	if(!victimUserClan)
	{
		ch->send("%s is not a member of the %s clan.\r\n", GET_NAME(victim), clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_LEVEL(victim) >= LVL_IMMORT)
	{
		ch->send("%s would probably smite you into a thousand bits if %s caught you doing that to %s!\r\n", StringUtil::cap(std::string(HSSH(victim))).c_str(), HSSH(victim), HMHR(victim));
		return;
	}

	if(victimUserClan->getRank() <= 0)
	{
		ch->send("%s is already at rank 0. You cannot lower %s rank any further.\r\n", GET_NAME(victim), HSHR(victim));
		return;
	}

	victimUserClan->setRank(victimUserClan->getRank() - 1);
	ClanUtil::putUserClan(gameDatabase, victimUserClan);

	MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE,
	       "%s lowered %s's rank to %d in the %s clan.%s",
	       GET_NAME(ch), GET_NAME(victim), (int)victimUserClan->getRank(), clan->Name.c_str(), load ? " (Offline)" : "");
	ch->send("You lowered %s's rank to %d in the %s clan.\r\n", GET_NAME(victim), (int)victimUserClan->getRank(), clan->Name.c_str());
}

ACMD(do_council)
{
	Character *victim;
	UserClan *userClan;
	Clan *clan;
	char player_name[MAX_INPUT_LENGTH], clan_name[MAX_INPUT_LENGTH];
	int clan_num;
	bool load = false;

	TwoArguments(argument, player_name, clan_name);

	if(!*player_name || !*clan_name)
	{
		ch->send("Syntax: Council <Player Name> <Clan Name or Number>\r\n");
		return;
	}

	if(!(victim = get_char_vis(ch, player_name)))
	{
		if(!playerExists(player_name) || (victim = CharacterUtil::loadCharacter(player_name)) == NULL)
		{
			ch->send(NOPERSON);
			return;
		}
		else
		{
			load = true;
		}
	}

	std::unique_ptr<Character> victimUniquePointer;

	if(load)
		victimUniquePointer = std::unique_ptr<Character>();

	//Get the clan
	if(!(clan_num = GetClanByString(clan_name)))
	{
		ch->send("Invalid clan name or number.\r\n");
		return;
	}

	if(!(userClan = victim->getUserClan(clan_num)))
	{
		ch->send("%s is not a member of that clan.\r\n", GET_NAME(victim));
		return;
	}

	clan = ClanUtil::getClan(clan_num);
	userClan->setIsCouncil(!userClan->getIsCouncil());

	ch->send("%s has been %s from the Council.\r\n", GET_NAME(victim), userClan->getIsCouncil() ? "added" : "removed");
	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE,
		"%s %s %s's council flag for clan %s.%s",
		GET_NAME(ch), (userClan->getIsCouncil() ? "added" : "removed"),
		GET_NAME(victim), ClanUtil::getClan(clan_num)->Name.c_str(), load ? " (Offline)" : "");

	try
	{
		flusspferd::object victimJavaScriptObject;
		if(load)
		{
			Room *loadRoom = FindRoomByVnum( victim->PlayerData->load_room );
			if(!loadRoom)
				loadRoom = victim->StartRoom();

			victim->MoveToRoom( loadRoom );

			victimJavaScriptObject = flusspferd::create_native_object<JSCharacter>(flusspferd::object(), victim).get_object();
		}
		else
			victimJavaScriptObject = lookupValue(victim).to_object();

		
		flusspferd::object clanUtil = flusspferd::global().get_property("ClanUtil").get_object();

		if(userClan->getIsCouncil())
			flusspferd::value result = clanUtil.call("onUserAddedToCouncil", victimJavaScriptObject, clan->vnum);
		else
			flusspferd::value result = clanUtil.call("onUserRemovedFromCouncil", victimJavaScriptObject, clan->vnum);

		if(load)
		{
			flusspferd::get_native<JSCharacter>(victimJavaScriptObject).setReal(NULL);
			victim->RemoveFromRoom();
		}
	}
	catch(flusspferd::exception e)
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "Error while changing %s's council flag by %s in clan %s: %s", GET_NAME(victim), GET_NAME(ch), clan->Name.c_str(), e.what());
	}

	if(load)
		victim->saveClans();
}

ACMD(do_clan)
{
	char playername[MAX_INPUT_LENGTH], clanstr[MAX_INPUT_LENGTH];
	Character *victim = NULL;
	UserClan *userClan;
	bool loaded = false;
	std::string syntax = "Clan <Player Name> <Clan Name or Number>";

	TwoArguments(argument, playername, clanstr);

	if(!*playername)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}
	if(!*clanstr)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}

	int clanId = 0;
	Clan *clan = NULL;
	if(!(clanId = GetClanByString(clanstr)) || !(clan = ClanUtil::getClan(clanId)))
	{
		ch->send("Invalid clan. Enter a valid clan name or clan number. You can see all clans by using the VIEW CLANS command.\r\n");
		return;
	}

	userClan = ch->getUserClan(clan->vnum);

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !userClan)
	{
		ch->send("You are not a member of the %s clan.\r\n", clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !userClan->getIsCouncil())
	{
		ch->send("You are not a council member of the %s clan\r\n", clan->Name.c_str());
		return;
	}

	StringUtil::formatPlayername(playername);
	if(!(victim = get_char_vis(ch, playername)))
	{
		if(!playerExists(playername) || !(victim = CharacterUtil::loadCharacter(playername)))
		{
			ch->send(NOPERSON);
			return;
		}
		else
			loaded = true;
	}

	std::unique_ptr<Character> victimAutoPointer;

	if(loaded)
		victimAutoPointer = std::unique_ptr<Character>(victim);

	if(victim->getUserClan(clanId))
	{
		ch->send("%s is already a member of the %s clan.\r\n", GET_NAME(victim), clan->Name.c_str());
		return;
	}

	if(GET_LEVEL(ch) <= COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_RACE(victim) != GET_RACE(ch))
	{
		ch->send("%s is not of the same race as you.\r\n", GET_NAME(victim));
		return;
	}

	userClan = UserClan::setupNewInstance(victim->player.idnum, clan->vnum);

	if(!IS_NPC(victim))
		ClanUtil::putUserClan(gameDatabase, userClan);

	ch->send("%s has been added to clan %s.\r\n", StringUtil::cap(StringUtil::allLower(playername)), clan->Name.c_str());
	MudLog(NRM, MAX(GET_INVIS_LEV(ch), LVL_GOD), TRUE, "%s clanned %s into clan %s.", ch->player.name.c_str(), StringUtil::cap(StringUtil::allLower(playername)), clan->Name.c_str());

	try
	{
		flusspferd::object victimJavaScriptObject;
		if(loaded)
		{
			Room *loadRoom = FindRoomByVnum( victim->PlayerData->load_room );
			if(!loadRoom)
				loadRoom = victim->StartRoom();

			victim->MoveToRoom( loadRoom );

			victimJavaScriptObject = flusspferd::create_native_object<JSCharacter>(flusspferd::object(), victim).get_object();
		}
		else
			victimJavaScriptObject = lookupValue(victim).to_object();

		
		flusspferd::object clanUtil = flusspferd::global().get_property("ClanUtil").get_object();

		flusspferd::value result = clanUtil.call("onUserAddedToClan", victimJavaScriptObject, clan->vnum);

		if(loaded)
		{
			flusspferd::get_native<JSCharacter>(victimJavaScriptObject).setReal(NULL);
			victim->RemoveFromRoom();
		}
	}
	catch(flusspferd::exception e)
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "Error while clanning %s by %s to clan %s: %s", GET_NAME(victim), GET_NAME(ch), clan->Name.c_str(), e.what());
	}

	victim->addToClan(userClan);
}

ACMD(do_declan)
{
	char playername[MAX_INPUT_LENGTH], clanstr[MAX_INPUT_LENGTH];
	int clanId;
	bool loaded = false;
	Character *victim = NULL;
	Clan *clan = NULL;
	std::string syntax = "Syntax: Declan <Player Name> <Clan Name or Number>";
	std::list<int> clanIdsUserWasRemovedFrom;

	TwoArguments(argument, playername, clanstr);

	if(!*playername)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}
	if(!*clanstr)
	{
		ch->send("%s\r\n", syntax.c_str());
		return;
	}

	if((!(clanId = GetClanByString(clanstr)) || !(clan = ClanUtil::getClan(clanId))) && (str_cmp(clanstr, "all") || GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL))
	{
		ch->send("Invalid clan. Enter a valid clan name or clan number. You can see all clans by using the VIEW CLANS command.\r\n");
		return;
	}

	StringUtil::formatPlayername(playername);
	if(!(victim = get_char_vis(ch, playername)))
	{
		if (playerExists(playername))
		{
			victim = CharacterUtil::loadCharacter(playername);
			loaded = true;
		}
	}

	if (!victim)
	{
		ch->send(NOPERSON);
		return;
	}

	std::unique_ptr<Character> victimUniquePointer;
	
	if(loaded)
		victimUniquePointer = std::unique_ptr<Character>(victim);

	if(clanId)
	{
		UserClan *chUserClan = ch->getUserClan(clanId);

		if(GET_LEVEL(ch) <= COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan)
		{
			ch->send("You are not a member of the %s clan.\r\n", clan->Name.c_str());
			return;
		}

		if(GET_LEVEL(ch) <= COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && !chUserClan->getIsCouncil())
		{
			ch->send("You are not a council member of the %s clan.\r\n", clan->Name.c_str());
			return;
		}

		UserClan *victimUserClan = victim->getUserClan(clanId);
		if(!victimUserClan)
		{
			ch->send("%s is not a member of the %s clan.\r\n", GET_NAME(victim), clan->Name.c_str());
			return;
		}

		if(GET_LEVEL(ch) <= COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && ch->getUserId() == victim->getUserId())
		{
			ch->send("You cannot declan yourself.\r\n");
			return;
		}
		
		if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL && GET_LEVEL(victim) >= LVL_IMMORT)
		{
			ch->send("You wouldn't dare declan %s for fear of retribution!\r\n", GET_NAME(victim));
			return;
		}

		//Only way clanId can be 0 is if GetClanByString() returned 0 and clanstr was set to "all"
		//Therefore we are removing all of the player's clans.
		
		ch->send("You have removed %s from clan %s.\r\n", playername, clan->Name.c_str());
		MudLog(NRM, MAX(GET_INVIS_LEV(ch), LVL_GOD), TRUE, "%s removed %s from clan %s.", ch->player.name.c_str(), playername, clan->Name.c_str());
		
		//Remove from the database.
		if(!victimUserClan->isNew())
		{
			std::list<int> userClanIds;
			userClanIds.push_back(victimUserClan->getId());
			ClanUtil::removeUserClansFromDatabase(gameDatabase, userClanIds);
		}

		if(victim)
		{
			victim->removeFromClan(clanId);
			clanIdsUserWasRemovedFrom.push_back(clanId);
		}
	}
	else
	{
		if(GET_LEVEL(ch) < COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL)
		{//This check should be covered above when we check for clan name, but just in case.
			ch->send("Only immortals level %d or greater can remove a player from all clans.\r\n", COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL);
			return;
		}

		ClanUtil::removeUserClansFromDatabase(gameDatabase, victim->userClans);
		ch->send("You have removed %s from all clans.\r\n", playername);
		MudLog(NRM, MAX(GET_INVIS_LEV(ch), LVL_GOD), TRUE, "%s removed %s from all clans.", ch->player.name.c_str(), playername);

		for(UserClan *userClan : victim->userClans)
		{
			clanIdsUserWasRemovedFrom.push_back(userClan->getClanId());
		}

		if(victim)
			victim->removeFromClan();
	}

	try
	{
		flusspferd::object victimJavaScriptObject;
		if(loaded)
		{
			Room *loadRoom = FindRoomByVnum( victim->PlayerData->load_room );
			if(!loadRoom)
				loadRoom = victim->StartRoom();

			victim->MoveToRoom( loadRoom );
			victimJavaScriptObject = flusspferd::create_native_object<JSCharacter>(flusspferd::object(), victim).get_object();
		}
		else
			victimJavaScriptObject = lookupValue(victim).to_object();

		flusspferd::object clanUtil = flusspferd::global().get_property("ClanUtil").get_object();

		for(int clanId : clanIdsUserWasRemovedFrom)
		{
			flusspferd::value result = clanUtil.call("onUserRemovedFromClan", victimJavaScriptObject, clanId);
		}
		
		if(loaded)
		{
			flusspferd::get_native<JSCharacter>(victimJavaScriptObject).setReal(NULL);
			victim->RemoveFromRoom();
		}
	}
	catch(flusspferd::exception e)
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "Error while clanning %s by %s to clan %s: %s", GET_NAME(victim), GET_NAME(ch), clan->Name.c_str(), e.what());
	}
}

ACMD(do_reset)
{
	Character *victim;
	bool load = false;

	skip_spaces(&argument);

	if(!*argument)
	{
		ch->send("Reset who?\r\n");
		return;
	}

	if(!(victim = get_char_vis(ch, argument)))
	{
		if( (victim = CharacterUtil::loadCharacter(argument)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
		{
			victim->loadSkills();
			load = true;
		}
	}

	if(GET_LEVEL(victim) > GET_LEVEL(ch))
	{
		ch->send("Yeah right...\r\n");

		if(load)
			delete victim;
		return;
	}

	victim->ResetAllSkills();
	victim->basicSave();
	victim->saveSkills();
	victim->send("You feel the loss of skill.\r\n");
	ch->send("Done.\r\n");

	MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
	       "%s reset %s's practices.%s", GET_NAME(ch), GET_NAME(victim), load ? " (Offline)" : "");

	if(load)
		delete victim;

}

ACMD(do_dig)
{
	char buf2[MAX_INPUT_LENGTH];
	char buf3[MAX_INPUT_LENGTH];
	int dir = 0, tvnum;
	Zone *tzone, *mzone;
	Room *room;

	TwoArguments(argument, buf2, buf3);
	/* buf2 is the direction, buf3 is the room */

	if (!*buf2)
	{
		ch->send("Format: dig <dir> <room number>\r\n");
		return;
	}

	else if (!*buf3)
	{
		ch->send("Format: dig <dir> <room number>\r\n");
		return;
	}

	//Grab a pointer to the targetted room, if it exists.
	tvnum = atoi(buf3);
	room = FindRoomByVnum(tvnum);

	Descriptor *d = ch->desc; /* will save us some typing */

	/* Main stuff */
	switch (*buf2)
	{
		case 'n':
		case 'N':
			dir = NORTH;
			break;
		case 'e':
		case 'E':
			dir = EAST;
			break;
		case 's':
		case 'S':
			dir = SOUTH;
			break;
		case 'w':
		case 'W':
			dir = WEST;
			break;
		case 'u':
		case 'U':
			dir = UP;
			break;
		case 'd':
		case 'D':
			dir = DOWN;
			break;
	}

	mzone = ch->in_room->getZone();
	tzone = ZoneManager::GetManager().GetZoneByRoomVnum(tvnum);

	if (tzone == NULL)
	{
		ch->send("You cannot link to a non-existing zone!\r\n");
		return;
	}

	if (GET_LEVEL(ch) < LVL_GOD)
	{
		if( !mzone->CanEdit(ch) )
		{
			ch->send("You do not have permission to edit room #%d.\r\n", ch->in_room->getVnum());
		}
		if( !tzone->CanEdit(ch) )
		{
			ch->send("You do not have permission to edit room #%d.\r\n", tvnum);
			return;
		}
	}

	//Targetted room does not exist. Make a new one.
	if (!room)
	{
		//Give the descriptor an olc struct.
		//This way we can let redit_save_internally handle the room adding.
		if (d->olc)
		{
			MudLog(BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE, "SYSERR: do_dig: Player already had olc structure.");
			cleanup_olc(d, CLEANUP_ALL);
			d->olc = NULL;
		}

		d->olc = new OLC();
		OLC_ZNUM(d) = tzone->GetRnum();
		OLC_NUM(d) = tvnum;
		OLC_ROOM(d) = new Room();

		OLC_ROOM(d)->setName("An unfinished room");

		/* Copy the room's description.*/
		OLC_ROOM(d)->description = str_dup("You are in an unfinished room.\r\n");
		OLC_ROOM(d)->setZoneNumber(tzone->GetRnum());
		OLC_ROOM(d)->setVnum(NOWHERE);

		/*
		 * Save the new room to memory.
		 * redit_save_internally handles adding the room in the right place, etc.
		 */
		redit_save_internally(d);
		AddOlcLog(d->character, "room", d->olc->room->getVnum());
		OLC_VAL(d) = 0;

		ch->send("New room (%d) created.\r\n", tvnum);
		cleanup_olc(d, CLEANUP_STRUCTS);

		room = FindRoomByVnum(tvnum);
	}

	if(!room)
	{
		ch->send("Sorry, there was an error when attempting to create this room. Please report this!\r\n");
		return;
	}

	room->dir_option[rev_dir[dir]] = new Exit();
	room->dir_option[rev_dir[dir]]->setToRoom(ch->in_room);

	ch->in_room->dir_option[dir] = new Exit();
	ch->in_room->dir_option[dir]->setToRoom(room);

	/* Only works if you have Oasis OLC */
	olc_add_to_save_list(tzone->getVnum(), OLC_SAVE_ROOM);
	if (room->getZone()->getVnum() != tzone->getVnum())
		olc_add_to_save_list(room->getZone()->getVnum(), OLC_SAVE_ROOM);

	ch->send("You make an exit %s to room %d.\r\n", buf2, tvnum);
}

ACMD(do_echo)
{
	Character *vict;
	skip_spaces(&argument);
	int count = 0;

	if (!*argument)
		ch->send("Yes.. but what?\r\n");
	else
	{
		for(count = 0, vict = ch->in_room->people;vict;vict = vict->next_in_room, ++count)
		{
			vict->send("%s\r\n", argument);
			if(!count)
			{
				MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
					"%s made echo %s in room %d.", StringUtil::cap(GET_NAME(ch)), argument, ch->in_room->getVnum());
			}
		}
	}
}

ACMD(do_award)
{
	PlayerIndex *targetUserPlayerIndex;
	char targetUserName[MAX_INPUT_LENGTH], clanName[MAX_INPUT_LENGTH], questPointAmountString[MAX_INPUT_LENGTH];
	short int questPointAmount, clanId;
	int targetUserId;
	ClanQuestPointTransaction *clanQuestPointTransaction = NULL;

	HalfChop(argument, targetUserName, argument);
	HalfChop(argument, clanName, argument);
	HalfChop(argument, questPointAmountString, argument);
	skip_spaces(&argument);

	if(!*targetUserName || !*clanName || !*questPointAmountString || !*argument)
	{
		ch->send("Syntax: <Player Name> <Clan Name> <Quest Points> <Reason>\r\n");
		return;
	}

	if(!(targetUserPlayerIndex = CharacterUtil::getPlayerIndexByUserName(targetUserName)))
	{
		ch->send("The target you have selected does not exist.\r\n");
		return;
	}

	targetUserId = targetUserPlayerIndex->id;

	if(GET_LEVEL(ch) < LVL_IMMORT && targetUserId == ch->player.idnum)
	{
		ch->send("You may not award yourself quest points.\r\n");
		return;
	}

	if(!(clanId = GetClanByString(clanName)))
	{
		ch->send("There is no such clan.\r\n");
		return;
	}

	if(!MiscUtil::isInt(questPointAmountString))
	{
		ch->send("The number of quest points you specify must be an integer.\r\n");
		return;
	}

	questPointAmount = atoi(questPointAmountString);
	questPointAmount = MIN(30000, questPointAmount);
	questPointAmount = MAX(-30000,questPointAmount);
	if(GET_LEVEL(ch) < LVL_IMMORT && !IS_NPC(ch) && (questPointAmount > 5 || questPointAmount < -5))
	{
		ch->send("You may only award quest points in the range of -5 to 5.\r\n");
		return;
	}

	//Check to see if the user is allowed to perform this action.
	bool isPermitted = false;
	if(GET_LEVEL(ch) >= LVL_GRGOD || IS_NPC(ch))
		isPermitted = true;
	else
	{
		UserClan *userClan = ch->getUserClan(clanId);
		if(userClan && userClan->getIsCouncil())
			isPermitted = true;
	}

	if(!isPermitted)
	{
		ch->send("You are not permitted to perform this action.\r\n");
		return;
	}

	try
	{
		clanQuestPointTransaction = ClanUtil::performQuestPointTransaction(gameDatabase, targetUserId, clanId, questPointAmount, ch->getUserType(), ch->getUserId(), argument); //'argument' is the reason.
	}
	catch(Exception e)
	{
		ch->send("%s\r\n", e.what());
	}
	catch(std::exception e)
	{
		ch->send("Could not award quest points.\r\n%s\r\n", e.what());
	}

	if(clanQuestPointTransaction != NULL)
	{//The operation succeeded.
		ch->send("You award %s %d quest points.\r\n", targetUserPlayerIndex->name.c_str(), clanQuestPointTransaction->getAmount(), clanQuestPointTransaction->getId());
		MudLog(BRF, MAX(GET_LEVEL(ch), LVL_APPR), TRUE, "%s has awarded %s %d quest points. Transaction ID: %d Reason given: `%s`", GET_NAME(ch), targetUserPlayerIndex->name.c_str(), clanQuestPointTransaction->getAmount(), clanQuestPointTransaction->getId(), clanQuestPointTransaction->getReason().c_str());

		Character *targetUser = CharacterUtil::getOnlineCharacterById(targetUserId);
		if(targetUser != NULL)
		{
			Clan *clan = ClanUtil::getClan(clanId);
			targetUser->send("You have been awarded %d quest points in %s.\r\n", clanQuestPointTransaction->getAmount(), clan->Name.c_str());
		}

		delete clanQuestPointTransaction;
	}
}

ACMD(do_send)
{
	Character *vict;

	HalfChop(argument, ::arg, buf);

	if (!*::arg)
	{
		ch->send("Send what to who?\r\n");
		return;
	}

	if (!(vict = get_char_vis(ch, ::arg)))
	{
		ch->send(NOPERSON, ch);
		return;
	}

	vict->send(buf);
	vict->send("\r\n");

	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		ch->send("Sent.\r\n");

	else
	{
		ch->send("You send '%s' to %s.\r\n", buf, GET_NAME(vict));
	}
}

/* take a string and return an rnum.. used for goto, at, etc.  -je 4/6/93 */
Room *FindTargetRoom(Character *ch, char *rawroomstr)
{
	int tmp;
	Room *location;
	Character *target_mob;
	Object *target_obj;
	char roomstr[MAX_INPUT_LENGTH];

	OneArgument(rawroomstr, roomstr);

	if (!*roomstr)
	{
		ch->send("You must supply a room number or name.\r\n");
		return 0;
	}

	if (isdigit(*roomstr) && !strchr(roomstr, '.'))
	{
		tmp = atoi(roomstr);

		if (!(location = FindRoomByVnum(tmp)))
		{
			ch->send("No room exists with that number.\r\n");
			return 0;
		}
	}
	else if ((target_mob = get_char_vis(ch, roomstr)))
		location = target_mob->in_room;
	else if ((target_obj = get_obj_vis(ch, roomstr)))
	{
		if (target_obj->in_room)
			location = target_obj->in_room;

		else
		{
			ch->send("That object is not available.\r\n");
			return 0;
		}
	}
	else
	{
//		if( !str_cmp(ch->player.name, "Fogel") )
//			ch->send("There's a private conversation going on in that room.\r\n");
//		else
			ch->send("No such creature or object around.\r\n");
		return 0;
	}

	/* a location has been found -- if you're < GRGOD, check restrictions. */
	if (GET_LEVEL(ch) < LVL_GRGOD)
	{
		if (ROOM_FLAGGED(location, ROOM_GODROOM))
		{
			ch->send("You are not godly enough to use that room!\r\n");
			return 0;
		}

		if (ROOM_FLAGGED(location, ROOM_PRIVATE) &&
		        location->people && location->people->next_in_room)
		{
			ch->send("There's a private conversation going on in that room.\r\n");
			return 0;
		}
	}

	return location;
}

ACMD(do_at)
{
	char command[MAX_INPUT_LENGTH];
	Room *loc, *original_loc;

	HalfChop(argument, buf, command);

	if (!*buf)
	{
		ch->send("You must supply a room number or a name.\r\n");
		return;
	}

	if (!*command)
	{
		ch->send("What do you want to do there?\r\n");
		return;
	}

	if (!(loc = FindTargetRoom(ch, buf)))
		return;

	if(GET_LEVEL(ch) >= LVL_IMMORT && GET_LEVEL(ch) <= LVL_BLDER || PLR_FLAGGED(ch, PLR_ZONE_BAN))
	{
		if(!loc->getZone()->CanEdit(ch))
		{
			ch->send("You must be higher level to leave your zone!\r\n");
			return;
		}
	}

	/* a location has been found. */
	original_loc = ch->in_room;
	ch->RemoveFromRoom();
	ch->MoveToRoom(loc);

	Room *r = ch->in_room;

	CommandInterpreter(ch, command);

	MudLog(NRM, MAX(GET_INVIS_LEV(ch), LVL_GRGOD),TRUE, "%s used the at command : \"%s\" in room %d.", GET_NAME(ch), command, r->getVnum());

	if( ch->IsPurged() )
		return;

	/* check if the char is still there */
	if (ch->in_room == loc)
	{
		ch->RemoveFromRoom();
		ch->MoveToRoom(original_loc);
	}
}

ACMD(do_goto)
{
	Room *loc;

	if (!(loc = FindTargetRoom(ch, argument)))
		return;

	/* Must have permission to leave your zone at lower levels... */
	if(GET_LEVEL(ch) >= LVL_IMMORT && GET_LEVEL(ch) <= LVL_BLDER || PLR_FLAGGED(ch, PLR_ZONE_BAN))
		if(!loc->getZone()->CanEdit(ch))
		{
			ch->send("You must be higher level to leave your zone!\r\n");
			return;
		}

	//Has a poofout set.
	if (!ch->points.poofout.empty())
		sprintf(buf, "%s", ch->points.poofout.c_str());
	else
		strcpy(buf, "$n steps back into another part of the Pattern.");

	Act(buf, TRUE, ch, 0, 0, TO_ROOM);
	ch->RemoveFromRoom();
	ch->MoveToRoom(loc);

	if(MOUNT(ch))
	{
		MOUNT(ch)->RemoveFromRoom();
		MOUNT(ch)->MoveToRoom(loc);
	}

	if (!ch->points.poofin.empty())
		sprintf(buf, "%s", ch->points.poofin.c_str());
	else
		strcpy(buf, "$n appears from no-where.");

	Act(buf, TRUE, ch, 0, 0, TO_ROOM);
	look_at_room(ch, 0);
}

ACMD(do_trans)
{
	Descriptor *i;
	Character *victim;

	OneArgument(argument, buf);

	if (!*buf)
		ch->send("Whom do you wish to transfer?\r\n");
	else if (str_cmp("all", buf) && str_cmp("t_all", buf))
	{
		if (!(victim = get_char_vis(ch, buf)))
			ch->send(NOPERSON);
		else if (victim == ch)
			ch->send("That doesn't make much sense, does it?\r\n");
		else
		{
			if ((GET_LEVEL(ch) < GET_LEVEL(victim)) && !IS_NPC(victim))
			{
				ch->send("Go transfer someone your own size.\r\n");
				return;
			}
			Act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
			victim->RemoveFromRoom();
			victim->MoveToRoom(ch->in_room);
			Act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
			Act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
			look_at_room(victim, 0);

			MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s transfers %s to room vnum %d.", GET_NAME(ch), GET_NAME(victim), victim->in_room->getVnum());
		}
	}

	else if(!str_cmp(buf, "t_all"))
	{
		if (GET_LEVEL(ch) < LVL_GRGOD)
		{
			ch->send("I think not.\r\n");
			return;
		}

		for (victim = character_list; victim; victim = victim->next)
		{
			if( victim == ch ) continue;
			if (GET_LEVEL(victim) >= GET_LEVEL(ch)) continue;

			Act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
			victim->RemoveFromRoom();
			victim->MoveToRoom(ch->in_room);
			Act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
			Act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
			look_at_room(victim, 0);
		}
		ch->send(OK);
	}
	else if(!str_cmp(buf, "all"))
	{			/* Trans All */
		if (GET_LEVEL(ch) < LVL_GRGOD)
		{
			ch->send("I think not.\r\n");
			return;
		}

		for (i = descriptor_list; i; i = i->next)
			if (STATE(i) == CON_PLAYING && i->character && i->character != ch)
			{
				victim = i->character;

				if (GET_LEVEL(victim) >= GET_LEVEL(ch))
					continue;

				Act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
				victim->RemoveFromRoom();
				victim->MoveToRoom(ch->in_room);
				Act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
				Act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
				look_at_room(victim, 0);
			}

		ch->send(OK);
	}
}

ACMD(do_teleport)
{
	Character *victim;
	Room *target;

	TwoArguments(argument, buf, buf2);

	if (!*buf)
		ch->send("Whom do you wish to teleport?\r\n");
	else if (!(victim = get_char_vis(ch, buf)))
		ch->send(NOPERSON);
	else if (victim == ch)
		ch->send("Use 'goto' to teleport yourself.\r\n");
	else if (GET_LEVEL(victim) >= GET_LEVEL(ch))
		ch->send("Maybe you shouldn't do that.\r\n");
	else if (!*buf2)
		ch->send("Where do you wish to send this person?\r\n");
	else if ((target = FindTargetRoom(ch, buf2)))
	{
		ch->send(OK);
		Act("$n disappears in a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
		victim->RemoveFromRoom();
		victim->MoveToRoom(target);
		Act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
		Act("$n has teleported you!", FALSE, ch, 0, (char *) victim, TO_VICT);
		look_at_room(victim, 0);


		MudLog(NRM, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE, "%s has teleported %s to %s.", GET_NAME(ch), GET_NAME(victim), victim->in_room->getName());

		victim->CancelTimer(false);
	}
}

ACMD(do_vnum)
{
	TwoArguments(argument, buf, buf2);

	if (!*buf || !*buf2 || (!IsAbbrev(buf, "mob") && !IsAbbrev(buf, "obj")
		&& !IsAbbrev(buf, "kit") && !IsAbbrev(buf, "room") && !IsAbbrev(buf, "js") && !IsAbbrev(buf, "zone")))
	{
		ch->send("Usage: vnum { obj | trig | mob | kit | room | js | zone } <name>\r\n");
		return;
	}

	if (IsAbbrev(buf, "mob"))
		if (!ch->VnumMobile(buf2))
			ch->send("No mobiles by that name.\r\n");
	if (IsAbbrev(buf, "obj"))
		if (!ch->VnumObject(buf2))
			ch->send("No objects by that name.\r\n");
	if (IsAbbrev(buf, "kit"))
		if (!ch->VnumKit(buf2))
			ch->send("No kit by that name.\r\n");
	if (IsAbbrev(buf, "zone"))
		if (!ch->VnumZone(buf2))
			ch->send("No zone by that name.\r\n");
	if (IsAbbrev(buf, "room"))
		if (!ch->VnumRoom(buf2))
			ch->send("No room by that name.\r\n");

    if (IsAbbrev(buf, "js"))
    {
        std::vector<JSTrigger*> results= JSManager::get()->searchTrigger(buf2);
        if (!results.empty())
        {
            for(int i = 0; i < results.size(); ++i)
            {
                ch->send("%3d. [%5d] %s\r\n", i+1, results[i]->vnum, results[i]->name.c_str());
            }
        }
        else
            ch->send("No JS Trigger by that name.\r\n");
    }
}

void do_stat_room(Character * ch)
{
	ExtraDescription *desc;
	Room *rm = ch->in_room;
	int i, found = 0;
	Object *j = 0;
	Character *k = 0;
	int dir_opt;

	sprintf(buf, "Room name: %s%s%s\r\n", COLOR_CYAN(ch, CL_NORMAL), rm->getName(), COLOR_NORMAL(ch, CL_NORMAL));

	ch->send(buf);

	sprintf(buf, "Zone: [%3d], VNum: [%s%5d%s], RNum: [%5d], Light [%2d], Type: %s\r\n",
		rm->getZone()->getVnum(), COLOR_GREEN(ch, CL_NORMAL), rm->getVnum(),
		COLOR_NORMAL(ch, CL_NORMAL), real_room(rm->getVnum()), (int)rm->getLight(), rm->getSector()->getStandardName().c_str());

	ch->send(buf);

	sprintbit((long)rm->room_flags, (const char**)room_bits, buf2);
	sprintf(buf, "SpecProc: %s, Flags: %s\r\n",
	        (rm->func == NULL) ? "None" : "Exists", buf2);

	ch->send(buf);

	ch->send("Description:\r\n");

	if (rm->description)
		ch->send(rm->description);
	else
		ch->send("  None.\r\n");

	if (rm->ex_description)
	{
		sprintf(buf, "Extra descs:%s", COLOR_CYAN(ch, CL_NORMAL));
		for (desc = rm->ex_description; desc; desc = desc->next)
		{
			strcat(buf, " ");
			strcat(buf, desc->keyword);
		}

		strcat(buf, COLOR_NORMAL(ch, CL_NORMAL));
		ch->send(strcat(buf, "\r\n"));
	}

	sprintf(buf, "Chars present:%s", COLOR_YELLOW(ch, CL_NORMAL));

	for (found = 0, k = rm->people; k; k = k->next_in_room)
	{
		if (!CAN_SEE(ch, k))
			continue;
		sprintf(buf2, "%s %s(%s)", found++ ? "," : "", GET_NAME(k),
		        (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")));

		strcat(buf, buf2);
		if (strlen(buf) >= 62)
		{
			if (k->next_in_room)
				ch->send(strcat(buf, ",\r\n"));
			else
				ch->send(strcat(buf, "\r\n"));
			*buf = found = 0;
		}
	}

	if (*buf)
		ch->send(strcat(buf, "\r\n"));

	ch->send(COLOR_NORMAL(ch, CL_NORMAL));

	if (rm->contents)
	{
		sprintf(buf, "Contents:%s", COLOR_GREEN(ch, CL_NORMAL));
		for (found = 0, j = rm->contents; j; j = j->next_content)
		{
			if (!CAN_SEE_OBJ(ch, j))
				continue;

			sprintf(buf2, "%s %s", found++ ? "," : "", j->GetSDesc());
			strcat(buf, buf2);

			if (strlen(buf) >= 62)
			{
				if (j->next_content)
					ch->send(strcat(buf, ",\r\n"));

				else
					ch->send(strcat(buf, "\r\n"));
				*buf = found = 0;
			}
		}

		if (*buf)
			ch->send(strcat(buf, "\r\n"));
		ch->send(COLOR_NORMAL(ch, CL_NORMAL));
	}

	for (i = 0; i < NUM_OF_DIRS; ++i)
	{
		if (rm->dir_option[i])
		{
			if (!rm->dir_option[i]->getToRoom())
				sprintf(buf1, " %sNONE%s", COLOR_CYAN(ch, CL_NORMAL), COLOR_NORMAL(ch, CL_NORMAL));
			else
				sprintf(buf1, "%s%5d%s", COLOR_CYAN(ch, CL_NORMAL), rm->dir_option[i]->getToRoom()->getVnum(), COLOR_NORMAL(ch, CL_NORMAL));
			dir_opt = rm->dir_option[i]->getExitInfo();
			char sBuf[1024];
			sprintbit((long)dir_opt, (const char**)(exit_bits), sBuf, ", ", COLOR_YELLOW(ch, CL_NORMAL), "");
			sprintf(buf, "Exit %s%-5s%s:  To: [%s], Key: [%5d], Keywrd: %s, Type: %s\r\n",
			        COLOR_CYAN(ch, CL_NORMAL), dirs[i], COLOR_NORMAL(ch, CL_NORMAL), buf1, rm->dir_option[i]->getKey(),
			        rm->dir_option[i]->getKeywords() ? rm->dir_option[i]->getKeywords() : "None", sBuf);
			ch->send(buf);


			if (rm->dir_option[i]->getGeneralDescription())
				strcpy(buf, rm->dir_option[i]->getGeneralDescription());
			else
				strcpy(buf, "  No exit description.\r\n");

			ch->send(buf);
		}
	}
	js_list_scripts(ch->in_room,ch);
}

void do_stat_object(Character * ch, Object * j)
{
	int i, vnum, found;
	Object *j2;
	ExtraDescription *desc;

	MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE,
	       "%s did a statfind on object %s.", GET_NAME(ch), j->GetSDesc());

	vnum = GET_OBJ_VNUM(j);
	sprintf(buf, "Name: '%s%s%s', Aliases: %s\r\n", COLOR_YELLOW(ch, CL_NORMAL),
	        ((j->GetSDesc()) ? j->GetSDesc() : "<None>"),
	        COLOR_NORMAL(ch, CL_NORMAL), j->getName());

	ch->send(buf);

	if (GET_OBJ_RNUM(j) >= 0)
		strcpy(buf2, (obj_index[GET_OBJ_RNUM(j)].func ? "Exists" : "None"));
	else
		strcpy(buf2, "None");

	sprintf(buf, "VNum: [%s%5d%s], RNum: [%5d], Type: %s, SpecProc: %s, Num Existing[%7s]\r\n",
	        COLOR_GREEN(ch, CL_NORMAL), vnum, COLOR_NORMAL(ch, CL_NORMAL), GET_OBJ_RNUM(j), item_types[(int)j->getType()],
	        buf2, (j->item_number < (int) ItemCount.size() && j->item_number > -1) ? itos(ItemCount[j->item_number]).c_str() : "Unknown");

	ch->send(buf);
	sprintf(buf, "L-Des: %s\r\n", ((j->description) ? j->description : "None"));
	ch->send(buf);

	if (j->ex_description)
	{
		sprintf(buf, "Extra descs:%s", COLOR_CYAN(ch, CL_NORMAL));

		for (desc = j->ex_description; desc; desc = desc->next)
		{
			strcat(buf, " ");
			strcat(buf, desc->keyword);
		}

		strcat(buf, COLOR_NORMAL(ch, CL_NORMAL));
		ch->send(strcat(buf, "\r\n"), ch);
	}
	ch->send("Item ID: %s%s%s%s\r\n", COLOR_BOLD(ch, CL_COMPLETE), COLOR_GREEN(ch, CL_COMPLETE),
		ToString(j->objID).c_str(), COLOR_NORMAL(ch, CL_COMPLETE));

	ch->send("Created: %s%s%s%s\r\n", COLOR_BOLD(ch, CL_COMPLETE), COLOR_GREEN(ch, CL_COMPLETE),
		DateTime::parse("%m-%d-%Y %H:%M:%S", j->createdDatetime).c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
	ch->send("Creator: %s%s%s%s\r\n", COLOR_BOLD(ch, CL_COMPLETE),
	         COLOR_GREEN(ch, CL_COMPLETE), j->creator.c_str(), COLOR_NORMAL(ch, CL_COMPLETE));

	ch->send("Can be worn on: ");
	sprintbit(j->obj_flags.wear_flags, wear_bits, buf);
	strcat(buf, "\r\n");
	ch->send(buf, ch);

	ch->send("Set char bits : ");
	sprintbitarray( (int *) j->obj_flags.bitvector, affected_bits, TW_ARRAY_MAX, buf);
	strcat(buf, "\r\n");
	ch->send(buf);

	ch->send("Extra flags   : ");
	sprintbit(GET_OBJ_EXTRA(j), extra_bits, buf);
	strcat(buf, "\r\n");
	ch->send(buf);

	sprintf(buf, "Weight: %d, Value: %d, Cost/day: %d, Timer: %d, Purged: %s%s%s\r\n",
	        (int) j->Weight(), GET_OBJ_COST(j), GET_OBJ_RENT(j), GET_OBJ_TIMER(j),
			COLOR_GREEN(ch, CL_COMPLETE), (j->IsPurged() ? "Yes" : "No"), COLOR_NORMAL(ch, CL_COMPLETE));
	ch->send(buf);

	strcpy(buf, "In room: ");
	if (!j->in_room)
		strcat(buf, "Nowhere");

	else
	{
		sprintf(buf2, "%d", j->in_room->getVnum());
		strcat(buf, buf2);
	}

	/*
	* NOTE: In order to make it this far, we must already be able to see the
	*       character holding the object. Therefore, we do not need CAN_SEE().
	*/
	strcat(buf, ", In object: ");
	strcat(buf, j->in_obj ? j->in_obj->GetSDesc() : "None");
	strcat(buf, ", Carried by: ");
	strcat(buf, j->carried_by ? GET_NAME(j->carried_by) : "Nobody");
	strcat(buf, ", Worn by: ");
	strcat(buf, j->worn_by ? GET_NAME(j->worn_by) : "Nobody");
	strcat(buf, "\r\n");
	ch->send(buf);

	switch (j->getType())
	{

		case ITEM_LIGHT:
			if (GET_OBJ_VAL(j, 2) == -1)
				strcpy(buf, "Hours left: Infinite");

			else
				sprintf(buf, "Hours left: [%d]", GET_OBJ_VAL(j, 2));
			break;

		case ITEM_POTION:
			break;

		case ITEM_WEAPON:
			sprintf(buf, "Todam: %dd%d, Message type: %d",
			        j->GetTotalVal1(), GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 3));
			if(GET_LEVEL(ch) == LVL_IMPL)
			{
				sprintf(buf + strlen(buf), ", Bash Rating: %.3f", j->BashRating());
			}
			break;

		case ITEM_ARMOR:
			sprintf(buf, "AC-apply: [%d]", GET_OBJ_VAL(j, 0));
			break;

		case ITEM_CONTAINER:
			sprintf(buf, "Weight capacity: %d, Lock Type: %s, Key Num: %d, Corpse: %s",
			        GET_OBJ_VAL(j, 0), container_bits[GET_OBJ_VAL(j, 1)], GET_OBJ_VAL(j, 2),
			        StringUtil::allUpper(StringUtil::yesNo(GET_OBJ_VAL(j, 3))).c_str());
			break;

		case ITEM_DRINKCON:
		case ITEM_FOUNTAIN:
			sprinttype(GET_OBJ_VAL(j, 2), (const char **) drinks, buf2);
			sprintf(buf, "Capacity: %d, Contains: %d, Poisoned: %s, Liquid: %s",
			        GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1), StringUtil::allUpper(StringUtil::yesNo(GET_OBJ_VAL(j, 3))).c_str(),
			        buf2);
			break;

		case ITEM_NOTE:
			sprintf(buf, "Tongue: %d", GET_OBJ_VAL(j, 0));
			break;

		case ITEM_KEY:
			strcpy(buf, "");
			break;

		case ITEM_FOOD:
			sprintf(buf, "Makes full: %d, Poisoned: %s", GET_OBJ_VAL(j, 0),
			        StringUtil::allUpper(StringUtil::yesNo(GET_OBJ_VAL(j, 3))).c_str());
			break;

		case ITEM_MONEY:
			sprintf(buf, "Coins: %d", GET_OBJ_VAL(j, 0));
			break;

		default:
			sprintf(buf, "Values 0-3: [%d] [%d] [%d] [%d]",
			        GET_OBJ_VAL(j, 0), GET_OBJ_VAL(j, 1),
			        GET_OBJ_VAL(j, 2), GET_OBJ_VAL(j, 3));
			break;

	}

	ch->send(strcat(buf, "\r\n"));

	/*
	 * I deleted the "equipment status" code from here because it seemed
	 * more or less useless and just takes up valuable screen space.
	 */

	if (j->contains)
	{
		sprintf(buf, "\r\nContents:%s", COLOR_GREEN(ch, CL_NORMAL));

		for (found = 0, j2 = j->contains; j2; j2 = j2->next_content)
		{
			sprintf(buf2, "%s %s", found++ ? "," : "", j2->GetSDesc());
			strcat(buf, buf2);

			if (strlen(buf) >= 62)
			{
				if (j2->next_content)
					ch->send(strcat(buf, ",\r\n"));

				else
					ch->send(strcat(buf, "\r\n"));

				*buf = found = 0;

			}
		}

		if (*buf)
			ch->send(strcat(buf, "\r\n"));

		ch->send(COLOR_NORMAL(ch, CL_NORMAL));

	}

	found = 0;
	ch->send("Affections:");

	for (i = 0; i < MAX_OBJ_AFFECT; ++i)
		if (j->affected[i].modifier)
		{
			sprinttype(j->affected[i].location, (const char **) apply_types, buf2);
			sprintf(buf, "%s %+d to %s", found++ ? "," : "",
			        j->affected[i].modifier, buf2);
			ch->send(buf);
		}

	if (!found)
		ch->send(" None");

	//ch->send("\r\nVAL1: %d\r\n", GET_OBJ_VAL(j, 1));
	ch->send("\r\n");

	js_list_scripts(j,ch);
}

void do_stat_character(Character * ch, Character * k)
{
	int i, i2, found = 0;

	Object *j;
	Follower *fol;
	affected_type *aff;
	UserClan *userClan;
	std::list<Warrant *>::iterator w;
	bool passed = false;

	get_char_cols(ch);

	if( GET_LEVEL(ch) < GET_LEVEL(k) )
	{
		ch->send("You can't do that.\r\n");
		return;
	}

	if( !IS_NPC(k) && GET_LEVEL(ch) < LVL_GRGOD )
	{
		ch->send("You can't statfind other players.\r\n");
		return;
	}

	MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, "%s did a statfind on %s.",
	       GET_NAME(ch), GET_NAME(k));

	switch (GET_SEX(k))
	{
		case SEX_NEUTRAL:
			strcpy(buf, "NEUTRAL-SEX");
			break;
		case SEX_MALE:
			strcpy(buf, "MALE");
			break;
		case SEX_FEMALE:
			strcpy(buf, "FEMALE");
			break;
		default:
			strcpy(buf, "ILLEGAL-SEX!!");
			break;
	}

	sprintf(buf2, " %s '%s%s%s'  IDNum [%s%5ld%s], In room [%s%5d%s], Start Room [%s%5d%s], Wait State[%s%3d%s]\r\n",
			(!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
			grn, GET_NAME(k), nrm,
			yel, (k->player.idnum), nrm,
			yel, (k->in_room ? k->in_room->getVnum() : -1), nrm,
			yel, (k->StartRoom() ? k->StartRoom()->getVnum() : -1), nrm,
			yel, k->wait, nrm);
	ch->send(strcat(buf, buf2));

	if (IS_MOB(k))
		ch->send("Alias: %s%s%s, VNum: [%s%5d%s], RNum: [%s%5d%s], Purged: %s%s%s\r\n",
		         grn, k->player.name.c_str(), nrm, yel, GET_MOB_VNUM(k), nrm, yel, GET_MOB_RNUM(k), nrm,
				 grn, (k->IsPurged() ? "Yes" : "No"), nrm);

	ch->send("Title: %s%s%s\r\n", grn, (!k->player.title.empty() ? k->player.title.c_str() : "<None>"), nrm);

	ch->send("L-Des: %s%s%s", grn, (k->player.long_descr ? k->player.long_descr : "<None>\r\n"), nrm);

	strcpy(buf, "Class: ");
	sprinttype(k->player.chclass, pc_class_types, buf2);
	strcat(buf, buf2);
	sprintf(buf + strlen(buf), ", Race: ");
	sprinttype(k->player.race, pc_race_types, buf2);
	strcat(buf, buf2);

	sprintf(buf2, ", Lev: [%s%2d%s], XP: [%s%7d%s] DeathWait: [%s%3d%s], FleeLag: [%s%3d%s]\r\n",
	        COLOR_YELLOW(ch, CL_NORMAL), GET_LEVEL(k), COLOR_NORMAL(ch, CL_NORMAL),
	        COLOR_YELLOW(ch, CL_NORMAL), (int)GET_EXP(k), COLOR_NORMAL(ch, CL_NORMAL),
	        COLOR_YELLOW(ch, CL_NORMAL), GET_DEATH_WAIT(k), COLOR_NORMAL(ch, CL_NORMAL),
	        COLOR_YELLOW(ch, CL_NORMAL), FLEE_LAG(k), COLOR_NORMAL(ch, CL_NORMAL));
	strcat(buf, buf2);
	ch->send(buf);

	if (!IS_NPC(k))
	{
		strcpy(buf1, DateTime::parse("%m-%d-%Y", k->player.time.birth).c_str());
		strcpy(buf2, DateTime::parse("%m-%d-%Y", k->points.last_logon).c_str());

		ch->send("Created: [%s%s%s], Last Logon: [%s%s%s], Played [%s%d%sh %s%d%sm], Age [%s%d%s]\r\n",
				grn, buf1, nrm, grn, buf2, nrm, yel, (k->player.time.played / 3600), nrm,
				yel, ((k->player.time.played % 3600) / 60), nrm, yel, age(k)->year, nrm);
	}

	ch->send("Str: [%s%d%s]  Int: [%s%d%s]  Wis: [%s%d%s]  "
	         "Dex: [%s%d%s]  Con: [%s%d%s]\r\n",
	         cyn, k->GetStr(), nrm,
	         cyn, k->GetInt(), nrm,
	         cyn, k->GetWis(), nrm,
	         cyn, k->GetDex(), nrm,
	         cyn, k->GetCon(), nrm);

	ch->send("Total Quest Points [%s%d%s], Weave Points [%s%d%s], Legend [%s%d%s].\r\n",
	         grn, k->TotalQP(), nrm,
	         grn, GET_WP(k), nrm,
	         grn, k->GetLegend(), nrm);

	ch->send("Hit p.:[%s%d/%d+%d%s]  Spell p.:[%s%d/%d+%d%s]  Move p.:[%s%d/%d(%d)+%d%s]  Shadow[%s%d/%d+%d%s]\r\n",
	         COLOR_GREEN(ch, CL_NORMAL), (int)GET_HIT(k), (int)GET_MAX_HIT(k), k->HitGain(), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), (int)GET_MANA(k),(int)GET_MAX_MANA(k), k->ManaGain(), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), (int)GET_MOVE(k), (int)GET_MAX_MOVE(k), k->BaseMoves(), k->MoveGain(), COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), (int)GET_SHADOW(k), (int)GET_MAX_SHADOW(k), k->ShadowGain(), COLOR_NORMAL(ch, CL_NORMAL));

	ch->send("Strain: [%s%d%s], Taint: [%s%d%s], Dizzy Time: [%s%d%s]\r\n",
             COLOR_GREEN(ch, CL_NORMAL), (int)k->points.strain, COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), (int)k->points.taint, COLOR_NORMAL(ch, CL_NORMAL),
	         COLOR_GREEN(ch, CL_NORMAL), k->dizzy_time, COLOR_NORMAL(ch, CL_NORMAL));
	/*
	 *	This section of the code will list the clans to the immortal doing the statfind. -Galnor
	 */
	if(k->userClans.empty() == false)
	{
		for(auto userClanIter = k->userClans.begin();userClanIter != k->userClans.end();++userClanIter)
		{
			UserClan *userClan = (*userClanIter);
			if(!ClanUtil::getClan(userClan->getClanId()))
			{
				ch->send("Invalid clan: [%s%d%s]", COLOR_GREEN(ch, CL_NORMAL), userClan->getClanId(), COLOR_NORMAL(ch, CL_NORMAL));
				return;
			}
			time_t rt = userClan->getLastRankedDatetime().getTime(), ct = userClan->getClannedDatetime().getTime();
			ch->send("Clan [%s%s%s], Rank [%s%d%s], Clanned [%s%s%s], Ranked [%s%s%s], Quest Points [%s%d%s], To Rank [%s%d%s]\r\n",
			         COLOR_GREEN(ch, CL_NORMAL), ClanUtil::getClan(userClan->getClanId())->Name.c_str(), COLOR_NORMAL(ch, CL_NORMAL),
			         COLOR_GREEN(ch, CL_NORMAL), (int)userClan->getRank(), COLOR_NORMAL(ch, CL_NORMAL),
					 COLOR_GREEN(ch, CL_NORMAL), Time::FormatDate("%m-%d-%Y",ct).c_str(), COLOR_NORMAL(ch, CL_NORMAL),
					 COLOR_GREEN(ch, CL_NORMAL), Time::FormatDate("%m-%d-%Y",rt).c_str(), COLOR_NORMAL(ch, CL_NORMAL),
			         COLOR_GREEN(ch, CL_NORMAL), userClan->getQuestPoints(), COLOR_NORMAL(ch, CL_NORMAL),
			         COLOR_GREEN(ch, CL_NORMAL), (rank_req[userClan->getRank()] - userClan->getQuestPoints()), COLOR_NORMAL(ch, CL_NORMAL));
		}
	}

	ch->send("Warrants: ");
	for(w = Warrants.begin();w != Warrants.end();++w)
	{
		if(IS_SET_AR(GET_WARRANTS(k), (*w)->vnum))
		{
			if(passed)
				ch->send(", ");
			else
				passed = true;
			ch->send("%s%s%s", COLOR_GREEN(ch, CL_COMPLETE), (*w)->Name.c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
		}
	}
	if(!passed)
		ch->send("%sNone%s", COLOR_GREEN(ch, CL_COMPLETE), COLOR_NORMAL(ch, CL_COMPLETE));
	ch->send("\r\n");

	ch->send("Gold: [%9d], Bank: [%9d] (Total: %d)\r\n",
             (int) k->points.gold, (int) GET_BANK_GOLD(k), (int) k->points.gold + (int) GET_BANK_GOLD(k));

	ch->send("ABS: [%d], Offense: [%2d], Dodge: [%2d], Parry: [%2d], Damroll: [%2d]\r\n",
             k->Absorb(), k->Offense(), k->Dodge(), k->Parry(),(int) k->points.damroll);

	sprinttype(GET_POS(k), (const char **) position_types, buf2);
	sprintf(buf, "Pos: %s, Fighting: %s", buf2,
	        (FIGHTING(k) ? GET_NAME(FIGHTING(k)) : "Nobody"));

	if (IS_NPC(k))
	{
		strcat(buf, ", Attack type: ");
		strcat(buf, attack_hit_text[k->MobData->attack_type].singular);
	}

	if (k->desc)
	{
		sprinttype(STATE(k->desc), (const char **) connected_types, buf2);
		strcat(buf, ", Connected: ");
		strcat(buf, buf2);
	}

	if(IS_NPC(k))
	{
		strcpy(buf, "Default position: ");
		sprinttype((k->MobData->default_pos), (const char **) position_types, buf2);
		strcat(buf, buf2);
	}
	sprintf(buf2, ", Idle Timer (in tics) [%d]\r\n", k->player.timer);
	strcat(buf, buf2);
	ch->send(buf);

	if (IS_NPC(k))
	{
		sprintbit(MOB_FLAGS(k), action_bits, buf2);
		sprintf(buf, "NPC flags: %s%s%s\r\n", COLOR_CYAN(ch, CL_NORMAL), buf2, COLOR_NORMAL(ch, CL_NORMAL));
		ch->send(buf);
	}

	else
	{
		sprintbit(PLR_FLAGS(k), player_bits, buf2);
		sprintf(buf, "PLR: %s%s%s\r\n", COLOR_CYAN(ch, CL_NORMAL), buf2, COLOR_NORMAL(ch, CL_NORMAL));
		ch->send(buf);
		sprintbitarray(PRF_FLAGS(k), preference_bits, PM_ARRAY_MAX, buf2);
		sprintf(buf, "PRF: %s%s%s\r\n", COLOR_GREEN(ch, CL_NORMAL), buf2, COLOR_NORMAL(ch, CL_NORMAL));
		ch->send(buf);
	}

	if(IS_MOB(k))
	{
		sprintf(buf, "Mob Spec-Proc: %s, NPC Bare Hand Dam: %dd%d\r\n",
			(MobManager::GetManager().GetIndex(k->nr)->func ? "Exists" : "None"),
			k->MobData->damnodice, k->MobData->damsizedice);
		ch->send(buf);
	}

	sprintf(buf, "Carried: weight: %d, items: %d; ",
	        (int) k->CarriedWeight(), IS_CARRYING_N(k));

	for (i = 0, j = k->carrying; j; j = j->next_content, i++)
		;
	sprintf(buf + strlen(buf), "Items in: inventory: %d, ", i);

	for (i = 0, i2 = 0; i < NUM_WEARS; i++)
		if (GET_EQ(k, i))
			i2++;

	sprintf(buf2, "eq: %d\r\n", i2);
	strcat(buf, buf2);
	ch->send(buf);

	if (!IS_NPC(k))
	{
		sprintf(buf, "Hunger: %d, Thirst: %d, Drunk: %d\r\n",
			k->PlayerData->conditions[ FULL ], k->PlayerData->conditions[ THIRST ],
			k->PlayerData->conditions[ DRUNK ]);
		ch->send(buf);
	}

	sprintf(buf, "Master is: %s, Followers are:",
	        ((k->master) ? GET_NAME(k->master) : "<none>"));

	for (fol = k->followers; fol; fol = fol->next)
	{
		sprintf(buf2, "%s %s", found++ ? "," : "", PERS(fol->follower, ch));
		strcat(buf, buf2);

		if (strlen(buf) >= 62)
		{
			if (fol->next)
				ch->send(strcat(buf, ",\r\n"));

			else
				ch->send(strcat(buf, "\r\n"));

			*buf = found = 0;
		}
	}

	if (*buf)
		ch->send(strcat(buf, "\r\n"));

	/* Showing the bitvector*/
	sprintbitarray(AFF_FLAGS(k), affected_bits, AF_ARRAY_MAX, buf2);
	sprintf(buf, "AFF: %s%s%s\r\n", COLOR_YELLOW(ch, CL_NORMAL), buf2, COLOR_NORMAL(ch, CL_NORMAL));
	ch->send(buf);

	/* Routine to show what spells a char is affected by */

	if (k->affected)
	{
		for (aff = k->affected; aff; aff = aff->next)
		{
			*buf2 = '\0';
			sprintf(buf, "SPL: (%3dhr) %s%-21s%s ", aff->duration + 1,
			        COLOR_CYAN(ch, CL_NORMAL), (aff->type >= 0 && aff->type <= MAX_SPELLS) ?
					WeaveManager::GetManager().GetWeaveName(aff->type).c_str() : "TYPE UNDEFINED", COLOR_NORMAL(ch, CL_NORMAL));

			if (aff->modifier)
			{
				sprintf(buf2, "%+d to %s", aff->modifier, apply_types[(int) aff->location]);
				strcat(buf, buf2);
			}

			if (aff->bitvector)
			{
				if (*buf2)
					strcat(buf, ", sets ");

				else
					strcat(buf, "sets ");

				sprintf(buf2, "%s", affected_bits[aff->bitvector]);
				strcat(buf, buf2);
			}

			ch->send(strcat(buf, "\r\n"));
		}
	}
	js_list_scripts( k, ch );
}

ACMD(do_statfind)
{
	Character *victim = 0;
	Object *object = 0;
	int tmp;

	HalfChop(argument, buf1, buf2);

	if (!*buf1)
	{
		ch->send("Stats on who or what?\r\n");
		return;
	}
	else if (IsAbbrev(buf1, "room"))
	{
		do_stat_room(ch);
	}
	else if (IsAbbrev(buf1, "mob"))
	{
		if (!*buf2)
			ch->send("Stats on which mobile?\r\n");
		else
		{
			if ((victim = get_char_vis(ch, buf2)))
				do_stat_character(ch, victim);
			else
				ch->send("No such mobile around.\r\n");
		}
	}
	else if (IsAbbrev(buf1, "player"))
	{
		if (!*buf2)
		{
			ch->send("Stats on which player?\r\n");
		}
		else
		{
			if ((victim = get_player_vis(ch, buf2, 0)) && GET_LEVEL(victim) < GET_LEVEL(ch))
				do_stat_character(ch, victim);
			else
				ch->send("No such player around.\r\n");
		}
	}
	else if (IsAbbrev(buf1, "file"))
	{
		if (!*buf2)
		{
			ch->send("Stats on which player?\r\n");
		}
		else
		{
			if( (victim = CharacterUtil::loadCharacter(buf2)) != NULL )
			{
				if (GET_LEVEL(victim) > GET_LEVEL(ch))
					ch->send("Sorry, you can't do that.\r\n");
				else {
					do_stat_character(ch, victim);
				}
				delete victim;
			}
			else
			{
				ch->send("There is no such player.\r\n");
			}
			victim = NULL;
		}
	}
	else if (IsAbbrev(buf1, "object"))
	{
		if (!*buf2)
			ch->send("Stats on which object?\r\n");
		else
		{
			try {
				if((object = get_obj_vis(ch, buf2)))
					do_stat_object(ch, object);
				else if(boost::regex_match(buf2, boost::regex("[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}")))
				{//User entered an object id.

					boost::uuids::string_generator uidGenerator;
					boost::uuids::uuid objectId = uidGenerator(buf2);

					object = Object::loadSingleItem(objectId, false);
					
					if(object != NULL)
					{
						do_stat_object(ch, object);

						object->Extract();
					}
					else {
						ch->send("No such object around.\r\n");
					}
				}
				else {
					ch->send("No such object around.\r\n");
				}
			}
			catch(...) {

				ch->send("wtferror\r\n");
				return;
			}
		}
	}
	else
	{
		if ((object = get_object_in_equip_vis(ch, buf1, ch->equipment, &tmp)))
			do_stat_object(ch, object);
		else if ((object = get_obj_in_list_vis(ch, buf1, ch->carrying)))
			do_stat_object(ch, object);
		else if ((victim = get_char_room_vis(ch, buf1)))
			do_stat_character(ch, victim);
		else if ((object = get_obj_in_list_vis(ch, buf1, ch->in_room->contents)))
			do_stat_object(ch, object);
		else if ((victim = get_char_vis(ch, buf1)))
			do_stat_character(ch, victim);
		else if ((object = get_obj_vis(ch, buf1)))
			do_stat_object(ch, object);
		else
			ch->send("Nothing around by that name.\r\n");
	}
}

void Character::stopSnooping()
{
	if( !desc || !desc->snooping )
		this->send("You aren't snooping anyone.\r\n");
	else
	{
		if( !this->hasPermissionToSnoop() )
		{
			this->send("You drop your snooping request.\r\n");
			this->desc->snooping->send("%s has dropped the snooping request.\r\n",
				PERS(this, this->desc->snooping->character));
			MudLog(BRF, MAX(GET_LEVEL(this), LVL_APPR), TRUE, "%s has dropped %s snooping request", GET_NAME(this),
				HSHR(this));
		}
		else
		{
			this->send("You stop snooping.\r\n");
			if( GET_LEVEL(this) < LVL_IMPL )
			{
				this->desc->snooping->send("%s has stopped snooping you.\r\n", PERS(this, this->desc->snooping->character));
				MudLog(BRF, MAX(GET_LEVEL(this), LVL_APPR), TRUE, "%s has stopped snooping %s.", GET_NAME(this),
					GET_NAME(this->desc->snooping->character));
			}
		}
		this->desc->snooping->snoop_by = NULL;
		this->desc->snooping = NULL;
		if( this->hasPermissionToSnoop() ) this->ToggleSnoop();
	}
}

ACMD(do_shutdown)
{
	char arg[MAX_INPUT_LENGTH];

	if (subcmd != SCMD_SHUTDOWN)
	{
		ch->send("If you want to shut something down, say so!\r\n");
		return;
	}

	OneArgument(argument, arg);

	if (!*arg)
	{
		Log("(GC) Shutdown by %s.", GET_NAME(ch));
		ch->send("Shutdown by %s.\r\n", GET_NAME(ch));
		circle_shutdown = 1;
	}
}

ACMD(do_deny)
{

	Character *Snooper;
	if( !ch->desc || !ch->desc->snoop_by || (ch->desc && ch->desc->snoop_by && ch->desc->snoop_by->character &&
	ch->desc->snoop_by->character->hasPermissionToSnoop() ) )
	{
		ch->send("But nobody is requesting to snoop you.\r\n");
		return;
	}
	Snooper = ch->desc->snoop_by->character;
	ch->send("You have denied %s permission to snoop you.\r\n", PERS(Snooper,ch));
	Snooper->send("%s has denied you permission to snoop.\r\n", PERS(ch,Snooper));
	MudLog(BRF, MAX(GET_LEVEL(ch), LVL_APPR), TRUE, "%s has denied %s permission to snoop.",
		GET_NAME(ch), GET_NAME(Snooper));

	ch->desc->snoop_by = NULL;
	Snooper->desc->snooping = NULL;
}

//Grant snooper permission to snoop.
ACMD(do_grant)
{
	if( !ch->desc || !ch->desc->snoop_by || (ch->desc && ch->desc->snoop_by && ch->desc->snoop_by->character &&
	ch->desc->snoop_by->character->hasPermissionToSnoop() ) )
	{
		ch->send("But nobody is requesting to snoop you.\r\n");
		return;
	}
	Character *Snooper = ch->desc->snoop_by->character;
	ch->send("You have granted %s permission to snoop you.\r\n", PERS(Snooper,ch));
	Snooper->send("%s has granted you permission to snoop.\r\n", PERS(ch,Snooper));
	Snooper->GrantPermissionToSnoop();

	MudLog(BRF, MAX(GET_LEVEL(ch), LVL_APPR), TRUE, "%s has granted %s permission to snoop.",
		GET_NAME(ch), GET_NAME(Snooper));
}

bool Character::hasPermissionToSnoop()
{
	return this->PermissionToSnoop;
}

void Character::GrantPermissionToSnoop()
{
	this->PermissionToSnoop = true;
}

void Character::ToggleSnoop()
{
	this->PermissionToSnoop = !this->PermissionToSnoop;
}

ACMD(do_snoop)
{
	Character *victim, *tch;

	if (!ch->desc)
		return;

	OneArgument(argument, ::arg);

	if (!*::arg)
		ch->stopSnooping();
	else if (!(victim = get_char_vis(ch, ::arg)))
		ch->send("No such person around.\r\n");
	else if (!victim->desc)
		ch->send("There's no link.. nothing to snoop.\r\n");
	else if (victim == ch)
		ch->stopSnooping();
	else if (victim->desc->snoop_by)
		ch->send("Busy already. \r\n");
	else if (victim->desc->snooping == ch->desc)
		ch->send("Don't be stupid.\r\n");
	else
	{
		if (victim->desc->original)
			tch = victim->desc->original;
		else
			tch = victim;

		if( GET_LEVEL(ch) < LVL_IMPL && GET_LEVEL(tch) > 5 )
		{
			if( ch->hasPermissionToSnoop() )
				ch->ToggleSnoop();//Unset the snooper's permission if it is set. They need to be granted permission again.
			ch->send("Requesting permission from %s...\r\n", PERS(tch, ch));
			tch->send("%s has requested permission to snoop you.\r\n", PERS(ch, tch));
		}
		else
		{
			ch->send(OK);
			if( !ch->hasPermissionToSnoop() )
				ch->GrantPermissionToSnoop();
		}

		MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s begins snooping %s.", GET_NAME(ch), GET_NAME(victim));

		if (ch->desc->snooping)
			ch->desc->snooping->snoop_by = NULL;

		ch->desc->snooping = victim->desc;
		victim->desc->snoop_by = ch->desc;
	}
}

ACMD(do_switch)
{
	Character *victim;

	OneArgument(argument, ::arg);

	if (ch->desc->original)
		ch->send("You're already switched.\r\n");
	else if (!*::arg)
		ch->send("Switch with who?\r\n");
	else if (!(victim = get_char_vis(ch, ::arg)))
		ch->send("No such character.\r\n");
	else if (ch == victim)
		ch->send("Hee hee... we are jolly funny today, eh?\r\n");
	else if (victim->desc)
		ch->send("You can't do that, the body is already in use!\r\n");
	else if ((GET_LEVEL(ch) < LVL_IMPL) && !IS_NPC(victim))
		ch->send("You aren't holy enough to use a mortal's body.\r\n");
	else
	{
		ch->send(OK);

		MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, "%s has switched into %s.",
		       GET_NAME(ch), GET_NAME(victim));

		std::copy(PRF_FLAGS(ch), &PRF_FLAGS(ch)[PR_ARRAY_MAX], PRF_FLAGS(victim));
		ch->desc->character = victim;
		ch->desc->original = ch;

		victim->desc = ch->desc;
		ch->desc = NULL;
	}
}

ACMD(do_return)
{
	if (ch->desc && ch->desc->original)
	{
		ch->send("You return to your original body.\r\n");

		/* JE 2/22/95 */
		/* if someone switched into your original body, disconnect them */
		if (ch->desc->original->desc)
			STATE(ch->desc->original->desc) = CON_DISCONNECT;

		ch->desc->character = ch->desc->original;
		ch->desc->original = NULL;

		ch->desc->character->desc = ch->desc;
		ch->desc = NULL;
	}
}

ACMD(do_mbload)
{
	Character *mob;
	int number, r_num;
	char logBuffer[256];

	OneArgument(argument, buf);

	if (!*buf || !isdigit(*buf))
	{
		ch->send("Usage: mbload <number>\r\n");
		return;
	}

	if ((number = atoi(buf)) < 0)
	{
		ch->send("A NEGATIVE number??\r\n");
		return;
	}

	if ((r_num = MobManager::GetManager().RealMobile(number)) < 0)
	{
		ch->send("There is no monster with that number.\r\n");
		return;
	}

	mob = new Character(r_num, REAL);
	snprintf(logBuffer, sizeof(logBuffer), "mbload by %s(UID %ld)", GET_NAME(ch), ch->player.idnum);
	mobLoadLogger.logMobLoad(mob->getVnum(), logBuffer);
	mob->MoveToRoom(ch->in_room);

	Act("$n has created $N!", FALSE, ch, 0, mob, TO_ROOM);
	Act("You create $N.", FALSE, ch, 0, mob, TO_CHAR);

	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s has created %s.", GET_NAME(ch), GET_NAME(mob));

	if( mob->IsPurged() ) return;

	js_load_triggers(mob);
}

ACMD(do_oload)
{
	Object *obj;
	int number, r_num;

	OneArgument(argument, buf);

	if (!*buf || !isdigit(*buf))
	{
		ch->send("Usage: oload <number>\r\n");
		return;
	}

	if ((number = atoi(buf)) < 0)
	{
		ch->send("A NEGATIVE number??\r\n");
		return;
	}
	if ((r_num = real_object(number)) < 0)
	{
		ch->send("There is no object with that number.\r\n");
		return;
	}

	if( obj_proto[r_num]->getType() == ITEM_SPECIAL && GET_LEVEL(ch) < LVL_GRGOD )
	{
		ch->send("That item is flagged as special. You may not load it.\r\n");
		return;
	}

	obj = read_object(r_num, REAL, true);
	//obj->creator = "Normal load by " + ch->player.name + ".";
	sprintf(obj->creator, "Normal load by %s.", GET_NAME(ch));

	Act("$n has created $p!", FALSE, ch, obj, 0, TO_ROOM);
	Act("You create $p.", FALSE, ch, obj, 0, TO_CHAR);
	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s has created %s.", GET_NAME(ch), obj->GetSDesc());

	if (CONFIG_LOAD_INVENTORY)
		obj_to_char(obj, ch);
	else
		obj->MoveToRoom(ch->in_room);
	if( obj->IsPurged() ) return;

	js_load_triggers(obj);
}

ACMD(do_vstat)
{
	Character *mob;
	Object *obj;
	int number, r_num;

	TwoArguments(argument, buf, buf2);

	if (!*buf || !*buf2 || !isdigit(*buf2))
	{
		ch->send("Usage: vstat { obj | mob } <number>\r\n");
		return;
	}

	if ((number = atoi(buf2)) < 0)
	{
		ch->send("A NEGATIVE number??\r\n");
		return;
	}

	if (IsAbbrev(buf, "mob"))
	{
		if ((r_num = MobManager::GetManager().RealMobile(number)) < 0)
		{
			ch->send("There is no monster with that number.\r\n");
			return;
		}

		mob = new Character(r_num, REAL);
		mob->MoveToRoom(0);
		do_stat_character(ch, mob);
		mob->Extract();
	}

	else if (IsAbbrev(buf, "obj"))
	{
		if ((r_num = real_object(number)) < 0)
		{
			ch->send("There is no object with that number.\r\n");
			return;
		}

		obj = read_object(r_num, REAL, false, false);
		do_stat_object(ch, obj);
		obj->Extract();
	}

	else
		ch->send("That'll have to be either 'obj' or 'mob'.\r\n");
}

/* clean a room of all mobiles and objects */
ACMD(do_purge)
{
	Character *vict, *next_v;
	Object *obj, *next_o;

	OneArgument(argument, buf);

	if (*buf)
	{
		if ((vict = get_char_room_vis(ch, buf)))
		{
			if (!IS_NPC(vict) && (GET_LEVEL(ch) <= GET_LEVEL(vict)) && !IS_KING(vict))
			{
				ch->send("Fuuuuuuuuu!\r\n");
				return;
			}
			if(IS_KING(vict))
			{
				ch->send("You can't strike the dictator! He is just much more... superb than you!\r\n");
				return;
			}
			Act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);

			if (!IS_NPC(vict))
			{
				MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s has purged %s.",
				       GET_NAME(ch), GET_NAME(vict));

				if (vict->desc)
				{
					STATE(vict->desc) = CON_CLOSE;
					vict->desc->character = NULL;
					vict->desc = NULL;
				}
			}

			vict->Extract(UserLogoutType::purge);
		}

		else if ((obj = get_obj_in_list_vis(ch, buf, ch->in_room->contents)))
		{
			Act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
			obj->Extract(true);
		}

		else
		{
			ch->send("Nothing here by that name.\r\n");
			return;
		}

		ch->send(OK);
	}

	else
	{			/* no argument. clean out the room */
		Act("$n gestures... You are surrounded by scorching flames!",
		    FALSE, ch, 0, 0, TO_ROOM);
		sendToRoom("The world seems a little cleaner.\r\n", ch->in_room);

		for (vict = ch->in_room->people; vict; vict = next_v)
		{
			next_v = vict->next_in_room;

			if (IS_NPC(vict))
				vict->Extract();
		}

		for (obj = ch->in_room->contents; obj;obj = next_o)
		{

			next_o = obj->next_content;

			if(obj && IS_OBJ_STAT(obj, ITEM_CHEST))
				continue;

			obj->Extract(true);
		}
	}
}

const char *logtypes[] =
    {
        "off",
        "brief",
        "normal",
        "complete",
        "\n"
    };

ACMD(do_syslog)
{
	int tp;

	OneArgument(argument, ::arg);

	if (!*::arg)
	{
		tp = ((PRF_FLAGGED(ch, PRF_LOG1) ? 1 : 0) +
		      (PRF_FLAGGED(ch, PRF_LOG2) ? 2 : 0));
		ch->send("Your syslog is currently %s.\r\n", logtypes[tp]);
		return;
	}

	if (((tp = search_block(::arg, ((const char **) logtypes), FALSE)) == -1))
	{
		ch->send("Usage: syslog { Off | Brief | Normal | Complete }\r\n");
		return;
	}

	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_LOG1);
	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_LOG2);

	if (tp & 1)
		SET_BIT_AR(PRF_FLAGS(ch), PRF_LOG1);
	if (tp & 2)
		SET_BIT_AR(PRF_FLAGS(ch), PRF_LOG2);

	ch->send("Your syslog is now %s.\r\n", logtypes[tp]);
}



ACMD(do_advance)
{
	Character *victim;
	char *name = ::arg, *level = buf2;
	int newlevel, oldlevel, i = 0;
	bool load = false;

	TwoArguments(argument, name, level);

	if(!*name)
	{
		ch->send("Advance who?\r\n");
		return;
	}

	if (!*level || (newlevel = atoi(level)) <= 0)
	{
		ch->send("That's not a level!\r\n");
		return;
	}

	if (newlevel > LVL_IMPL)
	{
		ch->send("%d is the highest possible level.\r\n", LVL_IMPL);
		return;
	}

	if (newlevel > GET_LEVEL(ch))
	{
		ch->send("Yeah, right.\r\n");
		return;
	}

	if(!(victim = get_char_vis(ch, name)))
	{
		if( (victim = CharacterUtil::loadCharacter(name)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
		{
			victim->loadManaRolls();
			victim->loadHitRolls();
			load = true;
		}
	}

	if (GET_LEVEL(ch) <= GET_LEVEL(victim))
	{
		ch->send("Maybe that's not such a great idea.\r\n");
		CLEANUP(victim, load);
	}
	if (IS_NPC(victim))
	{
		ch->send("NO!  Not on NPC's.\r\n");
		CLEANUP(victim, load);
	}
	if (newlevel == GET_LEVEL(victim))
	{
		ch->send("They are already at that level.\r\n");
		CLEANUP(victim, load);
	}

	oldlevel = GET_LEVEL(victim);

	if (newlevel < GET_LEVEL(victim))
	{
		ch->send("%s's level is higher than %d.\r\n", GET_NAME(victim), newlevel);
		CLEANUP(victim, load);
	}
	else
	{
		Act("You gain some levels thanks to some nice immortal!", FALSE, ch, 0, victim, TO_VICT);
	}

	ch->send(OK);

	for(i = GET_LEVEL(victim);i < newlevel;++i)
	{
		++GET_LEVEL(victim);
		victim->GainLevel(FALSE);
	}
	GET_EXP(victim) = level_exp(GET_LEVEL(victim));

	ch->send("You raise %s's level from %d to %d.%s\r\n", GET_NAME(victim), oldlevel, newlevel,
	         load ? " (Offline)" : "");
	victim->basicSave();
	victim->saveManaRolls();
	victim->saveHitRolls();

	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE,
	       "%s raised %s's level from %d to %d.%s", GET_NAME(ch), GET_NAME(victim), oldlevel, newlevel,
	       load ? " (Offline)" : "");

	CLEANUP(victim, load);
}

ACMD(do_restore)
{
	Character *vict;
	int i;

	OneArgument(argument, buf);

	if (!*buf)
		ch->send("Whom do you wish to restore?\r\n");

	else if (!(vict = get_char_vis(ch, buf)))
		ch->send(NOPERSON);

	else
	{
		GET_HIT(vict) = GET_MAX_HIT(vict);
		GET_MANA(vict) = GET_MAX_MANA(vict);
		GET_MOVE(vict) = GET_MAX_MOVE(vict);
		GET_SHADOW(vict) = GET_MAX_SHADOW(vict);

		if ((GET_LEVEL(ch) >= LVL_GRGOD) && (GET_LEVEL(vict) >= LVL_IMMORT))
		{
			for (i = 1; i <= MAX_SKILLS; i++)
				SET_SKILL(vict, i, 100);

			if (GET_LEVEL(vict) >= LVL_GOD)
			{
				vict->real_abils.intel = 25;
				vict->real_abils.wis = 25;
				vict->real_abils.dex = 25;
				vict->real_abils.str = 25;
				vict->real_abils.con = 25;
			}

			vict->aff_abils = vict->real_abils;
		}

		update_pos(vict);
		ch->send(OK);
		Act("You have been fully healed by $N!", FALSE, vict, 0, ch, TO_CHAR);

		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "%s restored %s's status points.",
		       GET_NAME(ch), GET_NAME(vict));

	}
}

void perform_immort_vis(Character *ch)
{
	if (GET_INVIS_LEV(ch) == 0 && !AFF_FLAGGED(ch, AFF_HIDE | AFF_INVISIBLE))
	{
		ch->send("You are already fully visible.\r\n");
		return;
	}

	GET_INVIS_LEV(ch) = 0;
	ch->Appear();
	ch->send("You are now fully visible.\r\n");
}


void perform_immort_invis(Character *ch, int level)
{
	Character *tch;

	if (IS_NPC(ch))
		return;

	for (tch = ch->in_room->people; tch; tch = tch->next_in_room)
	{

		if (tch == ch)
			continue;

		if (GET_LEVEL(tch) >= GET_INVIS_LEV(ch) && GET_LEVEL(tch) < level)
			Act("$n steps back into the shadows, and is gone.", FALSE, ch, 0,
			    tch, TO_VICT);

		if (GET_LEVEL(tch) < GET_INVIS_LEV(ch) && GET_LEVEL(tch) >= level)
			Act("You suddenly realize that $n is standing beside you.", FALSE, ch, 0,
			    tch, TO_VICT);
	}

	GET_INVIS_LEV(ch) = level;
	ch->send("Your invisibility level is %d.\r\n", level);
}

ACMD(do_invis)
{
	int level;

	if (IS_NPC(ch))
	{
		ch->send("You can't do that!\r\n");
		return;
	}

	OneArgument(argument, ::arg);

	if (!*::arg)
	{
		if (GET_INVIS_LEV(ch) > 0)
			perform_immort_vis(ch);

		else
			perform_immort_invis(ch, GET_LEVEL(ch));
	}

	else
	{
		level = atoi(::arg);

		if (level > GET_LEVEL(ch))
			ch->send("You can't go invisible above your own level.\r\n");

		else if (level < 1)
			perform_immort_vis(ch);
		else
			perform_immort_invis(ch, level);
	}
}

ACMD(do_gecho)
// EDITED BY STARK ON OCT 2003 TO TELL IMMS WHO DID WHAT GECHO
{
	Descriptor *pt;

	skip_spaces(&argument);
	delete_doubledollar(argument);

	if (!*argument)
		ch->send("That must be a mistake...\r\n");

	else
	{
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s has broadcasted the gecho: %s",
		       GET_NAME(ch), argument);

		sprintf(buf, "%s\r\n", argument);
		for (pt = descriptor_list; pt; pt = pt->next)
		{
			if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch)
				pt->send(buf);
		}
		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			ch->send(OK);
		else
			ch->send(buf);
	}
}

ACMD(do_poofset)
{
	skip_spaces(&argument);

	if(subcmd == SCMD_POOFIN)
		POOFIN(ch) = argument;
	else if(subcmd == SCMD_POOFOUT)
		POOFOUT(ch) = argument;
	else
		return;

	ch->send(OK);
}

ACMD(do_dc)
{
	Descriptor *d;
	int num_to_dc;

	OneArgument(argument, ::arg);

	if (!(num_to_dc = atoi(::arg)))
	{
		ch->send("Usage: DC <user number> (type USERS for a list)\r\n");
		return;
	}

	for (d = descriptor_list; d && d->desc_num != num_to_dc; d = d->next)
		;

	if (!d)
	{
		ch->send("No such connection.\r\n");
		return;
	}

	if (d->character && GET_LEVEL(d->character) >= GET_LEVEL(ch))
	{
		if (!CAN_SEE(ch, d->character))
			ch->send("No such connection.\r\n");
		else
			ch->send("Umm.. maybe that's not such a good idea...\r\n");

		return;
	}

	/* We used to just close the socket here using close_socket(), but
	 * various people pointed out this could cause a crash if you're
	 * closing the person below you on the descriptor list.  Just setting
	 * to CON_CLOSE leaves things in a massively inconsistent state so I
	 * had to add this new flag to the descriptor.
	 *
	 * It is a much more logical extension for a CON_DISCONNECT to be used
	 * for in-game socket closes and CON_CLOSE for out of game closings.
	 * This will retain the stability of the close_me hack while being
	 * neater in appearance. -gg 12/1/97
	 */
	STATE(d) = CON_DISCONNECT;
	ch->send("Connection #%d closed.\r\n", num_to_dc);
	Log("(GC) Connection closed by %s.", GET_NAME(ch));

	if(d->character)
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
		       "%s has closed %s's socket.", GET_NAME(ch), GET_NAME(d->character));
	else
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
		       "%s has closed socket %d.", GET_NAME(ch), d->desc_num);
}

ACMD(do_wizlock)
{
	int value;
	const char *when;

	OneArgument(argument, ::arg);

	if (*::arg)
	{
		value = atoi(::arg);

		if (value < 0 || value > GET_LEVEL(ch))
		{
			ch->send("Invalid wizlock value.\r\n");
			return;
		}

		circle_restrict = value;
		when = "now";
	}

	else
		when = "currently";

	switch (circle_restrict)
	{

		case 0:
			ch->send("The game is %s completely open.\r\n", when);
			break;

		case 1:
			ch->send("The game is %s closed to new players.\r\n", when);
			break;

		default:
			ch->send("Only level %d and above may enter the game %s.\r\n",
			         circle_restrict, when);
			break;
	}
	Conf->save();
}

ACMD(do_date)
{
	char *tmstr;
	time_t mytime;
	time_t d, h, m;

	if (subcmd == SCMD_DATE)
		mytime = time(0);

	else
		mytime = boot_time;

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	if (subcmd == SCMD_DATE)
		ch->send("Current machine time: %s\r\n", tmstr);

	else
	{
		mytime = time(0) - boot_time;
		d = mytime / 86400;
		h = (mytime / 3600) % 24;
		m = (mytime / 60) % 60;

		std::stringstream displayBuffer;
		displayBuffer << "Up since " << tmstr << ": " << d << " day" << ((d == 1) ? "" : "s") << ", " << h << ":" << m;
		ch->send("%s\r\n", displayBuffer.str().c_str());
	}
}

ACMD(do_last)
{
	Character *vict;
	std::list<pLogin> Logins;

	OneArgument(argument, ::arg);

	if (!*::arg)
	{
		ch->send("For whom do you wish to search?\r\n");
		return;
	}

	if(!playerExists(::arg))
	{
		ch->send("There is no such player.\r\n");
		return;
	}

	if( (vict = CharacterUtil::loadCharacter(::arg)) == NULL || !Character::LoadLogins(Logins,vict->player.name,1))
	{
		ch->send("Error loading player.");
		return;
	}
	else if ((GET_LEVEL(vict) > GET_LEVEL(ch)) && (GET_LEVEL(ch) < LVL_IMPL))
	{
		ch->send("You are not sufficiently godly enough for that!\r\n");
	}
	else if(!Logins.size())
	{
		ch->send("Player does not have any logins recorded.\r\n");
	}
	else
	{
		ch->send("[%5ld] %-12s : %-18s : %-12s : %-20s\r\n", vict->player.idnum, GET_NAME(vict),
			GET_LEVEL(ch) >= LVL_GRGOD ? Logins.front().Host.c_str() : "", Logins.front().gatewayDescriptorType->getStandardName().c_str(),
			Logins.front().time.toString().c_str() );
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "%s checked %s's last login.",
			GET_NAME(ch), GET_NAME(vict));
	}
	delete vict;
}

ACMD(do_force)
{
	Descriptor *i, *next_desc;
	Character *vict, *next_force;
	char to_force[MAX_INPUT_LENGTH + 2];

	HalfChop(argument, ::arg, to_force);

	sprintf(buf1, "$n has forced you to '%s'.", to_force);

	if (!*::arg || !*to_force)
		ch->send("Whom do you wish to force do what?\r\n");

	else if ((GET_LEVEL(ch) < LVL_GRGOD) || (str_cmp("all", ::arg) && str_cmp("room", ::arg) && str_cmp("t_all", ::arg)))
	{

		if (!(vict = get_char_vis(ch, ::arg)))
			ch->send(NOPERSON);

		else if (GET_LEVEL(ch) <= GET_LEVEL(vict))
			ch->send("No, no, no!\r\n");

		else
		{
			ch->send(OK);
			Act(buf1, TRUE, ch, NULL, vict, TO_VICT);

			MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
			       "(GC) %s forced %s to %s", GET_NAME(ch), GET_NAME(vict), to_force);

			CommandInterpreter(vict, to_force);
		}
	}

	else if (!str_cmp("room", ::arg))
	{
		ch->send(OK);

		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
			"(GC) %s forced room %d to %s", GET_NAME(ch), ch->in_room->getVnum(), to_force);


		std::list<Character*> lPeople = ch->in_room->getPeople();
		for(std::list<Character*>::iterator pIter = lPeople.begin();pIter != lPeople.end();++pIter)
		{
			vict = (*pIter);

			if (GET_LEVEL(vict) >= GET_LEVEL(ch))
				continue;

			Act(buf1, TRUE, ch, NULL, vict, TO_VICT);
			CommandInterpreter(vict, to_force);
		}
	}
	else if( !str_cmp(::arg, "all") )
	{
		ch->send(OK);
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s forced all to %s", GET_NAME(ch), to_force);
		for (i = descriptor_list; i; i = next_desc)
		{
			next_desc = i->next;

			if (STATE(i) != CON_PLAYING || !(vict = i->character) || GET_LEVEL(vict) >= GET_LEVEL(ch))
				continue;

			Act(buf1, TRUE, ch, NULL, vict, TO_VICT);
			CommandInterpreter(vict, to_force);
		}
	}
	else if( !str_cmp(::arg, "t_all") )
	{
		ch->send(OK);
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s forced all(mobs & players) to %s", GET_NAME(ch), to_force);
		for( Character *vict = character_list;vict;vict = vict->next )
		{
			if( GET_LEVEL(vict) >= GET_LEVEL(ch) ) continue;
			Act(buf1, TRUE, ch, NULL, vict, TO_VICT);
			CommandInterpreter(vict, to_force);
		}
	}
}

const char *gcommtypes[] =
    {
        "off",
        "colors",
        "flags",
        "all",
        "\n"
    };

/* Modeled after do_color in act.informative.cpp */
ACMD(do_gcomm)
{
	int tp;
	
	OneArgument(argument, ::arg);
	
	if (!*::arg)
	{
		ch->send("Your current gcomm preference is set to: %s.\r\n", gcommtypes[GCOMM_LEV(ch)]);
		return;
	}

	if (((tp = search_block(::arg, (const char **) gcommtypes, FALSE)) == -1))
	{
		ch->send("Usage: gcomm { Off | Colors | Flags | All }\r\n");
		return;
	}
	
	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_GCOMM_1);
	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_GCOMM_2);

	if (tp & 1)
		SET_BIT_AR(PRF_FLAGS(ch), PRF_GCOMM_1);
	if (tp & 2)
		SET_BIT_AR(PRF_FLAGS(ch), PRF_GCOMM_2);

	ch->send("Your gcomm preference is now set to %s%s%s.\r\n", COLOR_RED(ch, CL_SPARSE),
	         gcommtypes[tp], COLOR_NORMAL(ch, CL_OFF));
}

ACMD(do_wiznet)
{
	Descriptor *d;
	char emote = FALSE;
	char any = FALSE;
	int level = LVL_IMMORT;

	skip_spaces(&argument);
	delete_doubledollar(argument);

	if (!*argument)
	{
		ch->send(	"Usage: wiznet <text> | #<level> <text> | *<emotetext> |\r\n "
		          "       wiznet @@<level> *<emotetext> | wiz @@\r\n");
		return;
	}

	switch (*argument)
	{

		case '*':
			emote = TRUE;

		case '#':
			OneArgument(argument + 1, buf1);

			if (IsNumber(buf1))
			{
				HalfChop(argument+1, buf1, argument);
				level = MAX(atoi(buf1), LVL_IMMORT);

				if (level > GET_LEVEL(ch))
				{
					ch->send("You can't wizline above your own level.\r\n");
					return;
				}
			}

			else if (emote)
				++argument;

			break;

		case '@':

			for (d = descriptor_list; d; d = d->next)
			{
				if (STATE(d) == CON_PLAYING && GET_LEVEL(d->character) >= LVL_IMMORT &&
				        !PRF_FLAGGED(d->character, PRF_NOWIZ) &&
				        (CAN_SEE(ch, d->character) || GET_LEVEL(ch) == LVL_IMPL))
				{

					if (!any)
					{
						strcpy(buf1, "Gods online:\r\n");
						any = TRUE;
					}

					sprintf(buf1 + strlen(buf1), "  %s", GET_NAME(d->character));

					if (PLR_FLAGGED(d->character, PLR_WRITING))
						strcat(buf1, " (Writing)\r\n");

					else if (PLR_FLAGGED(d->character, PLR_MAILING))
						strcat(buf1, " (Writing mail)\r\n");

					else
						strcat(buf1, "\r\n");

				}
			}

			any = FALSE;

			for (d = descriptor_list; d; d = d->next)
			{
				if (STATE(d) == CON_PLAYING && GET_LEVEL(d->character) >= LVL_IMMORT &&
				        PRF_FLAGGED(d->character, PRF_NOWIZ) &&
				        CAN_SEE(ch, d->character))
				{

					if (!any)
					{
						strcat(buf1, "Gods offline:\r\n");
						any = TRUE;

					}
					sprintf(buf1 + strlen(buf1), "  %s\r\n", GET_NAME(d->character));
				}
			}

			ch->send(buf1);
			return;

		case '\\':
			++argument;
			break;

		default:
			break;
	}

	if (PRF_FLAGGED(ch, PRF_NOWIZ))
	{
		ch->send("You are offline!\r\n");
		return;
	}

	skip_spaces(&argument);


	if (!*argument)
	{
		ch->send("Don't bother the gods like that!\r\n");
		return;
	}

	if (level > LVL_IMMORT)
	{
		sprintf(buf1, "%s: <%d> %s%s\r\n", GET_NAME(ch), level,
		        emote ? "<--- " : "", argument);
		sprintf(buf2, "Someone: <%d> %s%s\r\n", level, emote ? "<--- " : "",
		        argument);
	}

	else
	{
		sprintf(buf1, "%s: %s%s\r\n", GET_NAME(ch), emote ? "<--- " : "",
		        argument);
		sprintf(buf2, "Someone: %s%s\r\n", emote ? "<--- " : "", argument);
	}
	CommManager::GetManager().SaveComm("wiznet", argument, ch, ch->in_room->getVnum(), level, ch->points.invis);

	for (d = descriptor_list; d; d = d->next)
	{

		if ((STATE(d) == CON_PLAYING) && (GET_LEVEL(d->character) >= level) &&
		        (!PRF_FLAGGED(d->character, PRF_NOWIZ)) &&
		        (!PLR_FLAGGED(d->character, PLR_WRITING) ||
		         !PLR_FLAGGED(d->character, PLR_MAILING))
		        && (d != ch->desc || !(PRF_FLAGGED(d->character, PRF_NOREPEAT))))
		{

			d->send(COLOR_CYAN(d->character, CL_NORMAL));

			if (CAN_SEE(d->character, ch))
				d->sendRaw(buf1);
			else
				d->sendRaw(buf2);

			d->send(COLOR_NORMAL(d->character, CL_NORMAL));
		}
	}

	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		ch->send(OK);
}

ACMD(do_zreset)
{
	unsigned int i;
	Zone *zone;

	OneArgument(argument, ::arg);

	if (!*::arg)
	{
		ch->send("You must specify a zone.\r\n");
		return;
	}

	if (*::arg == '*')
	{
		for (i = 0; i < ZoneManager::GetManager().NumZones();++i)
			ZoneManager::GetManager().GetZoneByRnum(i)->Reset();

		ch->send("Reset world.\r\n");
		MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s reset entire world.", GET_NAME(ch));
		return;
	}
	else if (*::arg == '.')
		zone = ZoneManager::GetManager().GetZoneByRnum(ch->in_room->getZoneNumber());
	else
		zone = ZoneManager::GetManager().GetZoneByVnum(atoi(::arg));

	if (zone)
	{
		zone->Reset();
		ch->send("Reset zone %d (#%d): %s.\r\n", zone->GetRnum(), zone->getVnum(), zone->getName().c_str());
		MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE,
		       "(GC) %s reset zone %d (%s)", GET_NAME(ch), zone->getVnum(), zone->getName().c_str());
	}
	else
		ch->send("Invalid zone number.\r\n");
}

ACMD(do_global_mute)
{
	Character *vict;
	char arg[MAX_INPUT_LENGTH];
	long result = 0;

	OneArgument(argument, arg);

	if(!*arg)
	{
		ch->send("But who are you wanting to do this to?\r\n");
		return;
	}

	if(!(vict = get_char_vis(ch, arg)))
	{
		ch->send("There is no player by that name.\r\n");
		return;
	}

	if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	{
		ch->send("That isn't such a good idea...\r\n");
		return;
	}

	result = PLR_TOG_CHK(vict, PLR_NOGLOBAL);
	MudLog(BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
	       "(GC) Global Mute %s for %s by %s.", ONOFF(result), GET_NAME(vict), GET_NAME(ch));

	if(result == TRUE)
	{
		ch->send("%s is now unable to use the global channel.\r\n", GET_NAME(vict));
		vict->send("You have lost your ability to use the global channel.\r\n");
	}
	else
	{
		ch->send("%s is now able to use the global channel.\r\n", GET_NAME(vict));
		vict->send("Your ability to use the global channel has been restored.\r\n");
	}
}

ACMD(do_tell_mute)
{
	Character *vict;
	char arg[MAX_INPUT_LENGTH];
	long result = 0;

	OneArgument(argument, arg);

	if(!*arg)
	{
		ch->send("But who are you wanting to do this to?\r\n");
		return;
	}

	if(!(vict = get_char_vis(ch, arg)))
	{
		ch->send("There is no player by that name.\r\n");
		return;
	}

	if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	{
		ch->send("That isn't such a good idea...\r\n");
		return;
	}

	result = PRF_TOG_CHK(vict, PRF_TELL_MUTE);
	MudLog(BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE,
	       "(GC) Tell Mute %s for %s by %s.", ONOFF(result), GET_NAME(vict), GET_NAME(ch));

	if(result == TRUE)
	{
		ch->send("%s is now mute to tells, and cannot send them.\r\n", GET_NAME(vict));
		vict->send("You have lost your ability to send tells.\r\n");
	}
	else
	{
		ch->send("%s is now able to use tells.\r\n", GET_NAME(vict));
		vict->send("Your ability to send and receive tells has been restored.\r\n");
	}
}

ACMD(do_reroll)
{
	Character *vict;
	char name[MAX_INPUT_LENGTH], type[MAX_INPUT_LENGTH];
	bool load = false;
	std::string type_name;

	TwoArguments(argument, name, type);

	if(!argument || !*name || !*type)
	{
		ch->send("You can reroll: Moves, Hit, Mana, Stats, Height, Weight(Height/Weight rolls both), or Shadow.\r\n");
		return;
	}

	if(!(vict = get_char_vis(ch, name)))
	{
		if( (vict = CharacterUtil::loadCharacter(name)) == NULL )
		{
			ch->send(NOPERSON);
			return;
		}
		else
			load = true;
	}

	if(!strn_cmp(type, "stats", strlen(type)))
	{
		type_name = "stats";
		ch->send("Stats rerolled...\r\n");
		StatManager::GetManager().RollStats( vict );

		MudLog(NRM, MAX(LVL_BLDER, GET_INVIS_LEV(ch)), TRUE,
		       "(GC) %s has rerolled %s's stats.", GET_NAME(ch), GET_NAME(vict));

		ch->send("New stats: Str %d, Int %d, Wis %d, Dex %d, Con %d.\r\n",
		         vict->GetStr(), vict->GetInt(), vict->GetWis(),
				 vict->GetDex(), vict->GetCon());
	}

	else if(!strn_cmp(type, "moves", strlen(type)))
	{
		type_name = "moves";
		ch->send("Moves rerolled...\r\n");
		vict->RollMoves();

		MudLog(NRM, MAX(LVL_BLDER, GET_INVIS_LEV(ch)), TRUE,
		       "(GC) %s has rerolled %s's moves.", GET_NAME(ch), GET_NAME(vict));

        ch->send("Rerolled moves to %d.\r\n", (int) vict->BaseMoves());
	}
	else if(!strn_cmp(type, "hit", strlen(type)))
	{
		vict->loadHitRolls();
		type_name = "health";
		ch->send("Hit Points re-rolled...\r\n");
		vict->ResetHitRolls(true);

        ch->send("New max hit: %d.\r\n", (int) vict->points.max_hit);
		vict->saveHitRolls();
	}

	else if(!strn_cmp(type, "mana", strlen(type)))
	{
		vict->loadManaRolls();
		type_name = "spell points";
		ch->send("Spell Points(mana) rerolled...\r\n");
		vict->ResetManaRolls(true);

        ch->send("New max spell points: %d.\r\n", (int) vict->points.max_mana);
		vict->saveManaRolls();
	}
	else if(!strn_cmp(type, "height", strlen(type)) || !strn_cmp(type, "weight", strlen(type)))
	{
		type_name = "height and weight";
		ch->send("Height and Weight rerolled...\r\n");
		vict->RollWeightAndHeight();

		ch->send("New Height: %s, Weight: %d\r\n", vict->HeightToFeetAndInches().c_str(), GET_WEIGHT(ch));
	}
	else if(!strn_cmp(type, "shadow", strlen(type)))
	{
		type_name = "shadow";
		ch->send("Shadow Points re-rolled...\r\n");
		vict->rollShadow();
		MudLog(NRM, MAX(LVL_BLDER, GET_INVIS_LEV(ch)), TRUE,
		       "(GC) %s has rerolled %s's shadow points.", GET_NAME(ch), GET_NAME(vict));

        ch->send("Rerolled shadow points to %d.\r\n", (int) GET_MAX_SHADOW(vict));
	}
	else
	{
		ch->send("Possible reroll options: Moves, Hit, Mana, Stats, Shadow.\r\n");
		if(load)
			delete vict;
		return;
	}

	MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, "%s rerolled %s's %s.%s",
	       GET_NAME(ch), GET_NAME(vict), type_name.c_str(), load ? "(offline)" : "");

	vict->basicSave();

	if(load)
		delete vict;

	return;
}

ACMD(do_wizutil)
{
	Character *vict;
	bool load = false;
	long result;

	OneArgument(argument, ::arg);

	if (!*::arg)
	{
		ch->send("Yes, but for whom?!?\r\n");
		return;
	}
	else if (!(vict = get_char_vis(ch, ::arg)))
	{
		if( (vict = CharacterUtil::loadCharacter(::arg)) == NULL )
		{
			ch->send("There is no such player.\r\n");
			return;
		}
		else
			load = true;
	}
	if (IS_NPC(vict))
		ch->send("You can't do that to a mob!\r\n");
	else if (GET_LEVEL(vict) > GET_LEVEL(ch))
		ch->send("Hmmm...you'd better not.\r\n");
	else
	{
		switch (subcmd)
		{
			case SCMD_NOTITLE:
				result = PLR_TOG_CHK(vict, PLR_NOTITLE);
				sprintf(buf, "(GC) Notitle %s for %s by %s.", ONOFF(result),
				        GET_NAME(vict), GET_NAME(ch));
				MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, buf);
				strcat(buf, "\r\n");
				ch->send(buf);
				break;
			case SCMD_SQUELCH:
			{
				result = PLR_TOG_CHK(vict, PLR_NOSHOUT);
				sprintf(buf, "(GC) Squelch %s for %s by %s.", (result?"ON":"OFF"),
				        GET_NAME(vict), GET_NAME(ch));
				MudLog(BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, buf);
				strcat(buf, "\r\n");
				ch->send(buf);
				break;
			}
			case SCMD_FREEZE:

				if (ch == vict)
				{
					ch->send("Oh, yeah, THAT'S real smart...\r\n", ch);
					return;
				}
				if (PLR_FLAGGED(vict, PLR_FROZEN))
				{
					ch->send("Your victim is already pretty cold.\r\n", ch);
					return;
				}
				SET_BITK(PLR_FLAGS(vict), Q_BIT(PLR_FROZEN));
				vict->PlayerData->freeze_level = GET_LEVEL(ch);
				vict->send("A bitter wind suddenly rises and drains the heat from your body!\r\nYou feel frozen!\r\n");
				ch->send("Frozen.\r\n");
				Act("A sudden cold wind conjured from nowhere freezes $n!", FALSE, vict, 0, 0, TO_ROOM);
				MudLog(BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s frozen by %s.",
				       GET_NAME(vict), GET_NAME(ch));
				break;
			case SCMD_THAW:

				if (!PLR_FLAGGED(vict, PLR_FROZEN))
				{
					ch->send("Sorry, your victim is not morbidly encased in ice at the moment.\r\n");
					return;
				}

				if (vict->PlayerData->freeze_level > GET_LEVEL(ch))
				{
					sprintf(buf, "Sorry, a level %d God froze %s... you can't unfreeze %s.\r\n",
						vict->PlayerData->freeze_level, GET_NAME(vict), HMHR(vict));
					ch->send(buf);
					return;
				}

				MudLog(BRF, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "(GC) %s un-frozen by %s.",
				       GET_NAME(vict), GET_NAME(ch));
				REMOVE_BIT(PLR_FLAGS(vict), Q_BIT(PLR_FROZEN));
				vict->send("A fireball suddenly explodes in front of you, melting the ice!\r\nYou feel thawed.\r\n");
				ch->send("Thawed.\r\n");
				Act("A sudden fireball conjured from nowhere thaws $n!", FALSE, vict, 0, 0, TO_ROOM);
				break;
			case SCMD_UNAFFECT:

				if (vict->affected)
				{
					while (vict->affected)
						affect_remove(vict, vict->affected);
					vict->points.temp_taint = 0;
					vict->SlowedBy = 0;

					vict->send(	"There is a brief flash of light!\r\n"
					            "You feel slightly different.\r\n");
					ch->send("All spells removed.\r\n");
				}
				else
				{
					ch->send("Your victim does not have any affections!\r\n");
					return;
				}

				break;
			case SCMD_ZONE_BAN:

				if(GET_LEVEL(vict) < LVL_IMMORT || GET_LEVEL(vict) >= LVL_IMPL)
				{
					ch->send("The level range must be between 100 and 104.\r\n");
					return;
				}

				if(!PLR_FLAGGED(vict, PLR_ZONE_BAN))
				{
					ch->send("%s now cannot leave %s zones.\r\n", GET_NAME(vict), HSHR(vict));
					SET_BITK(PLR_FLAGS(vict), Q_BIT(PLR_ZONE_BAN));
					MudLog(BRF, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE,
					       "%s has zone banned %s from leaving %s zones.", GET_NAME(ch), GET_NAME(vict), HSHR(vict));
				}
				else
				{
					ch->send("%s can now leave %s zones.\r\n", GET_NAME(vict), HSHR(vict));
					REMOVE_BIT(PLR_FLAGS(vict), Q_BIT(PLR_ZONE_BAN));
					MudLog(BRF, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE,
					       "%s has removed %s's zone ban.", GET_NAME(ch), GET_NAME(vict));
				}

				break;
			default:
				Log("SYSERR: Unknown subcmd %d passed to do_wizutil (%s)", subcmd, __FILE__);
				break;
		}

		vict->basicSave();
		if(load)
			delete vict;
	}
}

struct BackupFileNameFunctor
{
	bool operator ()( const std::string &sFileNameA, const std::string &sFileNameB )
	{
		try {
			int iMonthA, iYearA, iDayA, iMonthB, iYearB, iDayB;
			std::string sDateA, sDateB;
			std::vector< std::string > vChunksA = StringUtil::SplitToContainer< std::vector< std::string >, std::string >(sFileNameA, '.');
			sDateA = vChunksA[ 1 ];

			vChunksA = StringUtil::SplitToContainer< std::vector< std::string >, std::string >(sDateA, '-');
			iYearA  = atoi( vChunksA[ 0 ].c_str() );
			iMonthA = atoi( vChunksA[ 1 ].c_str() );
			iDayA   = atoi( vChunksA[ 2 ].c_str() );

			std::vector< std::string > vChunksB = StringUtil::SplitToContainer< std::vector< std::string >, std::string >(sFileNameB, '.');
			sDateB = vChunksB[ 1 ];

			vChunksB = StringUtil::SplitToContainer< std::vector< std::string >, std::string >(sDateB, '-');
			iYearB  = atoi( vChunksB[ 0 ].c_str() );
			iMonthB = atoi( vChunksB[ 1 ].c_str() );
			iDayB   = atoi( vChunksB[ 2 ].c_str() );

			if( iYearA > iYearB ) return false;
			if( iYearA < iYearB ) return true;
			
			if( iMonthA > iMonthB ) return false;
			if( iMonthA < iMonthB ) return true;

			if( iDayA > iDayB ) return false;
			if( iDayA < iDayB ) return true;

			//Same...
			return true;

		} catch( std::out_of_range e ) {
			return false;
		}
	}
};

ACMD(do_wshow)
{
	int j, k, l, con, i;
	char self = 0;
	Character *vict;
	Object *obj;
	char field[MAX_INPUT_LENGTH], value[MAX_INPUT_LENGTH], birth[80], arg3[MAX_INPUT_LENGTH],
		arg4[MAX_INPUT_LENGTH], arg5[MAX_INPUT_LENGTH];
	std::string Sys;

	struct show_struct
	{
		const char *cmd;
		int level;
	};

	struct show_struct fields[] =
	    {
		    { "nothing",		LVL_IMMORT	},			// 0
		    { "zones",			LVL_IMMORT	},			// 1
		    { "player",			LVL_GOD		},
		    { "rent",			LVL_GOD		},
		    { "stats",			LVL_IMMORT	},
		    { "errors",			LVL_GOD		},			// 5
		    { "death",			LVL_GOD		},
		    { "godrooms",		LVL_GOD		},
		    { "shops",			LVL_IMMORT	},
		    { "houses",			LVL_GOD		},
		    { "buffers",		LVL_GOD		},			// 10
		    { "skills",			LVL_GRGOD	},
		    { "descriptions",	LVL_IMMORT	},
		    { "zoneperms",		LVL_GOD		},
		    { "spells"	,		LVL_GRGOD	},
		    { "tells",			LVL_GRGOD   },			// 15
			{ "logins",			LVL_GRGOD	},
			{ "accounts",		LVL_GOD		},
			{ "logs",			LVL_IMMORT	},
			{ "backups",		LVL_GOD		},
			{ "switches",		LVL_GRGOD	},			// 20
			{ "lag",			LVL_GRGOD	},
			{ "userdeletions",	LVL_GRGOD	},
			{ "userrestores",	LVL_GRGOD	},
			{ "qptransactions",	LVL_GOD		},			// 24
		    { "\n",				0			}
	    };

	skip_spaces(&argument);

	if (!*argument)
	{
		strcpy(buf, "Show options:\r\n");

		for (j = 0, i = 1; fields[i].level; ++i)
		{
			if (fields[i].level <= GET_LEVEL(ch))
			{
				sprintf(buf + strlen(buf), "%-15s%s", fields[i].cmd, (!(++j % 5) ? "\r\n" : ""));
			}
		}
		strcat(buf, "\r\n");
		ch->send(buf);
		return;
	}

	strcpy(::arg, OneArgument(TwoArguments(TwoArguments(argument, field, value), arg3, arg4), arg5));

	for (l = 0; *(fields[l].cmd) != '\n'; ++l)
	{
		if (!strncmp(field, fields[l].cmd, strlen(field)))
		{
			break;
		}
	}
	if (GET_LEVEL(ch) < fields[l].level)
	{
		ch->send("You are not godly enough for that!\r\n");
		return;
	}

	if (!strcmp(value, "."))
		self = 1;

	buf[0] = '\0';

	switch (l)
	{

		case 1:			/* zone */
		{
			std::string Buffer;
			Zone *zone;
			char *cBuffer;
			if (self)
			{
				Zone *zone = ZoneManager::GetManager().GetZoneByRnum(ch->in_room->getZoneNumber());
				zone->PrintToBuffer(Buffer);
			}
			else if (*value && IsNumber(value))
			{
				if( (zone = ZoneManager::GetManager().GetZoneByVnum(atoi(value))) != NULL )
					zone->PrintToBuffer(Buffer);
				else
					Buffer = "That is not a valid zone.\r\n";
			}
			else
			{
				for (i = 0; (unsigned int)i < ZoneManager::GetManager().NumZones(); ++i)
				{
					Zone *zone = ZoneManager::GetManager().GetZoneByRnum(i);
					if( !(*value) || (!str_cmp(value, "closed") && zone->IsClosed())
						||(!str_cmp(value, "open") && !zone->IsClosed()))
					{
						zone->PrintToBuffer(Buffer);
					}
				}
			}
			cBuffer = new char[Buffer.size() + 1];
			strcpy(cBuffer, Buffer.c_str());
			page_string(ch->desc, cBuffer, TRUE);
			delete[] cBuffer;

			break;
		}
		case 2:			/* player */
		{
			if (!*value)
			{
				ch->send("A name would help.\r\n");
				return;
			}

			if( (vict = CharacterUtil::loadCharacter(value)) == NULL)
			{
				ch->send("There is no such player.\r\n");
				return;
			}
			if( vict->player.level > ch->player.level )
			{
				ch->send("You can't\r\n");
				return;
			}

			sprintf(buf, "Player: %-12s (%s) [%2d %s]\r\n", GET_NAME(vict),
			        genders[(int) GET_SEX(vict)], GET_LEVEL(vict), class_abbrevs[(int) GET_CLASS(vict)]);

			sprintf(buf+strlen(buf),
			        "Au: %-8lu  Bal: %-8d  Exp: %-8d  Practices: %-3d  Spell Practices: %-3d\r\n",
			        vict->Gold(), (int) GET_BANK_GOLD(vict), (int) GET_EXP(vict),
			        vict->PlayerData->skillpracs, vict->PlayerData->spellpracs);

			strcpy(birth, vict->player.time.birth.toString().c_str());

			sprintf(buf+strlen(buf),
			        "Started: %-20.16s  Last: %-20.16s  Played: %3dh %2dm\r\n",
			        birth, ctime(&ch->points.last_logon), (int) (vict->player.time.played / 3600),
			        (int) (vict->player.time.played / 60 % 60));

			ch->send(buf);

			delete vict;
			break;
		}
		case 3:
			crashListRent(ch, value);
			break;

		case 4:
		{
			i = 0;
			j = 0;
			k = 0;
			con = 0;


#if (defined CIRCLE_WINDOWS)

			Sys = "Windows";
#elif (defined CIRCLE_UNIX)

			Sys = "Unix";
#elif (defined CIRCLE_MACINTOSH)

			Sys = "Macintosh";
#endif

			for (vict = character_list; vict; vict = vict->next)
			{
				if( vict->IsPurged() ) continue;
				if (IS_NPC(vict))
					++j;

				else if (CAN_SEE(ch, vict))
				{
					++i;

					if (vict->desc)
						++con;
				}
			}

			for (obj = object_list; obj; obj = obj->next)
				k++;

			strcpy(buf, "Current stats:\r\n");
			sprintf(buf + strlen(buf), "Kinslayer is running on a %s system.\r\n", Sys.c_str());
			sprintf(buf + strlen(buf), "  %5d players in game  %5d connected\r\n",
			        i, con);

			sprintf(buf + strlen(buf), "  %5d registered      %5d is the top user ID\r\n",
			        PlayerBaseCount(), topUserId());

			sprintf(buf + strlen(buf), "  %5d mobiles          %5d prototypes\r\n",
				j, MobManager::GetManager().NumberOfPrototypes());

			sprintf(buf + strlen(buf), "  %5d objects          %5d prototypes\r\n",
			        k, top_of_objt + 1);

			sprintf(buf + strlen(buf), "  %5d rooms            %5d zones\r\n",
				(int)World.size(), (int)ZoneManager::GetManager().NumZones());

			sprintf(buf + strlen(buf), " Database: %s\r\n", (gameDatabase->isConnected() ? "Connected" : "Not Connected"));

			sprintf(buf + strlen(buf), " Web Socket Server: %s, Port: %d, Descriptors: %u\r\n", (game->playerPortalServerIsConnected() ? "Connected" : "Not Connected"), game->getPlayerPortalPort(), game->getNumberOfPlayerPortalDescriptors());
			sprintf(buf + strlen(buf), " Switch: %s\r\n", Conf->play.switch_restriction ? "Restricted" : "Not Restricted");
			sprintf(buf + strlen(buf), " Flusspferd Garbage Collections: %lld\r\n", JSManager::get()->getGC_Count());

			int iSecondsSinceBoot = time( 0 ) - bootTime;
			float fPulsesPerSec = (float)pulse / (float)iSecondsSinceBoot;

			sprintf(buf + strlen(buf), "  Average pulses per second : %.2f / %.2f\r\n", fPulsesPerSec, (float)PASSES_PER_SEC);

#ifndef WIN32
			sprintf(buf + strlen(buf), "  Remaining system memory : %.2f MB\r\n", (float)AvailableSystemMemory() / 1024 / 1024);
#endif
			ch->send(buf);
			break;
		}
		case 5:
			strcpy(buf, "Errant Rooms\r\n------------\r\n");

			for (i = 0, k = 0; i < (int)World.size(); ++i)
			{
				for (j = 0; j < NUM_OF_DIRS; j++)
				{
					if (World[i]->dir_option[j] && World[i]->dir_option[j]->getToRoom() == 0)
						sprintf(buf + strlen(buf), "%2d: [%5d] %s\r\n", ++k, World[i]->getVnum(), World[i]->getName());
				}
			}
			page_string(ch->desc, buf, TRUE);
			break;

		case 6:
			strcpy(buf, "Death Traps\r\n-----------\r\n");

			for (i = 0, j = 0;i < (int)World.size(); ++i)
			{
				if (ROOM_FLAGGED( (World[i]), ROOM_DEATH))
				{
					sprintf(buf + strlen(buf), "%2d: [%5d] %s\r\n", ++j, World[i]->getVnum(), World[i]->getName());
				}
			}
			page_string(ch->desc, buf, TRUE);
			break;

		case 7:
			strcpy(buf, "Godrooms\r\n--------------------------\r\n");

			for (i = 0, j = 0; (unsigned int)i < World.size(); ++i)
			{
				if (ROOM_FLAGGED( (World[i]), ROOM_GODROOM))
				{
					sprintf(buf + strlen(buf), "%2d: [%5d] %s\r\n", ++j, World[i]->getVnum(), World[i]->getName());
				}
			}
			page_string(ch->desc, buf, TRUE);
			break;

		case 8:
			show_shops(ch, value);
			break;

		case 9:
			ch->send("This has been removed.\r\n");
			break;

		case 10:
			ch->send("This has been removed.\r\n");
			break;
		case 11:

			if (!*value)
			{
				ch->send("A name would help.\r\n");
				return;
			}

			if( (vict = CharacterUtil::loadCharacter(value)) == NULL )
			{
				ch->send("There is no such player.\r\n");
				return;
			}
			vict->loadSkills();

			ch->send("%s knows the following skills, has %d practices and %d Spell Practices:\r\n",
			         GET_NAME(vict), vict->PlayerData->skillpracs, vict->PlayerData->spellpracs);

			for(i = 0;i < MAX_SKILLS;++i)
			{
				if(GET_SKILL(vict, i) && WeaveManager::GetManager().GetWeave(i))
					ch->send("%s: %d\r\n", WeaveManager::GetManager().GetWeaveName(i).c_str(), GET_SKILL(vict, i));
			}
			break;
		case 12:

			if( atoi(arg3) < 0 )
			{
				ch->send("You can only submit a positive number of lines.\r\n");
				return;
			}

			ch->send("Rooms with %d lines: \r\n", atoi(arg3));
			for(j = 0, i = 0;i < (int)World.size();++i)
			{
				Zone *zone = World[i]->getZone();
				if( (!strn_cmp(value, "all", strlen(value)) || (unsigned int)atoi(value) == zone->getVnum())
				&& World[i]->getLinesInDescription() == atoi(arg3))
				{
					if(j)
						ch->send(", ");
					if(!(j % 10))
						ch->send("\r\n");
					if( !World[i]->getZone()->IsClosed() )
						ch->send("%s%s", COLOR_BOLD(ch, CL_COMPLETE), COLOR_GREEN(ch, CL_COMPLETE));
					ch->send("%d", World[i]->getVnum());
					if( !World[i]->getZone()->IsClosed() )
						ch->send("%s", COLOR_NORMAL(ch, CL_COMPLETE));
					++j;
				}
			}
			break;
		case 13:
			vict = NULL;
			if( (vict = CharacterUtil::loadCharacter(value)) == NULL )
				ch->send(NOPERSON);
			else if(vict->player.level > GET_LEVEL(ch))
				ch->send("You aren't big brother... yet...\r\n");
			else
				vict->ShowZones(ch);

			if(vict)
				delete vict;
			break;
		case 14:
			ch->send("Weaves for Kinslayer:\r\n");
			ch->send("Weave Name          Earth   Fire   Water   Air   Spirit\r\n");
			ch->send(WeaveManager::GetManager().ListWeaves(2).c_str());
			break;
		case 15:
			ch->send("Removed.");//Unused as of 06/02/2010
			break;
		case 16:
		{
			std::list<pLogin> Logins;
			std::list<pLogin>::iterator it;
			std::string PlayerName;
			int iLimit=-1;
			time_t tLow=-1,tHigh=-1;

			if( playerExists(value) )
				PlayerName=value;
			else {
				strcpy(arg5, arg4);
				strcpy(arg4, arg3);
				strcpy(arg3, value);
			}

			if(!strn_cmp(arg3, "last", strlen(arg3)))
				iLimit = atoi(arg4);
			else if(!strn_cmp(arg3, "range", 5))
			{
			}
			else if(!strn_cmp(arg3, "during", 6))
			{
			}
			else if(PlayerName.empty() == true)
			{
				ch->send(NOPERSON);
				return;
			}
			else
			{
				ch->send("Usage: show logins <playername(?)> <last|during|range> <arguments>\r\n");
				return;
			}
			ch->send("      Login Time                Host        Socket Type    Player Name\r\n");
			ch->send("----------------------------------------------------------------------\r\n");
			//We should have all the parameters at this point. Now grab the logins.
			Character::LoadLogins(Logins, PlayerName, iLimit, tLow, tHigh);

			/*** Galnor 06/30/2009 - Changed to use a dynamically allocated buffer to prevent overflow ***/
			const int iBufferSizeLimit=(MAX_STRING_LENGTH*10);
			char *cBuffer = new char[iBufferSizeLimit];
			(*cBuffer) = ('\0');
			i = 0;
			for(std::list<pLogin>::iterator it = Logins.begin();it != Logins.end();it++)
			{
				std::string ThisLine = (*it).Print();
				if( strlen(cBuffer) + ThisLine.size() >= iBufferSizeLimit )
					break;
				strcat(cBuffer, ThisLine.c_str());
			}
			page_string(ch->desc, cBuffer, TRUE);
			delete[] cBuffer;//Free the buffer.
			break;
		}
		case 17:
		{
			if( !*value )
			{//List all accounts.
			}
			else
			{//List account for certain player.
				Account a = SwitchManager::GetManager().GetAccountByPlayerName( value );
				if( !a.IsValid() )
					ch->send("%s does not belong to an account.\r\n", StringUtil::cap(value));
				else {
					std::string playerName = StringUtil::cap(value);
					std::string accountName = StringUtil::cap(a.getName());
					ch->send("%s's account is %s.\r\n", playerName.c_str(), accountName.c_str());
				}
			}
			break;
		}
		case 18:
		{
			get_char_cols( ch );
			sql::Query MyQuery;
			std::stringstream sBuf;
			try {
				sBuf	<< "SELECT c.*,u.username "
						<< "FROM comm c LEFT JOIN users u ON u.user_id=c.sender_id "
						<< "WHERE c.sender_type='C' "
						<< "AND c.type='wiznet' "
						<< "AND c.recipient_id<='" << (int)(ch->player.level) << "' "
						<< "ORDER BY c.timestamp DESC LIMIT 500";
				MyQuery = gameDatabase->sendQuery( sBuf.str() );
			} catch( sql::QueryException e ) {
				e.report();
				return;
			}

			sBuf.str("");
			int iBufferLimit = 1024*1024;
			sBuf << "  #          Time                Sender                Message" << std::endl;
			sBuf << "---------------------------------------------------------------------" << std::endl;
			for(int i = 1; MyQuery->hasNextRow();++i )
			{
				sql::Row MyRow = MyQuery->getRow();

				sBuf << std::setw(4) << std::right << i;
				sBuf << "    ";
				sBuf << Time::FormatDate("%b %d, %Y %H:%M:%S", MiscUtil::Convert<time_t>(MyRow["timestamp"]));
				sBuf << "    ";
				sBuf << grn << bld << std::setw(16) << std::left
					<< (ch->player.level >= MiscUtil::Convert<int>(MyRow["invis_level"]) ? MyRow["username"] : "Someone") << nrm;
				sBuf << "    ";
				sBuf << cyn << MyRow["message"] << nrm;
				sBuf << std::endl;
			}
			char *cBuf = new char[ sBuf.str().size() + 1 ];
			strcpy(cBuf, sBuf.str().c_str());
			page_string(ch->desc, cBuf, TRUE);
			delete[] cBuf;

			break;
		}
		case 19:
		{
		   std::string sBackupDirectory = "/kinslayer/cache/";
		   //if( !std::tr2::sys::exists( std::tr2::sys::path(sBackupDirectory) ) ) {
			if (!boost::filesystem::exists(sBackupDirectory)) {
				ch->send("The directory holding the MUD backups could not be found.\r\n");
				return;
			}
			std::list< std::string > lBackupFileNames;
			GetDirectoryList( sBackupDirectory, lBackupFileNames, ".tar.bz2" );
			BackupFileNameFunctor backupFileNameFunctor;
			lBackupFileNames.sort( backupFileNameFunctor );
			for(std::list<std::string>::iterator fIter = lBackupFileNames.begin();fIter != lBackupFileNames.end();++fIter)
			{
				//kmud\.\d{4}-\d\d-\d\d\.bak\.tar\.bz2
				get_char_cols(ch);
				ch->send(" - %s%s%s%s\r\n", bld, grn, StripFilePath((*fIter)).c_str(), nrm);
			}
			break;
		}
		case 20:
		{
			std::stringstream SwitchBuffer;
			SwitchManager::GetManager().PrintSwitchesToBuffer( SwitchBuffer );

			ch->send( StringUtil::vaEscape(SwitchBuffer.str()) );
			break;
		}
		case 21:
		{
			std::stringstream buffer;
			buffer << "Name                               Times Run    Total Time      Average" << std::endl;
			buffer << "-----------------------------------------------------------------------" << std::endl;

			for(std::map<std::string, LagRoutineEntry>::iterator iter = lagMonitor.mRoutineTimeCard.begin();iter != lagMonitor.mRoutineTimeCard.end();++iter)
			{
				LagRoutineEntry entry = (*iter).second;

				double average = (entry.numberOfTimesRun ? (entry.runTime / entry.numberOfTimesRun) : 0);
				buffer << std::setw(30) << std::left << (*iter).first << " - " << std::setw(8) << std::right << entry.numberOfTimesRun << ",   "
					<< std::setprecision(3) << std::setw(11) << std::showpoint << std::fixed << std::right << entry.runTime
					<< ",  " << std::setw(11) << std::setprecision(3) << std::showpoint << std::fixed << std::right << (average) << std::endl;
			}
			ch->send("%s", buffer.str().c_str());
			break;
		}
		case 22:
		{//User Deletions
			std::stringstream sql;
			
			sql <<	" SELECT"
				<<	"   userDeletion.id,"
				<<	"   userDeletion.user_id,"
				<<	"   userDeletion.created_datetime,"
				<<	"   users.username AS users_username,"
				<<	"   userArchive.username AS userArchive_username"
				<<	" FROM userDeletion"
				<<	" LEFT JOIN users ON users.user_id = userDeletion.user_id"
				<<	" LEFT JOIN userArchive ON userArchive.user_id = userDeletion.user_id"
				<<	" ORDER BY userDeletion.id DESC";

			sql::Query query = gameDatabase->sendQuery(sql.str());
			sql::Row row;
			std::stringstream outputBuffer;
			get_char_cols(ch);
			
			outputBuffer << "ID       User ID     Username               Restored Datetime      Current Status" << std::endl;
			outputBuffer << "---------------------------------------------------------------------------------" << std::endl;

			while(query->hasNextRow()) {

				row = query->getRow();

				int userId = row.getInt("user_id");
				int id = row.getInt("id");
				DateTime createdDatetime = DateTime(row.getTimestamp("created_datetime"));
				std::string username;
				std::string currentStatus;

				if(!row.isFieldNull("users_username"))
				{
					username = row.getString("users_username");
					currentStatus = "Active";
				}
				else if(!row.isFieldNull("userArchive_username"))
				{
					username = row.getString("userArchive_username");
					currentStatus = "Deleted";
				}
				else
				{
					username = "<Unknown>";
					currentStatus = "Uknown";
				}

				outputBuffer << std::setw(6)  << std::right << id << "   ";
				outputBuffer << std::setw(8)  << std::right << userId << "   ";
				outputBuffer << std::setw(21) << std::left  << username << "   ";
				outputBuffer << DateTime::parse("%Y-%m-%d %H:%M:%S", createdDatetime) << "     ";
				outputBuffer << bld << (currentStatus == "Deleted" ? mag : cyn) << currentStatus << std::left << nrm << std::endl;
			}

			char *outputBufferCString = new char[ outputBuffer.str().size() + 1 ];
			strcpy(outputBufferCString, outputBuffer.str().c_str());
			page_string(ch->desc, outputBufferCString, TRUE);
			delete[] outputBufferCString;
			break;
		}
		case 23:
		{//User Restores
			std::stringstream sql;
			
			sql <<	" SELECT"
				<<	"   userRestore.id,"
				<<	"   userRestore.user_id,"
				<<	"   userRestore.created_datetime,"
				<<	"   users.username AS users_username,"
				<<	"   userArchive.username AS userArchive_username"
				<<	" FROM userRestore"
				<<	" LEFT JOIN users ON users.user_id = userRestore.user_id"
				<<	" LEFT JOIN userArchive ON userArchive.user_id = userRestore.user_id"
				<<	" ORDER BY userRestore.id DESC";

			sql::Query query = gameDatabase->sendQuery(sql.str());
			sql::Row row;
			std::stringstream outputBuffer;
			get_char_cols(ch);

			outputBuffer << "ID       User ID     Username               Restored Datetime      Current Status" << std::endl;
			outputBuffer << "---------------------------------------------------------------------------------" << std::endl;

			while(query->hasNextRow()) {

				row = query->getRow();

				int userId = row.getInt("user_id");
				int id = row.getInt("id");
				DateTime createdDatetime = DateTime(row.getTimestamp("created_datetime"));
				std::string username;
				std::string currentStatus;

				if(!row.isFieldNull("users_username"))
				{
					username = row.getString("users_username");
					currentStatus = "Active";
				}
				else if(!row.isFieldNull("userArchive_username"))
				{
					username = row.getString("userArchive_username");
					currentStatus = "Deleted";
				}
				else
				{
					username = "<Unknown>";
					currentStatus = "Uknown";
				}
				
				outputBuffer << std::setw(6)  << std::right << id << "   ";
				outputBuffer << std::setw(8)  << std::right << userId << "   ";
				outputBuffer << std::setw(21) << std::left  << username << "   ";
				outputBuffer << DateTime::parse("%Y-%m-%d %H:%M:%S", createdDatetime) << "     ";
				outputBuffer << bld << (currentStatus == "Deleted" ? mag : cyn) << currentStatus << std::left << nrm << std::endl;
			}

			char *outputBufferCString = new char[ outputBuffer.str().size() + 1 ];
			strcpy(outputBufferCString, outputBuffer.str().c_str());
			page_string(ch->desc, outputBufferCString, TRUE);
			delete[] outputBufferCString;
			break;
		}
		case 24:
		{
			std::stringstream outputBuffer;
			std::list<ClanQuestPointTransaction *> clanQuestPointTransactions = ClanUtil::getAllClanQuestPointTransactions(gameDatabase);

			outputBuffer << "ID      User Name         Clan Name          Time                   QP  Type    Iss. Name    Reason\r\n";
			outputBuffer << "------------------------------------------------------------------------------------------------------------------\r\n";

			for(auto iter = clanQuestPointTransactions.begin();iter != clanQuestPointTransactions.end();++iter)
			{
				ClanQuestPointTransaction *clanQuestPointTransaction = (*iter);
				std::string issuedByUserName, userName, clanName;
				Character *user;
				Clan *clan;
				PlayerIndex *userPlayerIndex;

				userPlayerIndex = CharacterUtil::getPlayerIndexByUserId(clanQuestPointTransaction->getUserId());
				if(userPlayerIndex != NULL)
					userName = userPlayerIndex->name;
				else
					userName = "<Unknown>";

				clan = ClanUtil::getClan(clanQuestPointTransaction->getClanId());

				if(clan)
					clanName = clan->Name;
				else
					clanName = "<Unknown>";

				if(clanQuestPointTransaction->getIssuedByUserType() == UserType::player)
				{
					PlayerIndex *issuedByUserIndex = CharacterUtil::getPlayerIndexByUserId(clanQuestPointTransaction->getIssuedByUserId());
					if(issuedByUserIndex)
						issuedByUserName = issuedByUserIndex->name;
					else
						issuedByUserName = "<Unknown>";
				}
				else
				{//Issued by a mob.
					Character *issuedByUserCharacter = MobManager::GetManager().GetPrototypeByVnum(clanQuestPointTransaction->getIssuedByUserId());
					if(issuedByUserCharacter != NULL)
						issuedByUserName = GET_NAME(issuedByUserCharacter);
					else
						issuedByUserName = "<Unknown>";
				}

				outputBuffer << std::right << std::setw(6) << clanQuestPointTransaction->getId() << "  ";
				outputBuffer << std::left << std::setw(MAX_NAME_LENGTH) << StringUtil::truncateWithEllipses(userName, MAX_NAME_LENGTH) << "  ";
				outputBuffer << std::left << std::setw(17) << StringUtil::truncateWithEllipses(clanName, 17) << "  ";
				outputBuffer << MiscUtil::formatDateYYYYdmmdddHHcMMcSS(clanQuestPointTransaction->getCreatedDatetime()) << "  ";
				outputBuffer << std::right << std::setw(4) << clanQuestPointTransaction->getAmount() << "  ";
				outputBuffer << std::left << std::setw(6) << clanQuestPointTransaction->getIssuedByUserType()->getStandardName() << "  ";
				outputBuffer << std::left << std::setw(11) << StringUtil::truncateWithEllipses(issuedByUserName, 11) << "  ";
				outputBuffer << std::left << clanQuestPointTransaction->getReason() << "\r\n";

				delete clanQuestPointTransaction;
			}

			char *outputBufferCString = new char[ outputBuffer.str().size() + 1 ];
			strcpy(outputBufferCString, outputBuffer.str().c_str());
			page_string(ch->desc, outputBufferCString, TRUE);
			delete[] outputBufferCString;

			break;
		}
		default:
			ch->send("Sorry, I don't understand that.\r\n");
			break;
	}
}

/***************** The do_set function ***********************************/

const int PC = 1;
const int NPC = 2;
const int BOTH = 3;

const int MISC = 0;
const int BINARY = 1;
const int NUMBER = 2;

#define RANGE(low, high) (value = MAX((low), MIN((high), (value))))

struct set_struct
{
	const char *cmd;
	int level;
	int pcnpc;
	int type;
	bool script_friendly;
};

set_struct set_fields[] =
    {
        { "brief"		,LVL_GOD, 		PC, 	BINARY,		true},  // 0 //
        { "invstart"	,LVL_GOD, 		PC, 	BINARY,		true},  // 1 //
        { "title"		,LVL_GOD, 		PC,		MISC,		true},
        { "maxhit"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "maxmana"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "maxmove"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},	// 5 //
        { "hit"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "mana"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "move"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "str"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "int"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},	// 10 //
        { "wis"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "dex"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "con"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "cha"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "gold"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},	// 15 //
        { "bank"		,LVL_GOD,		PC, 	NUMBER,		true},
        { "exp"			,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "damroll"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "invis"		,LVL_GRGOD, 	PC, 	NUMBER,		true},
        { "nohassle"	,LVL_GRGOD, 	PC, 	BINARY,		true},	// 20 //
        { "frozen"		,LVL_FREEZE,	PC, 	BINARY,		true},
        { "practices"	,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "drunk"		,LVL_GRGOD, 	BOTH, 	MISC,		true},
        { "hunger"		,LVL_GRGOD, 	BOTH, 	MISC,		true},
        { "thirst"		,LVL_GRGOD, 	BOTH, 	MISC,		true},	// 25 //
        { "darkfriend"	,LVL_APPR, 		PC, 	BINARY,		true},
        { "murderer"	,LVL_IMPL, 		PC, 	BINARY,		true},
        { "level"		,LVL_GRGOD, 	BOTH, 	NUMBER,		false},
        { "room"		,LVL_GRGOD, 	BOTH, 	NUMBER,		true},
        { "roomflag"	,LVL_GRGOD, 	PC, 	BINARY,		true},	// 30 //
        { "siteok"		,LVL_GRGOD, 	PC, 	BINARY,		true},
        { "deleted"		,LVL_IMPL, 		PC, 	BINARY,		false},
        { "class"		,LVL_GRGOD, 	BOTH, 	MISC,		true},
        { "nowizlist"	,LVL_GOD, 		PC, 	BINARY,		true},
        { "loadroom"	,LVL_GRGOD, 	PC, 	MISC,		true},	// 35 //
        { "color"		,LVL_GOD, 		PC, 	BINARY,		true},
        { "idnum"		,LVL_IMPL, 		PC, 	NUMBER,		false},
        { "password"	,LVL_GRGOD, 	PC, 	MISC,		false},
        { "nodelete"	,LVL_GOD, 		PC, 	BINARY,		true},
        { "sex"			,LVL_GRGOD, 	BOTH, 	MISC,		true},	// 40 //
        { "age"			,LVL_GRGOD,		BOTH,	NUMBER,		true},
        { "qp"			,LVL_GRGOD, 	PC, 	NUMBER,		true},
        { "race"		,LVL_GOD,		BOTH,	MISC,		true},
        { "ob"			,LVL_GOD,		BOTH,	NUMBER,		true},
        { "db"			,LVL_GOD,		BOTH,	NUMBER,		true},	// 45 //
        { "pb"			,LVL_GOD,		BOTH,	NUMBER,		true},
        { "abs"			,LVL_GOD,		BOTH,	NUMBER,		true},
        { "clan"		,LVL_GOD,		PC,		NUMBER,		true},
        { "mood"		,LVL_GRGOD,		BOTH,	NUMBER,		true},
        { "notell"		,LVL_APPR,		PC,		BINARY,		true},	// 50 //
        { "weave"		,LVL_GRGOD,		PC,		NUMBER,		true},
        { "shadow"		,LVL_GRGOD,		BOTH,	NUMBER,		true},
        { "maxshadow"	,LVL_GRGOD,		BOTH,	NUMBER,		true},
        { "masterweapon",LVL_APPR,		PC,		NUMBER,		true},
        { "taint"		,LVL_GRGOD,		BOTH,	NUMBER,		true},	// 55 //
        { "logger"		,LVL_IMPL,		PC,		BINARY,		false},
        { "whoisextra"	,LVL_GRGOD,		PC,		MISC,		true},
        { "weight"		,LVL_GOD,		BOTH,	NUMBER,		true},
        { "height"		,LVL_GOD,		BOTH,	NUMBER,		true},
        { "noidle"		,LVL_GOD,		BOTH,	BINARY,		true},	// 60 //
        { "deathwait"	,LVL_GRGOD,		BOTH,	NUMBER,		true},
        { "idle"		,LVL_GRGOD,		PC,		NUMBER,		true},
        { "bodystruct"	,LVL_GOD,		BOTH,	MISC,		true},
        { "wasin"		,LVL_GRGOD,		PC,		NUMBER,		true},
        { "noweave"		,LVL_GRGOD,		PC,		BINARY,		true},	// 65 //
		{ "spractices"  ,LVL_GRGOD,     PC,     NUMBER,		true},
        { "notrace"		,LVL_IMPL,		PC,		BINARY,		false},
        { "weaveeditor"	,LVL_IMPL,		PC,		BINARY,		false},
		{ "stateditor"	,LVL_IMPL,		PC,		BINARY,		false},
		{ "chargeeditor",LVL_IMPL,		PC,		BINARY,		false},	// 70 //
		{ "position"	,LVL_APPR,		BOTH,	NUMBER,		true},
		{ "tpractices"	,LVL_GRGOD,		BOTH,	NUMBER,		true},
		{ "account"		,LVL_GRGOD,		PC,		MISC,		false},
		{ "globaljs"	,LVL_GRGOD,		PC,		BINARY,		false}, // 74 //	
        { "\n", 0, BOTH, MISC, false }
    };

void list_set(Character *ch)
{
	int i = 0;

	for(i = 0;set_fields[i].cmd != "\n";i++)
		if(GET_LEVEL(ch) >= set_fields[i].level)
		{
			sprintf(buf, "Field: %s,    Min Level: %d\r\n", set_fields[i].cmd, set_fields[i].level);
			ch->send(buf);
		}
}

//#ifndef SET_OR_REMOVE
#define SET_OR_REMOVE(flagset, flags) { \
	if (on) SET_BIT_AR(flagset, flags); \
	else if (off) REMOVE_BIT_AR(flagset, flags); }

#define SET_OR_REMOVE2(flagset, flags) { \
	if (on) SET_BITK(flagset, Q_BIT(flags)); \
	else if (off) REMOVE_BIT(flagset, Q_BIT(flags)); }
//#endif
int perform_set(Character *ch, Character *vict, int mode, char *val_arg, int file)
{

	int i, on = 0, off = 0, value = 0;
	char output[MAX_STRING_LENGTH];
	Room *room;

	// Check to make sure all the levels are correct //
	if (ch && GET_LEVEL(ch) != LVL_IMPL)
	{
		if (!IS_NPC(vict) && GET_LEVEL(ch) <= GET_LEVEL(vict) && vict != ch)
		{
			ch->send("Maybe that's not such a great idea...\r\n");
			return 0;
		}
	}

	if (ch && GET_LEVEL(ch) < set_fields[mode].level)
	{
		ch->send("You are not godly enough for that!\r\n");
		return 0;
	}

	/* Make sure the PC/NPC is correct */
	if (IS_NPC(vict) && !(set_fields[mode].pcnpc & NPC))
	{
		if( ch )
			ch->send("You can't do that to a beast!\r\n");
		return 0;
	}

	else if (!IS_NPC(vict) && !(set_fields[mode].pcnpc & PC))
	{
		if( ch )
			ch->send("That can only be done to a beast!\r\n");
		return 0;
	}

	// Find the value of the argument //
	if (set_fields[mode].type == BINARY)
	{
		if (!strcmp(val_arg, "on") || !strcmp(val_arg, "yes"))
			on = 1;

		else if (!strcmp(val_arg, "off") || !strcmp(val_arg, "no"))
			off = 1;

		if (!(on || off))
		{
			if( ch )
				ch->send("Value must be 'on' or 'off'.\r\n");
			return 0;
		}

		sprintf(output, "%s %s for %s.", set_fields[mode].cmd, ONOFF(on),
		        GET_NAME(vict));
	}

	else if (set_fields[mode].type == NUMBER)
	{
		value = atoi(val_arg);
		sprintf(output, "%s's %s set to %d.", GET_NAME(vict),
		        set_fields[mode].cmd, value);
	}

	else
	{
		strcpy(output, "Okay.");  /* can't use OK macro here 'cause of \r\n */
	}

	switch (mode)
	{
		case 0:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_BRIEF);
			break;
		case 1:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_INVSTART);
			break;
		case 2:
			vict->SetTitle(val_arg);
			sprintf(output, "%s's title is now: %s", GET_NAME(vict), vict->player.title.c_str());
			break;
		case 3:
			i = vict->points.max_hit;
			vict->points.max_hit = RANGE(1, 5000);
			i -= vict->points.max_hit;

			if(vict->points.HitRolls.size())
				vict->points.HitRolls[0] += i;
			else
				vict->ResetHitRolls(false);

			affect_total(vict);
			break;
		case 4:
			i = vict->points.max_mana;
			vict->points.max_mana = RANGE(1, 5000);
			i -= vict->points.max_mana;

			if(vict->points.ManaRolls.size())
				vict->points.ManaRolls[0] += i;
			else
				vict->ResetManaRolls(false);
			affect_total(vict);
			break;
		case 5:
			vict->points.max_move = RANGE(1, 5000);
			affect_total(vict);
			break;
		case 6:
			vict->points.hit = RANGE(-9, vict->points.max_hit);
			affect_total(vict);
			break;
		case 7:
			vict->points.mana = RANGE(0, vict->points.max_mana);
			affect_total(vict);
			break;
		case 8:
			vict->points.move = RANGE(0, vict->points.max_move);
			affect_total(vict);
			break;
		case 9:
			RANGE(1, 25);

			vict->real_abils.str = value;
			affect_total(vict);
			break;
		case 10:
			RANGE(1, 25);

			vict->real_abils.intel = value;
			affect_total(vict);
			break;
		case 11:
			RANGE(1, 25);

			vict->real_abils.wis = value;
			affect_total(vict);
			break;
		case 12:
			RANGE(1, 25);

			vict->real_abils.dex = value;
			affect_total(vict);
			break;
		case 13:
			RANGE(1, 25);

			vict->real_abils.con = value;
			affect_total(vict);
			break;
		case 14:
			RANGE(1, 25);

			//vict->real_abils.cha = value;
			//affect_total(vict);
			break;
		case 15:
			vict->points.gold = value;
		case 16:
			GET_BANK_GOLD(vict) = RANGE(0, 100000000);
			break;
		case 17:
			vict->points.exp = RANGE(0, 500000000);
			break;
		case 18:
			vict->points.damroll = RANGE(-20, 20);
			affect_total(vict);
			break;
		case 19:
			if (ch && GET_LEVEL(ch) < LVL_IMPL && ch != vict)
			{
				ch->send("You aren't godly enough for that!\r\n");
				return 0;
			}
			GET_INVIS_LEV(vict) = RANGE(0, GET_LEVEL(vict));
			break;
		case 20:
			if (ch && GET_LEVEL(ch) < LVL_IMPL && ch != vict)
			{
				ch->send("You aren't godly enough for that!\r\n");
				return 0;
			}

			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_NOHASSLE);
			break;
		case 21:
			if (ch && ch == vict)
			{
				ch->send("Better not -- could be a long winter!\r\n");
				return 0;
			}

			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_FROZEN);
			break;
		case 22:
			vict->PlayerData->skillpracs = RANGE(0, 100);
			break;
		case 23:
		case 24:
		case 25:
			if (!str_cmp(val_arg, "off"))
			{
				vict->PlayerData->conditions[ (mode - 23) ] = (char) -1; /* warning: magic number here */
				sprintf(output, "%s's %s now off.", GET_NAME(vict), set_fields[mode].cmd);
			}
			else if (IsNumber(val_arg))
			{
				value = atoi(val_arg);
				RANGE(0, 24);
				vict->PlayerData->conditions[ (mode - 23) ] = (char) value; /* and here too */
				sprintf(output, "%s's %s set to %d.", GET_NAME(vict),
				        set_fields[mode].cmd, value);
			}

			else
			{
				if( ch )
					ch->send("Must be 'off' or a value from 0 to 24.\r\n");
				return 0;
			}

			break;
		case 26:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_DARKFRIEND);
			break;
		case 27:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_MURDERER);
			break;
		case 28:
			if ( (ch && value > GET_LEVEL(ch)) || value > LVL_IMPL)
			{
				if( ch )
					ch->send("You can't do that.\r\n");
				return 0;
			}

			RANGE(0, LVL_IMPL);
			vict->player.level = (byte) value;
			check_autowiz(vict);
			break;
		case 29:
			if ( !(room = FindRoomByVnum(value)) )
			{
				if( ch )
					ch->send("No room exists with that number.\r\n");
				return 0;
			}

			vict->RemoveFromRoom();
			vict->CancelTimer(false);
			vict->MoveToRoom(room);
			break;
		case 30:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_ROOMFLAGS);
			break;
		case 31:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_SITEOK);
			break;
		case 32:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_DELETED);
			break;
		case 33:
			if ((i = parse_class(*val_arg)) == RACE_UNDEFINED)
			{
				if( ch ) 
					ch->send("That is not a class.\r\n");
				return 0;
			}

			GET_CLASS(vict) = i;
			break;
		case 34:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_NOWIZLIST);
			break;
		case 35:
			break;
		case 36:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_COLOR_1);
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_COLOR_2);
			break;
		case 37:
			vict->player.idnum = value;
			break;
		case 38:
			if( strlen(val_arg) > MAX_PWD_LENGTH )
				sprintf(output, "Password must be under %lldd characters long.\r\n", (__int64)MAX_PWD_LENGTH);
			else
			{
				vict->PasswordUpdated( true );
				vict->player.passwd = MD5::getHashFromString(val_arg);
				sprintf(output, "Password changed to '%s'.", val_arg);
			}
			break;
		case 39:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_NODELETE);
			break;
		case 40:
			if (!str_cmp(val_arg, "male"))
				vict->player.sex = SEX_MALE;
			else if (!str_cmp(val_arg, "female"))
				vict->player.sex = SEX_FEMALE;
			else if (!str_cmp(val_arg, "neutral"))
				vict->player.sex = SEX_NEUTRAL;
			else
			{
				if( ch )
					ch->send("Must be 'male', 'female', or 'neutral'.\r\n");
				return 0;
			}
			break;
		case 41:	/* set age */
			if (value < 2 || value > 200)
			{	/* Arbitrary limits. */
				if( ch )
					ch->send("Ages 2 to 200 accepted.\r\n");
				return 0;
			}

			/*
			NOTE: May not display the exact age specified due to the integer
			division used elsewhere in the code.  Seems to only happen for
			some values below the starting age (17) anyway. -gg 5/27/98
			*/

			vict->player.time.birth.setTime(time(0) - ((value - 17) * SECS_PER_MUD_YEAR));
			break;
		case 42:
			if( ch )
				ch->send("Command disabled.\r\n");
			break;
		case 43:
			if ((i = parse_race(*val_arg)) == RACE_UNDEFINED)
			{
				if( ch )
					ch->send("That is not a race.\r\n");
				return 0;
			}

			GET_RACE(vict) = i;
			break;
		case 44:
			GET_OB(vict) = RANGE(0, 1000);
			break;
		case 45:
			GET_DB(vict) = RANGE(0, 1000);
			break;
		case 46:
			GET_PB(vict) = RANGE(0, 1000);
			break;
		case 47:
			GET_ABS(vict) = RANGE(0, 90);
			break;
		case 48:
			if( ch )
				ch->send("Command disabled.\r\n");
			break;
		case 49:
			vict->PlayerData->mood = atoi(val_arg);
			break;
		case 50:
			SET_OR_REMOVE(PRF_FLAGS(vict), PRF_NOTELL);
			if( ch )
				ch->send("Ok.\r\n");
			break;
		case 51:
			GET_WP(vict) = RANGE(0, 100000);
			break;
		case 52:
			GET_SHADOW(vict) = RANGE(0, 5000);
			break;
		case 53:
			GET_MAX_SHADOW(vict) = RANGE(0, 5000);
			break;
		case 54:
			if(atoi(val_arg) < 0)
			{
				if( ch )
					ch->send("The value must be above zero\r\n");
				return 0;
			}
			else
				GET_MASTER_WEAPON(vict) = atoi(val_arg);

			break;
		case 55:
			if (value < 0 || value > TAINT_MAX)
			{
				if( ch )
					ch->send("Taint must be above 0 and below TAINT_MAX.\r\n");
				return (0);
			}

			GET_TAINT(vict) = value;
			break;
		case 56:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_LOGGER);
			break;
		case 57:
			if(!str_cmp(val_arg, "NULL"))
			{
				GET_WHOIS_EXTRA(vict).erase();
				if( ch )
					ch->send("%s's whois extra is now empty.\r\n", GET_NAME(vict));
			}
			else
			{
				GET_WHOIS_EXTRA(vict) = val_arg;
				if( ch )
					ch->send("%s's whois extra is now: %s", GET_NAME(vict), GET_WHOIS_EXTRA(vict).c_str());
			}

			break;
		case 58:
			if(value <= 0)
			{
				if( ch )
					ch->send("Weight must be above 0.\r\n");
				return 0;
			}

			GET_WEIGHT(vict) = value;
			break;
		case 59:
			if(value <= 0)
			{
				if(ch)
					ch->send("Height must be above 0.\r\n");
				return 0;
			}

			GET_HEIGHT(vict) = value;
			break;
		case 60:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_NOIDLE);
			break;
		case 61:
			GET_DEATH_WAIT(vict) = RANGE(0, 100);
			break;
		case 62:
			vict->player.timer = RANGE(0, 1440);
			break;
		case 63:
			if(!strn_cmp(val_arg, "light", strlen(val_arg)))
				vict->body_structure = STRUCT_LIGHT;
			else if(!strn_cmp(val_arg, "medium", strlen(val_arg)))
				vict->body_structure = STRUCT_MEDIUM;
			else if(!strn_cmp(val_arg, "heavy", strlen(val_arg)))
				vict->body_structure = STRUCT_HEAVY;
			else
			{
				if(ch)
					ch->send("Body structure must be 'light', 'medium', 'heavy'\r\n");
				return 0;
			}
			break;
		case 64:
			if(real_room(value) == -1)
			{
				if(ch)
					ch->send("Room is invalid. Try again.\r\n");
				return 0;
			}
			vict->was_in_room = FindRoomByVnum(value);
		case 65:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_NO_WEAVE);
			break;
		case 66:
			if (value < 0 )
			{
				if(ch)
					ch->send("Practices must be above 0.\r\n");
				return (0);
			}

			vict->PlayerData->spellpracs = value;
		case 67:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_NO_TRACE);
			break;
		case 68:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_WEAVE_EDITOR);
			break;
		case 69:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_STAT_EDITOR);
			break;
		case 70:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_CHARGE_EDITOR);
			break;
		case 71:
			GET_POS(vict) = RANGE(POS_DEAD, POS_STANDING);
			break;
		case 72:
			if (value < 0 )
			{
				if(ch)
					ch->send("Trade practices must be non-negative.\r\n");
				return (0);
			}
			vict->PlayerData->tradepracs = value;
			break;
		case 73:
		{
			Account a = SwitchManager::GetManager().GetAccountByName( val_arg );
			if( !a.IsValid() )
			{
				if( ch )
					ch->send("There is no account by that name.\r\n");
				return 0;
			}
			vict->SetAccount( a.GetID() );
			if( ch )
				ch->send("%s's account has been set to %s.\r\n", vict->player.name.c_str(), a.getName().c_str());
			break;
		}
		case 74:
			SET_OR_REMOVE2(PLR_FLAGS(vict), PLR_GLOBAL_SCRIPTS);
			break;
		default:
			if(ch)
				ch->send("Can't set that!\r\n");
			return 0;
	}

	strcat(output, "\r\n");

	if(ch) {
		ch->send(StringUtil::cap(output));
		sprintf(buf, "%s: \"set %s%s %s %s\"",
		        GET_NAME(ch), file ? "file " : "", GET_NAME(vict), set_fields[mode].cmd, val_arg);
		MudLog(NRM, MAX(LVL_GRGOD, GET_INVIS_LEV(ch)), TRUE, buf);
	}

	return 1;
}

ACMD(do_set)
{
	Character *vict = NULL, *cbuf = NULL;
	char field[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH],
	val_arg[MAX_INPUT_LENGTH];
	int mode = -1, retval;
	size_t len;
	char is_file = 0, is_mob = 0, is_player = 0;

	HalfChop(argument, name, buf);

	if (!strcmp(name, "file"))
	{
		is_file = 1;
		HalfChop(buf, name, buf);
	}

	else if (!str_cmp(name, "player"))
	{
		is_player = 1;
		HalfChop(buf, name, buf);
	}

	else if (!str_cmp(name, "mob"))
	{
		is_mob = 1;
		HalfChop(buf, name, buf);
	}

	HalfChop(buf, field, buf);
	strcpy(val_arg, buf);

	if (!*name || !*field)
	{
		ch->send("Usage: set <victim> <field> <value>\r\n");
		list_set(ch);
		return;
	}

	/* find the target */
	if (!is_file)
	{
		if (is_player)
		{
			if (!(vict = get_player_vis(ch, name, 0)))
			{
				ch->send("There is no such player.\r\n");
				return;
			}
		}

		else
		{
			if (!(vict = get_char_vis(ch, name)))
			{
				ch->send("There is no such creature.\r\n");
				return;
			}
		}
	}

	else if (is_file)
	{
		// try to load the player off disk
		if( (cbuf = CharacterUtil::loadCharacter(name)) != NULL)
		{

			if (GET_LEVEL(cbuf) >= GET_LEVEL(ch))
			{
				delete cbuf;
				ch->send("Sorry, you can't do that.\r\n");
				return;
			}

			vict = cbuf;
		}
		else
		{
			delete cbuf;
			ch->send("There is no such player.\r\n");
			return;
		}
	}
	/* find the command in the list */
	len = strlen(field);

	for (mode = 0; *(set_fields[mode].cmd) != '\n'; ++mode)
		if (!strncmp(field, set_fields[mode].cmd, len))
			break;

	/* perform the set */
	retval = perform_set(ch, vict, mode, val_arg, is_file);

	/* save the character if a change was made */
	if (retval && !IS_NPC(vict))
	{
		vict->save();
	}

	/* free the memory if we allocated it earlier */
	if (is_file)
		delete cbuf;
}

ACMD(do_retool)
{
	char field[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH], mode[MAX_INPUT_LENGTH];
	char **sBufPtr = NULL;
	Object *obj;
	bool isExDesc = false;

	HalfChop( argument, mode, buf );
	HalfChop( buf, name, buf );

	if( (obj = get_obj_in_list_vis( ch, name, ch->carrying )) == NULL ) {
		ch->send("Retool what? You don't see %s %s anywhere.\r\n", AN(name), name);
		return;
	}
	//obj is guaranteed to be valid.

	if( !str_cmp(mode, "name") )
		sBufPtr = &obj->retool_name;
	else if( !str_cmp(mode, "sdesc") )
		sBufPtr = &obj->retool_sdesc;
	else if( !str_cmp(mode, "desc") )
		sBufPtr = &obj->retool_desc;
	else if( !str_cmp(mode, "exdesc") )
	{
		isExDesc = true;
		if( !obj->retool_ex_desc ) {
			obj->retool_ex_desc = new ExtraDescription();
		}
		sBufPtr = &obj->retool_ex_desc->description;
	}
	else {
		ch->send("retool [name | desc | sdesc | exdesc] [object name] [new text]\r\n");
		return;
	}
	if( !(*buf) )
	{
		if( (*sBufPtr) )
			delete[] (*sBufPtr);
		(*sBufPtr) = NULL;

		if(isExDesc)
		{//Special case. We must clear the structure.
			delete obj->retool_ex_desc;
			obj->retool_ex_desc = NULL;
		}
	}
	else if( !(*sBufPtr ) )
		(*sBufPtr) = str_dup( buf );
	else {
		if( strlen( *sBufPtr ) < strlen( buf ) ) {//old buffer is too small. trash & resize.
			delete[] (*sBufPtr);
			(*sBufPtr) = str_dup( buf );
		}
		else
			strcpy((*sBufPtr), buf);
	}
	ch->send("You successfully retool( %s ) %s.\r\n", StringUtil::allLower(mode), obj->short_description);
}

class CheckWotmudStatusJob : public Job
{
private:
	bool isUp;
	int userId;
public:

	CheckWotmudStatusJob(int userId) {

		this->userId = userId;
	}

	bool getIsUp() {

		return isUp;
	}

	void setIsUp(bool isUp) {

		this->isUp = isUp;
	}

	int getUserId() {

		return userId;
	}

	void setUserId(int userId) {

		this->userId = userId;
	}

	void performRoutine() {

		kuClient client;
		client.connect("wotmud.org", 2222);

		if(client.isConnected() == false) {
			isUp = false;
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		std::stringstream buffer;

		client.receive(buffer);

		if(buffer.str().find("Running since Summer 1993") != std::string::npos) {

			isUp=true;
		}
		else {

			isUp=false;
		}
	}

	void performPostJobRoutine() {

		Character *ch = CharacterUtil::getOnlineCharacterById(userId);

		std::string upOrDown = isUp ? "online" : "offline";

		ch->send("WoTMUD is %s.\r\n", upOrDown.c_str());
	}

};

ACMD(do_wotmud)
{
	ch->send("Checking wotmud status...\r\n");

	CheckWotmudStatusJob *job = new CheckWotmudStatusJob(ch->player.idnum);
	ThreadedJobManager::get().addJob(job);
}

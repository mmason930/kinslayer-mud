/**************************************************************************
*   File: Act.informative.c                             Part of CircleMUD *
*  Usage: Player-level commands of an informative nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
***************************************************************************/

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
#include "weaves.h"
#include "weather.h"
#include "dg_event.h"
#include "MiscUtil.h"
#include "StringUtil.h"
#include "CharacterUtil.h"
#include "Descriptor.h"
#include "zones.h"

#include "js.h"
#include "js_utils.h"

/* extern variables */
extern Wizard *wizlist;
extern struct GameTime time_info;
extern struct Index *obj_index;
extern const char *weekdays[];
extern const char *month_name[];
extern const char *pc_race_types[];
extern Descriptor *descriptor_list;
extern Character *character_list;
extern Object *object_list;
int rank_req[14] = {0, 15, 30, 65, 150, 300, 550, 1000, 1500, 2100, 2700, 3600, 4600, 5600 };
extern Room *FindTargetRoom(Character * ch, char * rawroomstr);
extern DateTime rebootTime;
extern int boot_high;
extern int Seconds;
extern char *credits;
extern char *news;
extern char *motd;
extern char *imotd;
extern char *policies;
extern char *handbook;
extern const char *class_abbrevs[];

int is_viewable_clan(Character *ch, int number);
const char *DARKNESS_CHECK(Character *ch, Character *vict);

/* extern functions */
ACMD(do_action);
long find_class_bitvector(char arg);
int level_exp(int level);
bool isInArena(Character *ch);
bool JS_isArenaInFFAMode();

struct GameTime *real_time_passed(time_t t2, time_t t1);
extern const char * weather_patterns[];
extern const char * weather_storm_names[];
extern const char * weather_event_names[];

/* local functions */
ACMD(do_color);
ACMD(do_commands);
ACMD(do_consider);
ACMD(do_diagnose);
ACMD(do_equipment);
ACMD(do_examine);
ACMD(do_exits);
ACMD(do_gen_ps);
ACMD(do_gold);
ACMD(do_incognito);
ACMD(do_inventory);
ACMD(do_levels);
ACMD(do_look);
ACMD(do_scan);
ACMD(do_stat);
ACMD(do_time);
ACMD(do_toggle);
ACMD(do_users);
ACMD(do_view);
ACMD(do_warrant);
ACMD(do_warrants);
ACMD(do_weather);
ACMD(do_where);

void printObjectLocation(std::stringstream &outputBuffer, int num, Object * obj, Character * ch, int recur);
void showObjectToCharacter(Object * object, Character * ch, int mode, int ammount);
void listObjectToCharacter(Object * listy, Character * ch, int mode, int show);
void performMortalWhere(Character * ch, char *arg);
void performImmortalWhere(Character * ch, char *arg);
void diagnoseCharacterToCharacter(Character * i, Character * ch);
void performAutoExits( Character * ch);
void lookAtCharacter( Character * i, Character * ch);
void listOneCharacter( Character * i, Character * ch);
void listCharacterToCharacter( Character * listy, Character * ch);
void lookInDirection( Character * ch, int dir);
void lookInObject( Character * ch, char *arg, Object *obj=0);
void lookAtTarget( Character * ch, char *arg);
char *findExtraDescription(char *word, struct extra_descr_data * listy);
const char *findSuffix(int number);
bool canSeeCharsInRoom(Character *ch, Room *room);
std::vector<bool> getInvertedAffects(Character *ch);
int getArenaTeam( Character *ch );

int JS_getCharacterArenaTeam( Character *ch );

const char *findSuffix(int number)
{
	if (number == 1)
		return "st";
	else if (number == 2)
		return "nd";
	else if (number == 3)
		return "rd";
	else if (number < 20)
		return "th";
	else if ((number % 10) == 1)
		return "st";
	else if ((number % 10) == 2)
		return "nd";
	else if ((number % 10) == 3)
		return "rd";
	else
		return "th";
}
int Character::ExperienceToLevel()
{
	return level_exp(GET_LEVEL(this) + 1) - GET_EXP(this);
}
int level_timer(Character *ch, int level)
{
	if(IS_NPC(ch) || GET_LEVEL(ch) >= LVL_GRGOD)
		return 0;
	else if(level >= 75)
		return 5;
	else if(level >= 50)
		return 9;
	else if(level >= 25)
		return 15;
	else if(level >= 0)
		return 22;

	return 0;
}

bool Character::CanViewClan(sh_int clannum)
{
	Clan *c = GetRealClan(clannum);

	//Non existing?!
	if(clannum < 0 || !c)
		return false;

	//Is this person part of the clan?
	if(this->GetClan(clannum))
		return true;

	//Hidden level at or below player's level
	if(c->hidden_level > GET_LEVEL(this))
		return false;

	//GRGOD or higher can view secret clans
	if(GET_LEVEL(this) >= LVL_GRGOD)
		return true;

	//Not a secret clan
	if(!c->secret)
		return true;


	return false;
}

void Character::ListWarrants()
{
	std::list<Warrant *>::iterator iter;
	std::string str;
	bool HasWarrant = false;

	str = "You are wanted by the following groups: ";

	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		if(IS_SET_AR(GET_WARRANTS(this), (*iter)->vnum))
		{
			if(HasWarrant)
				str += ", ";
			else
				HasWarrant = true;
			str += (COLOR_CYAN(this, CL_COMPLETE));
			str += (*iter)->Name;
			str += (COLOR_NORMAL(this, CL_COMPLETE));
		}
	}

	str += "\r\n";

	if(HasWarrant)
		this->Send(str.c_str());
}

std::string MoodString(int mood)
{
	switch(mood)
	{
	case MOOD_WIMPY:
		return "Wimpy";
	case MOOD_MILD:
		return "Mild";
	case MOOD_BRAVE:
		return "Brave";
	case MOOD_BERSERK:
		return "Berserk";
	default:
		return "ERROR!!";
	}
}

std::string StanceString(int stance)
{
	switch(stance)
	{
	case STANCE_DEFENSIVE:
		return "Defensive";
	case STANCE_NORMAL:
		return "Normal";
	case STANCE_OFFENSIVE:
		return "Offensive";
	default:
		return "ERROR!!";
	}
}

void Character::ListLegends()
{
	std::list<Legend *>::iterator l;
	int i = 1;
	Character *victim;
	std::stringstream Query;

	this->Send("Below is a list of the current Legends of the Wheel.\r\n\n");

	for(i = 1, l = Legends.begin();l != Legends.end();++l, ++i)
	{
		if(i > 8 && GET_LEVEL(this) < LVL_GRGOD)
			break;

		if( (victim = CharacterUtil::loadCharacter((*l)->name)) != NULL )
		{
			if(GET_LEVEL(this) >= LVL_GRGOD)
				this->Send("~  %d. %s the %s. - %d\r\n", i, GET_NAME(victim), victim->RaceName().c_str(), GET_WP(victim));
			else
				this->Send("~ %d. %s the %s.\r\n", i, GET_NAME(victim), victim->RaceName().c_str());

			if(!GET_SLEW_MESSAGE(victim).empty())
				this->Send("      %s\r\n\n", GET_SLEW_MESSAGE(victim).c_str());
			else
				this->Send("\r\n");
			delete victim;
		}
	}
}

std::string Character::GoldString(int amount, bool colors, bool abbrev)
{
	static std::stringstream strBuffer;
	strBuffer.str("");

	strBuffer << (colors ? COLOR_BOLD(this, CL_COMPLETE) : "");
	strBuffer << (colors ? COLOR_YELLOW(this, CL_COMPLETE) : "");
	strBuffer << (CalcGold(amount));
	strBuffer << (colors ? COLOR_NORMAL(this, CL_COMPLETE) : "");
	strBuffer << (!abbrev ? " Gold, " : "G ");

	strBuffer << (colors ? COLOR_BOLD(this, CL_COMPLETE) : "");
	strBuffer << (colors ? COLOR_CYAN(this, CL_COMPLETE) : "");
	strBuffer << (CalcSilver(amount));
	strBuffer << (colors ? COLOR_NORMAL(this, CL_COMPLETE) : "");
	strBuffer << (!abbrev ? " Silver, " : "S ");

	strBuffer << (colors ? COLOR_YELLOW(this, CL_COMPLETE) : "");
	strBuffer << (CalcCopper(amount));
	strBuffer << (colors ? COLOR_NORMAL(this, CL_COMPLETE) : "");
	strBuffer << (!abbrev ? " Copper" : "C");

	return strBuffer.str();
}

/* Fixes the broken scan command, displays all of the mobs in a room adjacent to a chars current room.
    This is done via several options:
            1. scan <dir>; displays mobs to that direction.
            2. scan <no args>; displays FIRST mob in each direction

    This command works for ALL classes, but for NON-THIEVES there is a 3 pulse timer accompanied by the action */

void do_auto_scan(Character *ch, bool typed); // RHOLLOR 05.15.09

ACMD(do_scan)
{
   int exit=-1;
   Room *room = NULL;
   char arg[MAX_INPUT_LENGTH];
   std::stringstream TotalCommand;
   TotalCommand << "scan" << argument;

   OneArgument(argument, arg);

   if (!*arg)
   {
   }
   else
   {
      switch(*arg)
      {
         case 'n':
            exit = NORTH;
            break;
         case 'e':
            exit = EAST;
            break;
         case 's':
            exit = SOUTH;
            break;
         case 'w':
            exit = WEST;
            break;
         case 'u':
            exit = UP;
            break;
         case 'd':
            exit = DOWN;
            break;
         default:
            ch->Send("What direction is that?\r\n");
            return;
      }
   }

/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv   RHOLLOR 05.15.09   vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
/* Fixes the broken scan command, displays all of the mobs in a room adjacent to a chars current room.
   This is done via several options:
         1. scan <dir>; displays mobs to that direction.
         2. scan <no args>; displays FIRST mob in each direction

   This command works for ALL classes, but for NON-THIEVES there is a 3 pulse timer accompanied by the action */

   /* Galnor 07/14/2009 - Completely redid the if statements below */
	if (GET_POS(ch) != POS_STANDING) {
		ch->Send("You need to be on your feet to get a good look.\r\n");
		return;
	}
	if( exit != -1 )//If they chose a specific direction...
	{
		if (( !EXIT(ch,exit) || !EXIT(ch,exit)->to_room ))
		{//If the exit is invalid...
			ch->Send("There doesn't appear to be anything in that direction.\r\n");
			return;
		}
		else if( EXIT_FLAGGED( EXIT(ch,exit), EX_CLOSED ) )
		{//If the exit is closed...
			if((GET_SKILL(ch,SKILL_SEARCH) < EXIT(ch, exit)->HiddenLevel()) || !EXIT(ch,exit)->keyword )
				ch->Send("There doesn't appear to be anything in that direction.\r\n");
			else
				ch->Send("You cannot see through the %s.\r\n",fname(EXIT(ch, exit)->keyword));
			return;
		}
		room = ch->in_room->dir_option[exit]->to_room;
		if(room->IsDark() && !CAN_SEE_IN_DARK(ch))
		{//If the room to be scanned is not visible...
			ch->Send("It looks too dark in there to make anything out.\r\n");
			return;
		}
		if( room == ch->in_room )
		{//Target room is current room.
			ch->Send("You might as well look at the room you are currently in.\r\n");
			return;
		}
	}
	if (!ch->command_ready && (GET_CLASS(ch) != CLASS_THIEF && !IS_GREYMAN(ch) && !IS_FADE(ch) && !IS_BLADEMASTER(ch) && !IS_OGIER(ch))) {
		ch->SetupTimer(TotalCommand.str(), 1.0f);
		ch->command_ready = true;
	}
	else {

		if (!*arg) {
			do_auto_scan(ch,true);
		}
		else {
			if(room->people && canSeeCharsInRoom(ch, room)) {
				listCharacterToCharacter(room->people, ch);
				ch->Send(COLOR_NORMAL(ch, CL_NORMAL));
			}
			else {
				ch->Send("You cannot make anything out in that direction.\r\n");
			}
		}
	}
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   RHOLLOR 05/02/09   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
/*
   if (ch->in_room->dir_option[exit] && (room = ch->in_room->dir_option[exit]->to_room) &&
           !EXIT_FLAGGED(EXIT(ch, exit), EX_CLOSED) && !ch->in_room->sector_type != SECT_INSIDE &&
           !ch->in_room->sector_type != SECT_CITY)
   {
      listCharacterToCharacter(room->people, ch);
      ch->Send(COLOR_NORMAL(ch, CL_NORMAL));
   }

*/

}

/* Imped by Galnor on 10-08-2003. Command is used to view different lists, for example, clans.*/
ACMD(do_view)
{
	int count = 0, clan_num = 0;
	unsigned int i = 0;
	Clan *c;
	std::vector< std::list<std::string> > TheClanned;
	std::list<std::string> TheCouncil, *CurrentList, UnRanked;

	TwoArguments(argument, buf1, buf2);

	if(!*buf1)
	{
		ch->Send("View list: Clan, Clans, Trophy, Legends <Weekly|Monthly|Yearly>\r\n");
		return;
	}

	if( !str_cmp(buf1, "clan") )
	{
		if(!*buf2 && !ch->clans)
		{
			ch->Send("But which clan?\r\n");
			return;
		}

		if(!(clan_num = GetClanByString(buf2)))
		{
			ch->Send("There is no such clan.\r\n");
			return;
		}

		if(!ch->CanViewClan(clan_num))
		{
			ch->Send("You don't know anything about them.\r\n");
			return;
		}

		if(!*buf2)
			c = GetRealClan(ch->clans->GetClanVnum());
		else
			c = GetRealClan(clan_num);

		if(c == NULL || c->hidden_level > GET_LEVEL(ch))
			ch->Send("There is no such clan.\r\n");

		ch->Send(" --%s Members--\r\n", c->Name.c_str());

		int n;
		c->GetPlayerList(TheClanned, TheCouncil);
		for(i = 0;i <= c->RankNames.size();++i)
		{
			ch->Send((i == c->RankNames.size()) ? "" : "\r\n");
			ch->Send("%s", (i == c->RankNames.size()) ? "" : c->RankNames[i].c_str());
			ch->Send((i == c->RankNames.size()) ? "" : "\r\n");

			if(i == c->RankNames.size())
				CurrentList = &UnRanked;
			else if(i < TheClanned.size())
				CurrentList = &(TheClanned[i]);
			else
				continue;

			n = 1;
			if( i == c->RankNames.size() )
				++n;

			for(;n;--n)
			{
				if( i == c->RankNames.size() && n == 1 )
				{
					ch->Send("\r\nThe Council\r\n");
					CurrentList = &TheCouncil;
				}
				std::list<std::string>::iterator name = CurrentList->begin();
				for(count = 0;name != CurrentList->end();++name, ++count)
				{
					if(count)
						ch->Send(", ");
					else
						ch->Send("   ");
					if(!(count % 5) && count)
						ch->Send("\r\n   ");
					ch->Send("%s%s%s", COLOR_CYAN(ch, CL_COMPLETE), (*name).c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
				}
			}
		}
	}
	else if( !str_cmp(buf1, "clans") )
	{/* Display all of the clans visible to the commander */
		Clan *clan;
		int i;

		for(i = 1, clan = ClanList;clan;clan = clan->Next, ++i)
		{
			if(clan->hidden_level <= GET_LEVEL(ch))
			{
				ch->Send("#%d %s\r\n", GET_LEVEL(ch) < LVL_GRGOD ? i : clan->vnum, clan->Name.c_str());
			}
		}
	}
	else if( !str_cmp(buf1, "trophy") )
	{
		std::map< std::string, int>::iterator pos;
		char buf[MAX_STRING_LENGTH];

		ch->Send(
			"*** TROPHY *** (Number Killed, Mobile)\n\r"
		);

		for( pos = ch->kill_list.begin(); pos != ch->kill_list.end(); ++pos)
		{
			sprintf(    buf,
						"[%3d] %s\n\r"
						, pos->second
						, pos->first.c_str()
				   );

			ch->Send( buf );
			continue;
		}
	}
	else if( !str_cmp(buf1, "legends") )
	{
		/* Determine the front and back of the week. */
		tm Low, High;
		Low  = Time::TimeToDate(time(0));
		High = Time::TimeToDate(time(0));
		std::string Type;

		if( *buf2 && !strn_cmp(buf2, "weekly", strlen(buf2)) )
		{
			while( Low.tm_wday != 0 ) Low = Time::PriorDay( Low );
			while( High.tm_wday != 6) High= Time::NextDay( High );
			Type = "week";
		}
		else if( *buf2 && !strn_cmp(buf2, "monthly", strlen(buf2)) )
		{
			while( Low.tm_mday != 1 ) Low = Time::PriorDay( Low );
			int m = High.tm_mon+1, y = High.tm_year+1900;
			while( High.tm_mday != Time::DaysPerMonth(m,y) ) High = Time::NextDay( High );
			Type = "month";
		}
		else if( *buf2 && !strn_cmp(buf2, "yearly", strlen(buf2)) )
		{
			while( Low.tm_yday != 0 ) Low = Time::PriorDay( Low );
			int y = High.tm_year+1900;
			while( High.tm_yday != Time::DaysPerYear(y)-1 ) High = Time::NextDay( High );
			Type = "year";
		}
		else
		{
			ch->ListLegends();//Default.
			return;
		}

		High = Time::NextDay( High );//We'll get the bottom of the start of the next week, which is essentially
									 //the same as the top of the end of the current week.
		High = Time::FloorDay( High );
		Low = Time::FloorDay( Low );

	//	Log("Low: %s, High: %s", Time::FormatDate("%m-%d-%Y %H-%M-%S", mktime(&Low)).c_str(), Time::FormatDate("%m-%d-%Y %H-%M-%S", mktime(&High)).c_str());

		std::stringstream QueryText;
		sql::Query MyQuery;
		QueryText << "SELECT pk.*,u.username,u.race,u.chclass,u.level,u.slew,COUNT(pk.killer_id) AS kill_count, "
			<< "SUM(pk.wp_transfer) AS wp_total FROM player_kills pk, users u WHERE pk.time_of_death >= "
			<< "FROM_UNIXTIME(" << mktime( &Low ) << ")" << " AND pk.time_of_death <= FROM_UNIXTIME(" << mktime( &High ) << ") AND u.user_id=pk.killer_id "
			<< "GROUP BY pk.killer_id ORDER BY wp_total DESC LIMIT 8;";

		try {
			MyQuery = gameDatabase->sendQuery(QueryText.str());
		} catch(sql::QueryException e) {
			e.report();
			MudLog(BRF,LVL_APPR,TRUE,"Error viewing Legends interval : %s", e.getMessage().c_str());
			return;
		}

		ch->Send("Below is a list of the Legends of the %s:\r\n\r\n", Type.c_str());
		for(unsigned int i = 1;MyQuery->hasNextRow();++i) {
			sql::Row MyRow = MyQuery->getRow();

			int chclass=MiscUtil::Convert<int>(MyRow["chclass"]), race=MiscUtil::Convert<int>(MyRow["race"]), level=MiscUtil::Convert<int>(MyRow["level"]);
			std::string Slew = MyRow["slew"];
			std::string NameAndRace= (MyRow["username"]) + (" the ") + (Character::RaceName(chclass,race,level));

			if(Slew.size()) {
				Slew.erase(Slew.size()-1,1);
				Slew += (", resulting in a kill count of " + MyRow["kill_count"] + ".");
			}

			ch->Send("~ %d) %-17s - (%4s weave points)\r\n", i, NameAndRace.c_str(), MyRow["wp_total"].c_str());
			ch->Send("    %s\r\n\r\n", Slew.c_str());
		}

		//	ch->Send("%s\r\n", Time::FormatDate("%m-%d-%y - %H:%M:%S",atoi(arg1)).c_str());
	}
	else
	{
		ch->Send("Invalid view command.\r\n");
	}
}

void perform_search(Character *ch, int direction)
{
	if(EXIT(ch, direction))
	{
		if(GET_SKILL(ch, SKILL_SEARCH) >= EXIT(ch, direction)->hidden && EXIT(ch, direction)->keyword)
		{
			ch->Send("You discovered the %s!\r\n", fname(EXIT(ch, direction)->keyword));
			return;
		}
	}

	ch->Send("You found nothing in that direction.\r\n");
}

ACMD(do_search)
{
	int direction;
	char arg[MAX_INPUT_LENGTH];
	Object *obj;

	OneArgument(argument, arg);

	if((GET_LEVEL(ch) >= LVL_IMMORT && GET_LEVEL(ch) < LVL_GRGOD))
	{
		ch->Send("You shouldn't be doing this...\r\n");
		return;
	}

	if ((direction = search_block(arg, (const char **) dirs, FALSE)) < 0)
	{
		for(obj = ch->in_room->contents;obj;obj = obj->next_content)
		{
			if(obj->hidden && is_name(arg, obj->getName()))
			{
				if(obj->Weight() + ch->CarriedWeight() > CAN_CARRY_W(ch))
				{
					ch->Send("You cannot carry that much weight.\r\n");
					return;
				}

				obj->hidden = false;
				obj->RemoveFromRoom();
				obj_to_char(obj, ch);

				ch->Send("You found %s!\r\n", obj->GetSDesc());

				sprintf(buf, "%s found %s.", GET_NAME(ch), obj->GetSDesc());
				Act(buf, TRUE, ch, NULL, NULL, TO_NOTVICT);

				return;
			}
		}

		ch->Send("You couldn't find that item here.\r\n");
		return;
	}

	if ((direction = search_block(arg, (const char **) dirs, FALSE)) < 0)
	{
		ch->Send("Search in what direction?\r\n");
		return;
	}
	if(IS_NPC(ch))
	{
		ch->Send("Mobs can't do this.\r\n");
		return;
	}

	if(EXIT(ch, direction) && EXIT(ch, direction)->to_room )
	{
		if(!EXIT_FLAGGED(EXIT(ch, direction), EX_CLOSED))
		{
			ch->Send("The way is already open!\r\n");
			return;
		}
	}
	if(!ch->command_ready)
	{
		if(EXIT(ch, direction) && EXIT(ch, direction)->to_room)
			ch->timer	 = (float)(EXIT(ch, direction)->hidden / 5) - ch->GetSkillLevel(SKILL_SEARCH);
		else
			ch->timer	 = (float)(15 - ch->GetSkillLevel(SKILL_SEARCH));
		ch->command_ready = true;
	}
	else
		perform_search(ch, direction);
}


ACMD(do_warrants)
{
	std::list<Warrant *>::iterator iter;
	for(iter = Warrants.begin();iter != Warrants.end();++iter)
	{
		ch->Send("%d. %s\r\n", (*iter)->vnum, (*iter)->Name.c_str());
	}
}

bool Character::WhoisShowClass()
{
	return (!IS_FADE(this) && !IS_DREADLORD(this) && !IS_GREYMAN(this) && !IS_DREADGUARD(this) && !IS_BLADEMASTER(this) && !IS_OGIER(this));
}
bool Character::WhoisShowClass(const int &chclass, const int &race, const int &level)
{
	return !(chclass == CLASS_FADE || chclass == CLASS_DREADLORD || chclass == CLASS_GREYMAN || chclass == CLASS_DREADGUARD || chclass == CLASS_BLADEMASTER || chclass == CLASS_OGIER);
}

/* Added by Galnor early 2003. Command hides information of a player. */
ACMD(do_incognito)
{
	if (!PRF_FLAGGED(ch, PRF_INCOG))
	{
		ch->Send("Your identity is now safe.\r\n");
		SET_BIT_AR(PRF_FLAGS(ch), PRF_INCOG);
	}

	else
	{
		ch->Send("Your identity is no longer a secret.\r\n");
		REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_INCOG);
	}
}


/*
 * This function screams bitvector... -gg 6/45/98
 */
void showObjectToCharacter(Object *object, Character *ch, int mode, int ammount)
{
	bool found;
	char text[MAX_INPUT_LENGTH];

	*buf = '\0';

	if ((mode == 0) && object->description)
	{
		if(AFF_FLAGGED(ch, AFF_BLIND))
			strcpy(buf, "Something indistinguishable lies here.");
		else
			strcpy(buf, object->GetDesc());
	}

	else if (object->GetSDesc() && ((mode == 1) ||
	                                       (mode == 2) || (mode == 3) || (mode == 4)))
		strcpy(buf, object->GetSDesc());

	if(ammount > 1)
	{
		sprintf(text, " [%d]", ammount);
		strcat(buf, text);
	}
	else if( (mode == 5 || mode == 6) && GET_OBJ_TYPE(object) == ITEM_WEAPON)
	{
		if( GET_OBJ_VAL(object, 0) < 0 || GET_OBJ_VAL(object, 0) >= NUM_WEAPON_TYPES )
			strcat(buf, "You are unable to determine what kind of weapon it is.");
		else
			sprintf(buf + strlen(buf), "It is %s %s.", AN(weapon_types[GET_OBJ_VAL(object, 0)]),
			StringUtil::allLower(weapon_types[GET_OBJ_VAL(object, 0)]));
	}
	else if (mode == 5)
	{
		if (GET_OBJ_TYPE(object) == ITEM_NOTE)
		{
			if (object->action_description)
			{
				strcpy(buf, "There is something written upon it:\r\n\r\n");
				strcat(buf, object->action_description);
				page_string(ch->desc, buf, 1);
			}

			else
				Act("It's blank.", FALSE, ch, 0, 0, TO_CHAR);

			return;
		}
		else if (GET_OBJ_TYPE(object) == ITEM_DRINKCON)
			strcpy(buf, "It looks like a drink container.");
		else
			strcpy(buf, "You see nothing special..");			/* ITEM_TYPE == ITEM_DRINKCON||FOUNTAIN */

	}

	if (mode != 3)
	{
		found = FALSE;

		if (IS_OBJ_STAT(object, ITEM_INVISIBLE))
		{
			strcat(buf, " (invisible)");
			found = TRUE;
		}
#ifdef KINSLAYER_JAVASCRIPT
		try {
			std::string expression = "getCurrentDecayMessage(" + lookupName(object) + ");";
			flusspferd::string decayString = JSManager::get()->executeExpression(expression);
			if( !decayString.empty() )
			{
				strcat(buf, std::string(" (" + decayString.to_string() + ")").c_str());
				found = TRUE;
			}
		} catch( flusspferd::exception e ) {
			//...
		}
#endif
	}
	if(GET_OBJ_TYPE(object) == ITEM_LIGHT && mode == 6)
	{
		sprintf(buf+strlen(buf), "\r\n%s%sWhen testing the light source, you find that ",
		        COLOR_BOLD(ch, CL_NORMAL), COLOR_CYAN(ch, CL_NORMAL));
		if(object->GetTotalVal2() >= 48)
			strcat(buf, "it seems to be very well lit.");
		else if(object->GetTotalVal2() >= 24)
			strcat(buf, "it seems to be fairly well lit.");
		else if(object->GetTotalVal2() >= 12)
			strcat(buf, "it seems to be slightly dimmed.");
		else if(object->GetTotalVal2() >= 6)
			strcat(buf, "it seems to be very dim.");
		else if(object->GetTotalVal2() == 0)
			strcat(buf, "it's burnt out!");
		strcat(buf, COLOR_NORMAL(ch, CL_NORMAL));
	}
	strcat(buf, "\r\n");
	page_string(ch->desc, buf, TRUE);
}

int count_object(Object *obj)
{
	int count = 0;
	Object *i;

	for(i = obj;i;i = i->next_content)
	{

		if(GET_OBJ_VNUM(obj) == GET_OBJ_VNUM(i) && GET_OBJ_VNUM(obj) > -1)
			++count;
		else
			if (!strcmp(obj->GetSDesc(), i->GetSDesc()))
				++count;
	}

	return count;
}

struct SpamItem
{
		int count;
		Object *obj;
};
void listObjectToCharacter(Object *listy, Character *ch, int mode, int show)
{
	Object *i;
	bool hasSeen = FALSE, found = FALSE;
	std::list< SpamItem > SpamList;

	if(PRF_FLAGGED(ch, PRF_SPAM))
	{
		for(i = listy;i;i = i->next_content)
		{
			//Galnor 07/13/2009 - Fixed invis items showing in inventory(spam complete)
			if(i->hidden || OBJ_FLAGGED(i, ITEM_INVISIBLE))
				continue;

			hasSeen = false;
			for(std::list<SpamItem>::iterator sIter = SpamList.begin();sIter != SpamList.end();++sIter)
			{
				if(mode != 0 && !strcmp(i->GetSDesc(),(*sIter).obj->GetSDesc()))
				{
					hasSeen = true;
					++(*sIter).count;
					break;
				}
				else if( mode == 0 && !strcmp(i->GetDesc(),(*sIter).obj->GetDesc()) )
				{
					hasSeen = true;
					++(*sIter).count;
					break;
				}
			}

			if(!hasSeen)
			{
				SpamItem cur;
				cur.count = 1;
				cur.obj = i;

				if(GET_OBJ_TYPE(i) == ITEM_MONEY)
					cur.count = i->GetTotalVal1();

				SpamList.push_back( cur );
			}
		}

		for(std::list<SpamItem>::iterator sIter = SpamList.begin();sIter != SpamList.end();++sIter)
		{
			showObjectToCharacter((*sIter).obj, ch, mode, (*sIter).count);
			found = true;
		}
	}
	else
	{
		for (i = listy; i; i = i->next_content)
		{
			if(!i->hidden)

                if(!IS_OBJ_STAT((i), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT) // RHOLLOR 05.03.09 corrects invis items showing in INV
                    showObjectToCharacter(i, ch, mode, 1);

			found = true;
		}
	}

	for(i = listy;i;i = i->next_content)
	{
		if(!mode && i->hidden &&
		        ( (AFF_FLAGGED(ch, AFF_NOTICE) && GET_SKILL(ch, SKILL_NOTICE) && !AFF_FLAGGED(ch, AFF_BLIND)) || GET_LEVEL(ch) >= LVL_IMMORT))
			ch->Send("You notice %s hidden here.\r\n", i->GetSDesc());
	}

	if (!found && show)
		ch->Send(" Nothing.\r\n");
}


void diagnoseCharacterToCharacter(Character *i, Character *ch)
{
	int percent;
	char disp[MAX_STRING_LENGTH], name[MAX_INPUT_LENGTH];

	if (GET_MAX_HIT(i) > 0)
		percent = (100 * GET_HIT(i)) / GET_MAX_HIT(i);

	else
		percent = -1;		/* How could MAX_HIT be < 1?? */

	strcpy(name, PERS(i, ch));
	name[0] = UPPER(name[0]);

	if (percent >= 100)
		strcpy(disp, " is in excellent condition.\r\n");
	else if (percent >= 90)
		strcpy(disp, " has a few scratches.\r\n");
	else if (percent >= 75)
		strcpy(disp, " has some small wounds and bruises.\r\n");
	else if (percent >= 50)
		strcpy(disp, " has quite a few wounds.\r\n");
	else if (percent >= 30)
		strcpy(disp, " has some big nasty wounds and scratches.\r\n");
	else if (percent >= 15)
		strcpy(disp, " looks pretty hurt.\r\n");
	else if (percent >= 0)
		strcpy(disp, " is in awful condition.\r\n");
	else
		strcpy(disp, " is bleeding awfully from big wounds.\r\n");

	ch->Send("%s%s", name, disp);
}

void lookAtCharacter(Character * i, Character * ch)
{
	int j, found;
	Object *tmp_obj;
	char msg[MAX_INPUT_LENGTH];

	if (!ch->desc)
		return;

	if (i->player.description)
		ch->Send(i->player.description);
	else
		Act("You see nothing special about $m.\r\n", FALSE, i, 0, ch, TO_VICT);

	diagnoseCharacterToCharacter(i, ch);

	found = FALSE;

	for (j = 0; !found && j < NUM_WEARS; j++)
		if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
			found = TRUE;

	if (found)
	{
		Act("\r\n$n is using:", FALSE, i, 0, ch, TO_VICT);

		for (j = 0; j < NUM_WEARS; j++)
			if (GET_EQ(i, j) && CAN_SEE_OBJ(ch, GET_EQ(i, j)))
			{
				if(j == WEAR_WIELD && IS_OBJ_STAT(GET_EQ(i, j), ITEM_TWO_HANDED))
					strcpy(msg, "<wielded with two hands>    ");
				else
					strcpy(msg, where[j]);

				ch->Send(msg);
				showObjectToCharacter(GET_EQ(i, j), ch, 1, 1);
			}
	}

	if (ch != i && (IS_THIEF(ch) || IS_GREYMAN(ch) || GET_LEVEL(ch) >= LVL_IMMORT))
	{
		found = FALSE;
		Act("\r\nYou attempt to peek at $s inventory:", FALSE, i, 0, ch, TO_VICT);

		for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content)
		{
			if (CAN_SEE_OBJ(ch, tmp_obj) && (MiscUtil::random(0, 20) < GET_LEVEL(ch)))
			{
				showObjectToCharacter(tmp_obj, ch, 1, 1);
				found = TRUE;
			}
		}

		if (!found)
			ch->Send("You can't see anything.\r\n");

	}
}


void listOneCharacter(Character *i, Character *ch)
{
	const char *positions[] =
	    {
	        " is lying here, dead",
	        " is lying here, mortally wounded",
	        " is lying here, incapacitated",
	        " is lying here, stunned",
	        " is sleeping here",
	        " is resting here",
	        " is sitting here",
	        " fighting",
	        " is standing here",
			" flying around"
	    };

	if(ch->dizzy_time && TAINT_CALC(ch))
	{
		if(TAINT_CALC(ch))	//If SUPER tainted, the person doesn't see anything at all.
			return;
		ch->Send("A blurry shape of something %s is here.\r\n", GET_WEIGHT(i) >= GET_WEIGHT(ch) ? "large" : "small");
	}

	if(AFF_FLAGGED(ch, AFF_BLIND))
	{
		ch->Send("A blurry shape of something %s is here.\r\n", GET_WEIGHT(i) >= GET_WEIGHT(ch) ? "large" : "small");
		return;
	}

	if (IS_NPC(i) && i->player.long_descr && GET_POS(i) == i->MobData->default_pos && RIDDEN_BY(i) != ch &&
	!MOUNT(i))
	{
		if (AFF_FLAGGED(i, AFF_INVISIBLE))
			strcpy(buf, "*");
		else
			*buf = '\0';

		if(RIDDEN_BY(i))
			return;

		ch->Send(i->player.long_descr);

		if ( PRF_FLAGGED(i, PRF_SOURCE) && GET_SEX(i) == SEX_FEMALE && GET_SEX(ch) == SEX_FEMALE )
			Act("...$e is surrounded by a halo of light!", FALSE, i, 0, ch, TO_VICT);

		if ( PRF_FLAGGED(i, PRF_SOURCE) && GET_SEX(i) == SEX_MALE && GET_SEX(ch) == SEX_MALE )
			Act("...$e emits an aura of menace and awe!", FALSE, i, 0, ch, TO_VICT);

		if (AFF_FLAGGED(i, AFF_SANCTUARY))
			Act("...$e glows with a bright light!", FALSE, i, 0, ch, TO_VICT);
		return;
	}

	if (IS_NPC(i))
	{
		strcpy(buf, StringUtil::cap(i->player.short_descr));
	}
	else if(isInArena(ch) && isInArena(i))
	{
		if( !JS_isArenaInFFAMode() )
		{
			int chArenaTeam = getArenaTeam(ch);
			int iArenaTeam = getArenaTeam(i);

			if( chArenaTeam == iArenaTeam && chArenaTeam > 0 )
				sprintf(buf, "%s %s", GET_NAME(i), i->player.title.c_str());
			else if( iArenaTeam == ARENA_BLUE )
				sprintf(buf, "*%s%s%s*", COLOR_CYAN(ch, CL_NORMAL), GET_NAME(i), COLOR_YELLOW(ch, CL_NORMAL));
			else if( iArenaTeam == ARENA_RED )
				sprintf(buf, "*%s%s%s*", COLOR_RED(ch, CL_NORMAL), GET_NAME(i), COLOR_YELLOW(ch, CL_NORMAL));
			else if( iArenaTeam == ARENA_YELLOW )
				sprintf(buf, "*%s%s%s%s%s*", COLOR_YELLOW(ch, CL_NORMAL), COLOR_BOLD(ch, CL_NORMAL), GET_NAME(i), COLOR_NORMAL(ch, CL_SPARSE), COLOR_YELLOW(ch, CL_NORMAL));
			else if( iArenaTeam == ARENA_GREEN )
				sprintf(buf, "*%s%s%s*", COLOR_GREEN(ch, CL_NORMAL), GET_NAME(i), COLOR_YELLOW(ch, CL_NORMAL));
			else
				sprintf(buf, "%s %s", GET_NAME(i), i->player.title.c_str());
		}
		else
		{
			sprintf(buf, "*%s%s%s*", COLOR_RED(ch, CL_NORMAL), GET_NAME(i), COLOR_YELLOW(ch, CL_NORMAL));
		}
	}
	else
	{
		if(GET_RACE(ch) != GET_RACE(i) && GET_LEVEL(ch) < LVL_IMMORT && GET_LEVEL(i) < LVL_IMMORT)
			sprintf(buf, "*%s%s%s*", COLOR_RED(ch, CL_NORMAL), GET_NAME(i), COLOR_YELLOW(ch, CL_NORMAL));
		else
			sprintf(buf, "%s %s", GET_NAME(i), i->player.title.c_str());
	}

	if(AFF_FLAGGED(i, AFF_INVISIBLE))
		strcat(buf, " (invisible)");
	if(!IS_NPC(i) && !i->desc)
		strcat(buf, " (linkless)");
	if(PLR_FLAGGED(i, PLR_WRITING))
		strcat(buf, " (writing)");
	if(GET_POS(i) != POS_FIGHTING)
		strcat(buf, positions[(int) GET_POS(i)]);
	else
	{
		if (FIGHTING(i))
		{
			strcat(buf, " is here, fighting ");

			if (FIGHTING(i) == ch)
				strcat(buf, "YOU!");
			else
			{
				if (i->in_room == FIGHTING(i)->in_room)
					strcat(buf, PERS(FIGHTING(i), ch));
				else
					strcat(buf, "someone who has already left");

				strcat(buf, "!");
			}
		}

		else			/* NIL fighting pointer */
			strcat(buf, " is here struggling with thin air");
	}
	if(GET_POS(i) == POS_SITTING && i->player.sitting_on != NULL)
		sprintf(buf + strlen(buf), " on %s", i->player.sitting_on->GetSDesc());

	if(RIDDEN_BY(i) == ch)
		strcat(buf, ", being ridden by you");

	if(MOUNT(i))
	{
		strcat(buf, ", riding ");
		strcat(buf, GET_NAME(MOUNT(i)));
	}

	//Added by Galnor. Copy over buf and dont show the name if hidden but can see. 9-13-2003 //
	if(AFF_FLAGGED(i, AFF_HIDE) && GET_LEVEL(ch) < LVL_IMMORT)
		sprintf(buf, "You feel the presence of someone nearby...");

	if(GET_LEVEL(ch) >= LVL_IMMORT && AFF_FLAGGED(i, AFF_HIDE))
		strcat(buf, ". [HIDDEN]\r\n");
	else
		strcat(buf, ".\r\n");

	ch->Send(buf);

	if ( PRF_FLAGGED(i, PRF_SOURCE) && GET_SEX(i) == SEX_FEMALE && GET_SEX(ch) == SEX_FEMALE && ch->ChannelingAbility() && i->ChannelingAbility() )
		Act("...$e is surrounded by a halo of light!", FALSE, i, 0, ch, TO_VICT);

	if ( PRF_FLAGGED(i, PRF_SOURCE) && GET_SEX(i) == SEX_MALE && GET_SEX(ch) == SEX_MALE && ch->ChannelingAbility() && i->ChannelingAbility() )
		Act("...$e emits an aura of menace and awe!", FALSE, i, 0, ch, TO_VICT);

	if (AFF_FLAGGED(i, AFF_SANCTUARY))
		Act("...$n glows with a bright light!", FALSE, i, 0, ch, TO_VICT);
}

void listCharacterToCharacter( Character * listy,  Character * ch)
{
	Character *i;

	for (i = listy; i; i = i->next_in_room)
	{
		if (ch != i)
		{
			if (CAN_SEE(ch, i))
				listOneCharacter(i, ch);
		}
	}
}

void performAutoExits(Character * ch)
{
	int door, slen = 0;

	*buf = '\0';

	for (door = 0; door < NUM_OF_DIRS; ++door)
	{
		if( !EXIT(ch,door) || EXIT(ch, door)->IsDisabled() )
			continue;
		if((EXIT(ch, door)->to_room) &&
		(!EXIT(ch, door)->HiddenLevel() || !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED)))
		{
			slen += sprintf(buf + slen, "%c ", LOWER(*dirs[door]));
		}
		else if(EXIT(ch, door) && EXIT(ch, door)->HiddenLevel() && GET_LEVEL(ch) >= LVL_GRGOD)
			slen += sprintf(buf + slen, "%s%s*%c%s%s ", COLOR_BOLD(ch, CL_COMPLETE),COLOR_GREEN(ch, CL_COMPLETE),
			                UPPER(*dirs[door]), COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE));
	}

	ch->Send("%s[ Exits: %s]%s\r\n", COLOR_CYAN(ch, CL_COMPLETE),
	         *buf ? buf : "None! ", COLOR_NORMAL(ch, CL_COMPLETE));
}

ACMD(do_exits)
{
	int door;

	*buf = '\0';

/*	if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		ch->Send("You can't see a damned thing, you're blind!\r\n");
		return;
	}
*/
	for (door = 0; door < NUM_OF_DIRS; ++door)
		if (EXIT(ch, door) && EXIT(ch, door)->to_room)
			if (!EXIT(ch, door)->HiddenLevel() || !EXIT_FLAGGED(EXIT(ch, door), EX_CLOSED))
			{
				if( EXIT(ch,door)->IsDisabled() )
					continue;
				if (GET_LEVEL(ch) >= LVL_IMMORT)
					sprintf(buf2, "%-5s - [%5d] %s\r\n", dirs[door],
					        EXIT(ch, door)->to_room->vnum,
					        EXIT(ch, door)->to_room->name);
				else
				{
					sprintf(buf2, "%-5s - ", dirs[door]);

					if (IS_DARK(EXIT(ch, door)->to_room) && !CAN_SEE_IN_DARK(ch))
						strcat(buf2, "Too dark to tell\r\n");

					else
					{
						strcat(buf2, EXIT(ch, door)->to_room->name);
						strcat(buf2, "\r\n");
					}
				}

				strcat(buf, StringUtil::cap(buf2));
			}

	ch->Send("Obvious exits:\r\n");

	if (*buf)
		ch->Send(buf);

	else
		ch->Send(" None.\r\n");
}

//Returns the # of tracks a player can see, based on certain conditions.
//This function will also set full_view to true or false to see if player
//can see names over races of who is being tracked.
int Character::TrackLines(Room *room, bool auto_track, bool &full_view)
{
	full_view = false;
	// Thieves receive less indoor/city tracks & nothing outdoors, always same.
	if( (SECT(room) == SECT_INSIDE || SECT(room) == SECT_CITY) )
	{
		//Must not be mounted.
		if( /*!MOUNT(this) && */( IS_THIEF(this) || IS_GREYMAN(this) ) && !auto_track )
			return (this->GetSkillLevel(SKILL_TRACK) - 2);
		else if( ( ( IS_THIEF(this) || IS_GREYMAN(this) ) && auto_track ) || !auto_track )
			return (this->GetSkillLevel(SKILL_TRACK) - 2) / 2;
		return 0;
	}
	//A normal track
	else if(!auto_track)
	{
		full_view = (this->GetSkillLevel(SKILL_TRACK) >= 7);
		if(this->GetSkillLevel(SKILL_TRACK))
			return MAX(1, this->GetSkillLevel(SKILL_TRACK) - 1);
	}
	//Primary autotrack sectors for race(non thieves).
	else if((IS_WARRIOR(this)&&(SECT(room) == SECT_FIELD||SECT(room) == SECT_ROAD||SECT(room) == SECT_HILLS))
	|| ((IS_RANGER(this) || IS_FADE(this) || IS_BLADEMASTER(this) || IS_OGIER(this)) && (SECT(room) == SECT_FOREST || SECT(room) == SECT_MOUNTAIN))  )
	{
		full_view = (this->GetSkillLevel(SKILL_TRACK) >= 7);
		return (this->GetSkillLevel(SKILL_TRACK) - 2) /*/ (MOUNT(this) ? 2 : 1)*/;
	}
	//Secondary warrior autotrack sectors.
	else if(IS_WARRIOR(this) && (SECT(room) == SECT_FOREST || SECT(room) == SECT_MOUNTAIN))
	{
		return MIN(2, (this->GetSkillLevel(SKILL_TRACK) - 2) / (MOUNT(this) ? 2 : 1));
	}
	else if((IS_RANGER(this) || IS_FADE(this) || IS_BLADEMASTER(this) || IS_OGIER(this)) && (SECT(room) == SECT_FIELD
	|| SECT(room) == SECT_ROAD || SECT(room) == SECT_HILLS))
	{
		return MIN(3, (this->GetSkillLevel(SKILL_TRACK) - 2) / (MOUNT(this) ? 2 : 1));
	}
	return 0;
}

void Character::PrintTracks(Room *room, bool auto_track)
{
	bool full_view = false;
	int C = 0;
	if(!room)
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(this)), TRUE,
		       "%s attempting to track in a non-existing room.", GET_NAME(this));
		return;
	}
	std::list<Track *>::iterator track;
	int num_can_see = this->TrackLines(room, auto_track, full_view);

	for(track = room->Tracks.begin();track != room->Tracks.end() && num_can_see > 0;++track, --num_can_see)
	{
		C++;
		this->Send( (*track)->ToString( full_view ).c_str());
	}

	if(!C && !auto_track)
		this->Send("You do not see any tracks here.");
}

//Galnor: 2-25-2005	Print the tracks in a room to a player, based on their input, which goes to name.
void Character::PrintTracks(Room *room, std::string name)
{
	std::list<Track *>::iterator iter;
	bool full_view;
	int C = 0;

	if(!room)
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(this)), TRUE,
		       "%s attempting to track in a non-existing room with rnum of %d", room);
		return;
	}

	if(!this->CanTrack(room))
		return;

	int num_can_see = this->TrackLines(room, false, full_view);

	for(iter = room->Tracks.begin();
	        iter != room->Tracks.end() && num_can_see > 0;
	        iter++)
	{
		Track *track = (*iter);
		if(!str_cmp(name, "human") && track->race != RACE_HUMAN)
			continue;
		else if(!str_cmp(name, "trolloc") && (track->race != RACE_TROLLOC
			|| track->ch_class == CLASS_FADE || track->ch_class == CLASS_DREADLORD
		    || track->ch_class == CLASS_GREYMAN))
			continue;
		else if(!str_cmp(name, "horse") && !(track->mount))
			continue;
		else if(!str_cmp(name, "aiel") && track->race != RACE_SEANCHAN)
			continue;
		else if(!str_cmp(name, "seanchan") && track->race != RACE_AIEL)
			continue;
		else if( (!str_cmp(name, "fade") || !str_cmp(name, "myrddraal") || !str_cmp(name, "halfman"))
		         && track->ch_class != CLASS_FADE)
			continue;
		else if(!str_cmp(name, "dreadlord") && track->ch_class != CLASS_DREADLORD)
			continue;
		else if( (!str_cmp(name, "greyman") || !str_cmp(name, "soulless")) && track->ch_class != CLASS_GREYMAN)
			continue;
		//Whoever said it would be PRETTY?!
		else if(!isname(name, track->alias) && !name.empty() && str_cmp(name, "human") && str_cmp(name, "trolloc")
		        && str_cmp(name, "aiel") && str_cmp(name, "seanchan") && str_cmp(name, "fade")
		        && str_cmp(name, "dreadlord") && str_cmp(name, "soulless") && str_cmp(name, "horse"))
			continue;
		else
		{
			--num_can_see;
			C++;
			//At this point we have a match
//			this->Send(track->ToString( full_view ).c_str());
			this->Send("You see %s tracks of %s leaving %s.\r\n", track->AgeString().c_str(),
			           this->GetSkillLevel(SKILL_TRACK) >= 7 ? track->name.c_str() : track->NameString().c_str(), dirs[track->direction]);
		}
	}

	if(!C)
		this->Send("You do not see any tracks here.");
}
std::string Track::ToString(bool full_view)
{
	std::stringstream TheTrack;

	TheTrack << "You see " << this->AgeString() << " tracks of " <<
		(full_view ? this->name : this->NameString()) << " leaving "
		<< dirs[this->direction] << "." << std::endl;
	return TheTrack.str();
}

/* Galnor: 02-25-2005 - Return the time in minutes since the track was layed. */
time_t Track::Age()
{
	return ((time(0) - laytime.getTime()) / 60);
}

/* Galnor: 02-25-2006 - Return the appropriate string for a track based on its age. */
std::string Track::AgeString()
{
	time_t age = Age();

	if(age >= 48)
		return "two days old";
	else if(age >= 24)
		return "day old";
	else if(age >= 20)
		return "almost day old";
	else if(age >= 12)
		return "half day old";
	else if(age >= 9)
		return "faint";
	else if(age >= 5)
		return "fairly recent";
	else if(age >= 3)
		return "very recent";
	else if(age >= 1)
		return "almost fresh";
	else if(age == 0)
		return "fresh";
	else
	{
		MudLog(BRF, LVL_APPR, TRUE, "SYSERR: Track for %s has an age of %d.", name.c_str(), age);
		return "BUGGY AGED";
	}
}

std::string Track::NameString()
{
	if( this->mount_ridden )//Always an NPC & mount, check before mount & NPC
		return "a ridden mount";
	else if( this->mount )//Always an NPC, check before NPC
		return "a mount";
	else if( this->npc && race != RACE_OTHER && race != RACE_ANIMAL )
		return "a humanoid";
	else if(ch_class == CLASS_FADE)
		return "a fade";
	else if(ch_class == CLASS_DREADLORD)
		return "a dreadlord";
	else if(ch_class == CLASS_GREYMAN)
		return "a greyman";
	else if(race == RACE_HUMAN)
		return "a human";
	else if(race == RACE_TROLLOC)
		return "a trolloc";
	else if(race == RACE_SEANCHAN)
		return "a seanchan";
	else if(race == RACE_AIEL)
		return "an aiel";
	else if(race == RACE_OTHER || race == RACE_ANIMAL)
		return "a critter";
	else
	{
		return "Error!";
		MudLog(BRF, LVL_APPR, TRUE, "SYSERR: Attempting to return unidentified race name on track for %s.",
		       name.c_str());
	}
}

// 02/02/2009 - Coded by Galnor - this will show the user what is on the other side of the gate.
void Character::LookAtGate( class Gate* gate )
{
	Room* OtherRoom = gate->OtherEnd(this->in_room);//Get the other side of the gate.

	if( OtherRoom == NULL )//This really should never happen, but...
		this->Send("You can't make out what is on the other end of the gate.\r\n");
	else
		this->Send("On the other side of the gate, you see: %s\r\n", OtherRoom->name);
}

/* vvvvvvvvvvvvvvvvvvvvv RHOLLOR 05.15.09 vvvvvvvvvvvvvvvvvv */
/*  do_auto_scan to be used in look_at_room, displays one line of mobs in all visible directions  */
void do_auto_scan(Character *ch, bool typed) {

   Room *room;
   int exit;
   int none_seen = 0;

   if (GET_POS(ch) != POS_STANDING) {
      return;
   }

   for(exit = 0; exit < NUM_OF_DIRS; exit++) {
      if (EXIT(ch, exit) && EXIT(ch, exit)->to_room && ( !EXIT(ch, exit)->HiddenLevel()
      || (!EXIT_FLAGGED(EXIT(ch, exit), EX_CLOSED) ) ) && EXIT(ch,exit)->to_room != ch->in_room)
	  { //There is an exit in this direction && there is a room
         room = ch->in_room->dir_option[exit]->to_room;

		 //Added to the If below to fix a small bug
         if( ( room->people && canSeeCharsInRoom(ch, room) && (!room->IsDark() || CAN_SEE_IN_DARK(ch)))|| EXIT_FLAGGED(EXIT(ch, exit), EX_CLOSED)) {

            ch->Send(COLOR_CYAN(ch, CL_NORMAL));
            ch->Send("%-5s: ",StringUtil::cap(StringUtil::allLower(dirs[exit])));
            ch->Send(COLOR_NORMAL(ch, CL_NORMAL));

            if (EXIT_FLAGGED(EXIT(ch, exit), EX_CLOSED) && !EXIT(ch, exit)->HiddenLevel()
            && EXIT(ch, exit)->keyword)  {
               ch->Send("The %s obstructs your view.\r\n",fname(EXIT(ch, exit)->keyword));
            }

            else if(room->IsDark() && !CAN_SEE_IN_DARK(ch))
            {
               ch->Send("It looks too dark in there to make out anything.\r\n");
            }
            else {
				/* Galnor 07/14/2009 - Bug patch for mounted horses. listOneCharacter() leaves them invisible.
					Therefore if they are to come up as the first mob/player in the room, they will not
					show up. The solution is to skip until we find the first non mounted united in the room
				   
				   Fogel 09/17/2009 - Small fix so invisible immortals are skipped. Previously having an 
				    invisible immortal as the first character in the room would cause nothing to be displayed,
					so now we loop through all characters first to check if any can be seen.
				*/
				Character *Target;
				for(Target = room->people;Target && RIDDEN_BY(Target);Target = Target->next_in_room);
				if (Target != NULL) {
					for(Character *peep = Target; peep; peep = peep->next_in_room)
					{
						if (CAN_SEE(ch, peep))
						{
							listOneCharacter(peep, ch);
							break;
						}
					}
               }
            }
         }
         if( !room->people || !CAN_SEE(ch,room->people) )
            none_seen++;
      }
      else { none_seen++; }
   }

   if (none_seen == NUM_OF_DIRS && typed == true) {

      ch->Send("There does not appear to be anything beyond this location.\r\n");

   }

   return;

}

/* ^^^^^^^^^^^^^^^^^^^^ RHOLLOR 05.15.09 ^^^^^^^^^^^^^^^^^ */
void look_at_room(Character * ch, int ignore_brief)
{
	if (!ch->desc)
		return;

	if(ch->in_room < 0)
	{
		MudLog(BRF, MAX(GET_INVIS_LEV(ch), LVL_APPR), TRUE, "Player %s's room is less than 0!", GET_NAME(ch));
		ch->RemoveFromRoom();
		char sRoom[5];
		strcpy(sRoom,"1");
		ch->MoveToRoom(FindTargetRoom(ch, sRoom));
	}

	if (ch->in_room->IsDark() && !CAN_SEE_IN_DARK(ch))
	{
		ch->Send("It is pitch black...\r\n");
		return;
	}

/*	else if (AFF_FLAGGED(ch, AFF_BLIND))
	{
		ch->Send("You see nothing but infinite darkness...\r\n");
		return;
	}
*/	else if(ch->dizzy_time && TAINT_CALC(ch))
	{
		ch->Send("You are too dizzy to make out anything!\r\n");
		return;
	}

	ch->Send(COLOR_CYAN(ch, CL_NORMAL));

	if(ch->dizzy_time && TAINT_CALC(ch))
		ch->Send("Too blurred to see!");
	else if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_ROOMFLAGS))
	{
		sprintbit((long)ch->in_room->room_flags, (const char**)room_bits, buf, " ", "", "");
		ch->Send("[%5d] %s [ %s]", ch->in_room->vnum,
				ch->in_room->name, buf);
	}
	else
		ch->Send(ch->in_room->name);

	ch->Send(COLOR_NORMAL(ch, CL_NORMAL));
	ch->Send("\r\n");

	if ((!IS_NPC(ch) && !PRF_FLAGGED(ch, PRF_BRIEF)) || ignore_brief ||
	ROOM_FLAGGED(ch->in_room, ROOM_DEATH))
	{
		if(AFF_FLAGGED(ch, AFF_BLIND))
			ch->Send("You can't seem to make out the details of your surroundings.\r\n");
		else
			ch->Send(ch->in_room->description);
	}

	/* autoexits */
	if (PRF_FLAGGED(ch, PRF_AUTOEXIT))
		performAutoExits(ch);
	/* vvvvvvvvv RHOLLOR 05.15.09 vvvvvvvvv */
	/*  Adds autoscan to thieves when they perform look_at_room  */
	if ( ( GET_CLASS(ch) == CLASS_THIEF || IS_GREYMAN(ch) ) && !PRF_FLAGGED(ch, PRF_AUTOSCAN) )
		do_auto_scan(ch,false);
	/* ^^^^^^^^^ RHOLLOR 05.15.09 ^^^^^^^^^ */
	if(!AFF_FLAGGED(ch, AFF_BLIND))
		ch->PrintTracks(ch->in_room, true);

	/* now list characters & objects */
	ch->Send(COLOR_GREEN(ch, CL_NORMAL));
	listObjectToCharacter(ch->in_room->contents, ch, 0, FALSE);

	std::list<Gate*> TempGateList = ch->in_room->GetGates();
	for(std::list<Gate *>::iterator i = TempGateList.begin();i != TempGateList.end();++i)
	{
		ch->Send("A strange gateway is here, leading to an unknown destination.\r\n");
	}

	ch->Send(COLOR_YELLOW(ch, CL_NORMAL));
	listCharacterToCharacter(ch->in_room->people, ch);

	ch->Send(COLOR_NORMAL(ch, CL_NORMAL));
}

void lookInDirection(Character * ch, int dir)
{
	int hid;

	if (EXIT(ch, dir))

	{
		hid = EXIT(ch, dir)->HiddenLevel();


		if (EXIT(ch, dir)->general_description)
			ch->Send(EXIT(ch, dir)->general_description);

		if (EXIT_FLAGGED(EXIT(ch, dir), EX_CLOSED) && EXIT(ch, dir)->keyword && !hid)
		{
			ch->Send("The %s is closed.\r\n", fname(EXIT(ch, dir)->keyword));
		}

		else if (EXIT_FLAGGED(EXIT(ch, dir), EX_ISDOOR) && EXIT(ch, dir)->keyword &&
		         !EXIT_FLAGGED(EXIT(ch, dir), EX_CLOSED))
		{
			ch->Send("The %s is open.\r\n", fname(EXIT(ch, dir)->keyword));
		}

	}

	else
		ch->Send("Nothing special there...\r\n");
}



void lookInObject(Character * ch, char *arg, Object *obj)
{
	Character *dummy = NULL;
	int amt, bits;

	if ( (!arg || !*arg)  && !obj)
	{
		ch->Send("Look in what?\r\n");
		return;
	}
	if (!obj && !(bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM |
	                               FIND_OBJ_EQUIP, ch, &dummy, &obj)))
	{
		ch->Send("There doesn't seem to be %s %s here.\r\n", AN(arg), arg);
		return;
	}
	//At this point, we know we have a valid object.
	if (GET_OBJ_TYPE(obj) == ITEM_CONTAINER || IS_SHEATH(obj))
	{
		if (obj->IsClosed() && !IS_SHEATH(obj))
			ch->Send("It is closed.\r\n");
		else
		{
			ch->Send(fname(obj->getName()));
			if( obj->carried_by )
				ch->Send(" (carried): \r\n");
			else if( obj->in_room )
				ch->Send(" (here): \r\n");
			else if( obj->worn_by )
				ch->Send(" (used): \r\n");
			if(obj->Money)
				ch->Send("%s%s%d%s Gold, %s%s%d%s Silver, %s%d%s Copper\r\n", COLOR_BOLD(ch, CL_COMPLETE), COLOR_YELLOW(ch, CL_COMPLETE),
				         CalcGold(obj->Money), COLOR_NORMAL(ch, CL_COMPLETE), COLOR_BOLD(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE),
				         CalcSilver(obj->Money), COLOR_NORMAL(ch, CL_COMPLETE), COLOR_YELLOW(ch, CL_COMPLETE), CalcCopper(obj->Money),
				         COLOR_NORMAL(ch, CL_COMPLETE));
			listObjectToCharacter(obj->contains, ch, 2, TRUE);
		}
	}

	else
	{
		/* item must be a fountain or drink container */
		if (obj->GetTotalVal1() <= 0)
			ch->Send("It is empty.\r\n");

		else
		{
			if (GET_OBJ_VAL(obj,0) <= 0 || GET_OBJ_VAL(obj,1) > GET_OBJ_VAL(obj, 0))
				ch->Send("Its contents seem somewhat murky.\r\n"); /* BUG */
			else
			{
				amt = (GET_OBJ_VAL(obj, 1) * 3) / GET_OBJ_VAL(obj, 0);
				ch->Send("It's %sfull of a %s liquid.\r\n", fullness[amt], color_liquid[GET_OBJ_VAL(obj, 2)]);
			}
		}
	}
}

char *findExtraDescription(char *word, struct extra_descr_data * listy)
{
	struct extra_descr_data *i;

	for (i = listy; i; i = i->next)
		if (isname(word, i->keyword))
			return (i->description);

	return NULL;
}

/*
 * Given the argument "look at <target>", figure out what object or Character
 * matches the target.  First, see if there is another char in the room
 * with the name.  Then check local objs for exdescs.
 */

/*
 * BUG BUG: If fed an argument like '2.bread', the extra description
 *          search will fail when it works on 'bread'!
 * -gg 6/24/98 (I'd do a fix, but it's late and non-critical.)
 */
void lookAtTarget(Character * ch, char *arg)
{
	int bits, found = FALSE, j;
	char temp_arg[MAX_INPUT_LENGTH], *ptr;
	Character *found_char = NULL;
	Object *obj = NULL, *found_obj = NULL;
	Gate *found_gate = NULL;
	char *desc;

	if (!ch->desc)
		return;

	if (!*arg)
	{
		ch->Send("Look at what?\r\n");
		return;
	}

	bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP |
	                    FIND_CHAR_ROOM, ch, &found_char, &found_obj);

	/* We use a temp_arg here because some functions need the 'arg' parameter to stay
	   pure. If we call GetNumber() below, it will change the value of 'arg', thus
	   potentially causing issues in outer functions. */
	strcpy(temp_arg, arg);
	ptr = reinterpret_cast<char*>(&temp_arg);
	/* Also search the gates in the room */
	int nr = GetNumber(&ptr) - 1;

	if( !str_cmp(temp_arg, "gate" ) )
	{
		std::list<Gate*> GatesInRoom = GateManager::GetManager().GetGatesInRoom(ch->in_room);
		for(std::list<Gate*>::iterator gIter = GatesInRoom.begin();gIter != GatesInRoom.end() && nr >= 0;++gIter, --nr)
		{
			if( nr == 0 )//found by number
			{
				found_gate = (*gIter);
				break;
			}
		}
	}
	if( found_gate != NULL )
	{
		ch->LookAtGate( found_gate );
		return;
	}

	/* Is the target a character? */
	if (found_char != NULL)
	{
		lookAtCharacter(found_char, ch);

		if (ch != found_char)
		{
			if (CAN_SEE(found_char, ch))
				Act("$n looks at you.", TRUE, ch, 0, found_char, TO_VICT);
			Act("$n looks at $N.", TRUE, ch, 0, found_char, TO_NOTVICT);
		}

		return;
	}

	/* Does the argument match an extra desc in the room? */
	if ((desc = findExtraDescription(arg, ch->in_room->ex_description)) != NULL)
	{
		page_string(ch->desc, desc, FALSE);
		return;
	}

	/* Does the argument match an extra desc in the char's equipment? */
	for (j = 0; j < NUM_WEARS && !found; j++)
		if (GET_EQ(ch, j) && CAN_SEE_OBJ(ch, GET_EQ(ch, j)))
			if ((desc = findExtraDescription(arg, GET_EQ(ch, j)->GetExDesc())) != NULL)
			{
				ch->Send(desc);
				found = TRUE;
			}

	/* Does the argument match an extra desc in the char's inventory? */
	for (obj = ch->carrying; obj && !found; obj = obj->next_content)
	{
		if (CAN_SEE_OBJ(ch, obj))
			if ((desc = findExtraDescription(arg, obj->GetExDesc())) != NULL)
			{
				ch->Send(desc);
				found = TRUE;
			}
	}


	/* Does the argument match an extra desc of an object in the room? */
	for (obj = ch->in_room->contents; obj && !found; obj = obj->next_content)
		if (CAN_SEE_OBJ(ch, obj))
			if ((desc = findExtraDescription(arg, obj->ex_description)) != NULL)
			{
				ch->Send(desc);
				found = TRUE;
			}


	/* If an object was found back in generic_find */
	if (bits)
	{

		if (!found)
			showObjectToCharacter(found_obj, ch, 5, 1);	/* Show no-description */

		else
			showObjectToCharacter(found_obj, ch, 6, 1);	/* Find hum, glow etc */
	}

	else if (!found)
		ch->Send("You do not see that here.\r\n");
}

ACMD(do_look)
{
	char arg2[MAX_INPUT_LENGTH], arg[MAX_INPUT_LENGTH];
	int look_type;

	if (!ch->desc)
		return;

	if (GET_POS(ch) < POS_SLEEPING)
		ch->Send("You can't see anything but stars!\r\n");

/*	else if (AFF_FLAGGED(ch, AFF_BLIND))
		ch->Send("You can't see a damned thing, you're blind!\r\n");*/

	else if (IS_DARK(ch->in_room) && !CAN_SEE_IN_DARK(ch))
	{
		ch->Send("It is pitch black...\r\n");
		listCharacterToCharacter(ch->in_room->people, ch);	/* glowing red eyes */
	}

	else
	{
		HalfChop(argument, arg, arg2);

		if (subcmd == SCMD_READ)
		{
			if (!*arg)
				ch->Send("Read what?\r\n");
			else
				lookAtTarget(ch, arg);

			return;
		}

		if (!*arg)			/* "look" alone, without an argument at all */
			look_at_room(ch, 1);
		else if (IsAbbrev(arg, "in"))
			lookInObject(ch, arg2);
		else if ((look_type = search_block(arg, (const char **) dirs, FALSE)) >= 0)
			lookInDirection(ch, look_type);
		else if (IsAbbrev(arg, "at"))
			lookAtTarget(ch, arg2);
		else
			lookAtTarget(ch, arg);
	}
}

ACMD(do_examine)
{
	int bits;
	char arg[MAX_INPUT_LENGTH];
	Character *tmp_char;
	Object *tmp_object;

	OneArgument(argument, arg);

	if (!*arg)
	{
		ch->Send("Examine what?\r\n");
		return;
	}

	lookAtTarget(ch, arg);

	bits = generic_find(arg, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_CHAR_ROOM | FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

	if (tmp_object)
	{
		if ((GET_OBJ_TYPE(tmp_object) == ITEM_DRINKCON) ||
		        (GET_OBJ_TYPE(tmp_object) == ITEM_FOUNTAIN) ||
		        (GET_OBJ_TYPE(tmp_object) == ITEM_CONTAINER) ||
		        (IS_OBJ_STAT(tmp_object, ITEM_SWORD_SHEATH)) ||
		        (IS_OBJ_STAT(tmp_object, ITEM_DAGGER_SHEATH))||
				(IS_OBJ_STAT(tmp_object, ITEM_SPEAR_SHEATH)))
		{
			ch->Send("When you look inside, you see:\r\n");
			lookInObject(ch, arg, tmp_object);
		}
	}
}

ACMD(do_gold)
{
	ch->Send("You have: %d %s%sGold%s, %d %s%sSilver%s, %d %sCopper%s.\r\n", ch->Gold(), COLOR_BOLD(ch, CL_NORMAL),
	         COLOR_YELLOW(ch, CL_NORMAL), COLOR_NORMAL(ch, CL_NORMAL), ch->Silver(), COLOR_BOLD(ch, CL_NORMAL),
	         COLOR_CYAN(ch, CL_NORMAL), COLOR_NORMAL(ch, CL_NORMAL), ch->Copper(), COLOR_YELLOW(ch, CL_NORMAL),
	         COLOR_NORMAL(ch, CL_NORMAL));
}

ACMD(do_stat)
{
	char arg[MAX_INPUT_LENGTH];
	TwoArguments(argument, arg, buf);

	std::string ClassName = ch->ClassName();
	std::vector<bool> invertedAffects = getInvertedAffects(ch);

	if( ClassName.length() ) ClassName = " " + ClassName;

	ch->Send("You are a %s%d%s year old %s%s %s%s%s.\r\n",
			COLOR_CYAN(ch, CL_COMPLETE), GET_AGE(ch), COLOR_NORMAL(ch, CL_COMPLETE),
			COLOR_CYAN(ch, CL_COMPLETE), StringUtil::allLower((const char*)genders[GET_SEX(ch)]), ch->RaceName().c_str(),
			ClassName.c_str(), COLOR_NORMAL(ch, CL_COMPLETE));

	if(!IS_NPC(ch))
	{
		if(IS_BLADEMASTER(ch))
			ch->Send("Your mood is currently: %s%s%s, your stance is: %s%s%s\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), MoodString(ch->PlayerData->mood).c_str(),
			 COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE), StanceString(ch->stance).c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
		else
			ch->Send("Your mood is currently: %s%s%s\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), MoodString(ch->PlayerData->mood).c_str(),
			 COLOR_NORMAL(ch, CL_COMPLETE));
	}

	if(GET_WARNINGS(ch))
		ch->Send("You have %s%d%s warning%s.\r\n",
		         COLOR_CYAN(ch, CL_COMPLETE), GET_WARNINGS(ch), COLOR_NORMAL(ch, CL_COMPLETE), GET_WARNINGS(ch) == 1 ? "" : "s");

	if (age(ch)->month == 0 && age(ch)->day == 0)
		ch->Send("  It's your birthday today.\r\n");


	ch->Send("Your height is %s%d%s feet, %s%d%s inches, weight %s%d%s pounds, weight carried %s%.1f%s lbs, worn: %s%.1f%s lbs.\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), GET_HEIGHT(ch) / 12, COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), GET_HEIGHT(ch) % 12, COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), GET_WEIGHT(ch), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->CarriedWeight(), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->WornWeight(), COLOR_NORMAL(ch, CL_COMPLETE));

	ch->Send("Offensive Bonus: %s%d%s Dodge Bonus: %s%d%s, Parry Bonus: %s%d%s\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), ch->Offense(), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->Dodge()  , COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->Parry()  , COLOR_NORMAL(ch, CL_COMPLETE));

	ch->ListWarrants();

	if(PRF_FLAGGED(ch, PRF_STATTED))
	{
		ch->Send("Your abilities are: Str: %s%d%s, Int: %s%d%s, Wis: %s%d%s, Dex: %s%d%s, Con: %s%d%s\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), ch->GetStr(), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->GetInt(), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->GetWis(), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->GetDex(), COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->GetCon(), COLOR_NORMAL(ch, CL_COMPLETE));
	}
	ch->Send("You have died %s%d%s times, %s%d%s times to other players, %s%d%s times to monsters, and killed %s%d%s other worthy players.\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), ch->pk_deaths + ch->mob_deaths, COLOR_NORMAL(ch, CL_COMPLETE),
	         COLOR_CYAN(ch, CL_COMPLETE), ch->pk_deaths, COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE),
	         ch->mob_deaths, COLOR_NORMAL(ch, CL_COMPLETE), COLOR_CYAN(ch, CL_COMPLETE), ch->pks, COLOR_NORMAL(ch, CL_COMPLETE));
	ch->Send("Your armor absorbs %s%d%s%% damage on average.\r\n",
	         COLOR_CYAN(ch, CL_COMPLETE), ch->Absorb(), COLOR_NORMAL(ch, CL_COMPLETE));

	ch->Send("You are affected by:\r\n");

	for (affected_type *af = ch->affected; af; af = af->next)
		if(!invertedAffects.at(af->bitvector))
			ch->Send("%-15s %2d\r\n", affected_bits[af->bitvector], af->duration + 1);

	if(PRF_FLAGGED(ch, PRF_BUILDWALK))
		ch->Send("BUILDWALK\r\n");
	if(ch->dizzy_time)
		ch->Send("DIZZINESS\r\n");
}

ACMD(do_inventory)
{
	ch->Send("You are carrying:\r\n");
	listObjectToCharacter(ch->carrying, ch, 1, TRUE);

}

ACMD(do_equipment)
{
	int i, found = 0;
	char msg[MAX_INPUT_LENGTH];

	ch->Send("You are using:\r\n");

	for (i = 0; i < NUM_WEARS; i++)
	{
		if (GET_EQ(ch, i))
		{
            if(!IS_OBJ_STAT(GET_EQ(ch, i), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT) { //RHOLLOR 05.03.09 fix showing INVIS items on equipment
                if(i == WEAR_WIELD && IS_OBJ_STAT(GET_EQ(ch, i), ITEM_TWO_HANDED))
                    strcpy(msg, "<wielded with two hands>    ");

                else
                    strcpy(msg, where[i]);

                ch->Send(msg);
                showObjectToCharacter(GET_EQ(ch, i), ch, 1, 1);
                found = TRUE;
            }
		}
	}

	if (!found)
		ch->Send(" Nothing.\r\n");

}

ACMD(do_time)
{
	const char *suf;
	int weekday, day;
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	sprintf(buf, "It is %d o'clock %s, on ",
	        ((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
	        ((time_info.hours >= 12) ? "pm" : "am"));

	/* 35 days in a month */
	weekday = ((35 * time_info.month) + time_info.day + 1) % 7;

	strcat(buf, weekdays[weekday]);
	strcat(buf, "\r\n");
	ch->Send(buf);

	day = time_info.day + 1;	/* day in [1..35] */

	suf = findSuffix(day);

	ch->Send("The %d%s Day of the %s, Year %d.\r\n",
	         day, suf, month_name[(int) time_info.month], time_info.year);

	ch->Send("Server: %s\r\n", asctime(timeinfo));

	ch->Send("The MUD will reboot in %d minutes at %s.\r\n", ((int) (rebootTime.getTime()-time(0)))/60,
		rebootTime.toString().c_str());

	if(GET_LEVEL(ch) >= LVL_GRGOD)
	{
		ch->Send("THE NEXT MUD TICK WILL BE IN %s%s%d%s SECONDS.\r\n",
		         COLOR_BOLD(ch, CL_COMPLETE), COLOR_GREEN(ch, CL_COMPLETE),
		         (60 - (Seconds % 60)), COLOR_NORMAL(ch, CL_COMPLETE));
	}
}

std::string MakeWeatherAdj(int i)
{
	std::string s;
	if (i < 20)
		s = "barely";
	else if (i < 40)
		s = "slightly";
	else if (i < 60)
		s = "fairly";
	else if (i < 80)
		s = "rather";
	else if (i < 100)
		s = "very";
	else if (i >= 100)
		s = "impossibly";
	else
		s = "ERROR";
	return s;
}

std::string MakeTemperatureAdj(int i)
{
	std::string s;


	if (i < -80)
		s = "blastedly cold";
	else if (i < -60)
		s = "extremely cold";
	else if (i < -40)
		s = "freezing cold";
	else if (i < -20)
		s = "cold";
	else if (i < 0)
		s = "nippy";
	else if (i < 20)
		s = "chilly";
	else if (i < 40)
		s = "warm";
	else if (i < 60)
		s = "hot";
	else if (i < 80)
		s = "very hot";
	else if (i < 100)
		s = "boiling hot";
	else if (i >= 100)
		s = "impossibly hot";
	else
		s = "ERROR";
	return s;
}

ACMD(do_weather)
{
	Weather * weather = ch->in_room->GetZone()->GetWeather();
	if (OUTSIDE(ch))
	{
		// This shows sky info.
		ch->Send("%s\r\n", weather_patterns[weather->get_sky()]);

		ch->Send("The air feels %s wet and %s.\r\n", MakeWeatherAdj(weather->get_precipitation()).c_str(), MakeTemperatureAdj(weather->get_temp()).c_str());

		if (weather->get_storm()->get_type() != ST_NONE)
			ch->Send("%s looms on the horizon.\r\n", weather_storm_names[weather->get_storm()->get_type()]);

		if (weather->get_event()->get_event_type() != WE_NONE)
			ch->Send("\r\nYou are in the midst of %s.\r\n", weather_event_names[weather->get_event()->get_event_type()]);

		if (weather->get_visibility() != 0)
		{
			// 10 is blind, 0 is normal
			std::string vis;
			switch (weather->get_visibility())
			{
			case 0:
				vis = "normal";
				break;
			case 1:
				vis = "fair";
				break;
			case 2:
				vis = "fair";
				break;
			case 3:
				vis = "slightly obscured";
				break;
			case 4:
				vis = "slightly obscured";
				break;
			case 5:
				vis = "obscured";
				break;
			case 6:
				vis = "obscured";
				break;
			case 7:
				vis = "hindered";
				break;
			case 8:
				vis = "hindered";
				break;
			case 9:
				vis = "low";
				break;
			case 10:
				vis = "nonexistant";
				break;
			}
			ch->Send("\r\nThe visibility is %s.\r\n", vis.c_str());
		}
	}
	else
		ch->Send("There's no weather to see, here!\r\n");
	if(GET_LEVEL(ch) >= LVL_GOD)
	{
		ch->Send("Imm info\r\nPrecipitation: %d\r\nTemperature: %d\r\nSky: %d\r\nSun: %d\r\nVisibility decrease: %d\r\n",
		weather->get_precipitation(), weather->get_temp(), weather->get_sky(),
		weather->get_sun(), weather->get_visibility());
	}
}

/*                                                          *
 * This hardly deserves it's own function, but I see future *
 * expansion capabilities...                                *
 *                                               TR 8-18-98 *
 *                                          Fixed:  8-25-98 */
int show_on_who_list(Descriptor *d)
{
	if ((STATE(d) != CON_PLAYING) && (STATE(d) != CON_MEDIT)    &&
		(STATE(d) != CON_OEDIT)   && (STATE(d) != CON_REDIT)    &&
		(STATE(d) != CON_SEDIT)   && (STATE(d) != CON_ZEDIT)    &&
		(STATE(d) != CON_AEDIT)   &&
		(STATE(d) != CON_CEDIT)   && (STATE(d) != CON_SGEDIT)   &&
		(STATE(d) != CON_STATEDIT)&& (STATE(d) != CON_CLEDIT)   &&
		(STATE(d) != CON_WEDIT)   && (STATE(d) != CON_WEDIT)	&&
		(STATE(d) != CON_AUCTION) && (STATE(d) != CON_JEDIT)
		)
		return 0;
	else
		return 1;
}

#define USERS_FORMAT \
"format: users [-l minlevel[-maxlevel]] [-n name] [-h host] [-c classlist] [-o] [-p]\r\n"

ACMD(do_users)
{
	Character *tch;
	Descriptor *d;
	int num_can_see = 0;
	int idle;
	std::string name, state, timestamp, host;

	ch->Send("Num   Name         State        Idl  Login@             Site\r\n");
	ch->Send("--- -------     ------------    --- --------    ------------------------\r\n");

	for (d = descriptor_list; d; d = d->next)
	{
		if (STATE(d) == CON_PLAYING)
		{
			if (d->original)
				tch = d->original;
			else if (!(tch = d->character))
				continue;
			if (GET_INVIS_LEV(ch) > GET_LEVEL(ch))
				continue;
		}

		timestamp = d->loginTime.toString().substr(11,8);
		state = (STATE(d) == CON_PLAYING && d->original) ? "Switched" : connected_types[STATE(d)];

		if (d->character && STATE(d) == CON_PLAYING && GET_LEVEL(d->character) < LVL_GRGOD)
			idle = d->character->player.timer * SECS_PER_MUD_HOUR / SECS_PER_REAL_MIN;
		else
			idle = 0;

		if (d->character && !d->character->player.name.empty())
		{
			if (d->original)
				name = d->original->player.name;
			else
				name = d->character->player.name;
		}
		else
			name = "Undefined";
		host = (*d->host) ? d->host : "Unknown";

		if(d->character ? (CAN_SEE(ch, d->character)) : (1))
		{
			ch->Send("%s%3d  %-12s %-14s %-3d %-8s       %-14s%s\r\n",
				(STATE(d) != CON_PLAYING) ? COLOR_GREEN(ch, CL_SPARSE) : "",
				d->desc_num, name.c_str(), state.c_str(),
				idle, timestamp.c_str(), host.c_str(),
				(STATE(d) != CON_PLAYING) ? COLOR_NORMAL(ch, CL_SPARSE) : "");
			num_can_see++;
		}
	}

	ch->Send("\r\n\n%d visible sockets connected.\r\n", num_can_see);
}

void print_wizards(Character *ch, int level, bool all)
{
	time_t idle = 0;
	size_t total = 0, i = 0;
	Wizard *cur;
	Character *imm = 0;
	std::list< Character* >::iterator i_imm;

	std::list< Character* > Immortals;

	for(cur = wizlist;cur;cur = cur->next)
	{
		if(cur->level == level)
		{
			
			if( (imm = CharacterUtil::loadCharacter(cur->name)) != NULL )
			{
				if(all || !PLR_FLAGGED(imm, PLR_NOWIZLIST))
					Immortals.push_back( imm );
				else
					delete imm;
			}
		}
	}

	total = Immortals.size();
	for(i_imm = Immortals.begin();i_imm != Immortals.end();i_imm = Immortals.erase(i_imm))
	{
		imm = (*i_imm);
		if(!(++i % 5))
			ch->Send("\r\n\t\t");

		/* Print the name in color depending on their idle time. */
		if(GET_LEVEL(ch) >= LVL_GRGOD)
		{
			idle = ((time(0) - LAST_LOGON(imm)) / SECS_PER_REAL_DAY);

			if(idle < 7)
				{}
			else if(idle < 14)
				ch->Send("%s%s", COLOR_BOLD(ch, CL_NORMAL), COLOR_CYAN(ch, CL_NORMAL));
			else if(idle < 21)
				ch->Send("%s%s", COLOR_BOLD(ch, CL_NORMAL), COLOR_YELLOW(ch, CL_NORMAL));
			else if(idle < 28)
				ch->Send("%s%s", COLOR_BOLD(ch, CL_NORMAL), COLOR_GREEN(ch, CL_NORMAL));
			else
				ch->Send("%s%s", COLOR_BOLD(ch, CL_NORMAL), COLOR_RED(ch, CL_NORMAL));
		}

		ch->Send("%s%s%s", imm->player.name.c_str(), NORMAL, ((i < total) ? (", ") : ("")) );

		delete (*i_imm);
	}


	ch->Send("\r\n\n");
}

void print_wizlist(Character *ch, bool all)
{

	ch->Send("Below are a list of all of the Immortals of the Wheel.\r\n\n\n");

	ch->Send("\t\t~Creators~\r\n\t\t");
	print_wizards(ch, LVL_IMPL, all);

	ch->Send("\t\t~Lords of the Wheel~\r\n\t\t");
	print_wizards(ch, LVL_GRGOD, all);

	ch->Send("\t\t~Tellers of the Wheel~\r\n\t\t");
	print_wizards(ch, LVL_GOD, all);

	ch->Send("\t\t~Apprentices of the Wheel~\r\n\t\t");
	print_wizards(ch, LVL_APPR, all);

	ch->Send("\t\t~Builders of the Wheel~\r\n\t\t");
	print_wizards(ch, LVL_BLDER, all);

	ch->Send("\t\t~Immortals of the Wheel~\r\n\t\t");
	print_wizards(ch, LVL_IMMORT, all);
}

/* Generic page_string function for displaying text */
ACMD(do_gen_ps)
{
	skip_spaces(&argument);
	switch (subcmd)
	{
		case SCMD_CREDITS:
			page_string(ch->desc, credits, 0);
			break;
		case SCMD_NEWS:
			page_string(ch->desc, news, 0);
			break;
		case SCMD_WIZLIST:
			print_wizlist(ch, !str_cmp(argument,"all"));
			break;
		case SCMD_HANDBOOK:
			page_string(ch->desc, handbook, 0);
			break;
		case SCMD_POLICIES:
			page_string(ch->desc, policies, 0);
			break;
		case SCMD_MOTD:
			page_string(ch->desc, motd, 0);
			break;
		case SCMD_IMOTD:
			page_string(ch->desc, imotd, 0);
			break;
		case SCMD_CLEAR:
			ch->Send("\033[H\033[J");
			break;
		case SCMD_VERSION:
			break;
		default:
			return;
	}
}

void performMortalWhere(Character * ch, char *arg)
{
	register Character *i;
	register Descriptor *d;
	bool horse = false;
	bool channeler = false;
	int num_trolls = 0, num_horses = 0, counter;

	PlayerClan *cl;

	if (!*arg)
	{
		ch->Send("Players in your Zone\r\n--------------------\r\n");
		for (d = descriptor_list; d; d = d->next)
		{
			if (!show_on_who_list(d))
				continue;

			if ((i = (d->original ? d->original : d->character)) == NULL)
				continue;

			if (!i->in_room || !CAN_SEE(ch, i))
				continue;

			if (ch->in_room->zone != i->in_room->zone)
				continue;

			if (MOUNT(i) && IS_TROLLOC(ch) && !IS_FADE(i) && !IS_DREADLORD(i) && !IS_DREADGUARD(i) &&
			        !IS_FADE(ch) && !IS_DREADLORD(ch) && !IS_DREADGUARD(ch) && GET_LEVEL(i) < LVL_IMMORT)
			{
				++num_horses;
				horse = true;
			}

			if (IS_CHANNELER(i) && GET_LEVEL(i) < LVL_IMMORT && IS_TROLLOC(ch) && IS_FADE(ch))
				channeler = true;

			if(!IS_TROLLOC(ch) && IS_TROLLOC(i) && GET_LEVEL(i) < LVL_IMMORT)
				++num_trolls;

			if(GET_RACE(ch) != GET_RACE(i) && GET_LEVEL(i) < LVL_IMMORT && GET_LEVEL(ch) < LVL_IMMORT)
				continue;

			if(AFF_FLAGGED(i, AFF_HIDE) && GET_LEVEL(ch) < LVL_IMMORT)
				continue;

			ch->Send("%-20s - %s\r\n", GET_NAME(i), i->in_room->name);
		}
	}

	else
	{			/* print only FIRST char, not all. */
		for (i = character_list; i; i = i->next)
		{
			if( i->IsPurged() ) continue;
			if (i->in_room || i == ch)
				continue;

			if (ch->in_room->zone != i->in_room->zone)
				continue;

			if ((GET_INVIS_LEV(i) > GET_LEVEL(ch) || i->in_room->zone != ch->in_room->zone)
			        && GET_LEVEL(ch) < LVL_IMMORT)
				continue;

			if (!isname(arg, i->player.name))
				continue;

			if(GET_RACE(ch) != GET_RACE(i) && GET_LEVEL(i) <= LVL_IMMORT)
				continue;

			ch->Send("%-25s - %s\r\n", GET_NAME(i), i->in_room->name);
			return;
		}

		ch->Send(NOPERSON);
	}

	//NORMAL TROLLOC SMELL BONUS
	if(horse && IS_TROLLOC(ch) && !IS_FADE(ch) && !IS_DREADLORD(ch) && !IS_DREADGUARD(ch) && GET_LEVEL(ch) < LVL_IMMORT)
		ch->Send("You hear and smell horses in the surrounding area.\r\n");

	//DHA'VOL SMELL BONUS
	if(ch->GetClan(CLAN_DHAVOL) && GET_LEVEL(ch) < LVL_IMMORT)
	{
		for(counter = 0;counter < num_horses;++counter)
			ch->Send("a horse			- Nearby\r\n");
	}

	//RANDOM HUMAN TROLL SMELLING BONUS
	if(num_trolls > MiscUtil::random(1, 30))
		ch->Send("You catch a fleeting whiff of the stench of shadowspawn.\r\n");

	//WOLFBROTHER SENSING BONUS
	if(ch->GetClan(CLAN_WOLFBROTHER) && GET_LEVEL(ch) < LVL_IMMORT)
	{
		for(counter = 0;counter < num_trolls;counter++)
		{
			ch->Send("You smell shadowspawn nearby.\r\n");
		}
	}

	//GREYMEN TARGET LOCATING
	if(IS_GREYMAN(ch))
	{
		if(GET_MARKED(ch))
		{
			int distance = GET_MARKED(ch)->in_room->GetZone()->Distance(ch->in_room->GetZone());
			int slope = find_zone_slope(GET_MARKED(ch)->in_room->zone, ch->in_room->zone);

			if(distance <= MAX(4, ( (cl = ch->GetClan(CLAN_SOULLESS)) ? cl->GetRank() / 2 : 1)))
			{
				ch->Send("%s%s%-25s - %s %s%s\r\n", COLOR_BOLD(ch, CL_COMPLETE), COLOR_RED(ch, CL_COMPLETE),
				         GET_NAME(GET_MARKED(ch)), DistanceString(distance).c_str(), loc[slope], COLOR_NORMAL(ch, CL_COMPLETE));
			}

			else
			{
				ch->Send("%s%s%-25s - Somewhere %s%s\r\n", COLOR_BOLD(ch, CL_COMPLETE),
				         COLOR_RED(ch, CL_COMPLETE), GET_NAME(GET_MARKED(ch)),
				         loc[slope], COLOR_NORMAL(ch, CL_COMPLETE));
			}
		}

		else
		{
			ch->Send("%s%sYou have no target marked.%s\r\n", COLOR_BOLD(ch, CL_COMPLETE),
			         COLOR_RED(ch, CL_COMPLETE), COLOR_NORMAL(ch, CL_COMPLETE));
		}
	}

	//FADE CHANNELER SENSING
	if (channeler && IS_FADE(ch) && IS_TROLLOC(ch))
		ch->Send("You sense the presence of channelers nearby.\r\n");

}


void printObjectLocation(std::stringstream &outputBuffer, int num, Object *obj, Character *ch, int recur)
{
	if (num > 0)
		sprintf(buf, "O%3d. %-25s - ", num, obj->GetSDesc());
	else
		sprintf(buf, "%33s", " - ");

	if (obj->in_room)
	{
		sprintf(buf + strlen(buf), "[%5d] %s\r\n", obj->in_room->vnum, obj->in_room->name);
		outputBuffer << buf;
	}

	else if (obj->carried_by)
	{
		sprintf(buf + strlen(buf), "carried by %s\r\n", PERS(obj->carried_by, ch));
		outputBuffer << buf;
	}

	else if (obj->worn_by)
	{
		sprintf(buf + strlen(buf), "worn by %s\r\n", PERS(obj->worn_by, ch));
		outputBuffer << buf;
	}

	else if (obj->in_obj)
	{
		sprintf(buf + strlen(buf), "inside %s%s\r\n", obj->in_obj->GetSDesc(), (recur ? ", which is" : " "));
		outputBuffer << buf;

		if (recur)
			printObjectLocation(outputBuffer, 0, obj->in_obj, ch, recur);
	}

	else
	{
		sprintf(buf + strlen(buf), "in an unknown location\r\n");
		outputBuffer << buf;
	}
}

void performImmortalWhere(Character * ch, char *arg)
{
	register Character *i;
	register Object *k;
	Descriptor *d;
	int num, found = 0;
	char color[300];

	std::stringstream outputBuffer;

	if (!*arg)
	{
		outputBuffer << "Players\r\n-------" << std::endl;

		for (d = descriptor_list; d; d = d->next)
		{
			if (show_on_who_list(d))
			{
				i = (d->original ? d->original : d->character);

				if (i && CAN_SEE(ch, i) && i->in_room)
				{
					if (d->original)
						sprintf(buf, "%-20s - [%5d] %s (in %s)", GET_NAME(i), d->character->in_room->vnum, d->character->in_room->name, GET_NAME(d->character));
					else
					{
						if(IS_TROLLOC(i) && GET_LEVEL(i) < LVL_IMMORT)
							strcpy(color, YELLOW);
						else if(IS_HUMAN(i) && GET_LEVEL(i) < LVL_IMMORT)
							strcpy(color, GREEN);
						else
							strcpy(color, NORMAL);

						sprintf(buf, "%s%-20s - [%5d] %s %s", color, GET_NAME(i), i->in_room->vnum, i->in_room->name, NORMAL);
					}
					outputBuffer << buf << std::endl;
				}
			}
		}
	}

	else
	{
		for (num = 0, i = character_list; i; i = i->next)
		{
			if (!i->IsPurged() && isname(arg, i->player.name) && i->in_room && CAN_SEE(ch, i))
			{
				found = 1;
				sprintf(buf, "M%3d. %-25s - [%5d] %s\r\n", ++num, GET_NAME(i), i->in_room->vnum, i->in_room->name);
				outputBuffer << buf;
			}
		}
		for (num = 0, k = object_list; k; k = k->next)
		{
			if (!k->IsPurged() && isname(arg, k->getName()) && CAN_SEE_OBJ(ch, k))
			{
				found = 1;
				printObjectLocation(outputBuffer, ++num, k, ch, TRUE);
			}
		}
		if (!found)
		{
			ch->Send("Couldn't find any such thing.\r\n");
			return;
		}
	}

	char *cStringOutputBuffer = str_dup(outputBuffer.str().c_str());
	page_string(ch->desc, cStringOutputBuffer, TRUE);
	delete[] cStringOutputBuffer;
}

ACMD(do_where)
{
	char arg[MAX_INPUT_LENGTH];
	OneArgument(argument, arg);

	if (GET_LEVEL(ch) >= LVL_GRGOD)
		performImmortalWhere(ch, arg);

	else
		performMortalWhere(ch, arg);
}

ACMD(do_levels)
{
	int i;

	if (IS_NPC(ch))
	{
		ch->Send("You ain't nothin' but a hound-dog.\r\n");
		return;
	}

	*buf = '\0';

	for (i = 1; i < LVL_IMMORT; ++i)
	{
		sprintf(buf + strlen(buf), "[%2d] %d", i,
		        level_exp(i + 1) - level_exp(i));

		strcat(buf, "\r\n");
	}

	sprintf(buf + strlen(buf), "[%2d] %8d          : Immortality\r\n",
	        LVL_IMMORT, level_exp(LVL_IMMORT));
	page_string(ch->desc, buf, 1);
}



ACMD(do_consider)
{
	Character *victim;
	int diff;

	OneArgument(argument, buf);

	if (!(victim = get_char_room_vis(ch, buf)))
	{
		ch->Send("Consider killing who?\r\n");
		return;
	}

	if (victim == ch)
	{
		ch->Send("Easy!  Very easy indeed!\r\n");
		return;
	}
/****
	if (!IS_NPC(victim))
	{
		ch->Send("Would you like to borrow a cross and a shovel?\r\n");
		return;
	}
****/
	diff = (victim->Difficulty() - ch->Difficulty());

	if( GET_LEVEL(ch) >= LVL_IMMORT ) {
		ch->Send("You: %d\r\n", ch->Difficulty());
		ch->Send("Opp: %d\r\n", victim->Difficulty());
		ch->Send("\r\n");
		ch->Send("Dif: %d\r\n", diff);
	}
	if( diff >= 10000 )
		ch->Send("Start burying your own grave!\r\n");
	else if( diff >= 7500 )
		ch->Send("Are you mad?\r\n");
	else if( diff >= 4000 )
		ch->Send("You'd best be bloody lucky!\r\n");
	else if( diff >= 1000 )
		ch->Send("You'd best come well prepared.\r\n");
	else if( diff >= -1000 )
		ch->Send("It would be a close match.\r\n");
	else if( diff >= -4000 )
		ch->Send("You'd have the upper hand.\r\n");
	else if( diff >= -7500 )
		ch->Send("You could do it with a needle!\r\n");
	else
		ch->Send("Now where did that chicken go?\r\n");
/*****
	if (diff <= -10)
		ch->Send("Now where did that chicken go?\r\n");

	else if (diff <= -5)
		ch->Send("You could do it with a needle!\r\n");

	else if (diff <= -2)
		ch->Send("Easy.\r\n");

	else if (diff <= -1)
		ch->Send("Fairly easy.\r\n");

	else if (diff == 0)
		ch->Send("The perfect match!\r\n");

	else if (diff <= 1)
		ch->Send("You would need some luck!\r\n");

	else if (diff <= 2)
		ch->Send("You would need a lot of luck!\r\n");

	else if (diff <= 3)
		ch->Send("You would need a lot of luck and great equipment!\r\n");

	else if (diff <= 5)
		ch->Send("Do you feel lucky, punk?\r\n");

	else if (diff <= 10)
		ch->Send("Are you mad!?\r\n");

	else if (diff <= 100)
		ch->Send("You ARE mad!\r\n");
*****/
}



ACMD(do_diagnose)
{
	Character *vict;

	OneArgument(argument, buf);

	if (*buf)
	{
		if (!(vict = get_char_room_vis(ch, buf)))
		{
			ch->Send(NOPERSON);
			return;
		}
		else
			diagnoseCharacterToCharacter(vict, ch);
	}
	else
	{
		if (FIGHTING(ch))
			diagnoseCharacterToCharacter(FIGHTING(ch), ch);
		else
			ch->Send("Diagnose who?\r\n");
	}
}


const char *ctypes[] =
    {
        "off",
        "sparse",
        "normal",
        "complete",
        "\n"
    };

ACMD(do_color)
{
	int tp;
	char arg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;

	OneArgument(argument, arg);

	if (!*arg)
	{
		ch->Send("Your current color level is %s.\r\n", ctypes[COLOR_LEV(ch)]);
		return;
	}

	if (((tp = search_block(arg, (const char **) ctypes, FALSE)) == -1))
	{
		ch->Send("Usage: color { Off | Sparse | Normal | Complete }\r\n");
		return;
	}

	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_COLOR_1);
	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_COLOR_2);

	if (tp & 1)
		SET_BIT_AR(PRF_FLAGS(ch), PRF_COLOR_1);
	if (tp & 2)
		SET_BIT_AR(PRF_FLAGS(ch), PRF_COLOR_2);

	ch->Send("Your %scolor%s is now %s.\r\n", COLOR_RED(ch, CL_SPARSE),
	         COLOR_NORMAL(ch, CL_OFF), ctypes[tp]);
}


ACMD(do_toggle)
{
	if (IS_NPC(ch))
		return;

	if ( !ch->PlayerData->wimp_level )
		strcpy(buf2, "OFF");
	else
		sprintf(buf2, "%-3d", ch->PlayerData->wimp_level);

	sprintf(buf,
	        "Hit Pnt Display: %-3s    "
	        "     Brief Mode: %-3s    "
	        " Summon Protect: %-3s\r\n"

	        "   Move Display: %-3s    "
	        "   Compact Mode: %-3s    "
	        "       On Quest: %-3s\r\n"

	        "   Mana Display: %-3s    "
	        "         NoTell: %-3s    "
	        "   Repeat Comm.: %-3s\r\n"

	        " Auto Show Exit: %-3s    "
	        "     Wimp Level: %-3s\r\n"

	        "Narrate Channel: %-3s    "
	        "   Chat Channel: %-3s    "
	        "    Color Level: %-3s\r\n"
			"Immortal Speaks: %-3s\r\n"
			"Global Channel : %-3s\r\n"

	        "      Buildwalk: %-3s\r\n",

	        ONOFF(PRF_FLAGGED(ch, PRF_DISPHP)),
	        ONOFF(PRF_FLAGGED(ch, PRF_BRIEF)),
	        ONOFF(!PRF_FLAGGED(ch, PRF_SUMMONABLE)),

	        ONOFF(PRF_FLAGGED(ch, PRF_DISPMOVE)),
	        ONOFF(PRF_FLAGGED(ch, PRF_COMPACT)),
	        StringUtil::allUpper(StringUtil::yesNo(PRF_FLAGGED(ch, PRF_QUEST))).c_str(),

	        ONOFF(PRF_FLAGGED(ch, PRF_DISPMANA)),
	        ONOFF(PRF_FLAGGED(ch, PRF_NOTELL)),
			StringUtil::allUpper(StringUtil::yesNo((!PRF_FLAGGED(ch, PRF_NOREPEAT)))).c_str(),

	        ONOFF(PRF_FLAGGED(ch, PRF_AUTOEXIT)),
	        buf2,

	        ONOFF(!PRF_FLAGGED(ch, PRF_NONARR)),
	        ONOFF(!PRF_FLAGGED(ch, PRF_NOCHAT)),

	        ctypes[COLOR_LEV(ch)],

			ONOFF(!PRF_FLAGGED(ch, PRF_NOSPEAKS)),
			ONOFF(!PRF_FLAGGED(ch, PRF_NOGLOBAL)),
	        ONOFF(!PRF_FLAGGED(ch, PRF_BUILDWALK)));

	ch->Send(buf);
}


struct sort_struct
{
	int sort_pos;
	byte is_social;
}
*cmd_sort_info = NULL;

int num_of_cmds;


void SortCommands(void)
{
	int a, b, tmp;

	num_of_cmds = 0;

	/*
	* first, count commands (num_of_commands is Actually one greater than the
	* number of commands; it inclues the '\n'.
	*/

	while (complete_cmd_info[num_of_cmds].command[0] != '\n')
		num_of_cmds++;

	/* check if there was an old sort info.. then free it -- aedit -- M. Scott*/
	if (cmd_sort_info)
		delete(cmd_sort_info);

	/* create data array */
	cmd_sort_info = new sort_struct[num_of_cmds];
	memset(cmd_sort_info, 0, sizeof(sort_struct) * num_of_cmds);

	/* initialize it */

	for (a = 1; a < num_of_cmds;++a)
	{
		cmd_sort_info[a].sort_pos = a;
		cmd_sort_info[a].is_social = (complete_cmd_info[a].command_pointer == do_action);
	}

	/* the infernal special case */
	cmd_sort_info[FindCommand("insult")].is_social = TRUE;

	/* Sort.  'a' starts at 1, not 0, to remove 'RESERVED' */
	for (a = 1; a < num_of_cmds - 1;++a)
		for (b = a + 1; b < num_of_cmds;++b)
			if (strcmp(complete_cmd_info[cmd_sort_info[a].sort_pos].command,
			           complete_cmd_info[cmd_sort_info[b].sort_pos].command) > 0)
			{

				tmp = cmd_sort_info[a].sort_pos;
				cmd_sort_info[a].sort_pos = cmd_sort_info[b].sort_pos;
				cmd_sort_info[b].sort_pos = tmp;
			}
}
void destroySortInfo( void )
{
	delete[] cmd_sort_info;
}

ACMD(do_commands)
{
	int no, i, cmd_num;
	int wizhelp = 0, socials = 0;
	Character *vict;
	char arg[MAX_INPUT_LENGTH];

	OneArgument(argument, arg);

	if (*arg)
	{
		if (!(vict = get_char_vis(ch, arg)) || IS_NPC(vict))
		{
			ch->Send("Who is that?\r\n");
			return;
		}

		if (GET_LEVEL(ch) < GET_LEVEL(vict))
		{
			ch->Send("You can't see the commands of people above your level.\r\n");
			return;
		}

	}
	else
		vict = ch;

	if (subcmd == SCMD_SOCIALS)
		socials = 1;
	else if (subcmd == SCMD_WIZHELP)
		wizhelp = 1;

	sprintf(buf, "The following %s%s are available to %s:\r\n",
	        wizhelp ? "privileged " : "",
	        socials ? "socials" : "commands",
	        vict == ch ? "you" : GET_NAME(vict));

	/* cmd_num starts at 1, not 0, to remove 'RESERVED' */

	for (no = 1, cmd_num = 1; cmd_num < num_of_cmds; cmd_num++)
	{
		i = cmd_sort_info[cmd_num].sort_pos;

		if (GET_LEVEL(vict) >= complete_cmd_info[i].minimum_level &&
		    (complete_cmd_info[i].minimum_level >= LVL_IMMORT) == wizhelp &&
			(wizhelp || socials == cmd_sort_info[i].is_social) &&
			(GET_LEVEL(vict) >= complete_cmd_info[i].view_minimum_level))
		{

			sprintf(buf + strlen(buf), "%-15s", complete_cmd_info[i].command.c_str());
			if (!(no % 7))
				strcat(buf, "\r\n");

			++no;
		}
	}

	strcat(buf, "\r\n");
	ch->Send(buf);
}

bool canSeeCharsInRoom(Character *ch, Room *room)
{
	if(room->people) 
	{
		for( Character *peep = room->people; peep; peep = peep->next_in_room)
		{
			if ( CAN_SEE(ch, peep) )
				return true;
		}				
	}
	return false;
}

std::vector<bool> getInvertedAffects(Character *ch)
{
	struct affected_type *hjp;
	std::vector<bool> inverted(52, false);

	for (hjp = ch->affected; hjp; hjp = hjp->next)
		if(hjp->inverted)
			inverted.insert(inverted.begin() + (int)hjp->bitvector, true);

	return inverted;
}

int getArenaTeam(Character *ch)
{
	if( ch && !ch->IsPurged() && JS_getCharacterArenaTeam(ch) )
		return JS_getCharacterArenaTeam(ch);

	return 0;
}

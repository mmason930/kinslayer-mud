/* ************************************************************************
*   File: handler.c                                     Part of CircleMUD *
*  Usage: internal funcs: moving and finding chars/objs                   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"

#include "structs.h"
#include "spells.h"
#include "screen.h"
#include "weaves.h"
#include "mobs.h"
#include "dg_event.h"

#include "StringUtil.h"
#include "UserLogoutType.h"
#include "Descriptor.h"
#include "UserClan.h"
#include "UserType.h"
#include "ObjectMoveLogger.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "editor-interface/EditorInterfaceInstance.h"

#include "items/ItemUtil.h"

#include "js/js.h"

/* external vars */
extern Character *combat_list;
extern event_info *event_list;
extern Object *object_list;
extern Character *character_list;
extern struct Index *obj_index;
extern Descriptor *descriptor_list;
extern Room *FindTargetRoom(Character * ch, char * rawroomstr);
extern const char *MENU;
extern std::list<Track *> TrackList;
extern Melee MeleeData;
int boot_high = 0;
time_t time_of_boot_high = 0;

/* local functions */
int apply_ac(Character * ch, int eq_pos);
void update_object(Object * obj, int use);
void update_char_objects(Character * ch);
extern ObjectMoveLogger objectMoveLogger;

/* external functions */
void remove_follower(Character * ch);
void die_follower(Character * ch);
//int weapon_skill(Character *ch, int weapon_type);
int show_on_who_list(Descriptor *d);
bool isInArena(Character *ch);
int getArenaTeam(Character *ch);
bool JS_isArenaInFFAMode();

//returns length of a string to display in order to get a visible length of 'sz' size.
/* Thanks to Seronis for this function - 11/13/2004 */
int str_display_sz(int sz, char *str)
{
	int bw_len = 0;    //effective string length ignoring color codes
	int length = 0;    //actual string length to achieve bw_len == sz

	int loop = 0;
	int limit    =    strlen(str);

	for(loop = 0;loop < limit && length < limit && bw_len < sz;++loop)
	{
		if( str[loop] != '{' )
		{
			++bw_len;
			++length;
			continue;
		}

		if( str[loop+1] == '{' )
		{
			loop++;
			++bw_len;
			length+=2;
			continue;
		}

		++loop;
		length += 2;
	}

	//catchall for strings containing too much color information but having
	// an Actual stringlength smaller than desired
	if( bw_len < sz )
		length += (sz - bw_len);

	return length;
}

/* Return the weight of held object and back object(worn), of a player. Galnor: 6-23-2004 */
int held_and_back(Character *ch)
{
	int weight = 0;

	if(GET_EQ(ch, WEAR_HOLD))
		weight += int(GET_EQ(ch, WEAR_HOLD)->Weight());
	if(GET_EQ(ch, WEAR_BACK))
		weight += int(GET_EQ(ch, WEAR_BACK)->Weight());

	return weight;
}

int weapon_skill(Character *ch, int weapon_type)
{
	int skill_type = 0;

	switch(weapon_type)
	{
		case WEAPON_LONG_BLADE:
			skill_type = SKILL_LONG_BLADE;
			break;
		case WEAPON_SHORT_BLADE:
			skill_type = SKILL_SHORT_BLADE;
			break;
		case WEAPON_STAFF:
			skill_type = SKILL_STAFF;
			break;
		case WEAPON_SPEAR:
			skill_type = SKILL_SPEAR;
			break;
		case WEAPON_AXE:
			skill_type = SKILL_AXE;
			break;
		case WEAPON_CLUB:
			skill_type = SKILL_CLUB;
			break;
		case WEAPON_CHAIN:
			skill_type = SKILL_CHAIN;
			break;
		case WEAPON_LANCE:
			skill_type = SKILL_LANCE;
			break;
		case WEAPON_BOW:
			skill_type = SKILL_BOW;
			break;
		default:
			return -1;
	}
	return GET_SKILL(ch, skill_type);
}

void UpdateBootHigh( const int new_high, bool first )
{
	sql::Query MyQuery;
	sql::Row MyRow;
	tm Then, Now;

	memset(&Then, 0, sizeof(tm));

	try {
		MyQuery = gameDatabase->sendQuery("SELECT * FROM player_highs WHERE 1 ORDER BY tdate DESC LIMIT 1");
	} catch( sql::QueryException &e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error when attempting to update boot high: %s", e.message.c_str());
		return;
	}

	//What we really need here is to see if the timestamp for this query is of the same day as the current timestamp.
	Now =  Time::TimeToDate( time(0) );

	if( MyQuery->numRows() > 0 )
	{
		MyRow = MyQuery->getRow();
		Then = Time::TimeToDate( atoi( MyRow["tdate"].c_str() ) );
	}
	std::stringstream Query;
	if( Time::SameDay( Then, Now ) == true )
	{
		//We also need to see if we're loading the previous boot high.
		if( first == true )
		{
			boot_high = atoi(MyRow["tcount"].c_str());
			time_of_boot_high = atoi(MyRow["tdate"].c_str());
		}
		else
		{
			Query << "UPDATE player_highs SET tdate='" << time(0) << "', tcount='" << new_high
				<< "' WHERE id='" << MyRow["id"] << "';";
			boot_high = new_high;
			time_of_boot_high = time(0);
		}
	}
	else
	{
		Query << "INSERT INTO player_highs(tdate,tcount) VALUES('"
			<< time(0) << "','" << new_high << "');";
		boot_high = new_high;
		time_of_boot_high = time(0);
	}

	if( Query.str().size() > 0 )
	{
		try {
			gameDatabase->sendRawQuery(Query.str());
		} catch( sql::QueryException &e ) {
			MudLog(BRF, LVL_APPR, TRUE, "Unable to update/insert player_highs: %s", e.message.c_str());
		}
	}
}

int NumPlayers(bool linkless, bool immortals)
{
	Character *ch;
	int n = 0;

	for(ch = character_list;ch;ch = ch->next)
	{
		if( ch->IsPurged() ) continue;
		if( ((linkless || ch->desc) && !IS_NPC(ch)) && (immortals || ch->player.level < LVL_IMMORT) )
			++n;
	}

	return n;
}

/* Rewritten by Galnor on February 9, 2004 */
int Character::GetLegend()
{
	int i;
	std::list<Legend *>::iterator l;

	for(i = 1, l = Legends.begin();l != Legends.end();++l, ++i)
	{
		if(!str_cmp(player.name, (*l)->name))
			return i;
	}

	return 0;
}

/* Is this person a master? -- Galnor */
bool Character::IsMaster()
{
	for(auto iter = userClans.begin();iter != userClans.end();++iter)
	{
		UserClan *userClan = (*iter);
		if(userClan->getRank() >= 8)
			return true;

		if(IS_FADE(this) || IS_DREADLORD(this) || IS_GREYMAN(this) || IS_BLADEMASTER(this) || IS_OGIER(this))
			return true;
	}

	return false;
}

//Return character's base moves -Galnor: 11-25-2004
int Character::BaseMoves()
{
	int i = 0, j = 0, moves = GET_MAX_MOVE(this);
	Object *obj;

	for(i = 0;i < NUM_WEARS;++i)
	{
		if(!(obj = GET_EQ(this, i)))
			continue;

		for (j = 0; j < MAX_OBJ_AFFECT;++j)
		{
			if(obj->affected[j].location == APPLY_MOVE)
				moves -= obj->affected[j].modifier;
		}
	}
	return moves;
}

// Redone into function -Galnor November, 2003
char *PERS(Character *target, Character *looker, const char *bgColor)
{
	static char result[256];
	*result = '\0';

	if(target->IsPurged() || looker->IsPurged()) {
		strcpy(result, "Someone");
		return result;
	}

	if(AFF_FLAGGED(looker, AFF_BLIND))
	{
		if (GET_RACE(target) == GET_RACE(looker))
			strcpy(result, "Someone");
		else
			strcpy(result, "*Someone*");
		return result;
	}

	if(IS_NPC(target))
		return (char*)GET_NAME(target);

	if(CAN_SEE(looker, target))
	{
		if( (GET_LEVEL(target) >= 100 || GET_LEVEL(looker) >= 100 && !isInArena(looker)) || (!isInArena(looker) && GET_RACE(target) == GET_RACE(looker)) || (isInArena(looker) && getArenaTeam(target) == getArenaTeam(looker) && !JS_isArenaInFFAMode() ) )
		{
			return (char*)GET_NAME(target);
		}
		else
		{

			if( getArenaTeam(target) == ARENA_BLUE )
				sprintf(result, "%s*%s%s%s*%s", COLOR_NORMAL(looker, CL_NORMAL), 
					COLOR_CYAN(looker, CL_NORMAL),
					GET_LEVEL(target) >= 12 ? GET_NAME(target) : target->RaceName().c_str(),
					COLOR_NORMAL(looker, CL_NORMAL), (bgColor ? bgColor : COLOR_NORMAL(looker, CL_NORMAL)));
			else if( getArenaTeam(target) == ARENA_RED )
				sprintf(result, "%s*%s%s%s*%s", COLOR_NORMAL(looker, CL_NORMAL), 
					COLOR_RED(looker, CL_NORMAL),
					GET_LEVEL(target) >= 12 ? GET_NAME(target) : target->RaceName().c_str(),
					COLOR_NORMAL(looker, CL_NORMAL), (bgColor ? bgColor : COLOR_NORMAL(looker, CL_NORMAL)));
			else if( getArenaTeam(target) == ARENA_YELLOW )
				sprintf(result, "%s*%s%s%s%s*%s", COLOR_NORMAL(looker, CL_NORMAL), 
					COLOR_YELLOW(looker, CL_NORMAL),
					COLOR_BOLD(looker, CL_NORMAL),
					GET_LEVEL(target) >= 12 ? GET_NAME(target) : target->RaceName().c_str(),
					COLOR_NORMAL(looker, CL_NORMAL), (bgColor ? bgColor : COLOR_NORMAL(looker, CL_NORMAL)));
			else if( getArenaTeam(target) == ARENA_GREEN )
				sprintf(result, "%s*%s%s%s*%s", COLOR_NORMAL(looker, CL_NORMAL), 
					COLOR_GREEN(looker, CL_NORMAL),
					GET_LEVEL(target) >= 12 ? GET_NAME(target) : target->RaceName().c_str(),
					COLOR_NORMAL(looker, CL_NORMAL), (bgColor ? bgColor : COLOR_NORMAL(looker, CL_NORMAL)));
			else
				sprintf(result, "%s*%s%s%s*%s", COLOR_NORMAL(looker, CL_NORMAL),
					COLOR_RED(looker, CL_NORMAL),
					GET_LEVEL(target) >= 12 ? GET_NAME(target) : target->RaceName().c_str(),
					COLOR_NORMAL(looker, CL_NORMAL),
					(bgColor ? bgColor : COLOR_NORMAL(looker, CL_NORMAL)));
		}
	}
	else
		strcpy(result, "Someone");
	return result;
}

// Can see, return name, else, return someone -Galnor: November, 2003
const char *DARKNESS_CHECK(Character *ch, Character *vict)
{
	if(CAN_SEE(ch, vict))
		return (char*)GET_NAME(vict);
	else
		return "Someone";
}

char *fname(const char *namelist)
{
	const int space = 30;
	static char holder[space];
	char *point;
	int i = 0;

	for (point = holder; isalpha(*namelist) && i < (space - 1); ++namelist, ++point, ++i)
		*point = *namelist;

	*point = '\0';
	return (holder);
}

void Character::Dismount()
{
	if(MOUNT(this))
	{
		RIDDEN_BY(MOUNT(this)) = NULL;
		MOUNT(this) = NULL;
	}
}

bool isname(const char *str, const char *namelist)
{
	const char *beg = str;
	for (;;)
	{
		for (str = beg; ; ++str, ++namelist)
		{
			//If the 'str' has all been read...
			if (!*str)
			{
				if(!IS_ALPHA(*namelist))
					return true;
				else if(*namelist == ' ')
					break;
			}
			//If the entire namelist has been read then we're done.
			if (!*namelist)
				return false;
			if (LOWER(*str) != LOWER(*namelist))
				break;
		}

		++namelist;
		/* skip to next name */
		for (; IS_ALPHA(*namelist); ++namelist);
		for (; *namelist == ' '; ++namelist);
	}
}

bool isname(const std::string &str, const char *namelist)
{
	return isname(str.c_str(), namelist);
}
bool isname(const char *str, const std::string &namelist)
{
	return isname(str, namelist.c_str());
}

bool isname(const std::string &str, const std::string &namelist)
{
	std::list< std::string > NameList = StringUtil::splitToList(namelist,' ');
	for(std::list<std::string>::iterator nIter=NameList.begin();nIter != NameList.end();++nIter)
	{
		if( !(*nIter).empty() && !str_cmp(str, (*nIter)) )
			return true;
	}
	return false;
}

/* Stock isname().  Leave this here even if you put in a newer  *
 * isname().  Used in olc.c					*/
bool is_name(const char *str, const char *namelist)
{
	const char *curname, *curstr;

	if (!str || !namelist || !*str || !*namelist)
		return false;

	curname = namelist;

	for (;;)
	{
		for (curstr = str;; ++curstr, ++curname)
		{
			if (!*curstr && !isalpha(*curname))
				return true;

			if (!*curname)
				return false;

			if (!*curstr || *curname == ' ')
				break;

			if (LOWER(*curstr) != LOWER(*curname))
				break;
		}

		/* skip to next name */

		for (; isalpha(*curname); ++curname);

		if (!*curname)
			return false;

		++curname;			/* first char of new name */

	}
}

void aff_apply_modify(Character * ch, sbyte loc, int mod, char* msg)
{
	int maxabil;

	maxabil = (IS_NPC(ch) ? 25 : 18);

	switch (loc)
	{
		case APPLY_NONE:
			break;
		case APPLY_STR:
			ch->SetStr(ch->GetStr() + mod);
			break;
		case APPLY_DEX:
			ch->SetDex(ch->GetDex() + mod);
			break;
		case APPLY_INT:
			ch->SetInt(ch->GetInt() + mod);
			break;
		case APPLY_WIS:
			ch->SetWis(ch->GetWis() + mod);
			break;
		case APPLY_CON:
			ch->SetCon(ch->GetCon() + mod);
			break;
		case APPLY_CLASS:
			/* ??? GET_CLASS(ch) += mod; */
			break;

			/*
			 * My personal thoughts on these two would be to set the person to the
			 * value of the apply.  That way you won't have to worry about people
			 * making +1 level things to be imp (you restrict anything that gives
			 * immortal level of course).  It also makes more sense to set someone
			 * to a class rather than adding to the class number. -gg
			 */
		case APPLY_LEVEL:
			/* ??? GET_LEVEL(ch) += mod; */
			break;
		case APPLY_AGE:
			//Galnor 2010-12-22 - Do we really want this???
//			ch->player.time.birth -= (mod * SECS_PER_MUD_YEAR);
			break;
		case APPLY_CHAR_WEIGHT:
			GET_WEIGHT(ch) += mod;
			break;
		case APPLY_CHAR_HEIGHT:
			GET_HEIGHT(ch) += mod;
			break;
		case APPLY_MANA:
			GET_MAX_MANA(ch) += mod;
			break;
		case APPLY_HIT:
			GET_MAX_HIT(ch) += mod;
			break;
		case APPLY_MOVE:
			GET_MAX_MOVE(ch) += mod;
			break;
		case APPLY_GOLD:
			break;
		case APPLY_EXP:
			break;
		case APPLY_DB:
			GET_DB(ch) += mod;
		case APPLY_HITROLL:
			break;
		case APPLY_DAMROLL:
			GET_DAMROLL(ch) += mod;
			break;
		case 28:
			break;
		default:
			Log("SYSERR: Unkown apply adjust... Number %d, on char %s, and mod = %d.", loc, GET_NAME(ch), mod);
			break;

	} /* switch */
}

void affect_modify(Character *ch, sbyte loc, sbyte mod, long bitv, bool add
	                  )
{

	if (add
	   )
	{
		SET_BIT_AR(AFF_FLAGS(ch), bitv);
	}

	else
	{
		REMOVE_BIT_AR(AFF_FLAGS(ch), bitv);
		mod = -mod;
	}

	char str[32];
	strcpy(str, "affect_modify");
	aff_apply_modify(ch, loc, mod, str);
}

void affect_modify_ar(Character *ch, sbyte loc, sbyte mod, int bitv[], bool add
	                     )
{
	int i , j;

	if (add)
	{
		for(i = 0; i < AF_ARRAY_MAX; ++i)
			for(j = 0; j < 32; ++j)
				if(IS_SET_AR(bitv, (i*32)+j))
					SET_BIT_AR(AFF_FLAGS(ch), (i*32)+j);
	}

	else
	{
		for(i = 0; i < AF_ARRAY_MAX; i++)
			for(j = 0; j < 32; j++)
				if(IS_SET_AR(bitv, (i*32)+j))
					REMOVE_BIT_AR(AFF_FLAGS(ch), (i*32)+j);

		mod = -mod;
	}

	char str[32];
	strcpy(str,"affect_modify_ar");
	aff_apply_modify(ch, loc, mod, str);
}


/* This updates a character by subtracting everything he is affected by */
/* restoring original abilities, and then affecting all again           */
void affect_total(Character *ch)
{
	struct affected_type *af;
	int i, j;

	for (i = 0; i < NUM_WEARS; ++i)
	{
		if (GET_EQ(ch, i))
			for (j = 0; j < MAX_OBJ_AFFECT; ++j)
				affect_modify_ar(ch, GET_EQ(ch, i)->affected[j].location,
				                 GET_EQ(ch, i)->affected[j].modifier,
				                 (int *) GET_EQ(ch, i)->obj_flags.bitvector, FALSE);
	}

	for (af = ch->affected; af; af = af->next)
		affect_modify(ch, af->location, af->modifier, af->bitvector, FALSE);

	ch->aff_abils = ch->real_abils;

	for (i = 0; i < NUM_WEARS; ++i)
	{
		if (GET_EQ(ch, i))
			for (j = 0; j < MAX_OBJ_AFFECT; ++j)
				affect_modify_ar(ch, GET_EQ(ch, i)->affected[j].location,
				                 GET_EQ(ch, i)->affected[j].modifier,
				                 (int *) GET_EQ(ch, i)->obj_flags.bitvector, TRUE);
	}


	for (af = ch->affected; af; af = af->next)
		affect_modify(ch, af->location, af->modifier, af->bitvector, TRUE);
}

/* Search the affection list not saved to find the structure that matches sp. Galnor: 6-18-2004 */
struct affect_type_not_saved *find_affection(Character *target, struct affected_type *sp)
{
	struct affect_type_not_saved *al;

	for(al = target->affection_list;al;al = al->next)
	{

		if(al->affect == sp)
		{
			return al;
		}
	}

	return NULL;
}

/**********Non Saving Affection List**********/

/* This is the not_saved affected list that will make it easier to trace who casted a spell on someone */
void add_affection_to_list(Character *ch, struct affect_type_not_saved *al)
{
	return;
	if(!ch || !al)
		return;

	al->next = ch->affection_list;
	ch->affection_list = al;
}

/* Remove the non-saving item from the non-saving affection list. */
void remove_affection_list(Character *ch, struct affect_type_not_saved *al, int loop)
{

	return;
	struct affect_type_not_saved *temp;

	if(!ch || !al)
		return;

	REMOVE_FROM_LIST(al, ch->affection_list, next);
	affect_remove(ch, al->affect);

	delete al;
}

/* Insert an affect_type in a Character structure
   Automatically sets apropriate bits and apply's */
void affect_to_char(Character * ch, struct affected_type * af)
{
	struct affected_type *affected_alloc;

	affected_alloc = new affected_type;

	*affected_alloc = *af;
	affected_alloc->next = ch->affected;
	ch->affected = affected_alloc;

	affect_modify(ch, af->location, af->modifier, af->bitvector, TRUE);
	affect_total(ch);

	if( af->callBack && af->callTime > 0 )
		add_event( af->callTime, effMainCallback, new wTuple(ch, affected_alloc) );
	else
	{
		WeaveCallback wc = WeaveManager::GetManager().GetCallback( af->bitvector );
		if( wc.callBack && wc.callTime > 0 )
			add_event( wc.callTime, effMainCallback, new wTuple(ch, affected_alloc) );
	}
}

/*
 * Remove an affected_type structure from a char (called when duration
 * reaches zero). Pointer *af must never be NIL!  Frees mem and calls
 * affect_location_apply
 */
void affect_remove(Character * ch, struct affected_type * af)
{
	struct affected_type *temp;
	struct affect_type_not_saved *al;

	if (!ch->affected)
	{
		core_dump();
		return;
	}

	if((al = find_affection(ch, af)))
	{
		remove_affection_list(ch, al, TRUE);
	}

	affect_modify(ch, af->location, af->modifier, af->bitvector, FALSE);
	REMOVE_FROM_LIST(af, ch->affected, next);
	delete(af);
	af = NULL;
	affect_total(ch);
}



/* Call affect_remove with every spell of spelltype "skill" */
void affect_from_char(Character * ch, int type, int bit)
{
	struct affected_type *hjp, *next;

	for (hjp = ch->affected; hjp; hjp = next)
	{
		next = hjp->next;

		if (hjp->type == type || hjp->bitvector== bit)
			affect_remove(ch, hjp);
	}
}



/*
 * Return if a char is affected by a spell (SPELL_XXX), NULL indicates
 * not affected
 */
bool affected_by_spell(Character * ch, int type)
{
	struct affected_type *hjp;

	for (hjp = ch->affected; hjp; hjp = hjp->next)
		if (hjp->type == type)
			return TRUE;

	return FALSE;
}

void affect_join(Character * ch, struct affected_type * af,
                 bool add_dur, bool avg_dur, bool add_mod, bool avg_mod)
{
	struct affected_type *hjp;
	bool found = FALSE;

	for (hjp = ch->affected; !found && hjp; hjp = hjp->next)
	{

		if ((hjp->type == af->type) && (hjp->location == af->location))
		{
			if (add_dur)
				af->duration += hjp->duration;

			if (avg_dur)
				af->duration /= 2;

			if (add_mod)
				af->modifier += hjp->modifier;

			if (avg_mod)
				af->modifier /= 2;

			affect_remove(ch, hjp);
			affect_to_char(ch, af);
			found = TRUE;
		}
	}
	if (!found)
		affect_to_char(ch, af);
}

Character *Object::FindHolder()
{
	Object *cont;

	if(this->worn_by)
	{
		return this->worn_by;
	}
	else if(this->carried_by)
	{
		return this->carried_by;
	}
	else if(this->in_obj)
	{
		for(cont = this->in_obj;cont->in_obj;cont = cont->in_obj);
		return cont->FindHolder();
	}

	// Serai - 03/27/05 - and somehow if nothing is found?  lets try returning NULL..
	return (NULL);
}

void Character::StepThroughGate( class Gate *g )
{
	Room* FirstRoom = this->in_room, *OtherEnd;
	try
	{
		OtherEnd = (g)->OtherEnd(FirstRoom);
	}
	catch( RoomNotFoundException e )
	{
		e.report();
		this->send("For some odd reason, you were unable to step through the gate!\r\n");
		return;
	}

	if( MOUNT(this) && OtherEnd->getSector() == RoomSector::inside )
	{
		this->send("You cannot ride there.\r\n");
		return;
	}

	this->send("You enter through the gate.\r\n");
	this->RemoveFromRoom();

	//Send the exit message to the room the character was in.
	for(Character *onLooker = FirstRoom->people;onLooker;onLooker = onLooker->next_in_room)
		if( GET_POS(onLooker) > POS_SLEEPING && CAN_SEE(onLooker,this))
			onLooker->send("%s steps through the gateway.\r\n", PERS(this,onLooker));
	//Now send the message to the other end of the gate.
	for(Character *onLooker = OtherEnd->people;onLooker;onLooker = onLooker->next_in_room)
		if( GET_POS(onLooker) > POS_SLEEPING && CAN_SEE(onLooker,this) )
			onLooker->send("%s enters through the gateway.\r\n", PERS(this,onLooker));

	this->MoveToRoom(OtherEnd);
	look_at_room(this, FALSE);

	//Move the pony.
	if( this->player.mount )
	{
		this->player.mount->RemoveFromRoom();
		this->player.mount->MoveToRoom(this->in_room);
	}
	for( Follower* f = this->followers;f;f=f->next )
	{
		if( f->follower->in_room == FirstRoom )
			f->follower->StepThroughGate( g );
	}
}

/* move a player out of a room */
void Character::RemoveFromRoom()
{
	Character *temp;

	if(!in_room)
		return;

	if (FIGHTING(this))
		this->StopFighting();
	if( this->player.sitting_on != NULL )
	{
		player.sitting_on->SatOnBy = NULL;
		player.sitting_on = NULL;
	}

	for(temp = this->in_room->people;temp;temp = temp->next_in_room)
	{
		if(FIGHTING(temp) == this)
			temp->SetNewFighting();
	}

	if (GET_EQ(this, WEAR_LIGHT))
		if (GET_EQ(this, WEAR_LIGHT)->getType() == ITEM_LIGHT)
			if (GET_OBJ_VAL(GET_EQ(this, WEAR_LIGHT), 2))	/* Light is ON */
				in_room->setLight(in_room->getLight() - 1);

	REMOVE_FROM_LIST(this, this->in_room->people, next_in_room);
	in_room = 0;
	next_in_room = NULL;
}

void Character::MoveToRoom(int rnum)
{
	if(rnum < 0 || (unsigned int)rnum > World.size())
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(this)), TRUE, "Character::MoveToRoom() : %s attempting to move to an "
		       "illegal room.", GET_NAME(this));
		return;
	}
	this->MoveToRoom(World[rnum]);
}

/* place a character in a room */
void Character::MoveToRoom(Room *room)
{
	Character* v;

	this->next_in_room = room->people;
	room->people = this;
	this->in_room = room;

	if (GET_EQ(this, WEAR_LIGHT))
		if (GET_EQ(this, WEAR_LIGHT)->getType() == ITEM_LIGHT)
			if (GET_OBJ_VAL(GET_EQ(this, WEAR_LIGHT), 2))	/* Light ON */
				room->setLight(room->getLight() + 1);

	/* Stop fighting now, if we left. */
	if (FIGHTING(this) && this->in_room != FIGHTING(this)->in_room)
	{
		FIGHTING(this)->StopFighting();
		StopFighting();
	}

	//We're checking to see if this character is shielded. If so, check how far from the caster they are.
	if( (v = ShieldManager::GetManager().ShieldedBy( this )) != NULL && v->ShieldOutOfRange( this ) )
		ShieldManager::GetManager().RemoveShield( v, this );
	//Do the same as above, except checking to see if the caster moved out of range of the target.
	if( (v = ShieldManager::GetManager().IsShielding( this )) != NULL && this->ShieldOutOfRange( v ) )
	{
		ShieldManager::GetManager().RemoveShield( this, v );
	}

	if(this->Eavesdropping)
	{
		this->stopEavesdropping();		
	}
	if (this->in_room->getEavesdroppingWarder() == this)
	{
		this->stopWarding();
	}

	if(ROOM_FLAGGED(this->in_room, ROOM_NOSOURCE) && this->ChannelingAbility())
	{
		if(PRF_FLAGGED(this, PRF_SOURCE))
		{
			this->send("The One Power suddenly leaves you, and vanishes out of sight.\r\n");
			this->RemoveSource();
		}
		else
			this->send("The True Source vanishes from your sight.\r\n");
	}
}

/* give an object to a char   */
void obj_to_char(Object *object, Character *ch)
{
	if (object && ch && !object->carried_by && !object->worn_by && !object->in_obj)
	{

		objectMoveLogger.logObjectMove(object->objID, std::string("Moved to ") + ch->getUserType()->getStandardName() + std::string(" ") + ToString(ch->getUserId()));

		object->next_content = ch->carrying;
		ch->carrying = object;
		object->carried_by = ch;
		object->in_room = 0;
		++IS_CARRYING_N(ch);

		/* set flag for crash-save system, but not on mobs! */
		if (!IS_NPC(ch))
			SET_BITK(PLR_FLAGS(ch), Q_BIT(PLR_CRASH));
	}

	else
		Log("SYSERR: NULL obj (%p) or char (%p) passed to obj_to_char.", (void *) object, (void *) ch);
}


/* take an object from a char */
void obj_from_char(Object *object)
{
	Object *temp;

	if (object == NULL)
	{
		Log("SYSERR: NULL object passed to obj_from_char.");
		return;
	}
	if(!object->carried_by)
		return;

	objectMoveLogger.logObjectMove(object->objID, std::string("Removed from ") + object->carried_by->getUserType()->getStandardName() + std::string(" ") + ToString(object->carried_by->getUserId()));

	REMOVE_FROM_LIST(object, object->carried_by->carrying, next_content);

	/* set flag for crash-save system, but not on mobs! */
	if (!IS_NPC(object->carried_by))
		SET_BITK(PLR_FLAGS(object->carried_by), Q_BIT(PLR_CRASH));

	--IS_CARRYING_N(object->carried_by);
	object->carried_by = NULL;
	object->next_content = NULL;
}

bool Character::HasObjectInInventory(int vnum)
{
	return GetObjectFromInventory(vnum) != NULL;
}

//Grab the first item from the character's inventory with the virtual number vnum.
Object *Character::GetObjectFromInventory(int vnum)
{
	Object *o;
	for (o = this->carrying; o && GET_OBJ_VNUM(o) != vnum; o = o->next_content);
	return o;
}

//Get the first occuring item from the character's inventory with the name 'name'
Object *Character::GetObjectFromInventory(const std::string &name)
{
	Object *o;
	for (o = this->carrying; o && strcmp(o->getName(), name.c_str()); o = o->next_content);
	return o;
}

float Object::Weight()
{
	float weight = 0.0f;
	Object *j;

	weight += GetTotalWeight();

	for(j = this->contains;j;j = j->next_content)
	{
		weight += j->Weight();
	}

	if ( ( this->carried_by && this->carried_by->isInClan(GET_OBJ_CLAN(this)) ) || ( this->worn_by && this->worn_by->isInClan(GET_OBJ_CLAN(this)) ) )
		weight += GET_OBJ_CL_WEIGHT(this);

	return weight;
}

/********************************************************/

void equip_char(Character * ch, Object * obj, int pos)
{
	int j;

	if (pos < 0 || pos >= NUM_WEARS)
	{
		core_dump();
		return;
	}

	if (GET_EQ(ch, pos))
	{
		Log("SYSERR: Char is already equipped: %s, %s", GET_NAME(ch),
		    obj->GetSDesc());
		return;
	}

	if (obj->carried_by)
	{
		Log("SYSERR: EQUIP: Obj is carried_by when equip.");
		return;
	}

	if (obj->in_room)
	{
		Log("SYSERR: EQUIP: Obj is in_room when equip.");
		return;
	}

	objectMoveLogger.logObjectMove(obj->objID, std::string("Equipped to ") + ch->getUserType()->getStandardName() + std::string(" ") + ToString(ch->getUserId()));

	GET_EQ(ch, pos) = obj;
	obj->worn_by = ch;
	obj->worn_on = pos;


	if (ch->in_room)
	{
		if (pos == WEAR_LIGHT && obj->getType() == ITEM_LIGHT)
			if (GET_OBJ_VAL(obj, 2))	/* if light is ON */
				ch->in_room->setLight(ch->in_room->getLight() + 1);
	}

	for (j = 0; j < MAX_OBJ_AFFECT; ++j)
		affect_modify_ar(ch, obj->affected[j].location, obj->affected[j].modifier, (int *) obj->obj_flags.bitvector, TRUE);

	if ( ch->isInClan( GET_OBJ_CLAN( obj ) ) )
	{
		affect_modify_ar(ch, APPLY_MOVE, GET_OBJ_CL_MVS(obj), (int *) obj->obj_flags.bitvector, TRUE);
		affect_modify_ar(ch, APPLY_HIT, GET_OBJ_CL_HPS(obj), (int *) obj->obj_flags.bitvector, TRUE);
	}

	//affect_total(ch);
}



Object *unequip_char(Character * ch, int pos)
{
	int j;
	Object *obj;

	if ((pos < 0 || pos >= NUM_WEARS) || GET_EQ(ch, pos) == NULL)
	{
		core_dump();
		return NULL;
	}

	obj = GET_EQ(ch, pos);
	obj->worn_by = NULL;
	obj->worn_on = -1;

	objectMoveLogger.logObjectMove(obj->objID, std::string("Removed equip from ") + ch->getUserType()->getStandardName() + std::string(" ") + ToString(ch->getUserId()));

	if (ch->in_room)
	{
		if (pos == WEAR_LIGHT && obj->getType() == ITEM_LIGHT && GET_OBJ_VAL(obj, 2))
		{

			ch->in_room->setLight(ch->in_room->getLight() - 1);
		}
	}

	GET_EQ(ch, pos) = NULL;

	for (j = 0; j < MAX_OBJ_AFFECT; ++j)
	{
		affect_modify_ar(ch, obj->affected[j].location, obj->affected[j].modifier,
		                 (int *) obj->obj_flags.bitvector, FALSE);
	}

	if ( ch->isInClan( GET_OBJ_CLAN( obj ) ) )
	{
		affect_modify_ar(ch, APPLY_MOVE, GET_OBJ_CL_MVS(obj), (int *) obj->obj_flags.bitvector, FALSE);
		affect_modify_ar(ch, APPLY_HIT, GET_OBJ_CL_HPS(obj), (int *) obj->obj_flags.bitvector, FALSE);
	}

	affect_total(ch);
	return (obj);
}


int GetNumber(char **name)
{
	int i;
	char *ppos;
	char number[MAX_INPUT_LENGTH];

	*number = '\0';

	if ((ppos = strchr(*name, '.')))
	{
		*ppos++ = '\0';
		strcpy(number, *name);
		memmove(*name, ppos, strlen(ppos) + 1);
		//		strcpy(*name, ppos);  // Serai - *name, ppos overlap - use memmove()

		for (i = 0; *(number + i); ++i)
			if (!isdigit(*(number + i)))
				return 0;

		return (atoi(number));
	}

	return 1;
}

int GetNumber(std::string &name)
{
	std::string::size_type pos;
	std::string number;

	if( (pos = name.find(".", 0)) != std::string::npos)
	{
		number.assign(name, 0, pos);
		name.erase(0, pos + 1);

		if(number.size() && isdigit(number[0]))
			return atoi(number.c_str());
	}

	return 1;
}

int room_visibility(Character *ch, Character *vict)
{
	if(!vict || ch->IsPurged() || vict->IsPurged())
		return 0;

	if(ch->in_room == vict->in_room && CAN_SEE(ch, vict))
		return 1;

	return 0;
}


/* search a room for a char, and return a pointer if found..  */
Character *get_char_room(const char *name, room_rnum room)
{
	Character *i;
	int j = 0, number;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;

	strcpy(tmp, name);
	if (!(number = GetNumber(&tmp)))
		return NULL;

	for (i = World[room]->people; i && (j <= number); i = i->next_in_room)
	{
		if (isname(tmp, i->player.name) && ++j == number)
			return i;
	}

	return NULL;
}



/* search all over the world for a char num, and return a pointer if found */
Character *get_char_num(mob_rnum nr)
{
	Character *i;

	for (i = character_list; i; i = i->next)
	{
		if( i->IsPurged() ) continue;
		if (GET_MOB_RNUM(i) == nr)
			return i;
	}

	return NULL;
}


void Object::MoveToRoom(int rnum, bool vaultSave)
{
	if(rnum < 0 || (unsigned int)rnum >= World.size())
	{
		MudLog(BRF, LVL_APPR, TRUE,
		       "Object::MoveToRoom() Object attempting to enter into invalid room(%d of %d).", rnum, World.size() - 1);
		return;
	}
	this->MoveToRoom(World[rnum], vaultSave);
}

void Object::MoveToRoom(Room *room, bool vaultSave)
{
	if (!room)
		MudLog(BRF, LVL_APPR, TRUE, "Attempting to pass object(vnum %d) to illegal room.", GET_OBJ_VNUM(this));
	else if(this->in_room){}
	else
	{
		objectMoveLogger.logObjectMove(this->objID, std::string("Moved to room #") + ToString(room->getVnum()));

		this->next_content = room->contents;
		room->contents = this;
		this->in_room = room;
		this->carried_by = NULL;
		
		if (ROOM_FLAGGED(room, ROOM_VAULT))
		{
			if(vaultSave)
				room->itemSave();
		}
		else if(IS_CORPSE(this)) {
			Room::corpseRooms.insert(room->getVnum());
		}
	}
}

/* Remove the object from anything that may hold it */
void Object::RemoveFromAll()
{
	if( this->carried_by )
		obj_from_char(this);
	if( this->worn_by )
		unequip_char(this->worn_by, this->worn_on);
	if( this->in_obj )
		obj_from_obj(this);
	if( this->in_room )
		this->RemoveFromRoom();
}

/* Take an object from a room */
void Object::RemoveFromRoom(bool vaultSave)
{
	Object *temp;
	Room *r = this->in_room;

	if (!r)
	{
		Log("SYSERR: Object with invalid room passed to Object::RemoveFromRoom.");
		return;
	}
	if( SatOnBy != NULL )
	{
		SatOnBy->player.sitting_on = NULL;
		SatOnBy = NULL;
	}

	REMOVE_FROM_LIST(this, r->contents, next_content);

	if (ROOM_FLAGGED(r, ROOM_VAULT) && vaultSave) {

		Object::saveItemToTopLevelHolder('\0', "", this);
//		r->itemSave();
	}

	objectMoveLogger.logObjectMove(this->objID, std::string("Removed from room #") + ToString(this->item_number));

	this->in_room = 0;
	this->next_content = NULL;
}

/* put an object in an object (quaint)  */
void obj_to_obj(Object * obj, Object * obj_to)
{
	if (!obj || !obj_to || obj == obj_to)
	{
		Log("SYSERR: NULL object (%p) or same source (%p) and target (%p) obj passed to obj_to_obj.",
		    (void *) obj, (void *) obj, (void *) obj_to);
		return;
	}

	objectMoveLogger.logObjectMove(obj->objID, std::string("Moved to obj ") + ToString(obj->objID));

	obj->next_content = obj_to->contains;
	obj_to->contains = obj;
	obj->in_obj = obj_to;
}


/* remove an object from an object */
void obj_from_obj(Object * obj)
{
	Object *temp, *obj_from;

	if (obj->in_obj == NULL)
	{
		Log("SYSERR: (%s): trying to illegally extract obj from obj.", __FILE__);
		return;
	}

	objectMoveLogger.logObjectMove(obj->objID, std::string("Removed from obj ") + ToString(obj->in_obj->objID));

	obj_from = obj->in_obj;
	REMOVE_FROM_LIST(obj, obj_from->contains, next_content);

	obj->in_obj = NULL;
	obj->next_content = NULL;
}

// Extract an object from the world. If lowerItemCount is set to true, then
// the item count is decreased. Passing no arguments sets lowerItemCount to false by default.
void Object::Extract( bool lowerItemCount )
{
	// Serai - 09/17/05 - We need to check for RNUM of -1
	if( lowerItemCount && GET_OBJ_RNUM(this) != NOTHING && !this->IsPurged() )
		--ItemCount[GET_OBJ_RNUM(this)];

	js_extraction_scripts( this );

	this->RemoveFromAll();

	if( this->IsPurged() )
		return;//Don't need to purge this again...

	/* Get rid of the contents of the object, as well. */
	while (this->contains)
		this->contains->Extract( lowerItemCount );

	this->purged = true;
	ObjPurgeList.push_back( this );

	if (GET_OBJ_RNUM(this) >= 0)
		--(obj_index[GET_OBJ_RNUM(this)].number);
}

void update_object(Object * obj, int use)
{
	/* dont update objects with a timer trigger */
	if (GET_OBJ_TIMER(obj) > 0)
		GET_OBJ_TIMER(obj) -= use;

	if (obj->contains)
		update_object(obj->contains, use);

	if (obj->next_content)
		update_object(obj->next_content, use);
}

void update_char_objects(Character * ch)
{
	int i;

	if (GET_EQ(ch, WEAR_LIGHT) != NULL)
		if (GET_EQ(ch, WEAR_LIGHT)->getType() == ITEM_LIGHT)
			if (GET_OBJ_VAL(GET_EQ(ch, WEAR_LIGHT), 2) > 0)
			{
				i = --GET_OBJ_VAL(GET_EQ(ch, WEAR_LIGHT), 2);

				if (i == 3)
				{
					Act("Your light begins to flicker and fade.", FALSE, ch, 0, 0, TO_CHAR);
					Act("$n's light begins to flicker and fade.", FALSE, ch, 0, 0, TO_ROOM);
				}
				if (i == 1)
				{
					Act("Your light is about to go out.", FALSE, ch, 0, 0, TO_CHAR);
					Act("$n's light is about to go out.", FALSE, ch, 0, 0, TO_ROOM);
				}
				else if (i == 0)
				{
					Act("Your light sputters out and dies.", FALSE, ch, 0, 0, TO_CHAR);
					Act("$n's light sputters out and dies.", FALSE, ch, 0, 0, TO_ROOM);
					ch->in_room->setLight(ch->in_room->getLight() - 1);
				}
			}

	for (i = 0; i < NUM_WEARS;++i)
		if (GET_EQ(ch, i))
			update_object(GET_EQ(ch, i), 2);

	if (ch->carrying)
		update_object(ch->carrying, 1);
}

void Character::Extract()
{
	Extract(UserLogoutType::unknown, false);
}
void Character::Extract(bool fullDelete)
{
	Extract(UserLogoutType::unknown, fullDelete);
}
void Character::Extract(UserLogoutType *userLogoutType)
{
	Extract(userLogoutType, false);
}

/* Extract a ch completely from the world, and leave his stuff behind */
void Character::Extract( UserLogoutType *userLogoutType, bool fullDelete, bool purgeItems )
{
	Character *k, *temp;
	Descriptor *t_desc;
	Object *obj;
	int i = 0, freed = 0;
	int playerRoomVnum = this->in_room ? this->in_room->getVnum() : -1;

	if( purged )
		return;

	js_extraction_scripts( this );

	if (!IS_NPC(this) && !desc)
	{
		for (t_desc = descriptor_list; t_desc; t_desc = t_desc->next)
			if (t_desc->original == this)
				do_return(t_desc->character, NULL, 0, 0);
	}
	Dismount();
	RemoveSource();
	ShieldManager::GetManager().KillShieldsByTarget(this);//Shields by caster are handled in RemoveSource()

	if (this->followers || this->master)
		die_follower(this);

	/* Forget snooping, if applicable */
	if (desc)
	{
		if(desc->snooping && desc->snooping->snoop_by)
		{
			desc->snooping->snoop_by = NULL;
			desc->snooping = NULL;
		}
		if (desc->snoop_by)
		{
			desc->snoop_by->send("Your snoop target is no longer among us.\r\n");
			desc->snoop_by->snooping = NULL;
			desc->snoop_by = NULL;
		}
	}

	/* transfer objects to room, if any */
	while (this->carrying)
	{
		obj = this->carrying;
		obj_from_char(obj);
		if(purgeItems) {
			obj->Extract();
		} else {
			obj->MoveToRoom(this->in_room);
		}
	}

	/* transfer equipment to room, if any */
	for (i = 0; i < NUM_WEARS; ++i)
	{
		if (GET_EQ(this, i)) {
			Object *removedObj = unequip_char(this, i);
			if(purgeItems) {
				removedObj->Extract();
			} else {
				removedObj->MoveToRoom(this->in_room);
			}
		}
	}

	/* Set greyman marked target to NULL if we are extracting their target. */
	for(k = character_list;k;k = k->next)
	{
		if(GET_MARKED(k) == this)
			GET_MARKED(k) = NULL;
		if(FIGHTING(k) == this)
			k->StopFighting();
		if(GET_TARGET(k) == this)
			GET_TARGET(k) = NULL;
		if(GET_TARGET2(k) == this)
			GET_TARGET2(k) = NULL;
		if(k->SlowedBy == this)
			k->SlowedBy = 0;
		if(k->DecayedBy == this)
			k->DecayedBy = 0;
		if(k->BurnedBy == this)
			k->BurnedBy = 0;
		if(k->PlaguedBy == this)
			k->PlaguedBy = 0;
		if(HUNTING(k) == this)
			HUNTING(k) = 0;
	}
	event_info *e, *eNext;
	for( event_info* e = event_list; e ; e = eNext )
	{
		eNext = e->next;
		if( e->func == effMainCallback )
		{
			wTuple* Data = reinterpret_cast<wTuple*>(e->info);
			if( Data->first == this ) {
				Data->first = 0;
				Data->second = 0;
			}
		}
	}

	if (FIGHTING(this))
		StopFighting();

	if(editorInterfaceInstance)
		editorInterfaceInstance->terminate();

	if(!IS_NPC(this))
		this->save();

	if(!IS_NPC(this) && userLogoutType != NULL && userLogoutType != UserLogoutType::notRealLogout) {
	
		std::stringstream sqlBuffer;

		std::string roomString = in_room ? MiscUtil::convert<std::string>(in_room->getVnum()) : "NULL";

		sqlBuffer	<< "INSERT INTO userLogout("
					<< " `user_id`,"
					<< " `created_datetime`,"
					<< " `logout_type`,"
					<< " `room_id`"
					<< ") VALUES ("
					<< this->player.idnum << ","
					<< "NOW(),"
					<< userLogoutType->getValue() << ","
					<< roomString << ");";

		gameDatabase->sendRawQuery(sqlBuffer.str());
	}

	if(this->in_room)
		this->RemoveFromRoom();


	CharPurgeList.push_back( this );
	this->purged = true;

	if (this->desc && this->desc->original)
		do_return(this, NULL, 0, 0);

	if (!IS_NPC(this))
	{
		MudLog(CMP, MAX(GET_INVIS_LEV(this), LVL_IMMORT), TRUE,
		       "%s extracted at room %d.", GET_NAME(this), playerRoomVnum);
	}
	else
	{
		if (GET_MOB_RNUM(this) > -1)		/* if mobile */
			--MobManager::GetManager().GetIndex((unsigned int)this->nr)->number;
//		delete (this);
		freed = 1;
	}

	if (!freed && (this->desc && !fullDelete))
		STATE(this->desc) = CON_CLOSE;
}

/* ***********************************************************************
* Here follows high-level versions of some earlier routines, ie functions*
* which incorporate the Actual player-data                               *.
*********************************************************************** */


Character *get_player_vis(Character * ch, char *name, int inroom)
{
	Character *i;

	if( ch->IsPurged() ) return (0);

	for (i = character_list; i; i = i->next)
	{
		if( i->IsPurged() ) continue;
		if (!IS_NPC(i) && (!inroom || i->in_room == ch->in_room) &&
		        !str_cmp(i->player.name, name) && CAN_SEE(ch, i))
			return i;
	}

	return NULL;
}

Character *get_char_room_vis(Character *ch, const char *name)
{
	Character *i;
	int j = 0, num;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;

	if( ch->IsPurged() ) return (0);

	if (!str_cmp(name, "self") || !str_cmp(name, "me"))
		return (ch);
	/* 0.<name> means PC with name */
	strcpy(tmp, name);

	if (!(num = GetNumber(&tmp)))
		return get_player_vis(ch, tmp, 1);

	for (i = ch->in_room->people; i && j <= num; i = i->next_in_room)
	{
		bool pass = false;
		if( ( isInArena(ch) && isInArena(i) && (getArenaTeam(ch) != getArenaTeam(i) || JS_isArenaInFFAMode()) && ch != i ) 
			|| ( !isInArena(ch) && GET_RACE(ch) != GET_RACE(i) && GET_LEVEL(ch) < LVL_IMMORT && GET_LEVEL(i) < LVL_IMMORT ) ) {
			if( !IS_NPC(i) && GET_LEVEL(i) > 12 ) {
				if( isname(tmp, i->BuildRaceAndClassNameList().c_str()) )
					pass=true;
			}
		}
		if( pass );
		else if( isname(tmp,i->player.name) )
			pass = true;
		else if( !IS_NPC(i) && GET_LEVEL(i) <= 12 && !str_cmp(tmp,"newbie") )
			pass = true;
		if( !pass )
			continue;
		if (CAN_SEE(ch, i))
		{
			if (++j == num)
				return i;
		}
	}
	return NULL;
}

/* Search for matching name regardless of anything else. */
Character *get_char_by_name(const char *name, int npc)
{
	Character *i;

	if(!name)
		return NULL;

	for (i = character_list; i; i = i->next)
	{
		if ((npc || !IS_NPC(i)) && isname(name, i->player.name))
			return i;
	}

	return NULL;
}

Character *get_char_vis(Character * ch, const char *name)
{
	Character *i;
	int j = 0, num = 0;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;

	if( ch->IsPurged() ) return (0);

	/* check the room first */
	if ((i = get_char_room_vis(ch, name)) != NULL)
	{
		return i;
	}

	strcpy(tmp, name);

	if (!(num = GetNumber(&tmp)))
	{
		return get_player_vis(ch, tmp, 0);
	}
	for (i = character_list; i && (j <= num); i = i->next)
	{
		if( i->IsPurged() ) continue;
		if (isname(tmp, i->player.name) && CAN_SEE(ch, i))
		{
			if (++j == num)
			{
				return i;
			}
		}
	}
	return NULL;
}

/* Generic Find, designed to find any object/character                    */
/* Calling :                                                              */
/*  *arg     is the sting containing the string to be searched for.       */
/*           This string doesn't have to be a single word, the routine    */
/*           extracts the next word itself.                               */
/*  bitv..   All those bits that you want to "search through".            */
/*           Bit found will be result of the function                     */
/*  *ch      This is the person that is trying to "find"                  */
/*  **tar_ch Will be NULL if no character was found, otherwise points     */
/* **tar_obj Will be NULL if no object was found, otherwise points        */
/*                                                                        */
/* The routine returns a pointer to the next word in *arg (just like the  */
/* OneArgument routine).                                                 */

int generic_find(char *arg, int bitvector, Character * ch,
                 Character ** tar_ch, Object ** tar_obj)
{
	int i, found;
	char name[256];

	OneArgument(arg, name);

	if (!*name)
		return (0);

	*tar_ch = NULL;
	*tar_obj = NULL;

	if (IS_SET(bitvector, FIND_CHAR_ROOM))
	{	/* Find person in room */
		if ((*tar_ch = get_char_room_vis(ch, name)))
		{
			return (FIND_CHAR_ROOM);
		}
	}

	if (IS_SET(bitvector, FIND_CHAR_WORLD))
	{
		if ((*tar_ch = get_char_vis(ch, name)))
		{
			return (FIND_CHAR_WORLD);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_EQUIP))
	{
		for (found = FALSE, i = 0; i < NUM_WEARS && !found; i++)
			if (GET_EQ(ch, i) && isname(name, GET_EQ(ch, i)->getName()))
			{
				*tar_obj = GET_EQ(ch, i);
				found = TRUE;
			}

		if (found)
		{
			return (FIND_OBJ_EQUIP);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_INV))
	{
		if ((*tar_obj = ItemUtil::get()->getObjectInListVis(ch, name, ch->carrying)))
		{
			return (FIND_OBJ_INV);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_ROOM))
	{
		if ((*tar_obj = ItemUtil::get()->getObjectInListVis(ch, name, ch->in_room->contents)))
		{
			return (FIND_OBJ_ROOM);
		}
	}

	if (IS_SET(bitvector, FIND_OBJ_WORLD))
	{
		if ((*tar_obj = ItemUtil::get()->getObjectVis(ch, name)))
		{
			return (FIND_OBJ_WORLD);
		}
	}

	return (0);
}


/* a function to scan for "all" or "all.x" */
int find_all_dots(char *arg)
{

	if (!strcmp(arg, "all"))
		return FIND_ALL;

	else if (!strncmp(arg, "all.", 4))
	{
		strcpy(arg, std::string(arg + 4).c_str());
		return FIND_ALLDOT;
	}

	else
		return FIND_INDIV;
}

/***********************************************************
		-Galnor: 08-06-2005
These is for calculating gold/silver/copper amounts
***********************************************************/
unsigned long int CalcGold(unsigned long int total)
{
	return total / 10000;
}

unsigned long int CalcSilver(unsigned long int total)
{
	return (total % 10000) / 100;
}

unsigned long int CalcCopper(unsigned long int total)
{
	return ((total % 10000) % 100);
}




















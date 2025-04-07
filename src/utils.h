/* ************************************************************************
*   File: utils.h                                       Part of CircleMUD *
*  Usage: header file: utility macros and prototypes of utility funcs     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include "MiscUtil.h"

/* external declarations and prototypes **********************************/

extern FILE *logfile;

#define Log			BasicMudLog

long	asciiflag_conv(char *flag);

//Done by Galnor: March 2nd, 2006
//Insert an item into a vector of any kind AFTER the object with the given "index"
template <class _T>
bool V_InsertAfter(std::vector<_T> &c, _T obj, unsigned int index)
{
	//Illegal index.
	if(index < 0)
		return false;
	typename std::vector<_T>::iterator i;
	for(i = c.begin(); i != c.end(); ++i, --index)
	{
		//If index == 0, that means we've found the item.
		if(!index)
		{
			//If the object we are looking for(found by the index given) is c.end(), it obviously does NOT exist.
			if(i == c.end())
				return false;
			//Move to next iterator(even if it's c.end()) because .insert() inserts BEFORE, and we want to insert AFTER.
			++i;
			//Insert
			c.insert(i, obj);
			return true;
		}
	}
	//End of vector was reached. No item with given index was found.
	return false;
}

//Done by Galnor: March 2nd, 2006
//Insert an item into a vector of any kind AFTER the object with the given "index"
template <class _T>
bool V_InsertBefore(std::vector<_T> &c, _T obj, unsigned int index)
{
	//Illegal index.
	if(index < 0 || index > c.size())
		return false;
	typename std::vector<_T>::iterator i;
	for(i = c.begin();i != c.end();++i, --index)
	{
		//If index == 0, that means we've found the item.
		if(!index)
		{
			//Insert
			c.insert(i, obj);
			return true;
		}
	}
	//End of vector was reached. Append.
	c.push_back(obj);
	return true;
}
template <typename _T>
void ConvertBitvectorFromString(const char *str, _T *bitvector, const int &size)
{
	std::string TemporaryString;

	for(int i = 0;i < size;++i)
	{
		for(;*str == ' ';++str);
		if(!*str)
			break;
		TemporaryString.clear();
		for(;*str;++str)
		{
			if(*str == ' ')
				break;
			TemporaryString += (*str);
		}
		bitvector[i] = asciiflag_conv((char*)TemporaryString.c_str());
		for(;*str == ' ';++str);
	}
}

#ifndef MAX
#define MAX(a, b)		(a > b ? a : b)
#endif
#ifndef MIN
#define MIN(a, b)		(a < b ? a : b)
#endif

std::string	&sprintf(std::string &str, const char *format, ...);
std::string	&strcat(std::string &str, const char *format, ...);
int		Touch(const std::string &Path);
int		get_line(FILE *fl, char *buf);
void	BasicMudLog(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void	MudLog(int type, int level, int file, const char *str, ...);
void	sprintbit(std::bitset<64> &bitvector, const char *names[], char *result, const std::string delim,
			   const std::string colVec, const std::string colDel);
void	sprintbit(long vektor, const char *names[], char *result, const std::string delim=" ",
				  const std::string colVec="", const std::string colDel="" );
void	sprinttype(int type, const char *names[], char *result);
void	sprintbitarray(int bitvector[], const char *names[], int maxar, char *result);
void	core_dump_real(const char *, ush_int);

bool	HasParameter( const std::string &Buffer );
bool	HasParameter( const std::string &Buffer, const std::string &Param );
std::string ParamValue( const std::string &Buffer, const std::string &Param );
void	format_text(char **ptr_string, int mode, Descriptor *d, int maxlen);
void	update_pos(Character *victim);
struct	GameTime *age(Character *ch);
std::string	itos(int i);
std::string	ftos(float i);
std::string	dtos(double i);
std::string eatwhite( std::string str );
std::string StripFilePath( const std::string &sFilePath );

#ifndef WIN32
std::string ParseMemoryFromMemInfo( std::string sLineBuffer );
__int64 AvailableSystemMemory();
#endif

int strcmp(const std::string &str1, const std::string &str2);
int strcmp(const std::string &str1, const char *str2);
int strcmp(const char *str1, const std::string &str2);
int str_cmp(const char *str1, const char *str2);
int str_cmp(const std::string &str1, const char *str2);
int str_cmp(const char *str1, const std::string &str2);
int str_cmp(const std::string &str1, const std::string &str2);
int strn_cmp(const char *str1, const char *str2, size_t n);
int strn_cmp(const std::string &str1, const char *str2, size_t n);
int strn_cmp(const char *str1, const std::string &str2, size_t n);
int strn_cmp(const std::string &str1, const std::string &str2, size_t n);

#ifndef WIN32
std::string Execute( const char *command );
#endif

#define LOWER(c)		(((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)		(((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c))
#define is_alpha(c)		( 'A' < c && 'z' > c )

#define	core_dump()		core_dump_real(__FILE__, __LINE__)

#define CLEANUP(ch, boolean)		if(boolean) \
										delete ch; \
									return;

/* in act.informative.c */
void	look_at_room(Character *ch, int mode);

/* in act.movmement.c */
int	perform_move(Character *ch, int dir, int following);

/* in limits.c */

int		check_idling(Character *ch);
void	gain_exp(Character *ch, int gain, bool enforceMax=true);
void	gain_exp_regardless(Character *ch, int gain);
void	gain_condition(Character *ch, int condition, int value);
void	point_update(void);
std::string	getNChars(char ch, int n);

/* various constants *****************************************************/

/* defines for MudLog() */
const int OFF = 0;
const int BRF = 1;
const int NRM = 2;
const int CMP = 3;

/* Conf */
#define CONFIG_LEVEL_CAN_SHOUT  Conf->play.level_can_shout
#define CONFIG_TUNNEL_SIZE      Conf->play.tunnel_size
#define CONFIG_MAX_EXP_GAIN     Conf->play.max_exp_gain
#define CONFIG_MAX_NPC_CORPSE_TIME Conf->play.max_npc_corpse_time
#define CONFIG_MAX_PC_CORPSE_TIME Conf->play.max_pc_corpse_time
#define CONFIG_IDLE_VOID        Conf->play.idle_void
#define CONFIG_IDLE_RENT_TIME   Conf->play.idle_rent_time
#define CONFIG_IDLE_MAX_LEVEL   Conf->play.idle_max_level
#define CONFIG_LOAD_INVENTORY   Conf->play.load_into_inventory
#define CONFIG_OK               Conf->play.OK
#define CONFIG_NOPERSON         Conf->play.NOPERSON
#define CONFIG_NOEFFECT         Conf->play.NOEFFECT

/** Crash Saves **/
#define CONFIG_FREE_RENT        Conf->csd.free_rent
#define CONFIG_MAX_OBJ_SAVE     Conf->csd.max_obj_save
#define CONFIG_AUTO_SAVE        Conf->csd.auto_save
#define CONFIG_AUTOSAVE_TIME    Conf->csd.autosave_time
#define CONFIG_CRASH_TIMEOUT    Conf->csd.crash_file_timeout
#define CONFIG_RENT_TIMEOUT     Conf->csd.rent_file_timeout

/** Room Numbers **/
#define CONFIG_HUMAN_START		Conf->room_nums.human_start_room
#define CONFIG_TROLLOC_START	Conf->room_nums.trolloc_start_room
#define CONFIG_AIEL_START		Conf->room_nums.aiel_start_room
#define CONFIG_SEANCHAN_START	Conf->room_nums.seanchan_start_room
#define CONFIG_IMMORTAL_START	Conf->room_nums.immortal_start_room
#define CONFIG_FROZEN_START		Conf->room_nums.frozen_start_room

/** Game Operation **/
#define CONFIG_MAX_PLAYING      Conf->operation.max_playing
#define CONFIG_MAX_BAD_PWS      Conf->operation.max_bad_pws
#define CONFIG_OLC_SAVE         Conf->operation.auto_save_olc
#define CONFIG_NEW_SOCIALS      Conf->operation.use_new_socials
#define CONFIG_NS_IS_SLOW       Conf->operation.nameserver_is_slow
#define CONFIG_LOGNAME          Conf->operation.LOGNAME
#define CONFIG_WELC_MESSG       Conf->operation.WELC_MESSG
#define CONFIG_START_MESSG      Conf->operation.START_MESSG

extern std::vector<int> BodyPercents;

/* breadth-first searching */
#define BFS_ERROR			-1
#define BFS_ALREADY_THERE	-2
#define BFS_NO_PATH			-3

/* Memory Sizes */
#define KB_SIZE			(1024)
#define MB_SIZE			(KB_SIZE * 1024)
#define GB_SIZE			(MB_SIZE * 1024)

#define KB(memory)		((float) (memory) / (float) KB_SIZE)
#define MB(memory)		((float) (memory) / (float) MB_SIZE)
#define GB(memory)		((float) (memory) / (float) GB_SIZE)

const int BASIC = 1;
const int THOROUGH = 2;

#define IS_HORSE(ch)			(MOB_FLAGGED(ch, MOB_MOUNT) || MOB_FLAGGED(ch, MOB_SHADOW_MOUNT) || MOB_FLAGGED(ch, MOB_OGIER_MOUNT))
#define HAS_SOURCE(ch)			(PRF_FLAGGED(ch, PRF_SOURCE))

/*
 * XXX: These constants should be Configurable. See Act.informative.
 *	and utils.c for other places to change.
 */
/* mud-life time */
const int SECS_PER_MUD_HOUR = 60;
#define SECS_PER_MUD_DAY	(24*SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH	(30*SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR	(17*SECS_PER_MUD_MONTH)

// real-life time (remember Real Life?) //
const int SECS_PER_REAL_MIN = 60;
#define SECS_PER_REAL_HOUR	(60*SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY	(24*SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR	(365*SECS_PER_REAL_DAY)

/* Chest log values */
const int TYPE_IN = 1;
const int TYPE_OUT = 2;

/* string utils **********************************************************/

#define ONOFF(a) ((a) ? "ON" : "OFF")

#define IF_STR(st) ((st) ? (st) : "\0")

#define AN(string) (strchr("aeiouAEIOU", *string) ? "an" : "a")


/* memory utils **********************************************************/

#if !defined(__STRING)
#define __STRING(x)	#x
#endif

#define CREATE(result, type, number)  do {\
	if(number > 1) { \
	if (!((result) = new type[(number)]))\
		{ perror("new failure"); abort(); } \
	else \
		memset((result), 0, sizeof(type) * (number)); \
	} else {\
	if (!((result) = new type))\
		{ perror("new failure"); abort(); } \
	else \
		memset((result), 0, sizeof(type)); \
	}} while(0)

/* Serai - 07/22/04 - Doubly-linked list macros */
#define ADD_NODE(first,last,node,prevPtr,nextPtr,prev,next) \
	do \
	{ \
		if ((prev) == NULL) \
		{ \
			ADD_FIRST_NODE(first,last,node,prevPtr,nextPtr); \
		} \
		else if ((next) == NULL) \
		{ \
			ADD_END_NODE(first,last,node,prevPtr,nextPtr); \
		} \
		else \
		{ \
			(prev)->nextPtr = (node); \
			(next)->prevPtr = (node); \
			(node)->nextPtr = (next); \
			(node)->prevPtr = (prev); \
		} \
	} while(0)

#define ADD_FIRST_NODE(first,last,node,prevPtr,nextPtr) \
	do \
	{ \
		(node)->prevPtr = NULL; \
		(node)->nextPtr = (first); \
		if (!(first)) \
			(last) = (node); \
		else \
			(first)->prevPtr = (node); \
		(first) = (node); \
	} while(0)

#define ADD_END_NODE(first,last,node,prevPtr,nextPtr) \
	do \
	{ \
		if (!(first)) \
		{ \
			(first) = (last) = (node); \
			(node)->nextPtr = (node)->prevPtr = NULL; \
		} \
		else if ((first) == (last)) \
		{ \
			(first)->nextPtr = (node); \
			(node)->prevPtr = (first); \
			(node)->nextPtr = NULL; \
			(last) = (node); \
		} \
		else \
		{ \
			(last)->nextPtr = (node); \
			(node)->prevPtr = (last); \
			(node)->nextPtr = NULL; \
			(last) = (node); \
		} \
	} while(0)

#define REMOVE_NODE(first,last,node,prevPtr,nextPtr) \
	do \
	{ \
		if ((first) == (last)) \
			(first) = (last) = NULL; \
		else if ((first) == (node)) \
		{ \
			(first) = (first)->nextPtr; \
			(first)->prevPtr = NULL; \
		} \
		else if ((last) == (node)) \
		{ \
			(last) = (last)->prevPtr; \
			(last)->nextPtr = NULL; \
		} \
		else \
		{ \
			((node)->prevPtr)->nextPtr = (node)->nextPtr; \
			((node)->nextPtr)->prevPtr = (node)->prevPtr; \
		} \
		(node)->prevPtr = (node)->nextPtr = NULL; \
	} while(0)

/* ... */

#define REMOVE_FROM_LIST(item, head, next)		\
	if ((item) == (head))						\
		head = (item)->next;					\
												\
	else {										\
		temp = head;							\
												\
		while (temp && (temp->next != (item)))	\
			temp = temp->next;					\
												\
			if (temp)							\
				temp->next = (item)->next;		\
   }											\

/* basic bitvectorutils *************************************************/

#define Q_FIELD(x) ((int) (x) / 32)
#define Q_BIT(x) (1 << ((x) % 32))
#define IS_SET_AR(var, bit)   ((var)[Q_FIELD(bit)] & Q_BIT(bit))
#define SET_BIT_AR(var, bit)	((var)[Q_FIELD(bit)] |= Q_BIT(bit))
#define REMOVE_BIT_AR(var, bit)  ((var)[Q_FIELD(bit)] &= ~Q_BIT(bit))
#define TOGGLE_BIT_AR(var, bit)  ((var)[Q_FIELD(bit)] = \
					              (var)[Q_FIELD(bit)] ^ Q_BIT(bit))
#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BITK(var,bit)  ((var) |= (bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~(bit))
#define TOGGLE_BIT(var,bit) ((var) = (var) ^ (bit))
#define TOGGLE_FLAG(var,flag) ((var) = (var) ^ Q_BIT(flag))

#define NEWB_FLAGS(ch)					((ch)->PlayerData->newb)
#define MOB_FLAGS(ch)					((ch)->player.act)
#define PLR_FLAGS(ch)					((ch)->player.act)
#define PRF_FLAGS(ch)					((ch)->player.pref)
#define AFF_FLAGS(ch)					((ch)->player.affected_by)
#define ROOM_FLAGS(loc)					(loc->room_flags)

#define IS_NPC(ch)						(ch->MobData != nullptr)
#define IS_MOB(ch)						(IS_NPC(ch))

#define MOB_FLAGGED(ch, flag)			(IS_NPC(ch)  && IS_SET(MOB_FLAGS(ch), Q_BIT(flag)))
#define PLR_FLAGGED(ch, flag)			(!IS_NPC(ch) && IS_SET(PLR_FLAGS(ch), Q_BIT(flag)))
#define NEWB_FLAGGED(ch, flag)			(IS_SET_AR(NEWB_FLAGS(ch), (flag)))
#define AFF_FLAGGED(ch, flag)			(IS_SET_AR(AFF_FLAGS(ch), (flag)))
#define PRF_FLAGGED(ch, flag)			(IS_SET_AR(PRF_FLAGS(ch), (flag)))
#define ROOM_FLAGGED(loc, flag)			(IS_SET(ROOM_FLAGS(loc), (1<<flag)))
#define OBJVAL_FLAGGED(obj, flag)		(IS_SET(GET_OBJ_VAL((obj), 1), (flag)))
#define OBJWEAR_FLAGGED(obj, flag)		(IS_SET((obj)->obj_flags.wear_flags, (flag)))
#define OBJ_FLAGGED(obj, flag)			(IS_SET(GET_OBJ_EXTRA(obj), Q_BIT(flag)))

#define IS_HORSE(ch)					(MOB_FLAGGED(ch, MOB_MOUNT) || MOB_FLAGGED(ch, MOB_SHADOW_MOUNT) || MOB_FLAGGED(ch, MOB_OGIER_MOUNT))

/* IS_AFFECTED for backwards compatibility */
#define IS_AFFECTED(ch, skill) (AFF_FLAGGED((ch), (skill)))

#define PLR_TOG_CHK(ch,flag) ((TOGGLE_BIT(PLR_FLAGS(ch), (1<<flag))) && \
							  (IS_SET(PLR_FLAGS(ch), (1<<flag))))
#define PRF_TOG_CHK(ch,flag) ((TOGGLE_BIT_AR(PRF_FLAGS(ch), (flag))) && \
							  (IS_SET_AR(PRF_FLAGS(ch), (flag))))
#define NEWB_TOG_CHK(ch,flag) ((TOGGLE_BIT_AR(NEWB_FLAGS(ch), (flag))) && \
							  (IS_SET_AR(NEWB_FLAGS(ch), (flag))))


/* room utils ************************************************************/

#define IS_LIGHT(room)  (!room->isDark())

#define GET_ROOM_SPEC(room) ((room) != nullptr ? (room)->func : nullptr)

/* char utils ************************************************************/

#define GET_WAS_IN(ch)	((ch)->was_in_room)
#define GET_AGE(ch)     (age(ch)->year)

#define GET_NAME(ch)    (IS_NPC(ch) ? (ch)->player.short_descr : (ch)->player.name.c_str())
#define GET_TITLE(ch)   ((ch)->player.title)
#define GET_LEVEL(ch)   ((ch)->player.level)
#define GET_PASSWD(ch)	((ch)->player.passwd)

/*
 * I wonder if this definition of GET_REAL_LEVEL should be the definition
 * of GET_LEVEL?  JE
 */

#define IS_KING(ch)				(!str_cmp(GET_NAME(ch), "Galnor"))	//That's right!
#define IS_GAY(ch)				(!str_cmp(GET_NAME(ch), "Alder"))
#define IS_AWESOME(ch)			(!str_cmp(GET_NAME(ch), "Fogel"))
#define IS_QUEEN(ch)			(!str_cmp(GET_NAME(ch), "Avaine"))

#define GET_REAL_LEVEL(ch) \
   (ch->desc && ch->desc->original ? GET_LEVEL(ch->desc->original) : \
    GET_LEVEL(ch))

#define GET_CLASS(ch)			((ch)->player.chclass)
#define GET_RACE(ch)			((ch)->player.race)
#define GET_HEIGHT(ch)			((ch)->player.height)
#define GET_WEIGHT(ch)			((ch)->player.weight)
#define GET_SEX(ch)				((ch)->player.sex)

/* levels of taintedness, taint added by spell timer */
#define TAINT_MAX      (5000)


#define TAINT_CALC(ch)			(MiscUtil::random(0, (int) (TAINT_MAX + (TAINT_MAX * .25))) < ch->GetTotalTaint())

#define GET_EXP(ch)				((ch)->points.exp)
#define GET_HIT(ch)				((ch)->points.hit)
#define GET_MAX_HIT(ch)			((ch)->points.max_hit)
#define GET_MOVE(ch)			((ch)->points.move)
#define GET_MAX_MOVE(ch)		((ch)->points.max_move)
#define GET_MANA(ch)			((ch)->points.mana)
#define GET_MAX_MANA(ch)		((ch)->points.max_mana)
#define GET_SHADOW(ch)			((ch)->points.shadow_points)
#define GET_MAX_SHADOW(ch)		((ch)->points.max_shadow_points)
#define GET_WARRANTS(ch)		((ch)->points.warrants)
#define GET_OB(ch)				((ch)->points.offensive)
#define GET_PB(ch)				((ch)->points.parry)
#define GET_DB(ch)				((ch)->points.dodge)
#define GET_INVIS_LEV(ch)		((ch)->points.invis)
#define GET_ABS(ch)				((ch)->points.absorb)
#define GET_WP(ch)				((ch)->points.weave)
#define GET_BANK_GOLD(ch)		((ch)->points.bank_gold)
#define GET_DAMROLL(ch)			((ch)->points.damroll)
#define GET_DEATH_WAIT(ch)		((ch)->points.death_wait)
#define GET_WARNINGS(ch)		((ch)->points.warning)
#define GET_SLEW_MESSAGE(ch)	((ch)->points.slew)
#define POOFOUT(ch)				((ch)->points.poofout)
#define POOFIN(ch)				((ch)->points.poofin)
#define LAST_LOGON(ch)			((ch)->points.last_logon)
#define LAST_LOGOUT(ch)			((ch)->points.last_logout)
#define GET_MASTER_WEAPON(ch)	((ch)->points.master_weapon)
#define IS_BASHED(ch)			((ch)->points.is_bashed)
#define GET_TAINT(ch)			((ch)->points.taint)
#define GET_WHOIS_EXTRA(ch)		((ch)->points.whois_extra)
#define GET_STRAIN(ch)			((ch)->points.strain)

#define GET_ID(x)				((x)->id)
#define GET_POS(ch)				((ch)->player.position)
#define IS_CARRYING_W(ch)		((ch)->player.carry_weight)
#define FLEE_LAG(ch)			((ch)->player.flee_lag)
#define FLEE_GO(ch)				((ch)->player.flee_go)
#define GET_DIRECTION(ch)		((ch)->player.direction)
#define IS_CARRYING_N(ch)		((ch)->player.carry_items)
#define FIGHTING(ch)			((ch)->player.fighting)
#define HUNTING(ch)				((ch)->player.hunting)
#define MOUNT(ch)				((ch)->player.mount)
#define RIDDEN_BY(ch)			((ch)->player.ridden_by)
#define GET_TARGET(ch)			((ch)->player.target)
#define GET_TARGET2(ch)         ((ch)->player.target2)
#define GET_MARKED(ch)			((ch)->player.marked)

const int CLAN_GAIDIN = 1;
const int CLAN_ANDORAN = 2;
const int CLAN_SHIENARAN = 3;
const int CLAN_MURANDIAN = 4;
const int CLAN_COMPANION = 5;
const int CLAN_DEFENDER = 6;
const int CLAN_KOBOL = 7;
const int CLAN_DHAVOL = 8;
const int CLAN_ALGHOL = 9;
const int CLAN_KNOMON = 10;
const int CLAN_GHOBHLIN = 11;
const int CLAN_CHOSEN = 12;
const int CLAN_WOLFBROTHER = 13;
const int CLAN_BLADEMASTERS = 14;
const int CLAN_BLACK_TOWER = 15;
const int CLAN_WHITE_TOWER = 16;
const int CLAN_GRAY_AJAH = 17;
const int CLAN_RED_AJAH = 18;
const int CLAN_YELLOW_AJAH = 19;
const int CLAN_BROWN_AJAH = 20;
const int CLAN_WHITE_AJAH = 21;
const int CLAN_GREEN_AJAH = 22;
const int CLAN_BLUE_AJAH = 23;
const int CLAN_BLACK_AJAH = 24;
const int CLAN_SOULLESS = 25;
const int CLAN_DREADGUARDS = 27;
const int CLAN_MYRDDRAAL = 28;
const int CLAN_NIENDENEI = 29;
const int CLAN_OGIER = 30;
const int CLANS_MAX = 31;

#define IS_BONDED(ch)			(0)
#define IS_WARDER(ch)			((IS_BONDED(ch)) && (ch->getUserClan(CLAN_GAIDIN)))

#define IS_LONG_BLADE(obj)		(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_LONG_BLADE)
#define IS_SHORT_BLADE(obj)		(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_SHORT_BLADE)
#define IS_CLUB(obj)			(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_CLUB)
#define IS_STAFF(obj)			(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_STAFF)
#define IS_SPEAR(obj)			(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_SPEAR)
#define IS_AXE(obj)				(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_AXE)
#define IS_CHAIN(obj)			(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_CHAIN)
#define IS_BOW(obj)				(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_BOW)
#define IS_LANCE(obj)			(obj->getType() == ITEM_WEAPON && GET_OBJ_VAL(obj, 0) == WEAPON_LANCE)


#define IS_BODY_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_BODY))
#define IS_HEAD_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_HEAD))
#define IS_SHOULDER_GEAR(obj)	(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_SHOULDERS))
#define IS_ARM_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_ARMS))
#define IS_HAND_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_HANDS))
#define IS_LEG_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_LEGS))
#define IS_FEET_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_FEET))
#define IS_WRIST_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_WRIST))
#define IS_WAIST_GEAR(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_WAIST))
#define IS_EARRING(obj)			(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_EAR))
#define IS_RING(obj)			(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_FINGER))
#define IS_NECKLACE(obj)		(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_NECK))
#define IS_SHIELD(obj)			(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_SHIELD))
#define IS_CLOAK(obj)			(obj->getType() == ITEM_ARMOR && IS_SET(GET_OBJ_WEAR(obj), ITEM_WEAR_ABOUT))

#define GET_SKILL(ch, i)		((ch)->getSkill(i))
#define SET_SKILL(ch, i, pct)	((ch)->setSkill(i, pct))

#define GET_EQ(ch, i)			((ch)->equipment[i])

#define GET_MOB_SPEC(ch)		(IS_MOB(ch) ? MobManager::GetManager().GetIndex((unsigned int)(ch)->nr)->func : NULL)
#define GET_MOB_RNUM(mob)		((mob)->nr)
#define GET_MOB_VNUM(mob)		(IS_MOB(mob) ? MobManager::GetManager().GetIndex(mob->nr)->vnum : -1)
#define MEMORY(ch)				((ch)->MobData->memory)
#define GET_AGGRO(ch, num)		(IS_SET(ch->MobData->aggro, (1<<num)))


#define CAN_CARRY_W(ch)			( ch->GetStr() * 7 )
#define CAN_CARRY_N(ch)			(5 + (ch->GetDex() >> 1) + (GET_LEVEL(ch) >> 1))
#define AWAKE(ch)				(GET_POS(ch) > POS_SLEEPING)

#define CAN_SEE_IN_DARK(ch)		(PRF_FLAGGED(ch, PRF_HOLYLIGHT) || IS_TROLLOC(ch) || \
								ch->hasWolfbrotherBonuses() || AFF_FLAGGED(ch, AFF_NIGHT_VISION))

/* descriptor-based utils ************************************************/

#define WAIT_STATE(ch, cycle) ( ch->WaitState( cycle ) )

#define LAST_COMMAND(ch) ((ch)->desc->delayed_command)
#define CHECK_WAIT(ch)	(ch->wait > 1 ? ch->wait : 0)
#define STATE(d)	((d)->connected)


/* object utils **********************************************************/

#define IS_SHEATH(obj)			(IS_OBJ_STAT(obj, ITEM_SWORD_SHEATH) || IS_OBJ_STAT(obj, ITEM_DAGGER_SHEATH))

#define GET_OBJ_COST(obj)		((obj)->obj_flags.cost)
#define GET_OBJ_RENT(obj)		((obj)->obj_flags.cost_per_day)
#define GET_OBJ_EXTRA(obj)		((obj)->obj_flags.extra_flags)
#define GET_OBJ_WEAR(obj)		((obj)->obj_flags.wear_flags)
#define GET_OBJ_VAL(obj, val)	((obj)->obj_flags.value[(val)])
#define GET_OBJ_WEIGHT(obj)		((obj)->obj_flags.weight)
#define GET_OBJ_OB(obj)			((obj)->obj_flags.offensive)
#define GET_OBJ_PB(obj)			((obj)->obj_flags.parry)
#define GET_OBJ_DB(obj)			((obj)->obj_flags.dodge)
#define GET_OBJ_ABS(obj)		((obj)->obj_flags.absorb)
#define GET_OBJ_CLAN(obj)		((obj)->obj_flags.clan)
#define GET_OBJ_CL_WEIGHT(obj)	((obj)->obj_flags.clan_weight)
#define GET_OBJ_CL_OB(obj)		((obj)->obj_flags.clan_offensive)
#define GET_OBJ_CL_PB(obj)		((obj)->obj_flags.clan_parry)
#define GET_OBJ_CL_DB(obj)		((obj)->obj_flags.clan_dodge)
#define GET_OBJ_CL_ABS(obj)		((obj)->obj_flags.clan_absorb)
#define GET_OBJ_CL_MVS(obj)		((obj)->obj_flags.clan_moves)
#define GET_OBJ_CL_HPS(obj)		((obj)->obj_flags.clan_hit)
#define GET_OBJ_CL_DMG1(obj)	((obj)->obj_flags.clan_dmg1)
#define GET_OBJ_CL_DMG2(obj)	((obj)->obj_flags.clan_dmg2)
#define GET_OBJ_TIMER(obj)		((obj)->obj_flags.timer)
#define GET_OBJ_RNUM(obj)		((obj)->item_number)
#define GET_OBJ_VNUM(obj)		(GET_OBJ_RNUM(obj) >= 0 ? \
								obj_index[GET_OBJ_RNUM(obj)].vnum : -1)

#define IS_OBJ_STAT(obj,stat)	(IS_SET((obj)->obj_flags.extra_flags,Q_BIT(stat)))

#define IS_CORPSE(obj)			(obj->getType() == ITEM_CONTAINER && \
								GET_OBJ_VAL((obj), 3) == 1)

#define GET_OBJ_SPEC(obj) ((obj)->item_number >= 0 ? \
	(obj_index[(obj)->item_number].func) : NULL)

#define CAN_WEAR(obj, part) (IS_SET((obj)->obj_flags.wear_flags, (part)))

/* compound utilities and other macros **********************************/

#define HSHR(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "his":"her") :"its")
#define HSSH(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "he" :"she") : "it")
#define HMHR(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "him":"her") : "it")
#define HSHS(ch) (GET_SEX(ch) ? (GET_SEX(ch)==SEX_MALE ? "himself":"herself") : "itself")

#define ANA(obj) (strchr("aeiouyAEIOUY", *(obj)->name) ? "An" : "A")
#define SANA(obj) (strchr("aeiouyAEIOUY", *(obj)->name) ? "an" : "a")


/* Various macros building up to CAN_SEE */

#define LIGHT_OK(sub)	(/*!AFF_FLAGGED(sub, AFF_BLIND) &&*/ \
   (IS_LIGHT((sub)->in_room) || IS_TROLLOC(sub) || sub->hasWolfbrotherBonuses() || IS_NPC(sub) || \
	AFF_FLAGGED(sub, AFF_NIGHT_VISION)))

#define INVIS_OK(sub, obj) \
	((!AFF_FLAGGED((obj), AFF_HIDE) || (AFF_FLAGGED(sub, AFF_NOTICE)) && \
	GET_SKILL(sub, SKILL_NOTICE) && MiscUtil::random(GET_SKILL(sub, SKILL_NOTICE), GET_SKILL(sub, SKILL_NOTICE) + 30) \
	>= GET_SKILL(obj, SKILL_HIDE)))

#define MORT_CAN_SEE(sub, obj) (LIGHT_OK(sub) && INVIS_OK(sub, obj))

#define IMM_CAN_SEE(sub, obj) \
	(MORT_CAN_SEE(sub, obj) || (!IS_NPC(sub) && PRF_FLAGGED(sub, PRF_HOLYLIGHT)))

#define SELF(sub, obj)  ((sub) == (obj))

/* Can subject see character "obj"? */
#define CAN_SEE(sub, obj) (((GET_REAL_LEVEL(sub) >= (GET_INVIS_LEV(obj))) && \
   IMM_CAN_SEE(sub, obj)) || SELF(sub, obj))

/* End of CAN_SEE */

#define INVIS_OK_OBJ(sub, obj) \
	(!IS_OBJ_STAT((obj), ITEM_INVISIBLE))

/* Is anyone carrying this object and if so, are they visible? */
#define CAN_SEE_OBJ_CARRIER(sub, obj) \
  ((!obj->carried_by || CAN_SEE(sub, obj->carried_by)) &&	\
   (!obj->worn_by || CAN_SEE(sub, obj->worn_by)))

#define MORT_CAN_SEE_OBJ(sub, obj) \
	(LIGHT_OK(sub) && INVIS_OK_OBJ(sub, obj) && CAN_SEE_OBJ_CARRIER(sub, obj))

#define CAN_SEE_OBJ(sub, obj) \
   (MORT_CAN_SEE_OBJ(sub, obj) || (!IS_NPC(sub) && PRF_FLAGGED((sub), PRF_HOLYLIGHT)))

#define OBJS(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? \
	((obj)->retool_sdesc ? (obj)->retool_sdesc : (obj)->short_description)  : "something")

#define OBJN(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? \
	fname( ( (obj)->retool_name ? (obj)->retool_name : (obj)->name ) ) : "something")


#define EXIT(ch, door)  (ch->in_room->dir_option[door])

#define CAN_GO(ch, door) (EXIT(ch,door) && \
			 (EXIT(ch,door)->getToRoom()) && \
			 !EXIT(ch, door)->isClosed() && \
			 !EXIT(ch,door)->isDisabled())

//Allow classes set to 1 to be selectable upon creation
#define	CHANNELER_SELECT
#define THIEF_SELECT
#define RANGER_SELECT
#define WARRIOR_SELECT

#define IS_WARRIOR(ch)		((GET_CLASS(ch) == CLASS_WARRIOR))
#define IS_THIEF(ch)		((GET_CLASS(ch) == CLASS_THIEF))
#define IS_RANGER(ch)		((GET_CLASS(ch) == CLASS_RANGER))
#define IS_CHANNELER(ch)	((GET_CLASS(ch) == CLASS_CHANNELER))
#define IS_FADE(ch)			((GET_CLASS(ch) == CLASS_FADE))
#define IS_DREADLORD(ch)	((GET_CLASS(ch) == CLASS_DREADLORD))
#define IS_BLADEMASTER(ch)	((GET_CLASS(ch) == CLASS_BLADEMASTER))
#define IS_GREYMAN(ch)		((GET_CLASS(ch) == CLASS_GREYMAN))
#define IS_DREADGUARD(ch)	((GET_CLASS(ch) == CLASS_DREADGUARD))
#define IS_OGIER(ch)		((GET_CLASS(ch) == CLASS_OGIER))

#define CAN_CHANNEL(ch)		((IS_DREADLORD(ch) || IS_DREADGUARD(ch) || IS_CHANNELER(ch)))


#define IS_HUMAN(ch)		((GET_RACE(ch) == RACE_HUMAN))
#define IS_TROLLOC(ch)		((GET_RACE(ch) == RACE_TROLLOC))
#define IS_SEANCHAN(ch)		((GET_RACE(ch) == RACE_SEANCHAN))
#define IS_AIEL(ch)			((GET_RACE(ch) == RACE_AIEL))
#define IS_ANIMAL(ch)		((GET_RACE(ch) == RACE_ANIMAL))

#define CAN_RIDE(ch)		(!( (IS_TROLLOC(ch) || ch->hasWolfbrotherBonuses()) && !IS_FADE(ch) && !IS_DREADLORD(ch)))

#define INTERVAL(a, b, c)   (((b) < (a)) ? (a) : (((b) > (c)) ? (c) : (b)))

const int AGGRO_ALL = 0;
const int AGGRO_HUMAN = 1;
const int AGGRO_TROLLOC = 2;
const int AGGRO_SEANCHAN = 3;
const int AGGRO_AIEL = 4;
const int AGGRO_NON_CLAN = 5;
const int AGGRO_MOB = 6;

#define OUTSIDE(ch) ((ch)->in_room->getSector() != RoomSector::inside)

#define ATTACK_TYPE(obj, val) (


/* OS compatibility ******************************************************/


/* there could be some strange OS which doesn't have NULL... */
#ifndef NULL
#define NULL (void *)0
#endif

#if !defined(FALSE)
const int FALSE = 0;
#endif

#if !defined(TRUE)
#define TRUE  (!FALSE)
#endif

/* defines for fseek */
#ifndef SEEK_SET
const int SEEK_SET = 0;
const int SEEK_CUR = 1;
const int SEEK_END = 2;
#endif


#define STM(ch, till)			ch->send("%s%sTICK IN %d SECONDS.%s\r\n", COLOR_BOLD(ch, CL_COMPLETE), \
COLOR_GREEN(ch, CL_COMPLETE), till, COLOR_NORMAL(ch, CL_COMPLETE))

/*
 * NOCRYPT can be defined by an implementor manually in sysdep.h.
 * CIRCLE_CRYPT is a variable that the 'Confure' script
 * automatically sets when it determines whether or not the system is
 * capable of encrypting.
 */
#if defined(NOCRYPT) || !defined(CIRCLE_CRYPT)
#define CRYPT(a,b) (a)
#else
#define CRYPT(a,b) ((char *) crypt((a),(b)))
#endif

#define SENDOK(ch)	((ch)->desc && (to_sleeping || AWAKE(ch)) && \
			!PLR_FLAGGED((ch), PLR_WRITING))


#define Plural(n)	(n == 1 ? "" : "s")

template<typename T>
std::string ToString(const T &c)
{
	std::ostringstream strstm;
	strstm << c;
	return strstm.str();
}

template< typename _container >
std::string PrintContainer( _container c )
{
	std::stringstream ss;
	for(typename _container::iterator i = c.begin();i != c.end();++i)
	{
		ss << (*i) << " ";
	}
	return ss.str();
}

int strlenIgnoreColors( const char* str );
bool convert_string_to_bool(const std::string &s);
std::vector<bool> split_to_bool_vector(const std::string &s);

template<typename T_thing, typename T_container >
bool in(T_thing thing, T_container container)
{
	typename T_container::iterator iter = container.begin();
	for ( ; iter != container.end() ; ++iter )
		{
			if (*iter == thing)
			{
				return true;
			}
		}
	return false;
}

#endif

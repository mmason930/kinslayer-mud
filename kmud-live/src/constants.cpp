/* ************************************************************************
*   File: constants.c                                   Part of CircleMUD *
*  Usage: Numeric and std::string contants used by the MUD                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "constants.h"

/* strings corresponding to ordinals/bitvectors in structs.h ***********/


/* (Note: strings for class definitions in class.c instead of here) */

/* Slopes and Graph Directions */
const char *loc[10] =
    {
        "unknown",
        "north",
        "east",
        "south",
        "west",
        "northeast",
        "northwest",
        "southeast",
        "southwest",
        "nearby",
    };

/* cardinal directions */
const char *dirs[] =
    {
        "north",
        "east",
        "south",
        "west",
        "up",
        "down",
        "\n"
    };

const char *aggros[] =
    {

        "ALL",
        "HUMANS",
        "TROLLOCS",
        "SEANCHAN",
        "AIEL",
        "ALL_NOT_IN_MY_CLAN",
        "MOBS",
        "\n"
    };

const char *class_types[] =
    {

        "WARRIOR",
        "THIEF",
        "RANGER",
        "CHANNELER",
        "FADE",
        "DREADLORD",
        "BLADEMASTER",
        "GREYMAN",
        "DRAGHKAR",
		"DREADGUARD",
		"OGIER",
        "\n"
    };

/* ROOM_x */
const char *room_bits[] = {
                        "DARK",
                        "DEATH",
                        "!MOB",
                        "INDOORS",
                        "PEACEFUL",
                        "SOUNDPROOF",
                        "!TRACK",
                        "!MAGIC",
                        "TUNNEL",
                        "PRIVATE",
                        "GODROOM",
                        "HOUSE",
                        "VAULT",
                        "ATRIUM",
                        "OLC",
                        "*",				/* BFS MARK */
                        "NO TRAVEL",
                        "POKER TABLE",
                        "NO RENT",
                        "NO SOURCE",
						"NO HIDE",
						"LIT",
						"MINING",
						"FISHING",
						"SMITHING",
						"TAILORING",
                        "\n"
                    };


/* EX_x */
const char *exit_bits[] =
{
	"DOOR",
	"CLOSED",
	"LOCKED",
	"\n"
};

/*
 * SEX_x
 * Not used in sprinttype() so no \n.
 */
const char *genders[] =
{
	"Neutral",
	"Male",
	"Female"
};


/* POS_x */
const char *position_types[] = {
                             "Dead",
                             "Mortally wounded",
                             "Incapacitated",
                             "Stunned",
                             "Sleeping",
                             "Resting",
                             "Sitting",
                             "Fighting",
                             "Standing",
							 "Flying",
                             "\n"
                         };


/* PLR_x */
const char *player_bits[] = {
                          "DARKFRIEND",
                          "MURDERER",
                          "FROZEN",
                          "DONTSET",
                          "WRITING",
                          "MAILING",
                          "CRASH",
                          "SITEOK",
                          "NOSHOUT",
                          "NOTITLE",
                          "DELETED",
                          "LOADRM",
                          "!WIZL",
                          "!DEL",
                          "INVST",
                          "CRYO",
                          "LOGGER",
                          "NO TRACK",
                          "",
                          "ZBAN",
						  "!IDLE",
						  "!HOST",
						  "!WEAVE",
						  "!TRACE",
						  "WEAVE_EDITOR",
						  "STAT_EDITOR",
						  "CHARGE_EDITOR",
						  "<UNUSED>",
						  "<UNUSED>",
						  "<UNUSED>",
						  "GLOBAL_SCRIPT_EDITOR",
						  "NOGLOBAL",
                          "\n"
                      };

const char * sun_types[] =
{
	"Dark",
	"Rising",
	"Light",
	"Setting",
	"None",
	"\n"
};

const int NUM_SUN_TYPES = 4;


const char * weather_patterns[] =
{
	"The sky is cloudless.",
	"The sky is cloudy.",
	"The sky is filled with rain.",
	"The sky is filled with snow.",
	"The sky is plagued by hail.",
	"The sky is washed out by a blizzard.",
	"The sky is filled with fog.",
	"\n"
};

const int NUM_WEATHER_PATTERNS = 9;

const char * weather_event_names[] =
{
	"nothing special",
	"a heat wave",
	"a cold snap",
	"a drought",
	"a flood",
	"\n"
};

const char * weather_storm_names[] =
{
	"nothing",
	"a faint drizzle",
	"a rain storm",
	"a freezing drizzle",
	"a bout of freezing rain",
	"a snow storm",
	"a large snow storm", // pellets
	"a small snow storm", // grains
	"a storm of ice pellets",
	"a hail storm",
	"a blizzard",
	"a wall of fog",
	"\n"
};

/* MOB_x */
const char *action_bits[] = {
                          "SPEC",					//0
                          "SENTINEL",
                          "SCAVENGER",
                          "ISNPC",
                          "AWARE",
                          "STAY-ZONE",			//5
                          "WIMPY",
                          "MEMORY",
                          "HELPER",
                          "!SLEEP",
                          "!BLIND",				//10
                          "MOUNT",
                          "TRACK",
                          "BASH",
                          "AWARD",
                          "SHADOW_MOUNT",			//15
                          "NOFIGHT",
                          "INVIS",
                          "GHOST",
                          "TRAINER",
                          "INNKEEPER",			//20
                          "BANKER",
						  "POKER DEALER",
						  "SMOB LEADER",
						  "TRADE TRAINER",
						  "!TRACK",
						  "!GROUP",
						  "OGIER_MOUNT",
                          "\n"
                      };

/* PRF_x */
const char *preference_bits[] = {
                              "BRIEF",
                              "COMPACT",
                              "SOURCE",
                              "!TELL",
                              "D_HP",
                              "D_MANA",
                              "D_MOVE",
                              "AUTOEX",
                              "!HASS",
                              "QUEST",
                              "SUMN",
                              "!REP",
                              "LIGHT",
                              "C1",
                              "C2",
                              "!WIZ",
                              "L1",
                              "L2",
							  "!SPEAKS",
                              "SPARE2",
                              "SPARE3",
                              "RMFLG",
                              "KIT",
                              "STATTED",
                              "COUNCIL",
                              "INCOG",
                              "!TELL",
                              "!NARR",
                              "!CHAT",
                              "!YELL",
                              "!SHOUT",
                              "\n"
                          };


/* AFF_x */
const char *affected_bits[] =
    {
        "\0",
        "BLIND",
        "INVIS",
        "WATWALK",
        "SANCTUARY",
        "GROUP",
        "POISON",
        "SLEEP",
        "INCOGNITO",
        "SNEAK",
        "HIDE",
        "NOTICE",
        "NO QUIT",
        "PARANOIA",
        "NIGHT VISION",
        "HASTE",
        "SHIELD",
        "EFFUSION",
        "AGILITY",
        "STRENGTH",
        "COVER TRACKS",
        "DIZZINESS",
		"WET",
		"CONFUSED",
		"FORCED PEACE",
		"HIDDEN INTENT",
		"DISGUISE",
		"INVERTED POWER",
		"ZING SHIELD",
		"INSANE",
		"SILENCE",
		"CHILL",
		"FEAR",
		"PANIC",
		"REFLECT",
		"SENSE SHADOW",
		"SHIELD BLOCK",
		"SEVERE WOUND",
		"HAMSTRING",
		"EAVESDROP",
		"DEAF",
		"WEAKEN",
		"CRIPPLE",
		"SLOW",
		"DECAY",
		"SLICE",
		"BURN",
		"DAZE",
		"SHADOW RAGE",
		"REGENERATION",
		"HEALING",
		"DARK PLAGUE",
        "\n"
    };

	const char *affected_bits_none_ended[] =
	{
		"NONE",
		"BLIND",
		"INVIS",
		"WATWALK",
		"SANCT",
		"GROUP",
		"POISON",
		"SLEEP",
		"INCOGNITO",
		"SNEAK",
		"HIDE",
		"NOTICE",
		"NOQUIT",
		"PARANOIA",
		"NIGHT VISION",
		"HASTE",
		"SOURCE SHIELD",
		"EFFUSION",
		"AGILITY",
		"STRENGTH",
		"COVER TRACKS",
		"DIZZINESS",
		"WET",
		"CONFUSED",
		"FORCED PEACE",
		"HIDDEN INTENT",
		"DISGUISE",
		"INVERTED POWER",
		"ZING SHIELD",
		"INSANE",
		"SILENCE",
		"CHILL",
		"FEAR",
		"PANIC",
		"REFLECT",
		"SENSE SHADOW",
		"SHIELD BLOCK",
		"SEVERE WOUND",
		"HAMSTRING",
		"EAVESDROP",
		"DEAF",
		"WEAKEN",
		"CRIPPLE",
		"SLOW",
		"DECAY",
		"SLICE",
		"BURN",
		"DAZE",
		"SHADOW RAGE",
		"REGENERATION",
		"HEALING",
		"DARK PLAGUE",
		"\n"
	};

const int LENGTH_OF_AFFECTED_BITS = 51;

// CON_x
const char *connected_types[] =
{
	"Playing",						//0
	"Disconnecting",
	"Get name",
	"Confirm name",
	"Get password",
	"Get new PW",					//5
	"Confirm new PW",
	"Select sex",
	"Select class",
	"Reading MOTD",
	"Main Menu",					//10
	"Get descript.",
	"Changing PW 1",
	"Changing PW 2",
	"Changing PW 3",
	"Self-Delete 1",				//15
	"Self-Delete 2",
	"Disconnecting",
	"Newbie message",
	"Choosing stats",
	"Choosing race",				//20
	"Resolving hostname",
	"Stat editor",
	"Auction interface",
	"Object editor",
	"Room editor",					//25
	"Zone editor",
	"Mobile editor",
	"Shop editor",
	"DG editor",
	"Kit editor",					//30
	"Config editor",
	"Clan editor",
	"Warrant editor",
	"Stat editor",
	"JavaScript editor",			//35
	"Gateway handshaking",
	"Configuring email",
	"Entering email",
	"Websocket login",
	"\n"
};

const char *weapon_types[] = {
	"LONG BLADE",
	"SHORT BLADE",
	"CLUB",
	"STAFF",
	"SPEAR",
	"AXE",
	"CHAIN",
	"BOW",
	"LANCE"
};

const int NUM_WEAPON_TYPES = 9;

/*
 * WEAR_x - for eq list
 * Not use in sprinttype() so no \n.
 */
const char *where[] = {
                    "<used as light>             ",
                    "<held>                      ",
                    "<worn around neck>          ",
                    "<worn around neck>          ",
                    "<worn in right ear>         ",
                    "<worn in left ear>          ",
                    "<worn on head>              ",
                    "<worn about body>           ",
                    "<worn on back>              ",
                    "<worn on body>              ",
                    "<worn on shoulders>         ",
                    "<worn on arms>              ",
                    "<worn about waist>          ",
                    "<worn around wrist>         ",
                    "<worn around wrist>         ",
                    "<worn on hands>             ",
                    "<worn on finger>            ",
                    "<worn on finger>            ",
                    "<wielded>                   ",
                    "<worn as shield>            ",
                    "<worn on legs>              ",
                    "<worn on feet>              ",
                };
/* WEAR_x - for stat */
const char *equipment_types[] = {
                              "Used as light",
                              "Held",
                              "First worn around Neck",
                              "Second worn around Neck",
                              "Worn in right ear",
                              "Worn in left ear",
                              "Worn on head",
                              "Worn about body",
                              "Worn on back",
                              "Worn on body",
                              "Worn on shoulders",
                              "Worn on arms",
                              "Worn around waist",
                              "Worn around right wrist",
                              "Worn around left wrist",
                              "Worn on hands",
                              "Worn on right finger",
                              "Worn on left finger",
                              "Wielded",
                              "Worn as shield",
                              "Worn on legs",
                              "Worn on feet",
                              "\n"
                          };

/* ITEM_x (ordinal object types) */
const char *item_types[] = {
                         "UNDEFINED",
                         "LIGHT",
                         "MONEY",
                         "WEAPON",
                         "ARMOR",
                         "POTION",
                         "OTHER",
                         "CONTAINER",
                         "NOTE",
                         "LIQ CONTAINER",
                         "KEY",
                         "FOOD",
                         "PEN",
                         "BOAT",
                         "FOUNTAIN",
                         "ANGREAL",
                         "CHAIR",
						 "SPECIAL",
						 "LAMPPOST",
						 "SKIN",
                         "\n"
                     };

/* ITEM_WEAR_ (wear bitvector) */
const char *wear_bits[] = {
                        "TAKE",
                        "FINGER",
                        "NECK",
                        "BODY",
                        "HEAD",
                        "LEGS",
                        "FEET",
                        "HANDS",
                        "ARMS",
                        "SHIELD",
                        "ABOUT",
                        "WAIST",
                        "WRIST",
                        "WIELD",
                        "HOLD",
                        "BACK",
                        "SHOULDERS",
                        "EARS",
                        "\n"
                    };

const char* body_parts[] =
    {
        "light",
        "held",
        "neck",
        "neck",
        "left ear",
        "right ear",
        "head",
        "about body",
        "back",
        "body",
        "shoulders",
        "arms",
        "waist",
        "right wrist",
        "left wrist",
        "hands",
        "right finger",
        "left finger",
        "weapon",
        "shield",
        "legs",
        "feet",
    };

/* ITEM_x (extra bits) */
const char *extra_bits[] = {
                         "!RENT",
                         "INVISIBLE",
                         "!DROP",
                         "WARD",
                         "!SELL",
                         "CHAIN",
                         "POISON",
                         "CHEST",
                         "TWO_HANDED",
                         "DAGGER SHEATH",
                         "SWORD SHEATH",
						 "TEMPORARY",
						 "NO SHOW",
						 "CLAN EQUIPMENT",
						 "SPEAR SHEATH",
						 "RARE",
						 "TRADES",
                         "\n"
                     };

/* APPLY_x */
const char *apply_types[] = {
                          "NONE",
                          "STR",
                          "DEX",
                          "INT",
                          "WIS",
                          "CON",
                          "CHA",
                          "CLASS",
                          "LEVEL",
                          "AGE",
                          "CHAR_WEIGHT",
                          "CHAR_HEIGHT",
                          "MAXMANA",
                          "MAXHIT",
                          "MAXMOVE",
                          "GOLD",
                          "EXP",
                          "ARMOR",
                          "HITROLL",
                          "DAMROLL",
                          "RACE",
                          "\n"
                      };

const int LENGTH_OF_APPLY_TYPES = 21;


/* CONT_x */
const char *container_bits[] = {
                             "CLOSEABLE",
                             "CLOSED",
                             "LOCKED",
                             "\n",
                         };


/* LIQ_x */
const char *drinks[] =
    {
        "water",
        "beer",
        "wine",
        "ale",
        "dark ale",
        "whisky",
        "lemonade",
        "firebreather",
        "local speciality",
        "slime mold juice",
        "milk",
        "tea",
        "coffee",
        "blood",
        "salt water",
        "clear water",
        "oil",
		"apple cider",
        "\n"
    };


/* other constants for liquids ******************************************/


/* one-word alias for each drink */
const char *drinknames[] =
    {
        "water",
        "beer",
        "wine",
        "ale",
        "ale",
        "whisky",
        "lemonade",
        "firebreather",
        "local",
        "juice",
        "milk",
        "tea",
        "coffee",
        "blood",
        "salt",
        "water",
        "oil",
		"apple cider",
        "\n"
    };


/* effect of drinks on hunger, thirst, and drunkenness -- see values.doc */
const int drink_aff[][3] = {
	//					  D  H  T
                         {0, 1, 10},
                         {3, 2, 5},
                         {5, 2, 5},
                         {2, 2, 5},
                         {1, 2, 5},
                         {6, 1, 4},
                         {0, 1, 8},
                         {10, 0, 0},
                         {3, 3, 3},
                         {0, 4, -8},
                         {0, 3, 6},
                         {0, 1, 6},
                         {0, 1, 6},
                         {0, 2, -1},
                         {0, 1, -2},
                         {0, 0, 13},
                         {0, 0, 0},
						 {0, 1, 4}
                     };


/* color of the various drinks */
const char *color_liquid[] =
    {
        "clear",
        "brown",
        "clear",
        "brown",
        "dark",
        "golden",
        "red",
        "green",
        "clear",
        "light green",
        "white",
        "brown",
        "black",
        "red",
        "clear",
        "crystal clear",
        "black",
        "\n"
    };

const char * weave_affect_usage[] =
{
	"Use one affect",
	"Use all affects",
	"Use a random affect",
	"\n"
};

const char * weave_types[] =
{
	"Simple",
	"Special",
	"Object",
	"\n"
};


const char * weave_target_types[] =
{
	"Self",
	"Room",
	"None",
	"Normal",
	"Item",
	"Fighting",
	"\n"
};

const char * weave_damage_types[] =
{
	"Health",
	"Moves",
	"Spell Points",
	"Dark Points",
	"\n"
};

const char * stats[] =
{
	"STR",
	"DEX",
	"INT",
	"WIS",
	"CON",
	"CHA",
	"\n"
};

const char * weave_can_cast_flags[] =
{
	"Male Channelers",
	"Female Channelers",
	"Fades",
	"Others",
	"All",
	"\n"
};

const char * weave_other_flags[] =
{
	"Taint",
	"Tower",
	"Level",
	"Dodge",
	"Parry",
	"Abs",
	"\n"
};

const char * weave_can_cast[] =
{
	"Male Channelers",
	"Female Channelers",
	"Fades",
	"Others",
	"All",
	"\n"
};


const char * sexes[] =
{
	"Neutral",
	"Male",
	"Female",
	"\n"
};

const int NUM_SEXES = 3;

const int usage_and_see_gain_constants[] =
{
	10,
	10,
	8,
	7,
	5,
	5,
	5,
	5,
	2,
	1,
};

/*
 * level of fullness for drink containers
 * Not used in sprinttype() so no \n.
 */
const char *fullness[] =
    {
        "less than half ",
        "about half ",
        "more than half ",
        ""
    };

std::vector<int> CarryWeight;

const char *spell_wear_off_msg[] = {
                                 "RESERVED DB.C",		/* 0 */
                                 "You feel less protected.",	/* 1 */
                                 "!Teleport!",
                                 "You feel less righteous.",
                                 "You feel a cloak of blindness disolve.",
                                 "!Burning Hands!",		/* 5 */
                                 "!Call Lightning",
                                 "You feel more self-confident.",
                                 "You feel your strength return.",
                                 "!Clone!",
                                 "!Color Spray!",		/* 10 */
                                 "!Control Weather!",
                                 "!Create Food!",
                                 "!Create Water!",
                                 "!Cure Blind!",
                                 "!Cure Critic!",		/* 15 */
                                 "!Cure Light!",
                                 "You feel more optimistic.",
                                 "You feel less protected.",
                                 "Your eyes stop tingling.",
                                 "The detect magic wears off.",/* 20 */
                                 "The detect poison wears off.",
                                 "!Dispel Evil!",
                                 "!Earthquake!",
                                 "Your vision fades back to normal.",
                                 "!Energy Drain!",		/* 25 */
                                 "!Fireball!",
                                 "!Harm!",
                                 "!Heal!",
                                 "You feel yourself exposed.",
                                 "!Lightning Bolt!",		/* 30 */
                                 "!Locate object!",
                                 "!Magic Missile!",
                                 "You feel less sick.",
                                 "You feel less protected.",
                                 "!Remove Curse!",		/* 35 */
                                 "The white aura around your body fades.",
                                 "!Shocking Grasp!",
                                 "You feel less tired.",
                                 "You feel weaker.",
                                 "!Summon!",			/* 40 */
                                 "!Ventriloquate!",
                                 "!Word of Recall!",
                                 "!Remove Poison!",
                                 "You feel less aware of your suroundings.",
                                 "!Animate Dead!",		/* 45 */
                                 "!Dispel Good!",
                                 "!Group Armor!",
                                 "!Group Heal!",
                                 "!Group Recall!",
                                 "Your night vision seems to fade.",	/* 50 */
                                 "Your feet seem less boyant.",
                                 "!UNUSED!"
                             };

const int rev_dir[] =
    {
        2,
        3,
        0,
        1,

#if defined(OASIS_MPROG)
        /*
         * Definitions necessary for MobProg support in OasisOLCharacter
         */
        char *mobprog_types[] = {
                                    "INFILE",
                                    "ACT",
                                    "SPEECH",
                                    "RAND",
                                    "FIGHT",
                                    "DEATH",
                                    "HITPRCNT",
                                    "ENTRY",
                                    "GREET",
                                    "ALL_GREET",
                                    "GIVE",
                                    "BRIBE",
                                    "\n"
                                };
#endif

        5,
        4
    };

/* Not used in sprinttype(). */
const char *weekdays[] = {
                       "Sunday",
                       "Monday",
                       "Tuesday",
                       "Wednesday",
                       "Thursday",
                       "Friday",
                       "Saturday"
                   };


/* Not used in sprinttype(). */
const char *month_name[] = {
                         "First Month",		/* 0 */
                         "Second Month",
                         "Third Month",
                         "Fourth Month",
                         "Fifth Month",
                         "Sixth Month",
                         "Seventh Month",
                         "Eighth Month",
                         "Ninth Month",
                         "Tenth Month",
                         "Eleventh Month",
                         "Twelfth Month",
                         "Thirteenth Month",
                         "Fourteenth Month",
                         "Fifteenth Month",
                         "Sixteenth Month",
                         "Seventeenth Month"
                     };

const std::string RealMonths[] =
    {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };

const char *weave_special_functions[] =
{
	"None",
	"Travel",
	"Change Weather",
	"Create Water",
	"Create Fog",
	"Locate Object",
	"Disguise",
	"Wall of Ice",
	"Whirlwind",
	"Incinerate",
	"Death Gate",
	"Animation",
	"Bond",
	"Enhance Weapon",
	"\n"
};

std::vector<int> DaysPerRealMonth(12);

std::string get_names_from_array(std::list<int> &values, const char * names[])
{
	std::list<int>::iterator iter = values.begin();
	std::string s = "";
	for ( ; iter != values.end() ; ++iter )
	{
			s.append(" ");
			s.append(names[*iter]);
	}
	return s;
}


std::string get_true_bool_names_from_array(const std::vector<bool> &values, const char * names[])
{
	std::string s = "";

	for (int i = 0 ; i < (signed) values.size() ; ++i )
	{
		if ((values.at(i) == 1) && strcmp(names[i], "\n"))
		{
				s.append(" ");
				s.append(names[i]);
		}
	}
	return s;
}

std::string get_names_from_nl_ended_array(const char * a[])
{
	std::string s = "";
	for ( int i = 0; strcmp(a[i],"\n") ; ++i  )
	{
		s.append(" ");
		s.append(a[i]);
	}
	return s;
}

void toggle_list_int(std::list<int> &li, int i)
{
	bool toggled = false;
	std::list<int>::iterator iter = li.begin();
	for ( ; iter != li.end() ; ++iter )
	{
		if (*iter == i)
		{
			li.erase(iter);
			toggled = true;
			break;
		}
	}
	if (!toggled)
		li.push_back(i);
}

const int NUM_STATS = 6;
const int WSP_NONE = 0;
const int WSP_TRAVEL = 1;
const int WSP_CHANGE_WEATHER = 2;
const int WSP_CREATE_WATER = 3;
const int WSP_CREATE_FOG = 4;
const int WSP_LOCATE_OBJECT = 5;
const int WSP_DISGUISE = 6;
const int WSP_WALL_OF_ICE = 7;
const int WSP_WHIRLWIND = 8;
const int WSP_INCINERATE = 9;
const int WSP_DEATH_GATE = 10;
const int WSP_ANIMATION = 11;
const int WSP_BOND = 12;
const int WSP_ENHANCE_WEAPON = 13;

const int NUM_SPECIAL_FUNCTIONS = 14;

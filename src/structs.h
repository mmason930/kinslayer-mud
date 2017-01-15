/* ************************************************************************
*   File: structs.h                                     Part of CircleMUD *
*  Usage: header file for central structures and contstants               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef STRUCTS_H
#define STRUCTS_H

#include "conf.h"
#include "sysdep.h"

#include "DateTime.h"

class UserType;
class Quest;
class Zone;
class Character;
class Room;
class Object;
class EditorInterfaceInstance;
char	*str_dup(const char *source);

/* preamble *************************************************************/

#define NOWHERE    -1    /* nil reference for room-database	*/
#define NOTHING	   -1    /* nil reference for objects		*/
#define NOBODY	   -1    /* nil reference for mobiles		*/

#define SPECIAL(name) int (name)(class Character *ch, void *me, char *cmd, char *argument)

/* format modes for format_text */
#define FORMAT_INDENT		(1 << 0)


/* room-related defines *************************************************/

/* The cardinal directions: used as index to Room.dir_option[] */
const int NORTH				= 0;
const int EAST				= 1;
const int SOUTH				= 2;
const int WEST				= 3;
const int UP				= 4;
const int DOWN				= 5;

/* Body Structures */
const int STRUCT_LIGHT		= 0;
const int STRUCT_MEDIUM		= 1;
const int STRUCT_HEAVY		= 2;

/* Room flags: used in Room.room_flags */

/* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved") */

const int ROOM_DARK			= 0;	// Dark
const int ROOM_DEATH		= 1;	// Death trap
const int ROOM_NOMOB		= 2;	// MOBs not allowed
const int ROOM_INDOORS		= 3;	// Indoors
const int ROOM_PEACEFUL		= 4;	// Violence not allowed
const int ROOM_SOUNDPROOF	= 5;	// Shouts, gossip blocked
const int ROOM_NOTRACK		= 6;	// Track won't go through
const int ROOM_NOMAGIC		= 7;	// Magic not allowed
const int ROOM_TUNNEL		= 8;	// room for only 1 pers
const int ROOM_PRIVATE		= 9;	// Can't teleport in
const int ROOM_GODROOM		= 10;	// LVL_GOD+ only allowed
const int ROOM_HOUSE		= 11;	// (R) Room is a house
const int ROOM_VAULT		= 12;	// (R) Room can save items over reboot / crash
const int ROOM_ATRIUM		= 13;	// (R) The door to a house
const int ROOM_OLC			= 14;	// (R) Modifyable/!compress
const int ROOM_BFS_MARK		= 15;	// (R) breath-first srch mrk
const int ROOM_NOPORT		= 16;	// (R) breath-first srch mrk
const int ROOM_POKER		= 17;	// Poker Table Setup
const int ROOM_NORENT		= 18;	// Can't save in this room
const int ROOM_NOSOURCE		= 19;	// Block the True Source
const int ROOM_NOHIDE		= 20;	//No one can hide in this room.
const int ROOM_LIT			= 21;	// The room is perma LIT (ROOM_DARK takes priority)
const int ROOM_MINING		= 22;	// The room is a mine
const int ROOM_FISHING		= 23;	// The room can be fished in
const int ROOM_SMITHING		= 24;	// The room can be smithed in
const int ROOM_TAILORING	= 25;	// The room can be used for tailoring

/* Exit info: used in Room.dir_option.exit_info				*/

#define EX_ISDOOR		(1 << 0)	/* Exit is a door				*/
#define EX_CLOSED		(1 << 1)	/* The door is closed			*/
#define EX_LOCKED		(1 << 2)	/* The door is locked			*/
#define EX_RAMMABLE		(1 << 3)	/* The door is rammable			*/
#define EX_DISABLED		(1 << 4)	/* The exit is disabled			*/
#define EX_TEMPORARY	(1 << 5)	/* The exit does not save		*/

/* char and mob-related defines *****************************************/
/* PC classes */

const int CLASS_UNDEFINED = 21;
const int CLASS_WARRIOR = 0;
const int CLASS_THIEF = 1;
const int CLASS_RANGER = 2;
const int CLASS_CHANNELER = 3;
const int CLASS_FADE = 4;
const int CLASS_DREADLORD = 5;
const int CLASS_BLADEMASTER = 6;
const int CLASS_GREYMAN = 7;
const int CLASS_DRAGHKAR = 8;
const int CLASS_DREADGUARD = 9;
const int CLASS_OGIER = 10;

const int NUM_CLASSES = 11; // This must be the number of classes!!



#define RACE_UNDEFINED 21
const int RACE_HUMAN = 0;
const int RACE_TROLLOC = 1;
const int RACE_SEANCHAN = 2;
const int RACE_AIEL = 3;
const int RACE_ANIMAL = 4;
const int RACE_OTHER = 5;

const int NUM_RACES = 6;


/* Arena teams */

const int ARENA_BLUE = 1;
const int ARENA_RED = 2;
const int ARENA_YELLOW = 3;
const int ARENA_GREEN = 4;


/* Sex */

const int SEX_UNDEFINED = 21;
const int SEX_NEUTRAL = 0;
const int SEX_MALE = 1;
const int SEX_FEMALE = 2;

/* Positions */

const int POS_DEAD = 0; // dead //
const int POS_MORTALLYW = 1; // mortally wounded //
const int POS_INCAP = 2; // incapacitated //
const int POS_STUNNED = 3; // stunned //
const int POS_SLEEPING = 4; // sleeping //
const int POS_RESTING = 5; // resting //
const int POS_SITTING = 6; // sitting //
const int POS_FIGHTING = 7; // fighting //
const int POS_STANDING = 8; // standing //
const int POS_FLYING = 9; // flying //

/* Used in Object::saveItems() */
enum ObjectSpecialType {
	SPECIAL_NONE=0,
	SPECIAL_PLAYER_SCALP,
	SPECIAL_SCALP,
	SPECIAL_CORPSE
};

/* Newbie Flags: Used by class Character.PlayerData.newb					*/

const int NEW_TELL = 0; // Message sent after first received tell
const int NEW_TIPS = 1; // Random tips generated every tic
const int NEW_FLEE = 2; // Tip sent about fleeing
const int NEW_CORPSE = 3; // Tip sent about corpse looting

/* Player flags: used by class Character.player.act				*/

const int PLR_DARKFRIEND = 0; // Player is a darkfriend
const int PLR_MURDERER = 1; // Player is a player-thief
const int PLR_FROZEN = 2; // Player is frozen
const int PLR_DONTSET = 3; // Don't EVER set ISNPC bit
const int PLR_WRITING = 4; // Player writing board/mail/olc
const int PLR_MAILING = 5; // Player is writing mail
const int PLR_CRASH = 6; // Player needs to be crash-saved
const int PLR_SITEOK = 7; // Player has been site-cleared
const int PLR_NOSHOUT = 8; // Player not allowed to shout/goss
const int PLR_NOTITLE = 9; // Player not allowed to set title
const int PLR_DELETED = 10; // Player deleted - space reusable
const int PLR_LOADROOM = 11; // Player uses nonstandard loadroom
const int PLR_NOWIZLIST = 12; // Player shouldn't be on wizlist
const int PLR_NODELETE = 13; // Player shouldn't be deleted
const int PLR_INVSTART = 14; // Player should enter game wizinvis
const int PLR_CRYO = 15; // Player is cryo-saved purge prog
const int PLR_LOGGER = 16; // Player is hooked up to a logger
const int PLR_NOTRACK = 17; // Player is temporary untrackable
const int PLR_ZONE_BAN = 19; // Player is banned from leaving a zone
const int PLR_NOIDLE = 20; // Player cannot idle
const int PLR_NOHOST = 21; // Player Requested blocked host
const int PLR_NO_WEAVE = 22; // Player Cannot gain weavepoints
const int PLR_NO_TRACE = 23; //Can't trace their IP
const int PLR_WEAVE_EDITOR = 24; //Can edit the weave editor in cedit
const int PLR_STAT_EDITOR = 25; //Can edit the stat editor
const int PLR_CHARGE_EDITOR = 26; //Can edit the weave editor in cedi
const int PLR_NOGLOBAL = 27; //Player is forbidden from using the global channel.
const int PLR_GLOBAL_SCRIPTS = 30; //Player can edit global JS editor

/* Mobile flags: used by class Character.player.act							*/

const int MOB_SPEC = 0; // Mob has a callable spec-proc
const int MOB_SENTINEL = 1; // Mob should not move
const int MOB_SCAVENGER = 2; // Mob picks up stuff on the ground
const int MOB_ISNPC = 3; // R Automatically set on all Mobs
const int MOB_AWARE = 4; // Mob can't be backstabbed
const int MOB_STAY_ZONE = 5; // Mob shouldn't wander out of zone
const int MOB_WIMPY = 6; // Mob flees if severely injured
const int MOB_MEMORY = 7; // remember attackers if attacked
const int MOB_HELPER = 8; // attack PCs fighting other NPCs
const int MOB_NOSLEEP = 9; // Mob can't be slept
const int MOB_NOBLIND = 10; // Mob can't be blinded
const int MOB_MOUNT = 11; // Mob is a mount
const int MOB_TRACK = 12; // Currently being tested: Tracking mobs
const int MOB_BASH = 13; // Mob can randomly bash while engaged
const int MOB_AWARD = 14; // Mob is a master of a clan
const int MOB_SHADOW_MOUNT = 15; // Mob is a horse for the Shadow
const int MOB_NOFIGHT = 16; // Mob does not fight back
const int MOB_INVIS = 17; // Mob can only be seen by immortals
const int MOB_GHOST = 18; // Mob is a ghost type of MOB
const int MOB_TRAINER = 19; // Mob is a skill trainer
const int MOB_INNKEEPER = 20; // Mob is an innkeeper
const int MOB_BANKER = 21; // Mob is a banker
const int MOB_DEALER = 22; // Mob is a poker dealer
const int MOB_SMOB = 23; //Mob is leader of a SMOB(for the website listing, mainly)
const int MOB_TRADE_TRAINER = 24; //Mob will teach trade skills(mining,fishing,smithing,etc...)
const int MOB_NOTRACK = 25; //Mob will not lay tracks.
const int MOB_NOGROUP = 26; //Mob cannot be grouped.
const int MOB_OGIER_MOUNT = 27; //Mob is an ogier horse
const int MOB_SMOB_FOLLOWER = 28; //Mob follows a SMOB leader or is otherwise related to a SMOB.


/* Preference flags: used by class Character.player_specials.pref					*/
const int PRF_BRIEF = 0; // Room descs won't normally be shown
const int PRF_COMPACT = 1; // No extra CRLF pair before prompts
const int PRF_SOURCE = 2; // Is in touch with the True Source
const int PRF_NOTELL = 3; // Can't receive tells
const int PRF_DISPHP = 4; // Display hit points in prompt
const int PRF_DISPMANA = 5; // Display mana points in prompt
const int PRF_DISPMOVE = 6; // Display move points in prompt
const int PRF_AUTOEXIT = 7; // Display exits in a room
const int PRF_NOHASSLE = 8; // Aggr mobs won't attack
const int PRF_QUEST = 9; // On quest (NOT USED)
const int PRF_SUMMONABLE = 10; // Can be summoned
const int PRF_NOREPEAT = 11; // No repetition of comm commands
const int PRF_HOLYLIGHT = 12; // Can see in dark
const int PRF_COLOR_1 = 13; // Color low bit
const int PRF_COLOR_2 = 14; // Color high bit
const int PRF_NOWIZ = 15; // Can't hear wizline
const int PRF_LOG1 = 16; // On-line System Log low bit
const int PRF_LOG2 = 17; // On-line System Log high bit
const int PRF_NOSPEAKS = 18;//This immortal will not see other immortals speaking to mortals, if set.
const int PRF_SPARETWO = 19; // Can't hear gossip channel (NOT USED)
const int PRF_SPARETHREE = 20; // Can't hear grats channel (NOT USED)
const int PRF_ROOMFLAGS = 21; // Can see room flags ROOM_x
const int PRF_KIT = 22; // Has he had a kit or not.
const int PRF_STATTED = 23; // Char has been statted
const int PRF_UNUSED1 = 24; // Currently unused (NOT USED)
const int PRF_INCOG = 25; // Char is incognito
const int PRF_TELL_MUTE = 26; // Char cannot send tells
const int PRF_SAY_MUTE = 27; // Char cannot talk
const int PRF_NONARR = 28; // Can't hear narrate channel
const int PRF_NOCHAT = 29; // Can't hear chat channel
const int PRF_NOYELL = 30; // Can't hear yell channel
const int PRF_NOSHOUT = 31; // Can't hear shout channel
const int PRF_BUILDWALK = 32; // Build new rooms when walking
const int PRF_SPAM = 33; // Player Spam on/off
const int PRF_ALERT = 34; // (NOT USED)
const int PRF_GCOMM_1 = 35; // Global Communication low bit
const int PRF_GCOMM_2 = 36; // Global Communication high bit
const int PRF_AUTOSCAN = 37; //Char has autoscan off ***RHOLLOR 05.15.09
const int PRF_NOGLOBAL = 38; //Player has turned off global channel.


/* Affect bits: used in class Character.player.affected_by		*/

/* WARNING: In the world files, NEVER set the bits marked "R" ("Reserved")	*/
const int AFF_DONOTUSE = 0; // NEVER USE!!!!
const int AFF_BLIND = 1; // (R) Char is blind
const int AFF_INVISIBLE = 2; // Char is invisible
const int AFF_WATERWALK = 3; // Char can walk on water
const int AFF_SANCTUARY = 4; // Char protected by sanct.
const int AFF_GROUP = 5; // (R) Char is grouped
const int AFF_POISON = 6; // (R) Char is poisoned
const int AFF_SLEEP = 7; // (R) Char magically asleep
const int AFF_INCOGNITO = 8; // Room for future expansion
const int AFF_SNEAK = 9; // Char can move quietly
const int AFF_HIDE = 10; // Char is hidden
const int AFF_NOTICE = 11; // Char has notice on
const int AFF_NOQUIT = 12; // Char cannot quit
const int AFF_PARANOIA = 13; // Char is paranoid
const int AFF_NIGHT_VISION = 14; // Char can see in the dark
const int AFF_HASTE = 15; // Char is affected by haste
const int AFF_SHIELD = 16; // Char is blocked from channeling
const int AFF_EFFUSION = 17; // Char leaves unnoticed
const int AFF_AGILITY = 18; // More Dodge Bonus
const int AFF_STRENGTH = 19; // Char has boosted strength
const int AFF_DISORIENT = 20; // Disorient effect
const int AFF_DIZZY = 21; // For Dizziness
const int AFF_WET = 22; // Char is wet, used amplifying weaves.
const int AFF_CONFUSED = 23; // Char is confused, used for amplifying weaves.
const int AFF_FORCED_PEACE = 24; // Char cannot use violent commands.
const int AFF_HIDDEN_INTENT = 25; // Removes *name* stars.
const int AFF_DISGUISE = 26; // Makes them look like a mob.
const int AFF_INVERTED_POWER = 27; // Makes their spells invisible.
const int AFF_ZING_SHIELD = 28; // Creates immunity to fire and zing weaves.
const int AFF_INSANE = 29;
const int AFF_SILENCE = 30;
const int AFF_CHILL = 31;
const int AFF_FEAR = 32;
const int AFF_PANIC = 33;
const int AFF_REFLECT = 34;
const int AFF_SENSE_SHADOW = 35;
const int AFF_SHIELD_BLOCK = 36; //Decreases the victim's parry bonus
const int AFF_SEVERE_WOUND = 37; //Decreases the victim's parry and dodge bonuses
const int AFF_HAMSTRING = 38; //Decreases the victim's dodge bonus
const int AFF_EAVESDROP = 39;
const int AFF_DEAF = 40;
const int AFF_WEAKEN = 41;
const int AFF_CRIPPLE = 42;
const int AFF_SLOW = 43;
const int AFF_DECAY = 44;
const int AFF_SLICE = 45;
const int AFF_BURN = 46;
const int AFF_DAZE = 47;
const int AFF_SHADOW_RAGE = 48;
const int AFF_REGENERATION = 49;
const int AFF_HEALING = 50;
const int AFF_DARK_PLAGUE = 51;

/* Modes of connectedness: used by class Descriptor.state					*/
const int CON_PLAYING = 0;			// Playing - Nominal state
const int CON_CLOSE = 1;			// Disconnecting
const int CON_GET_NAME = 2;			// By what name ..?
const int CON_NAME_CNFRM = 3;		// Did I get that right, x?
const int CON_PASSWORD = 4;			// Password:
const int CON_NEWPASSWD = 5;		// Give me a password for x
const int CON_CNFPASSWD = 6;		// Please retype password:
const int CON_QSEX = 7;				// Sex?
const int CON_QCLASS = 8;			// Class?
const int CON_RMOTD = 9;			// PRESS RETURN after MOTD
const int CON_MENU = 10;			// Your choice: (main menu)
const int CON_EXDESC = 11;			// Enter a new description:
const int CON_CHPWD_GETOLD = 12;	// Changing passwd: get old
const int CON_CHPWD_GETNEW = 13;	// Changing passwd: get new
const int CON_CHPWD_VRFY = 14;		// Verify new password
const int CON_DELCNF1 = 15;			// Delete confirmation 1
const int CON_DELCNF2 = 16;			// Delete confirmation 2
const int CON_DISCONNECT = 17;		// In-game disconnection
const int CON_NEWBMSG = 18;			//	Newbie quick-guide
const int CON_STAT_OPTION = 19;		// Select whether to choose stats or random stat.
const int CON_QRACE = 20;			// Race?
const int CON_HOSTNAME = 21;		// Retrieve Hostname
const int CON_STATEDIT = 22;		// Setup Stats
const int CON_AUCTION = 23;			// "OLC" mode - user is in auction interface
const int CON_OEDIT = 24;			//. OLC mode - object edit
const int CON_REDIT = 25;			//. OLC mode - room edit
const int CON_ZEDIT = 26;			//. OLC mode - zone info edit
const int CON_MEDIT = 27;			//. OLC mode - mobile edit
const int CON_SEDIT = 28;			//. OLC mode - shop edit
const int CON_AEDIT = 29;			//.	OLC mode - action editor
const int CON_KEDIT = 30;			//. OLC mode - kit editor
const int CON_CEDIT = 31;			// OLC mode - configuration editor
const int CON_CLEDIT = 32;			// OLC mode - Clan Edit
const int CON_WEDIT = 33;			// OLC mode - Warrant Edit
const int CON_SGEDIT = 34;			// OLC mode - SG edit
const int CON_JEDIT = 35;			// OLC mode - Javascript Edit
const int CON_GATEWAY = 36;
const int CON_EMAIL = 37;			// User is editing their email settings.
const int CON_REG_EMAIL = 38;		// User is entering their email address while registering.
const int CON_WEBSOCKET_LOGIN = 39;	// User is in the login form for the websocket client.


/* Character equipment positions: used as index for class Character.equipment[] */
/* NOTE: Don't confuse these constants with the ITEM_ bitvector
   which control the valid places you can wear a piece of equipment */
const int WEAR_LIGHT = 0;
const int WEAR_HOLD = 1;
const int WEAR_NECK_1 = 2;
const int WEAR_NECK_2 = 3;
const int WEAR_EAR_1 = 4;
const int WEAR_EAR_2 = 5;
const int WEAR_HEAD = 6;
const int WEAR_ABOUT = 7;
const int WEAR_BACK = 8;
const int WEAR_BODY = 9;
const int WEAR_SHOULDERS = 10;
const int WEAR_ARMS = 11;
const int WEAR_WAIST = 12;
const int WEAR_WRIST_R = 13;
const int WEAR_WRIST_L = 14;
const int WEAR_HANDS = 15;
const int WEAR_FINGER_R = 16;
const int WEAR_FINGER_L = 17;
const int WEAR_WIELD = 18;
const int WEAR_SHIELD = 19;
const int WEAR_LEGS = 20;
const int WEAR_FEET = 21;
const int NUM_WEARS = 22; // This must be the # of eq positions!!





/* object-related defines ********************************************/



/* Item types: used by Object.obj_flags.type_flag */
const int ITEM_LIGHT = 1; // Item is a light source
const int ITEM_MONEY = 2; // Item is a money
const int ITEM_WEAPON = 3; // Item is a weapon
const int ITEM_ARMOR = 4; // Item is armor
const int ITEM_POTION = 5; // Item is a potion
const int ITEM_OTHER = 6; // Misc object
const int ITEM_CONTAINER = 7; // Item is a container
const int ITEM_NOTE = 8; // Item is note
const int ITEM_DRINKCON = 9; // Item is a drink container
const int ITEM_KEY = 10; // Item is a key
const int ITEM_FOOD = 11; // Item is food
const int ITEM_PEN = 12; // Item is a pen
const int ITEM_BOAT = 13; // Item is a boat
const int ITEM_FOUNTAIN = 14; // Item is a fountain
const int ITEM_ANGREAL = 15; // Item is an angreal
const int ITEM_CHAIR = 16; // Item is a chair
const int ITEM_SPECIAL = 17; //Item has special abilities attached - typically for imms.
const int ITEM_LAMPPOST = 18; // Item is a Lamp Post (Lights a room from ground)
const int ITEM_SKIN = 19; // Item can be skinned from a corpse

/* Take/Wear flags: used by Object.obj_flags.wear_flags		*/
const int ITEM_WEAR_TAKE		= (1<<0);  // Item can be taken
const int ITEM_WEAR_FINGER		= (1<<1);  // Can be worn on finger
const int ITEM_WEAR_NECK		= (1<<2);  // Can be worn around neck
const int ITEM_WEAR_BODY		= (1<<3);  // Can be worn on body
const int ITEM_WEAR_HEAD		= (1<<4);  // Can be worn on head
const int ITEM_WEAR_LEGS		= (1<<5);  // Can be worn on legs
const int ITEM_WEAR_FEET		= (1<<6);  // Can be worn on feet
const int ITEM_WEAR_HANDS		= (1<<7);  // Can be worn on hands
const int ITEM_WEAR_ARMS		= (1<<8);  // Can be worn on arms
const int ITEM_WEAR_SHIELD		= (1<<9);  // Can be used as a shield
const int ITEM_WEAR_ABOUT		= (1<<10); // Can be worn about body
const int ITEM_WEAR_WAIST		= (1<<11); // Can be worn around waist
const int ITEM_WEAR_WRIST		= (1<<12); // Can be worn on wrist
const int ITEM_WEAR_WIELD		= (1<<13); // Can be wielded
const int ITEM_WEAR_HOLD		= (1<<14); // Can be held
const int ITEM_WEAR_BACK		= (1<<15); // Can be worn on back
const int ITEM_WEAR_SHOULDERS	= (1<<16); // Can be worn on shoulders
const int ITEM_WEAR_EAR			= (1<<17); // Can be worn on ears


/* Extra object flags: used by Object.obj_flags.extra_flags					*/
const int ITEM_NORENT = 0; // Item cannot be rented
const int ITEM_INVISIBLE = 1; // Item is invisible
const int ITEM_NODROP = 2; // Item is cursed: can't drop
const int ITEM_WARD = 3; // Item is weapon warded
const int ITEM_NOSELL = 4; // Shopkeepers won't touch it
const int ITEM_UNUSED1 = 5; // This is unused as of 2014-12-20. This was formerly to be ITEM_CHAIN. See https://app.asana.com/0/14751274030189/14855742503356
const int ITEM_POISON = 6; // Item randomly poisons
const int ITEM_CHEST = 7; // Item randomly poisons
const int ITEM_TWO_HANDED = 8; // Item can be worn with two hands
const int ITEM_DAGGER_SHEATH = 9; // Item is a sheath for daggers
const int ITEM_SWORD_SHEATH = 10; // Item is a sheath for swords
const int ITEM_TEMP = 11; // Item is temporary and created by a weave
const int ITEM_NO_SHOW = 12; //Item does not show up in the website listing.
const int ITEM_CLAN_EQUIPMENT = 13; //Item is a piece of clan equipment.
const int ITEM_SPEAR_SHEATH = 14; //Item can sheath spears
const int ITEM_RARE = 15; //Item is considered "rare"
const int ITEM_TRADES = 16; //Item is part of the trade system.


/* Modifier constants used with obj affects ('A' fields)			*/
const int APPLY_NONE = 0; // No effect
const int APPLY_STR = 1; // Apply to strength
const int APPLY_DEX = 2; // Apply to dexterity
const int APPLY_INT = 3; // Apply to constitution
const int APPLY_WIS = 4; // Apply to wisdom
const int APPLY_CON = 5; // Apply to constitution
const int APPLY_CHA = 6; // Apply to charisma
const int APPLY_CLASS = 7; // Reserved
const int APPLY_LEVEL = 8; // Reserved
const int APPLY_AGE = 9; // Apply to age
const int APPLY_CHAR_WEIGHT = 10; // Apply to weight
const int APPLY_CHAR_HEIGHT = 11; // Apply to height
const int APPLY_MANA = 12; // Apply to max mana
const int APPLY_HIT = 13; // Apply to max hit points
const int APPLY_MOVE = 14; // Apply to max move points
const int APPLY_GOLD = 15; // Reserved
const int APPLY_EXP = 16; // Reserved
const int APPLY_DB = 17; // Apply to Armor Class
const int APPLY_HITROLL = 18; // Apply to hitroll
const int APPLY_DAMROLL = 19; // Apply to damage roll
const int APPLY_RACE = 20; // Apply to race

/* Container flags - value[1] */
#define CONT_CLOSEABLE      (1 << 0)	/* Container can be closed	*/
#define CONT_CLOSED         (1 << 1)	/* Container is closed		*/
#define CONT_LOCKED         (1 << 2)	/* Container is locked		*/

/* Some different kind of liquids for use in values of drink containers */
const int LIQ_WATER = 0;
const int LIQ_BEER = 1;
const int LIQ_WINE = 2;
const int LIQ_ALE = 3;
const int LIQ_DARKALE = 4;
const int LIQ_WHISKY = 5;
const int LIQ_LEMONADE = 6;
const int LIQ_FIREBRT = 7;
const int LIQ_LOCALSPC = 8;
const int LIQ_SLIME = 9;
const int LIQ_MILK = 10;
const int LIQ_TEA = 11;
const int LIQ_COFFE = 12;
const int LIQ_BLOOD = 13;
const int LIQ_SALTWATER = 14;
const int LIQ_CLEARWATER = 15;
const int LIQ_OIL = 16;

/* Object decay types */
const int OBJECT_DECAY_GENERAL = 1;
const int OBJECT_DECAY_CLOTH = 2;
const int OBJECT_DECAY_WOOD = 3;
const int OBJECT_DECAY_METAL = 4;
const int OBJECT_DECAY_PLANT = 5;
const int OBJECT_DECAY_FOOD = 6;

/* Object decay timer frequencies */
const int OBJECT_DECAY_MINUTES = 1;
const int OBJECT_DECAY_HOURS = 2;
const int OBJECT_DECAY_DAYS = 3;
const int OBJECT_DECAY_WEEKS = 4;
const int OBJECT_DECAY_MONTHS = 5;
const int OBJECT_DECAY_YEARS = 6;


/****** Weapon Types *******/



const int WEAPON_LONG_BLADE = 0;
const int WEAPON_SHORT_BLADE = 1;
const int WEAPON_CLUB = 2;
const int WEAPON_STAFF = 3;
const int WEAPON_SPEAR = 4;
const int WEAPON_AXE = 5;
const int WEAPON_CHAIN = 6;
const int WEAPON_BOW = 7;
const int WEAPON_LANCE = 8;

const int MAX_WEAPON_TYPE = 9;
/***************************/


/* other miscellaneous defines *******************************************/

const int RF_ARRAY_MAX = 4;
const int PM_ARRAY_MAX = 4;
const int PR_ARRAY_MAX = 5;
const int AF_ARRAY_MAX = 4;
const int TW_ARRAY_MAX = 4;
const int EF_ARRAY_MAX = 4;
const int NB_ARRAY_MAX = 4;

/* Player conditions */
const int DRUNK = 0;
const int FULL = 1;
const int THIRST = 2;

/* Rent codes */
const int RENT_UNDEF = 0;
const int RENT_CRASH = 1;
const int RENT_RENTED = 2;
const int RENT_CRYO = 3;
const int RENT_FORCED = 4;
const int RENT_TIMEDOUT = 5;

/* Defense Values			*/

/* Offensive Bonuses		*/
const int OB_MAX_BASE = 30;
const int OB_RIDING_BONUS = 7;
const int OB_MASTER = 4;
const int OB_MILD = 5;
const int OB_BRAVE = 10;
const int OB_BERSERK = 15;
const int OB_STANCE_OFFENSIVE = 6;
const int OB_STANCE_NORMAL = 3;

/* Dodging Bonuses			*/
const int DB_MAX_BASE = 30;
const int DB_RIDING_MALUS = 5;
const int DB_BRAVE_MALUS = 2;
const int DB_BERSERK_MALUS = 8;
const float DB_SITTING_MALUS = .15f;

/* Parrying Bonuses			*/
const int PB_MASTER = 5;
const int PB_WIMPY = 15;
const int PB_MILD = 7;
const int PB_BERSERK = -7;
const int PB_STANCE_DEFENSIVE = 6;
const int PB_STANCE_NORMAL = 3;
const float PB_SITTING_MALUS = 0.60f;


/* other constants **********************************************/

typedef signed char	sbyte;
typedef unsigned char	ubyte;
typedef signed short int	sh_int;
typedef unsigned short int	ush_int;
typedef signed long	sh_long;
typedef unsigned long	ush_long;

#ifndef CIRCLE_WINDOWS
typedef char	byte;
#endif



typedef int	room_vnum;	/* A room's vnum type */
typedef int	obj_vnum;	/* An object's vnum type */
typedef int	mob_vnum;	/* A mob's vnum type */

typedef int	room_rnum;	/* A room's real (internal) number type */
typedef int	obj_rnum;	/* An object's real (internal) num type */
typedef int	mob_rnum;	/* A mobile's real (internal) num type */

/*

 * **DO**NOT** blindly change the number of levels in your MUD merely by
 * changing these numbers and without changing the rest of the code to match.
 * Other changes throughout the code are required.  See coding.doc for
 * details.
 *
 * LVL_IMPL should always be the HIGHEST possible immortal level, and
 * LVL_IMMORT should always be the LOWEST immortal level.  The number of
 * mortal levels will always be LVL_IMMORT - 1.
 */

const byte LVL_IMPL = 105;
const byte LVL_GRGOD = 104;
const byte LVL_GOD = 103;
const byte LVL_APPR = 102;
const byte LVL_BLDER = 101;
const byte LVL_IMMORT = 100;

/* Level of the 'freeze' command */
const byte LVL_FREEZE = LVL_GRGOD;

/* Different moods				*/
const int MOOD_WIMPY = 0;
const int MOOD_MILD = 1;
const int MOOD_BRAVE = 2;
const int MOOD_BERSERK = 3;

/* Different stances			*/
const int STANCE_DEFENSIVE	= 0;
const int STANCE_NORMAL		= 1;
const int STANCE_OFFENSIVE	= 2;

const int NUM_OF_DIRS = 6; // number of directions in a room (nsewud)

const int OPT_USEC	=	1000000 / 7;	/* 7 passes per second */
const int PASSES_PER_SEC = 1000000 / OPT_USEC;
#define RL_SEC			* PASSES_PER_SEC

#define PULSE_ZONE      (10 RL_SEC)
#define PULSE_MOBILE    (7 RL_SEC)
#define PULSE_VIOLENCE  (4 RL_SEC)

/* Variables for the output buffering system */
#define MAX_SOCK_BUF            (1024 * 1024) /* Size of kernel's sock buf   */
const int MAX_PROMPT_LENGTH = 150; // Max length of prompt
const int GARBAGE_SPACE = 32; // Space for **OVERFLOW** etc
const int SMALL_BUFSIZE = 4096; // Static output buffer size
/* Max amount of output that can be buffered */
#define LARGE_BUFSIZE	   (MAX_SOCK_BUF - GARBAGE_SPACE - MAX_PROMPT_LENGTH)


/*
 * --- WARNING ---
 * If you are using a BSD-derived UNIX with MD5 passwords, you _must_
 * make MAX_PWD_LENGTH larger.  A length of 20 should be good. If
 * you leave it at the default value of 10, then any character with
 * a name longer than about 5 characters will be able to log in with
 * _any_ password.  This has not (yet) been changed to ensure pfile
 * compatibility for those unaffected.
 */

const size_t MAX_STRING_LENGTH = 15360;
const size_t MAX_INPUT_LENGTH = 512; // Max length per *line* of input
const size_t MAX_MESSAGE_LENGTH = 2048;
const size_t MAX_MESSAGES = 60;
const size_t MAX_NAME_LENGTH = 16;
const size_t MIN_NAME_LENGTH = 2;
const size_t MAX_PWD_LENGTH = 20;
const size_t MIN_PWD_LENGTH = 3;
const size_t MAX_TITLE_LENGTH = 80;
const size_t HOST_LENGTH = 30;
const size_t EXDSCR_LENGTH = 325;
const int    MAX_TONGUE = 3;
const int    MAX_SKILLS = 200;
const int    MAX_AFFECT = 64;
const int    MAX_OBJ_AFFECT = 6;

class JSTrigger;
class JSInstance;

/*** Added by Galnor on 11/06/2009 - base class for Object/Character/Room ***/
class JSBindable
{
public:
	std::shared_ptr<std::vector<JSTrigger*> > js_scripts;
	std::shared_ptr<JSInstance> delayed_script;
	virtual class Room *getRoom() = 0;
	virtual bool IsPurged() = 0;
	void attachJS( const int vnum );
	void detachJS( const int vnum, const int nr );
	int countJS( const int vnum );
};

class EntityType;

class Entity
{
public:
	virtual const char *getDisplayName() = 0;
	virtual std::string getDisplayableId() = 0;
	virtual Room *getRoom() = 0;
	virtual class EntityType *getEntityType() = 0;
};

/**********************************************************************

* Structures                                                          *

**********************************************************************/

class Clock
{
	private:
		unsigned long long start;
		unsigned long long clocks;
		unsigned long long tics;
		unsigned long long lastClocks;
		bool on;
	public:
		Clock()
		{
			reset( false );
		}
		void reset( bool onstart )
		{
			start = Clock::getTick();
			on = onstart;
			clocks = 0;
			tics = 0;
			lastClocks = 0;
		}
		unsigned long long getClocks() const
		{
			return clocks;
		}
		float getSeconds() const
		{
            return (float) ( (float)this->getClocks() / (float)1000 );
		}
		unsigned long long getTics() const
		{
			return tics;
		}

		unsigned long long getLastClocks() const
		{
			return lastClocks;
		}

		void print() const
		{
			float t = getTics();
			float c = getClocks();
			float cpt = (t > 0 ? (c/t) : (0.000f));
			std::cout << getClocks() << " clocks. " << cpt << " clocks per tic." << std::endl;
		}
		void tic()
		{
			++tics;
		}
		void turnOn()
		{
			if ( !on )
			{
				start = Clock::getTick();
				on = true;
			}
		}
		void turnOff()
		{
			if ( on )
			{
				auto duration = ( Clock::getTick() - start );
				clocks += duration;
				lastClocks = duration;
				on = false;
				start = 0;
			}
		}
		static unsigned long long getTick()
		{
#ifdef WIN32
			return (unsigned long long)GetTickCount();
#else
			timeval tt;
			gettimeofday(&tt, (struct timezone*)0);
			return (tt.tv_sec*1000) + (tt.tv_usec/1000);
#endif
		}
};

class Time
{
	private:
		time_t t;
	public:
		static time_t		DateToTime(const std::string &date);
		static tm			TimeToDate(time_t t);
		static std::string	FormatDate(const std::string &Format, const tm *t );
		static std::string	FormatDate(const std::string &Format, const time_t t);
		static int			DaysPerMonth( const int month, const int year );
		static int          DaysPerYear( const int year );
		static tm			PriorDay( tm Today );
		static tm           NextDay(  tm Today );
		static tm			FloorDay( tm Today );
		static tm			Yesterday();
		static tm			Tomorrow();
		static bool			SameDay( const tm t1, const tm t2 );
		static bool			SameDay( const time_t t1, const time_t t2 );
		static bool			IsValidTime( tm *t );
		Time();
		Time( time_t tt );

		time_t Seconds();
		time_t Minutes();
		time_t Hours();
		time_t Days();
		time_t dSeconds();
		time_t dMinutes();
		time_t dHours();
		time_t dDays();
		time_t dtSeconds();
		time_t dtMinutes();
		time_t dtHours();
		time_t dtDays();
		void SetTime( time_t newTime );
};

/* Extra description: used in objects, mobiles, and rooms */

struct ExtraDescription
{
	ExtraDescription()
	{
		keyword = 0;
		description = 0;
		next = 0;
	}
	~ExtraDescription()
	{
		if( keyword != NULL )
			delete[] keyword;
		if( description != NULL )
			delete[] description;
	}
	static ExtraDescription *Parse( const std::string &eDescStr );
	static std::string Serialize(ExtraDescription *eDescList);
	char	*keyword;                 /* Keyword in look/examine          */
	char	*description;             /* What to see                      */
	struct ExtraDescription *next; /* Next in list                     */

};

/* object-related structures ******************************************/

/* object flags; used in Object										*/

struct obj_flag_data
{
	obj_flag_data()
	{
		memset( &value, 0, sizeof( value ) );
		type_flag = 0;
		wear_flags = 0;
		extra_flags = 0;
		weight = 0;
		cost = 0;
		cost_per_day = 0;
		timer = 0;
		memset( &bitvector, 0, sizeof( bitvector) );
		offensive = 0;
		parry = 0;
		dodge = 0;
		absorb = 0;
		clan = 0;
		clan_weight = 0;
		clan_offensive = 0;
		clan_parry = 0;
		clan_dodge = 0;
		clan_absorb = 0;
		clan_moves = 0;
		clan_hit = 0;
		clan_dmg1 = 0;
		clan_dmg2 = 0;
		mModifiers=0;
	}
	sh_int	value[ 11 ];							/* Values of the item (see list)					*/
	byte type_flag;									/* Type of item										*/
	int	wear_flags;									/* Where you can wear it							*/
	int	extra_flags;				/* If it hums, glows, etc.							*/
	float weight;									/* Weight, what else								*/
	int	cost;										/* Value when sold (gp.)						    */
	int	cost_per_day;								/* Cost to keep pr. real day							    */
	long bitvector[ AF_ARRAY_MAX ];					/* To set chars bits					*/
	sh_int timer;									/* Timer for object										*/
	sh_int offensive;								/* OB addition										*/
	sh_int parry;									/* PB addition										*/
	sh_int dodge;									/* DB addition										*/
	sh_int absorb;									/* ABS addition										*/
	sh_int clan;									/* What clan an object belongs to					*/
	float clan_weight;								/* Weight change for clan object					*/
	sh_int clan_offensive;							/* OB change for clan object						*/
	sh_int clan_parry;								/* PB change for clan object						*/
	sh_int clan_dodge;								/* DB change for clan object						*/
	sh_int clan_absorb;								/* ABS change for clan object						*/
	sh_int clan_moves;								/* Moves bonus for clan object						*/
	sh_int clan_hit;								/* HP bonus for clan object							*/
	sh_int clan_dmg1;								/* Damage bonus to low range for clan object		*/
	sh_int clan_dmg2;								/* Damage bonus to high range for clan object		*/

	std::map< byte,long double > *mModifiers;		/* Object modifiers (eModType => modifier value)	*/
};

struct FoodUnit
{
	FoodUnit()
	{
		vnum = 0;
		quantity = 0;
	}
	FoodUnit(FoodUnit *source)
	{
		vnum = source->vnum;
		quantity = source->quantity;
	}
	int vnum, quantity;
};

struct SkinningUnit
{
	SkinningUnit()
	{
		vnum = 0;
		required = 0;
		skinned = false;
	}
	SkinningUnit(SkinningUnit *source)
	{
		vnum = source->vnum;
		required = source->required;
		skinned = source->skinned;
	}
	int vnum, required;
	bool skinned;
};

class ScalpData
{
	public:
		ScalpData()
		{
			race = 0;
			level = 0;
			scalped = false;
			is_scalp = false;
			player_scalp = false;
			Food = 0;
			Skin = 0;
			blood = 0;
			memset( &warrants, 0, sizeof( warrants ) );
		}
		std::string name;
		sh_int race;
		sh_int level;
		sh_int blood;
		bool scalped;
		int warrants[ 4 ];
		bool is_scalp;
		bool player_scalp;
		struct FoodUnit	*Food; /* Food unit for butchering			*/
		struct SkinningUnit *Skin; /* Skin unit for skinning */
};



struct obj_affected_type
{
	obj_affected_type()
	{
		location = 0;
		modifier = 0;
	}
	byte location;      /* Which ability to change (APPLY_XXX) */
	sbyte modifier;     /* How much it changes by              */
};

/* ================== Memory Structure for Objects ================== */

class Object : public JSBindable, public Entity
//11/06/2009 - implement JSBindable interface
{
	public:
		static boost::uuids::random_generator uuidGenerator;
		static int nr_alloc;
		static int nr_dealloc;
		obj_vnum	item_number;							/* Where in data-base				*/
		class Room	*in_room;								/* In what room -1 when conta/carr	*/

		struct	obj_flag_data obj_flags;					/* Object information				*/
		struct	obj_affected_type affected[ MAX_OBJ_AFFECT ];/* effects					*/

		char	*name;										/* Title of object :get etc.        */
		char	*description;								/* When in room                     */
		char    *retool_name;								/* Title of object when retooled    */
		char    *retool_desc;								/* When in room, retooled           */

		char	*short_description;							/* when worn/carry/in cont.         */
		char    *retool_sdesc;                              /* retooled short description       */
		char	*action_description;						/* What to write when used          */
		struct ExtraDescription	*ex_description;			/* extra descriptions				*/
		struct ExtraDescription *retool_ex_desc;			/* extra descriptions when retooled */
		class Character	*carried_by;						/* Carried by :NULL in room/conta   */
		class Character	*worn_by;							/* Worn by?							*/
		class Character *SatOnBy;							/* Who's sitting on this?			*/
		sh_int worn_on;										/* Worn where?						*/

		class Object	*in_obj;							/* In what object NULL when none	*/
		class Object	*contains;							/* Contains objects					*/

		boost::uuids::uuid objID;							/* Object's UID						*/
		bool proto;											/* Object is a prototype			*/
		DateTime createdDatetime;							/* When the item was created		*/

		class Object	*next_content;						/* For 'contains' lists				*/
		class Object	*next;								/* For the object list				*/
		class ScalpData	*scalp;
		std::string creator;								/* String containing object creator */

		bool hidden;
		sh_int Max;

		byte decayType;										/* The type of material this object is made from */
		byte decayTimerType;								/* Type of decay timer (ie minutes, hours, etc) */
		sh_int decayTimer;									/* Timer for decay scripts */
		
		bool purged;										/* Is this Object queued for purging? */
		bool deleted;
		bool needs_save;
		bool IsRetooled() {
			return (retool_name || retool_desc || retool_sdesc || retool_ex_desc);
		}

		/********** Functions ************/
		Object();
		~Object();

		bool IsProto();

		void FreeLiveObject();
		void Extract( bool lowerItemCount = false );

		bool IsPurged() { return purged; }

		void UnsetPrototypeSpecifics();
		Character *FindHolder();

		const char *getName();
		const char *getDisplayName();
		char *GetDesc();
		char *GetSDesc();
		struct ExtraDescription *GetExDesc();

		unsigned long int Money;

		float Weight();
		std::string getDisplayableId();
		Room *getRoom();
		class EntityType *getEntityType();

		float BashRating();
		void MoveToRoom( Room *room, bool vaultSave=true );
		void MoveToRoom( int rnum, bool vaultSave=true );
		void RemoveFromAll();
		void RemoveFromRoom(bool vaultSave=true);

		void ProtoBoot( class sql::Row &MyRow, const int rnum, const std::list<int> &jsAttachmentList );
		void protoSave();
		void ProtoDelete();

		std::list<Object*> loadItemList( bool recursive );
		static Object *loadSingleItem(const boost::uuids::uuid &objID, bool recursive);
		static std::list< std::pair<Object*,int> > loadItemPairs( bool recursive, const char holderType, const std::string &holderID );
		static std::list< Object* > loadItemList( bool recursive, const char holderType, const std::string &holderID );
		void loadItems();
		void itemSave();
		void saveItems(char holderType, const std::string &holderID);
		void saveItems( bool self, char holderType, const std::string &holderID, char topLevelHolderType, const std::string &topLevelHolderID, sql::BatchInsertStatement &tempObjectsBatchInsertStatement, sql::BatchInsertStatement &tempObjectRetoolsBatchInsertStatement, sql::BatchInsertStatement &tempObjectSpecialsBatchInsertStatement, bool contents );

		static void saveItemToTopLevelHolder(const char holderType, const std::string &holderId, Object *obj);
		static void saveTopLevelHolderItems(const char holderType, const std::string &holderId, const std::list<Object *> &contents);
		static void saveHolderItems(const char holderType, const std::string &holderId, const char topLevelHolderType, const std::string &topLevelHolderId, const std::list<Object *> &contents, bool deleteHolderContents=false);
		static void addFieldsToBatchInsertStatement(sql::BatchInsertStatement &objectBatchInsertStatement, sql::BatchInsertStatement &objectRetoolBatchInsertStatement, sql::BatchInsertStatement &objectSpecialBatchInsertStatement);

		static Object *bootLiveObject( const sql::Row &MyRow, bool recursive = false );
		static void saveMultipleHolderItems(const std::map<std::pair<char, std::string>, std::list<Object *>> &holderTypeAndIdToContentsMap, bool deleteHolderContents);
		void addToBatchInsertStatement(sql::BatchInsertStatement &batchInsertStatement, bool recursive);

		float AverageAbsorb( int i, Character *ch );

		//Container Related Functions
		bool IsPickProof();
		bool IsClosed();
		bool IsOpen();
		bool CanOpen();
		bool IsContainer();
		bool IsLocked();
		bool CanLock();
		bool CanPick( Character *ch );
		bool IsRemortGear();
		bool IsValidChest();
		int PickReq();
		int KeyNum();
		int getVnum();
		byte getType() const { return obj_flags.type_flag; };
		void setType(const byte type) { obj_flags.type_flag = type;  }

		enum eObjectModifier {
			OBJECT_MOD_DODGE=0,
			OBJECT_MOD_ABSORB,
			OBJECT_MOD_PARRY,
			OBJECT_MOD_OFFENSE,
			OBJECT_MOD_WEIGHT,
			OBJECT_MOD_VAL1,
			OBJECT_MOD_VAL2,
			OBJECT_MOD_VAL3,
			OBJECT_MOD_VAL4,
			OBJECT_MOD_VAL5,
			OBJECT_MOD_VAL6,
			OBJECT_MOD_VAL7,
			OBJECT_MOD_VAL8,
			OBJECT_MOD_VAL9,
			OBJECT_MOD_VAL10,
			OBJECT_MOD_VAL11,
			OBJECT_MOD_VAL12
		};

		int GetTotalDodge();
		int GetTotalAbsorb();
		int GetTotalParry();
		int GetTotalOffense();
		int GetTotalVal0();
		int GetTotalVal1();
		int GetTotalVal2();
		int GetTotalVal3();
		int GetTotalVal4();
		int GetTotalVal5();
		int GetTotalVal6();
		int GetTotalVal7();
		int GetTotalVal8();
		int GetTotalVal9();
		int GetTotalVal10();
		int GetTotalVal11();
		float GetTotalWeight();

		void SetModifier( eObjectModifier modType, const long double ldNewValue );
		int GetModifierInt( eObjectModifier modType );
		float GetModifierFloat( eObjectModifier modType );
};

/* ====================================================================== */

struct RoomNotFoundException
{
	std::string message;
	RoomNotFoundException(){}
	RoomNotFoundException( std::string Msg ) {message = Msg;}
	void report() { std::cout << "SYSERR<RoomNotFoundException>: " << message.c_str() << std::endl; }
};

/* char-related structures ************************************************/


/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct GameTime
{
	GameTime()
	{
		hours = month = day = SecondsPerTic = year = 0;
	}
	int hours, day, month, SecondsPerTic;
	sh_int year;
};

/* These data contain information about a players time data */
struct TimeData
{
	TimeData()
	{
		birth = DateTime(0);
		logon = DateTime(0);
		logout = DateTime(0);
		played = 0;
	}
	DateTime birth;   /* This represents the characters age                */
	DateTime logon;   /* Time of the last logon (used to calculate played) */
	DateTime logout;	/* Time of the last logout */
	int	played;     /* This is the total accumulated time played in secs */
};

/* general player-related info, usually PC's and NPC's */
struct CharPlayerData
{
	CharPlayerData()
	{
		short_descr = 0;
		long_descr = 0;
		description = 0;
		ArriveMessage = 0;
		LeaveMessage = 0;
		sex = 0;
		chclass = 0;
		race = 0;
		level = 0;
		weight = 0;
		height = 0;
		fighting = 0;
		mount = 0;
		ridden_by = 0;
		target = 0;
		target2 = 0;
		marked = 0;
		hunting = 0;
		otarget = 0;
		sitting_on = NULL;
		idnum = 0;
		position = POS_STANDING;

		direction = 0;
		flee_lag = 0;
		flee_go = false;
		carry_weight = 0;
		carry_items = 0;
		timer = 0;
		memset( &act, 0, sizeof( act ) );
		memset( &affected_by, 0, sizeof( affected_by ) );
		memset( &pref, 0, sizeof( pref ) );
	}
	void operator =(CharPlayerData &source)
	{
		this->CopyFrom((&source), true);
	}
	void CopyFrom( CharPlayerData *Source, bool full_copy = false )
	{
		passwd = Source->passwd;
		name = Source->name;
		short_descr = (full_copy ? str_dup(Source->short_descr) : Source->short_descr);
		long_descr = (full_copy ? str_dup(Source->long_descr) : Source->long_descr);
		description = (full_copy ? str_dup(Source->description) : Source->description);
		ArriveMessage = (full_copy ? str_dup(Source->ArriveMessage) : Source->ArriveMessage);
		LeaveMessage = (full_copy ? str_dup(Source->LeaveMessage) : Source->LeaveMessage);
		title = Source->title;
		sex = Source->sex;
		chclass = Source->chclass;
		race = Source->race;
		level = Source->level;
		position = Source->position;
		direction = Source->direction;
		carry_items = Source->carry_items;
		memcpy(&this->time, &Source->time, sizeof(this->time));
		idnum = Source->idnum;
		memcpy(&act, &Source->act, sizeof(this->act));
		memcpy(&pref, &Source->pref, sizeof(this->pref));
		memcpy(&affected_by, &Source->affected_by, sizeof(this->affected_by));
		this->weight = Source->weight;
		this->height = Source->height;

		fighting = 0;
		mount = 0;
		ridden_by = 0;
		target = 0;
		target2 = 0;
		marked = 0;
		hunting = 0;

		otarget = 0;

		carry_weight = Source->carry_weight;
		timer = Source->timer;
		flee_lag = Source->flee_lag;
		flee_go = Source->flee_go;
	}
	std::string passwd;					/*	character's password				*/
	std::string	name;					/*	PC / NPC s name (kill ...  )			*/
	char	*short_descr;				/*	for NPC 'actions'						*/
	char	*long_descr;				/*	for 'look'								*/
	char	*description;				/*	Extra descriptions						*/
	char	*ArriveMessage;				/*	Message for mobs arriving				*/
	char	*LeaveMessage;				/*	Message for mobs leaving				*/
	std::string title;						/*	PC / NPC's title						*/
	byte sex;							/*	PC / NPC's sex							*/
	byte chclass;						/*	PC / NPC's class						*/
	byte race;							/*	PC / NPC's race							*/
	byte level;							/*	PC / NPC's level						*/
	byte position;						/* Standing, fighting, sleeping, etc.		*/
	byte direction;						/* For flee direction						*/
	byte carry_items;					/* Number of items carried					*/
	struct TimeData time;				/*	PC's AGE in days						*/
	int idnum;							/* player's idnum; -1 for mobiles			*/
	int act;							/* Act flag for NPC's; player flag for PC's */
	int	 pref[ PR_ARRAY_MAX ];	/* preference flags for PC's.		*/
	int	affected_by[AF_ARRAY_MAX];		/* Bitvector for spells/skills affected by	*/
	sh_int weight;							/*	PC / NPC's weight						*/
	sh_int height;							/*	PC / NPC's height						*/

	Character *fighting;	/* Opponent					*/
	Character *mount;		/* Who is being ridden		*/
	Character *ridden_by;	/* Being ridden by 			*/
	Character *target;		/* Timer target				*/
	Character *target2;		/* Secondary timer target	*/
	Character *marked;		/* Greyman target			*/
	Character *hunting;		/* Hunting target			*/

	Object *otarget;		/* Timer target(objects)	*/
	Object *sitting_on;		/* Object that this character is sitting on */

	float carry_weight;		/* Carried weight							*/
	sh_int timer;			/* Timer for update							*/
	sh_int flee_lag;		/* Ammount of flee lag stacked up			*/
	bool flee_go;			/* Ready to flee?							*/

};

/* Char's abilities. */
struct CharAbilityData
{
	CharAbilityData()
    {
		str = 0;
		wis = 0;
		intel = 0;
		con = 0;
		dex = 0;
	}
	void operator =(CharAbilityData &source)
	{
		str = source.str;
		intel = source.intel;
		wis = source.wis;
		dex = source.dex;
		con = source.con;
	}
	byte str;
    byte intel;
    byte wis;
    byte dex;
    byte con;
};


struct CharPointData
{
	CharPointData()
	{
		mana = 0;
		max_mana = 0;
		hit = 0;
		max_hit = 0;
		move = 0;
		max_move = 0;
		offensive = 0;
		parry = 0;
		dodge = 0;
		absorb = 0;
		weave = 0;
		invis = 0;
		taint = 0;
		temp_taint = 0;
		death_wait = 0;
		warning = 0;
		shadow_points = 0;
		max_shadow_points = 0;
		master_weapon = 0;
		is_bashed = false;
		strain = 0;
		last_logon = 0;
		last_logout = 0;
		gold = 0;
		bank_gold = 0;
		exp = 0;
		damroll = 0;
		memset( &warrants, 0, sizeof( warrants ) );
	}
	void operator =(CharPointData &source);

    int master_weapon;
    bool is_bashed;
    byte death_wait;
    byte warning;
    byte invis;
    std::vector<int> HitRolls;
    std::vector<int> ManaRolls;

    std::string poofin;     /* Immortal poofin                          */
    std::string poofout;        /* Immortal poofout                         */
    std::string slew;       /* Used in legend list. Last kill           */
    std::string whois_extra;    /* Imm used extra section for whois         */

	sh_int offensive;    /*   Offencive Bonus                         */
	sh_int parry;        /*   Parry Bonus                             */
	sh_int dodge;        /*   Dodge Bonus                             */
	sh_int absorb;       /*   Absorb Bonus                            */
	sh_int weave;        /*   Weave Points                            */

	time_t last_logon;
	time_t last_logout;
	int warrants[ 4 ];

    sh_int mana;
    sh_int max_mana;     /*   Max move for PC/NPC                    */
    sh_int hit;
    sh_int max_hit;      /*   Max hit for PC/NPC                      */
    sh_int move;
    sh_int max_move;     /*   Max move for PC/NPC                     */
    sh_int taint;
	sh_int temp_taint;
    sh_int shadow_points;
    sh_int max_shadow_points;
    sh_int strain;
    int gold;           /* Money carried                             */
    int bank_gold;      /* Gold the char has in a bank account       */
    int exp;            /* The experience of the player              */
    sbyte damroll;       /* Any bonus or penalty to the damage roll   */
};

typedef std::map< std::string, int> kill_record;
//Data that will be LOADED and NOT TOUCHED
struct LoadOnlyData
{
	std::vector<int> HitRolls;
	std::vector<int> ManaRolls;
	kill_record Trophies;
};

//Variables only used by players...
struct PlayerOnlyData
{
	PlayerOnlyData()
	{
		memset( &newb, 0, sizeof( newb ) );
		account_id = -1;
		mount_save = 0;
		load_room = 0;
		mood = 0;
		invis_level = 0;
		freeze_level = 0;
		bad_pws = 0;
		conditions[0] = 0;
		conditions[1] = 0;
		conditions[2] = 0;
		skillpracs = 0;
		spellpracs = 0;
		tradepracs = 0;
		last_olc_targ = 0;
		wimp_level = 0;
		last_olc_mode = 0;
	}
	PlayerOnlyData(PlayerOnlyData *source)
	{
		memcpy( &newb, &source->newb, sizeof( newb ) );
		account_id = source->account_id;
		mount_save = source->mount_save;
		load_room = source->load_room;
		mood = source->mood;
		invis_level = source->invis_level;
		freeze_level = source->freeze_level;
		bad_pws = source->bad_pws;
		conditions[0] = source->conditions[0];
		conditions[1] = source->conditions[1];
		conditions[2] = source->conditions[2];
		skillpracs = source->skillpracs;
		spellpracs = source->spellpracs;
		tradepracs = source->tradepracs;
		last_olc_targ = 0;
		wimp_level = source->wimp_level;
		last_olc_mode = source->last_olc_mode;
	}
	int mount_save;
	long newb[ NB_ARRAY_MAX ];	/* Newbie Tip Flags */
	room_vnum load_room;		/* Which room to place char in		*/

	byte mood;
	byte invis_level;			/* level of invisibility		*/
	byte freeze_level;			/* Level of god who froze char, if any	*/
	ubyte bad_pws;				/* number of bad password attemps	*/
	sbyte conditions[ 3 ];		/* Drunk, full, thirsty			*/

	sh_int skillpracs;			/* Normal skill practice sessions  */
	sh_int spellpracs;			/* Channeler practice sessions */
	sh_int tradepracs;			/* Trade practice sessions(mining,smithing,etc) */

	void *last_olc_targ;		/* olc control				*/
	int	wimp_level;				/* Below this # of hit points, flee!	*/
	int last_olc_mode;			/* olc control				*/
	int account_id;				/* Player's account_id identification number */

	std::list<class UserDisabledCommand*> userDisabledCommands;
};

//Variables only used by MOBs...
struct MobOnlyData
{
	
	std::list<int> assists;			// List of vnums for MOBs to assist.
	std::list<long> memory;			// List of attacker's ids to remember.
	std::list<long> ForgetList;		// List of attackers who the mob should forget.
	int	attack_type;				// The Attack Type Bit Vector for NPCs.
	byte default_pos;				// Default position for NPC.
	byte damnodice;					// The number of damage dices.
	byte damsizedice;				// The size of the damage dices.
	sh_int wait_state;				// Wait state for bashed mobs.
	sh_int moves_per_tic;			// Moves regenerated per tic. Assume -1 means it is calculated dynamically.
	int aggro;
	int nsects;						// Sectors MOB is not allowed to go into.

	class Kit *primary_kit;			// Primary eq kit.
	struct FoodUnit *Food;
	struct SkinningUnit *Skin;
	bool deleted;

	MobOnlyData()
	{
		attack_type = 0;
		default_pos = POS_STANDING;
		damnodice = 0;
		damsizedice = 0;
		wait_state = 0;
		aggro = 0;
		nsects = 0;
		primary_kit = 0;
		Food = 0;
		Skin = 0;
		moves_per_tic = -1;
		deleted = false;
	}
	~MobOnlyData();
	MobOnlyData(MobOnlyData *source)
	{
		assists.insert(this->assists.end(), source->assists.begin(), source->assists.end());
		memory.insert(this->memory.end(), source->memory.begin(), source->memory.end());
		attack_type = source->attack_type;
		default_pos = source->default_pos;
		damnodice = source->damnodice;
		damsizedice = source->damsizedice;
		aggro = source->aggro;
		nsects = source->nsects;
		primary_kit = source->primary_kit;
		moves_per_tic = source->moves_per_tic;

		if(source->Food)
			this->Food = new FoodUnit(source->Food);
		else
			this->Food = 0;

		if(source->Skin)
			this->Skin = new SkinningUnit(source->Skin);
		else
			this->Skin = 0;

		deleted = source->deleted;
	}
};

class Warrant
{
	public:
		sh_int vnum;
		std::string Name;

		void Insert( std::list<Warrant *> &List );
		Warrant();
		Warrant( int v, const std::string &n );
		~Warrant();
};

/* Specials used by NPCs, not PCs */

/* An affect structure. */
struct affected_type
{
	affected_type()
	{
		type = 0;
		duration = 0;
		modifier = 0;
		location = 0;
		bitvector= 0;
		next = 0;
		callBack = 0;
		callTime = 0;
		remove_affect = false;
		accum_duration = false;
		accum_affect = false;
		inverted = false;
		add = true;
		chance = 100;
	}
	sh_int	type;          /* The type of spell that caused this      */
	sh_int	duration;      /* For how long its effects will last      */
	sbyte	modifier;       /* This is added to apropriate ability     */
	byte	location;        /* Tells which ability to change(APPLY_XXX)*/
	long	bitvector;       /* Tells which bits to set (AFF_XXX)       */

	bool remove_affect;
	bool accum_duration;
	bool accum_affect;
	bool fail_weave_if_failed;
	bool add;
	bool inverted;

	void (*callBack)(void *info);	/* Event handler for the effect */
	int callTime;					/* How often the callback should run */

	int chance;

	struct affected_type *next;
};

struct affect_type_not_saved
{
	affect_type_not_saved()
	{
		caster = 0;
		affect = 0;
		next = 0;
	}
	class Character *caster;
	struct affected_type *affect;
	struct affect_type_not_saved *next;
};

struct Follower
{
	Follower()
	{
		follower = 0;
		next = 0;
	}
	class Character *follower;
	struct Follower *next;
};

class pLogin
{
	public:
		pLogin()
		{
			time = 0;
		}
		std::string Host;
		std::string PlayerName;
		DateTime time;
		class GatewayDescriptorType *gatewayDescriptorType;

		std::string Print();
		pLogin( const std::string &h, time_t t, class GatewayDescriptorType *gatewayDescriptorType );
};

class Legend
{
	public:
		std::string name;
		int wp;

		Legend()
		{
			wp = 0;
		}
};

enum eCharType
{
	CharMob, CharPlayer, CharNone
};

class PlayerSkill
{
private:
	byte percent;
	short int skillId;
public:

	void setPercent(const int percent)
	{
		this->percent = (byte)percent;
	}
	const int getPercent()
	{
		return percent;
	}

	short int getSkillId()
	{
		return skillId;
	}

	void setSKillId(short int skillId)
	{
		this->skillId = skillId;
	}

	PlayerSkill()
	{
		percent = 0;
		skillId = 0;
	}
};

class Character
//11/06/2009 - implement JSBindable interface
	: public JSBindable, public Entity
{
public:
	static int nr_alloc;
	static int nr_dealloc;
	static int top_mob_id;
	sh_int	nr;								/* Mob's rnum						*/
	Room	*in_room;						/* Location (real room number)		*/
	Room	*was_in_room;					/* Location for idle people			*/
	kill_record	kill_list;					/* Trophy list						*/

	struct CharPlayerData	player;							/* Normal data						*/
	struct CharAbilityData	real_abils;						/* Abilities without modifiers		*/
	struct CharAbilityData	aff_abils;						/* Abils with spells/stones/etc		*/
	struct CharPointData	points;							/* Points							*/
	struct PlayerOnlyData	*PlayerData;
	struct MobOnlyData		*MobData;
	struct LoadOnlyData		*LoadData;
	std::list<class UserClan *>	userClans;

	struct affected_type	*affected;					/* Affected by what spells			*/
	struct affect_type_not_saved*affection_list;
	class Object	*equipment[ NUM_WEARS ];			/* Equipment array					*/

	class Object	*carrying;							/* Head of list						*/
	class Descriptor	*desc;							/* NULL for mobiles					*/

	Character	*next_in_room;					/* For room->people - list			*/
	Character	*next;							/* For either monster or ppl-list	*/
	Character	*next_fighting;					/* For fighting list				*/

	class PokerPlayer	*PokerData;				/* Poker Table data					*/
	class PokerTable	*PokerTable;			/* The Poker table this character is watching. */
	Character	*NextSeated;					/* Next seated at a poker table		*/
	struct Follower	*followers;					/* List of chars followers			*/
	Character	*master;						/* Who is char following?			*/
	Character	*SlowedBy;						/* Who affected the char with the slow spell */
	Character	*DecayedBy;						/* Who affected the char with the decay spell */
	Character   *BurnedBy;						/* Who affected the char with the burn spell */
	Character	*PlaguedBy;						/* Who affected the char with the dark plague spell */

	std::list<Object *>		wards;
	std::list<Quest *>		quests;
	std::list<std::string>	ignores;
	std::map<std::string, std::string>	aliases;
	std::string Command;

	float	timer;
	float	pulverizeCooldown;	/* Stores the remaining cooldown for the Pulverize skill. */
	bool	pw_updated;				/* Boolean to represent if the user has updated from crypt() to md5()	*/
	bool	ignoreCommandTrigger;	/* Will this character be affected by command triggers?					*/
	bool	command_ready;
	bool	Restat;
	bool	PermissionToSnoop;		/* This user must be granted permission by the target in order to snoop. */
	bool	CannotFinishCharge;		/* When this is set, the character cannot finish a charge. */
	bool	ShieldBlock;			/* Set while the "Shield Block" skill is active. */
	bool	InvertNextWeave;
	std::string	delayed_command;
	Room		*Eavesdropping;			/* Contains the room a character is eavesdropping on, if any.*/
	sh_int		dizzy_time;
	sh_int		wait;
	sh_int		Stats;
	sh_int		pk_deaths;
	sh_int		mob_deaths;
	sh_int		pks;
	sh_int		cmd_no;
	sh_int		ps_tgt;				/* Stores the body part targetted by the next Precise Strike. */
	sh_int		stance;				/* Stores a Blademaster's current stance. */
	byte		body_structure;
	long	last_tell;				/* idnum of last tell from		*/
	DateTime	restat_time;
	DateTime	reset_time;
	std::map<short int, PlayerSkill > skills;
	std::shared_ptr<EditorInterfaceInstance> editorInterfaceInstance;

	bool purged;					/* Is this character queued for purging? */
	bool proto;

	Character();		/* Class constructor*/
	Character(eCharType MyType);
	Character(const int nr, const int type, bool full_copy=false);
	~Character();		/* Class destructor*/
	bool IsProto();

	sh_int GetStr()  {  return aff_abils.str;   }
	sh_int GetInt()  {  return aff_abils.intel; }
	sh_int GetWis()  {  return aff_abils.wis;   }
	sh_int GetDex()  {  return aff_abils.dex;   }
	sh_int GetCon()  {  return aff_abils.con;   }

	void SetStr( byte _val ) { aff_abils.str   = _val; }
	void SetInt( byte _val ) { aff_abils.intel = _val; }
	void SetWis( byte _val ) { aff_abils.wis   = _val; }
	void SetDex( byte _val ) { aff_abils.dex   = _val; }
	void SetCon( byte _val ) { aff_abils.con   = _val; }

	bool TooHeavyToPickUp( Object *obj );

	const int getVnumOrID() const;
	const char GetType() const;
	int Difficulty();		//MOB / Character's 'difficulty' based on equipment,level,HP,etc.
	int getVnum() const;
	int VnumMobile( std::string SearchName );
	int VnumObject( std::string SearchName );
	int VnumRoom( std::string SearchName );
	int VnumZone(std::string SearchName);
	int VnumKit( std::string SearchName );
	int TrackLines(Room *room, bool auto_track, bool &full_view);
	int ShowLoc( Object *obj, const std::string &name );
	int RollMana( int level );
	int RollHealth( int level );
	int BaseMoves();
	int TotalQP();
	int NeededToMove( int dir );
	int NeededToMove( Room* OtherRoom );
	int BMI();
	int Parry();
	int Dodge();
	int Offense();
	int Absorb();
	int MoodParry();
	int MoodDodge();
	int MoodOffense();
	int CalculateDamage( Character *victim, int Type, int BodyPart );
	int NumFighting();
	int BashOffenseRoll();
	int BashDefenseRoll();
	int ChargeOffenseRoll(int skill);
	int ChargeDefenseRoll(int skill);
	int PrecStrikeOffenseRoll();
	int PrecStrikeDefenseRoll();
	int ShieldBlockOffenseRoll();
	int ShieldBlockDefenseRoll( int attack_type );
	int GetLegend();
	int GetSkillLevel( int skill );
	int Sheath();
	int CommunicationLevel( Character *receiver );
	int Draw();
	int FindAssistPriority( Character *test );
	int OffenseMeleeRoll();
	int DefenseMeleeRoll();
	int StatMaximum( int stat );
	int StatMinimum( int stat );
	int SkillAdd( int skill );
	int CostPerStat( int stat );
	int PracticeGain( int level );
	int SPracticeGain( int level );
	int TPracticeGain(int level);
	int NumOfStats();
	int NativeTongue();
	int BestUnderstoodTongue( Character *target );
	int FindDoor( const char *type, char *dir, const char *cmdname );
	int listUnrentables(Character *innkeeper);
	int listUnrentables(Character *innkeeper, Object *container);
	int SimpleMove( int dir, int need_specials_check, bool flee );
	int group_members_in_zone();
	int GetAccount();
	int CountZoneLoads();
	int ExperienceToLevel();
	bool anyEquipmentMeetsCritera(const std::function<bool(Object *obj)> &predicate);

	const char *getName();
	const char *getDisplayName();

	void setSkill(short int skillId, int percent);
	int getSkill(short int skillId);
	bool hasSkill(short int skillId);

	sh_int HitGain();
	sh_int MoveGain();
	sh_int ManaGain();
	sh_int ShadowGain();
	sh_int GetSamesideWeaveLoss();
	sh_int GetWeaveLoss();
	sh_int GetTotalTaint();

	byte GetStat( int stat );

	float TotalWeight();
	float CarriedWeight();
	float WornWeight();

	unsigned long int Copper();
	unsigned long int Silver();
	unsigned long int Gold();

	static UserDisabledCommand *loadUserDisabledCommand(const sql::Row &row);
	static std::list<UserDisabledCommand *> loadUserDisabledCommands(const int user_id);
	static void putUserDisabledCommand(UserDisabledCommand *userDisabledCommand);
	static void saveUserDisabledCommands(int user_id, std::list<UserDisabledCommand *> &userDisabledCommands);
	static void deleteUserDisabledCommands(int user_id);

	bool CanTeach( class Weave *w );
	bool LoggedIn();
	void PasswordUpdated( bool val );
	bool PasswordUpdated();
	bool HasAccessToCommand( const int nr );
	bool HasMagicMarker();
	bool HasMagicWand();
	bool WhoisShowClass();
	static bool WhoisShowClass(const int &chclass, const int &race, const int &level);
	bool ShouldLoveMoves();
	bool SnuckOut();
	bool SnuckIn();
	bool ShouldLayTrack();
	bool TimedOut(const int days);
	static bool LoadLogins(std::list<pLogin> &Logins, std::string &Name, const int iLimit=-1, time_t tLow=-1, time_t tHigh=-1);
	bool IsMaster();
	bool CanViewClan( sh_int clannum );
	bool AES_SEDAI();
	bool TOWER_MEMBER();
	bool wantedByClan( short clanId );
	bool wantedByPlayer( Character *ch );
	bool IsIgnoring( const std::string &person );
	bool HasTowerWarrant();
	bool CanTaint();
	bool IsGroupedWith( Character *ch );
	bool BackstabLanded( Character *victim );
	bool ChannelingAbility();
	bool CanBeLegend();
	bool NeedsRestat();
	bool NeedsReset();
	bool HasBoat();
	bool HasKey( int key );
	bool HasObjectInInventory( int vnum );
	bool FluentWith( Character *receiver );
	bool CanWear(Object *obj, int &loc);
	bool CanWearRemortGear(Object *obj);
	bool CreateDatabaseEntry();
	bool CanAggro(Character *victim);
	bool isInClan(const int clanId);
	bool CanTrack(Room *room);
	bool canGainWeave( Character* victim );
	bool CanPractice( class Weave* weave );
	bool CanPractice( const int spell_vnum );
	bool check_if_pc_in_group();
	bool CanDraw(Object *obj);
	bool IsPurged() { return purged; }
	bool circleFollow(Character *target);
	void stopFollowing();
	bool passwordMatches(const std::string &passwordInput);
	bool hasWolfbrotherBonuses();
	bool disorientRoll();
	bool shouldBlockEngagementDueToNumberFighting(Character *victim, bool displayMessage);


	bool ShieldOutOfRange( Character* Target );

	bool save();
	bool basicSave();
	bool saveSkills();
	bool saveClans();
	bool saveTrophies();
	bool saveHitRolls();
	bool saveManaRolls();
	bool saveWeaves();

	bool mysqlInsertQuery();
	bool rollBash( Character *Victim, bool BashFaceoff );
	bool rollBash( Character *Victim, int &off, int &def );
	bool rollPrecStrike( Character *Victim );
	bool rollPrecStrike( Character *Victim, int &off, int &def);
	bool rollShieldBlock( Character *Victim, int attack_type );
	bool loadSkills();
	bool loadIgnores();
	bool loadAliases();
	bool loadClans();
	bool loadTrophies();
	bool loadHitRolls();
	bool loadManaRolls();
	bool loadQuests();
	bool hasPermissionToSnoop();

	void GrantPermissionToSnoop();
	void ToggleSnoop();

	static bool ProperNameFormat( const std::string &Name );

	class UserClan *getUserClan( short clanId );
	Quest *GetQuest(const std::string &QuestName );

	std::string GoldString( int amount, bool colors=true, bool abbrev=false );
	std::string HeightToFeetAndInches();
	std::string ScrambleSpeech( const std::string &Speech, Character *Listener );
	std::string RaceName();
	std::string ClassName();
	std::string BuildRaceAndClassNameList();
	static std::string RaceName( const int &chclass, const int &race, const int &level);
	static std::string ClassName(const int &chclass, const int &race, const int &level);

	const char *Health();

	Object *SittingOn();
	void StandFromChair();
	void SitOnChair( Object *Chair, bool show=false);

	Object *GetObjectFromInventory( int vnum );
	Object *GetObjectFromInventory( const std::string &name);

	Room *StartRoom();
	Room *ArenaStartRoom();

	Character *FindFirstAssist();
	Character *RandomFighting();
	Character *CanBash( char *argument );
	Character *MovePenalizer();
	Character *GetPrototype(); /* Return the prototype of this mobile. NULL if PC or prototypes does not exist */
	Character *IsLeadingMount();

	void WaitState( int pulses );
	void WaitEvents();
	bool WaitUpdate();

	void SetAccount( const int id );
	void PointUpdate( bool login=false );
	void UnsetMobPrototypeSpecifics();
	void SetNewFighting();
	void RemoveSource();
	void ShowZones( Character *ch );
	void InterpCommand( const char *Command, ... );
	void ShowTimer();
	void SetupTimer( std::string command, float time );
	void CancelTimer( bool show );
	void DeleteAliases();
	void DeleteQuests();
	void DeleteClans();
	void DeleteAffections();
	void StepThroughGate( class Gate *g );
	void RemoveFromRoom();
	void MoveToRoom( Room *room );
	void MoveToRoom( int rnum );
	void Dismount();
	void Die( Character *killer = NULL );
	void DeathCry();
	void MakeCorpse();
	void StopFighting();
	void SetFighting( Character *vict );
	void Init();
	void send( const char *messg, va_list args );
	void send( const char *messg, ... );
	void send( const std::string &s );
	void sendDisorientableMessage( const char *message, ... );
	void sendDisorientableMessage( const std::string &s );
	void HitAllFighting();
	void PerformFear( Character *victim );
	void PerformPoison( Character *victim );
	void UpdateLegendEntry();
	void InterruptTimer();
	void Appear();
	void Extract();
	void Extract(bool full_delete);
	void Extract(class UserLogoutType *userLogoutType);
	void Extract(class UserLogoutType *userLogoutType, bool full_delete);
	void SetWarrant( Character *warranter, int clan, bool hide );
	void RemoveWarrant( Character *pardoner, int num );
	void LayTrack( Room *room, int direction );
	void PrintTracks( Room *room, bool auto_track );
	void PrintTracks( Room *room, std::string name );
	void RollWeightAndHeight();
	void DropLevel();
	void GainLevel( int show );
	void SetTitle( std::string title );
	void ResetHitRolls( bool pts );
	void ResetManaRolls( bool pts );
	void RollMoves();
	void itemSave();
	std::list< std::pair<Object*,int> > loadItemList( bool recursive );
	void loadItems();
	void saveItems();
	void AddChannelingStrain( bool Failure, int spell );
	void AddStrain( int strain );
	void RandomTaintSocial();
	void RandomTaintVoices();
	void RemoveTaintDizzy();
	void ListWarrants();
	void KillAllGates();
	void ResetAllSkills();
	void ResetSkills();
	void ResetTrades();
	void ResetSpells();
	void rentSave();
	void saveAll();
	void RollLuck();
	void GainNoQuit( Character *victim );
	void CheckWiz();
	void ListLegends();
	void NewbieTip( const char *msg, ... );
	void SetSkillDefaults();
	void extractUnrentables();
	void StatSelectionMenu( bool notify );
	void LogOutput( const std::string &buffer );
	void ReadWeaves();
	void WriteWeaves();
	void SendTell( Character *target, char *arg );
	void RollStats();
	void AddIgnore(const std::string &name);
	void RemoveIgnore(const std::string &name);
	void removeFromClan();
	void removeFromClan(const int clanId);
	void addToClan(const int clanId);
	void addToClan(class UserClan *userClan);
	void AddLogin(const std::string &host, const DateTime &loginDatetime, class GatewayDescriptorType *gatewayDescriptorType);
	void HuntVictim();
	void Remember(Character *victim);
	void Forget(Character *victim);
	void processForgets();
	void clearMemory();
	void createPlayerIndex();
	void zero();
	void weaveGroupDistribution( Character* victim );
	void gainWeave( sh_int wp );
	void lookAtGate( class Gate* gate );
	void stopSnooping();
	void rollShadow();
	void setGateTimer();
	void stopEavesdropping();
	void stopWarding();
	void setBashState(const int nrPulses, bool cancelTimer = true, bool makeSit = true);
	void performWear(Object *obj, int wearLocation);
	void performRemove(int wearLocation);

	int getUserId();
	UserType *getUserType();

	Character& operator<< ( const std::string &s );
	Character& operator<< ( const int );
	Character& operator<< ( const float );
	Character& operator<< ( const double );
	Character& operator<< ( const bool );
	Character& operator<< ( const char * s );
	Character& operator<< ( const char s );

	std::string getDisplayableId();
	Room *getRoom();
	class EntityType *getEntityType();
};

/* other miscellaneous structures ***************************************/

class Wizard
{
	public:
		Wizard()
		{
			level = 0;
			next = 0;
		}
		std::string name;
		sh_int level;
		class Wizard *next;

};

const int NUM_OF_KITS = 3;

class KitItem
{
	int body_pos;
	int item_vnum;
	int percent;

	int dbID;
	bool deleted;
public:
	KitItem();
	KitItem( const int bp, const int iv, const int p, const int id );
	~KitItem();

	void SetBodyPos( const int bp );
	void SetItemVnum( const int iv );
	void SetPercent( const int perc );

	const int GetBodyPos();
	const int GetItemVnum();
	const int GetItemRnum();
	const int GetPercent();

	bool IsDeleted();
	void SetDeletion();
	void UnsetDeletion();

	bool IsInDB();
	const int GetDBID();
	void SetDBID( const int dbid );


};

class Kit
{
	public:
		std::string Name;

		std::vector< std::vector< KitItem > > KitItems;
		std::vector< KitItem > KitInventory;

		int vnum; /* unique kit number */
		bool deleted;
		bool in_db;
		class Kit *prev, *next; /* Doubly linked list */

		void save();
		void AppendBlankKit();
		void Boot(class sql::Row *MyRow, std::list<sql::Row> MyItems);
		void CopyFrom( Kit *Source, bool full=true );

		Kit();
};

/* Kit Manager class, maintains and stores anything to do with kits (Galnor - 04/21/2009) */
class KitManager
{
private:
	static KitManager *Self;		//The static KitManager
	std::list<Kit *> MyKits;		//List of kits
	KitManager();
	~KitManager();
public:
	static KitManager &GetManager();	//Grab/allocate(if needed) the KitManager(static)

	Kit *GetKitByVnum( const int vnum );
	void AddKit( Kit *k );
	std::list<Kit *>::iterator RemoveKit( Kit *k );
	std::list<Kit *>::iterator RemoveKit( const int vnum );

	std::list<Kit *> GetRangeOfKits( int lowVnum, int highVnum );
	std::list<Kit *> GetAllKits();

	void Free();
};

class Track
{
	public:
		Track();
		Track *Clone();
		std::string name;		//The display name of the layer.
		std::string alias;		//Alias of the track layer.
		time_t laytime;		//When was the track laid?
		byte direction;			//What direction?
		byte race;				//What race was the track layer?
		byte ch_class;			//Needed for fades and such.
		Room *room;
		bool bloody;
		bool npc;				//Laid by an NPC?
		bool mount;				//Laid by a mount?
		bool mount_ridden;		//Laid by a mount that was ridden?

		time_t Age(time_t timeContext);
		std::string NameString();
		std::string AgeString();
		std::string ToString(bool full_view);
		Track( Character *ch, int dir, Room *room );
		~Track();
};
struct MeleeMessage
{
	MeleeMessage()
	{
		attacker_msg = 0;
		victim_msg = 0;
		room_msg = 0;
	}
	~MeleeMessage()
	{
		if(attacker_msg)
			delete[] attacker_msg;
		if(victim_msg)
			delete[] victim_msg;
		if(room_msg)
			delete[] room_msg;
	}
	char	*attacker_msg;  /* message to attacker */
	char	*victim_msg;    /* message to victim   */
	char	*room_msg;      /* message to room     */
};

struct MeleeMessageType
{
	MeleeMessageType()
	{
		next = 0;
	}
	struct MeleeMessage die_msg;	/* messages when death			*/
	struct MeleeMessage miss_msg;	/* messages when miss			*/
	struct MeleeMessage hit_msg;	/* messages when hit			*/
	struct MeleeMessage god_msg;	/* messages when hit on god		*/
	struct MeleeMessageType *next;	/* to next messages of this kind.	*/
};

struct MeleeMessageList
{
	int	a_type;			/* Attack type				*/
	int	number_of_attacks;	/* How many attack messages to chose from. */
	struct MeleeMessageType *msg;	/* List of messages.			*/
};

/* element in monster and object index-tables   */
struct Index
{
	Index()
	{
		memset( this, 0, sizeof( Index ) );
		in_db=false;
	}
	int	vnum;		/* virtual number of this mob/obj		*/
	int	number;		/* number of existing units of this mob/obj	*/
	bool in_db;
	SPECIAL( *func );

	char *farg;         /* string argument for special function     */
};

struct Charge
{
	float O_OBMultiplier;
	float O_BMIMultiplier;
	float O_WeaponWeightMalus;
	float O_NegativeSway;
	float O_PositiveSway;

	int D_BaseBonus;
	float D_DBMultiplier;

	float D_BMIMultiplier;//Currently unused...

	float Timer;
	bool CanStartOnFightingVict;
	bool CanStartWhileFighting;
	bool CanFinishWhileFighting;
	int D_PulsesLagOnLand;
	int O_PulsesLagOnLand;
	int O_PulsesLagOnMiss;

	Charge()
	{
		memset( this, 0, sizeof( Charge ) );
	}
};

class Bash
{
public:
	Bash()
	{
		memset( this, 0, sizeof( Bash ) );
	}
	float O_OBMultiplier;
	float O_BMIMultiplier;
	int   O_BMIOffset;
	float O_WeaponWeightMultiplier;

	int	  D_BaseBonus;
	float D_DBMultiplier;
	float D_BMIMultiplier;

	float WeightDifferenceMultiplier;
	float LevelDifferenceMultiplier;
	float WeaponDifferenceMultiplier;

	static bool BashFailWeight( Character *Basher, Character *Victim );
	static bool BashFailLevel( Character *Basher, Character *Victim );
	static bool BashSucceedWeapon( Character *Basher, Character *Victim );
};

struct Flee
{
	Flee()
	{
		memset( this, 0, sizeof( Flee ) );
	}
	int PulsesPerEngagedFlee;
	int PulsesPerUnengagedFlee;
	int PulsesPerAutoFlee;
	int PulsesRemovedPerTic;
	int PulsesForFirstFlee;
	int PulsesPerExit;
	sh_int MaxFleeLag;
};

struct Melee
{
	Melee()
	{
		memset( this, 0, sizeof( Melee ) );
	}
	float WardMultiplier;
	float ParrySplit;
	float DodgeMultiplier;
	float ParryMultiplier;
	int PoundsPerOffense;
	int PoundsPerParry;
	int PoundsPerDodge;
};

struct PreciseStrike
{
	PreciseStrike()
	{
		memset( this, 0, sizeof( PreciseStrike ) );
	}
	float O_OBMultiplier;
	float O_OffenseRollFactor;
	float O_WeightMultiplier;

	int D_BaseDB;
	float D_BashedMultiplier;
	float D_DBMultiplier;
	float D_PBMultiplier;
};

class Taint
{
public:
	Taint()
	{
		memset( this, 0, sizeof( Taint ) );
	}
	int TaintForRandomCommand;
	int TaintForRandomChannel;
	int TaintForRandomSocial;
	int TaintForRandomVoice;
	int TaintForRandomTarget;
	int TaintForSeizeDizziness;
	int StrainToTaint;
	int SeizeCost;
	int HungerThirstCost;
	float LevelStrainRemoval;
	float WisStrainRemoval;
	float BlackTowerBonus;
	float TimerCost;
	float ManaCost;
	float FailureCost;
};

struct DefaultRebootTime
{
	DefaultRebootTime()
	{
		hour=minute=second=0;
	}
	int hour;
	int minute;
	int second;
};

class GameOperation
{
	public:
		GameOperation()
		{
			LOGNAME = 0;
			max_playing = 0;
			max_bad_pws = 0;
			nameserver_is_slow = 0;
			auto_save_olc = 0;
			use_new_socials = 0;
			MENU = 0;
			WELC_MESSG = 0;
			START_MESSG = 0;
		}
		~GameOperation() {
			if( LOGNAME )
				delete[] LOGNAME;
			if( MENU )
				delete[] MENU;
			if( NEWBIE_MSG )
				delete[] NEWBIE_MSG;
			if( WELC_MESSG )
				delete[] WELC_MESSG;
			if( START_MESSG )
				delete[] START_MESSG;
		}
		DefaultRebootTime defaultRebootTime;	/* Default time of day for reboot.		*/
		char *LOGNAME;				/* The file to log messages to.			*/
		int max_playing;			/* Maximum number of players allowed.	*/
		int max_bad_pws;			/* Maximum number of pword attempts.	*/
		int nameserver_is_slow;		/* Is the nameserver slow or fast?		*/
		int use_new_socials;		/* Use new or old socials file ?		*/
		int auto_save_olc;			/* Does OLC save to disk right away ?	*/
		char *MENU;					/* The MAIN MENU.						*/
		char *NEWBIE_MSG;			/* The newbie message after stat menu	*/
		char *WELC_MESSG;			/* The welcome message.					*/
		char *START_MESSG;			/* The start msg for new characters.	*/
};

/*
 * Config structs
 *
 */

/*
* The game configuration structure used for configurating the game play
* variables.
*/
class GameData
{
	public:
		GameData()
		{
			memset( this, 0, sizeof( GameData ) );
			switch_restriction=true;
		}
	~GameData()
	{
		if( OK )		delete[] OK;
		if( NOPERSON )	delete[] NOPERSON;
		if( NOEFFECT )	delete[] NOEFFECT;
	}
		int level_can_shout;		/* Level player must be to shout.			*/
		int tunnel_size;			/* Number of people allowed in a tunnel.	*/
		int max_exp_gain;			/* Maximum experience gainable per kill.	*/
		int max_npc_corpse_time;	/* Num tics before NPC corpses decompose	*/
		int max_pc_corpse_time;		/* Num tics before PC corpse decomposes.	*/
		int idle_void;				/* Num tics before PC sent to void(idle)	*/
		int idle_rent_time;			/* Num tics before PC is autorented.		*/
		int load_into_inventory;	/* Objects load in immortals inventory.		*/
		int max_hit_exp;			/* Max hit exp based on level exp			*/
		int decay_script;			/* Vnum of the item decays script			*/
		float hit_exp_mult;         /* Multiplier for hit exp                   */
		float ExpMultiplier;		/* EXP Bonuses								*/
		float WpGainMultiplier;		/* Weave Point gain bonus					*/
		float WpLossMultiplier;		/* Weave Point loss bonus					*/
		char *OK;					/* When player receives 'Okay.' text.		*/
		char *NOPERSON;				/* 'No-one by that name here.'				*/
		char *NOEFFECT;				/* 'Nothing seems to happen.'				*/
		DateTime restat_time;		/* Time of global reset						*/
		DateTime reset_time;		/* Time of global restat					*/
		bool switch_restriction;	/* Is switch restriction enabled?			*/
};

/*
 * The rent and crashsave options.
 */
class CrashSaveData
{
	public:
		CrashSaveData()
		{
			free_rent = 0;
			max_obj_save = 0;
			auto_save = 0;
			autosave_time = 0;
		}
		int free_rent;          /* Should the MUD allow rent for free?  */
		int max_obj_save;       /* Max items players can rent.          */
		int auto_save;          /* Does the game automatically save ppl?*/
		int autosave_time;      /* if auto_save=TRUE, how often?        */
};


struct StartRoom
{
	StartRoom()
	{
		vnum = 0;
		ptr = 0;
	}
	sh_int vnum;
	Room *ptr;
};
/*
 * The room numbers.
 */
class room_numbers
{
	public:
		StartRoom human_start;
		StartRoom trolloc_start;
		StartRoom aiel_start;
		StartRoom seanchan_start;
		StartRoom immortal_start;
		StartRoom frozen_start;
};

class Config
{
	public:
		class GameData play;		/* play related config   */
		class CrashSaveData csd;		/* rent and save related */
		class room_numbers room_nums;	/* room numbers          */
		class GameOperation operation;	/* basic operation       */

		Config();
		~Config(){}
		void Load();
		void save();
		void RenumberRooms();
		const int GetReboots()
		{
			return NumberOfReboots;
		}
		bool empty_world;
	private:
		int NumberOfReboots;
};

#endif

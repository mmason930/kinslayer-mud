/* ************************************************************************
*   File: spells.h                                      Part of CircleMUD *
*  Usage: header file: constants and fn prototypes for spell system       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef SPELLS_H
#define SPELLS_H

const int DEFAULT_STAFF_LVL = 12; 
const int DEFAULT_WAND_LVL = 12; 

#define CAST_UNDEFINED	-1
const int CAST_SPELL = 0; 
const int CAST_POTION = 1; 
const int CAST_WAND = 2; 
const int CAST_STAFF = 3; 
const int CAST_SCROLL = 4; 

#define MAG_DAMAGE	(1 << 0)
#define MAG_AFFECTS	(1 << 1)
#define MAG_UNAFFECTS	(1 << 2)
#define MAG_POINTS	(1 << 3)
#define MAG_ALTER_OBJS	(1 << 4)
#define MAG_GROUPS	(1 << 5)
#define MAG_MASSES	(1 << 6)
#define MAG_AREAS	(1 << 7)
#define MAG_SUMMONS	(1 << 8)
#define MAG_CREATIONS	(1 << 9)
#define MAG_MANUAL	(1 << 10)


#define TYPE_UNDEFINED               -1
const int SPELL_RESERVED_DBC = 0; // SKILL NUMBER ZERO -- RESERVED 

/* PLAYER SPELLS -- Numbered from 1 to MAX_SPELLS */

const int SKILL_MISC = 0;

const int SPELL_AGILITY = 1; // Reserved Skill[] DO NOT CHANGE
const int SPELL_TELEPORT = 2; // Reserved Skill[] DO NOT CHANGE
const int SPELL_BLESS = 3; // Reserved Skill[] DO NOT CHANGE
const int SPELL_BLINDNESS = 4; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CALL_LIGHTNING = 5; // Reserved Skill[] DO NOT CHANGE
const int SPELL_ICE_SHARDS = 6; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CONTROL_WEATHER = 7; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CREATE_FOOD = 8; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CREATE_WATER = 9; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CURE_BLIND = 10; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CURE_CRITIC = 11; // Reserved Skill[] DO NOT CHANGE
const int SPELL_CURE_LIGHT = 12; // Reserved Skill[] DO NOT CHANGE
const int SPELL_EARTHQUAKE = 13; // Reserved Skill[] DO NOT CHANGE
const int SPELL_WARD_ITEM = 14; // Reserved Skill[] DO NOT CHANGE
const int SPELL_FIREBALL = 15; // Reserved Skill[] DO NOT CHANGE
const int SPELL_LOCATE_OBJECT = 16; // Reserved Skill[] DO NOT CHANGE
const int SPELL_POISON = 17; // Reserved Skill[] DO NOT CHANGE
const int SPELL_SANCTUARY = 18; // Reserved Skill[] DO NOT CHANGE
const int SPELL_SLEEP = 19; // Reserved Skill[] DO NOT CHANGE
const int SPELL_STRENGTH = 20; // Reserved Skill[] DO NOT CHANGE
const int SPELL_REMOVE_POISON = 21; // Reserved Skill[] DO NOT CHANGE
const int SPELL_REGEN = 22; // Spell for regenerating moves
const int SPELL_AIR_SCYTHE = 23; // Spell for air scythe
const int SPELL_NIGHT_VISION = 24; // Spell to see in the dark
const int SPELL_SHIELD = 25; // Spell for shielding someone from the source
const int SPELL_HASTE = 26; // Spell for the haste weave
const int SPELL_LOCATE_LIFE = 27; // Spell for Locate Life
const int SPELL_TORNADO = 28; // Spell for the tornado weave
const int SPELL_RESTORE = 29; // Spell for restore
const int SPELL_BALEFIRE = 30; // Spell for balefire
const int SPELL_GATE = 31; // Spell for the gate weave
const int SPELL_BOND = 32; // Spell to bond a warder
const int SPELL_FIRE = 33;
const int SPELL_WATER = 34;
const int SPELL_EARTH = 35;
const int SPELL_AIR = 36;
const int SPELL_SPIRIT = 37;
const int SPELL_WARD_WEAPON = 38;
const int SPELL_CREATE_FIRE = 39;
const int SPELL_FLAME_STRIKE = 40;
const int SPELL_IMMOLATE = 41;
const int SPELL_HARNESS_THE_WIND = 42;
const int SPELL_HARDEN_AIR = 43;
const int SPELL_REND = 44;
const int SPELL_SAND_STORM = 45;
const int SPELL_BLAZE = 46;
const int SPELL_SCORCH = 47;
const int SPELL_FIERY_SWORD = 48;
const int SPELL_CHILL = 49;
const int SPELL_ARMS_OF_AIR = 50;
const int SPELL_RAISE_FOG = 51;
const int SPELL_FROSTBITE = 52;
const int SPELL_CURRENT = 53;
const int SPELL_FLOOD = 54;
const int SPELL_WHIRLPOOL = 55;
const int SPELL_BLIZZARD = 56;
const int SPELL_DAZE = 57;
const int SPELL_WEAKEN = 58;
const int SPELL_FRIGHTEN = 59;
const int SPELL_QUICKSAND = 60;
const int SPELL_RAIN_OF_EARTH = 61;
const int SPELL_IMPACT = 62;
const int SPELL_GRENADE = 63;
const int SPELL_RIVEN_EARTH = 64;
const int SPELL_EAVESDROP = 65;
const int SPELL_SILENCE = 66;
const int SPELL_DEAFEN = 67;
const int SPELL_DECAY = 80;
const int SPELL_DARK_PLAGUE = 86;

/* Insert new spells here, up to MAX_SPELLS */
const int MAX_SPELLS = 130; 

/* PLAYER SKILLS - Numbered from MAX_SPELLS+1 to MAX_SKILLS */
const int SKILL_BACKSTAB = 131; 
const int SKILL_BASH = 132; 
const int SKILL_HIDE = 133; 
const int SKILL_KICK = 134; 
const int SKILL_PICK_LOCK = 135; 
const int SKILL_RESCUE = 136; 
const int SKILL_SNEAK = 137; 
const int SKILL_STEAL = 138; 
const int SKILL_TRACK = 139; 
const int SKILL_ATTACK = 140; 
const int SKILL_BOW = 141; 
const int SKILL_DODGE = 142; 
const int SKILL_PARRY = 143; 
const int SKILL_NOTICE = 144; 
const int SKILL_RIDE = 145; 
const int SKILL_SEARCH = 146; 
const int SKILL_FADE = 147; 
const int SKILL_TROLL_TONGUE = 148; 
const int SKILL_EFFUSION = 149; 
const int SKILL_FEAR = 150; 
const int SKILL_CLUB = 151; 
const int SKILL_AXE = 152; 
const int SKILL_LONG_BLADE = 153; 
const int SKILL_SHORT_BLADE = 154; 
const int SKILL_STAFF = 155; 
const int SKILL_SPEAR = 156; 
const int SKILL_CHAIN = 157; 
const int SKILL_SURVIVAL = 158; 
const int SKILL_CHARGE = 159; 
const int SKILL_SKEWER = 160; 
const int SKILL_LANCE = 161; 
const int SKILL_COVER_TRACKS = 162; 
const int SKILL_OLD_TONGUE = 163; 
const int SKILL_NEW_TONGUE = 164; 
const int SKILL_BAREHANDS = 165;
const int SKILL_PRECISESTRIKE = 166;
const int SKILL_COMPEL = 167;
const int SKILL_HAMSTRING = 168;
const int SKILL_WHIRLWIND = 169;
const int SKILL_MIXING = 170;
const int SKILL_COOKING = 171;
const int SKILL_FISHING = 172;
const int SKILL_HERBALISM = 173;
const int SKILL_MINING = 174;
const int SKILL_SMITHING = 175;
const int SKILL_SHADOW_RAGE = 176;
const int SKILL_MULTISHOT = 177;
const int SKILL_CRITICAL_HIT = 178;
const int SKILL_PROJECTILES = 179;
const int SKILL_POWER_SHOT = 180;
const int SKILL_BARRAGE = 181;
const int SKILL_SHADOWSTEP = 182;
const int SKILL_LEATHERWORKING = 183;
const int SKILL_SKINNING = 184;
const int SKILL_DUALBASH = 185;
const int SKILL_SWITCHTARGET = 186;
const int SKILL_CHOKE = 187;
const int SKILL_PULVERIZE = 188;

/* New skills may be added here up to MAX_SKILLS (200)						*/


const int TOP_SPELL_DEFINE = 299; 
/* NEW NPC/OBJECT SPELLS can be inserted here up to 299 */


/* WEAPON ATTACK TYPES */

const int TYPE_HIT = 300; 
const int TYPE_STING = 301; 
const int TYPE_WHIP = 302; 
const int TYPE_SLASH = 303; 
const int TYPE_BITE = 304; 
const int TYPE_BLUDGEON = 305; 
const int TYPE_CRUSH = 306; 
const int TYPE_POUND = 307; 
const int TYPE_CLAW = 308; 
const int TYPE_MAUL = 309; 
const int TYPE_THRASH = 310; 
const int TYPE_PIERCE = 311; 
const int TYPE_BLAST = 312; 
const int TYPE_PUNCH = 313; 
const int TYPE_STAB = 314; 

/* new attack types can be added here - up to TYPE_SUFFERING				*/
const int TYPE_SUFFERING = 399; 



const int SAVING_PARA = 0; 
const int SAVING_ROD = 1; 
const int SAVING_PETRI = 2; 
const int SAVING_BREATH = 3; 
const int SAVING_SPELL = 4; 


const int TAR_IGNORE = 1; 
const int TAR_CHAR_ROOM = 2; 
const int TAR_CHAR_WORLD = 4; 
const int TAR_FIGHT_SELF = 8; 
const int TAR_FIGHT_VICT = 16; 
const int TAR_SELF_ONLY = 32; // Only a check, use with i.e. TAR_CHAR_ROOM 
const int TAR_NOT_SELF = 64; // Only a check, use with i.e. TAR_CHAR_ROOM 
const int TAR_OBJ_INV = 128; 
const int TAR_OBJ_ROOM = 256; 
const int TAR_OBJ_WORLD = 512; 
const int TAR_OBJ_EQUIP = 1024; 

class Spell
{
	public:
		sbyte	min_position;		/* Position for caster								*/
		sbyte	violent;
		int	mana;
		int	class_type;
		int	targets;			/* See below for use with TAR_XXX					*/
		int	routines;
		int	min_level[ NUM_CLASSES ];
		float	timer;
		bool	levels;				/* Does this skill go by a level system?			*/
		bool	element;

		int	Fire;
		int	Water;
		int	Earth;
		int	Air;
		int	Spirit;
};

/* Possible Targets:
 
   bit 0 : IGNORE TARGET
   bit 1 : PC/NPC in room
   bit 2 : PC/NPC in world
   bit 3 : Object held
   bit 4 : Object in inventory
   bit 5 : Object in room
   bit 6 : Object in world
   bit 7 : If fighting, and no argument, select tar_char as self
   bit 8 : If fighting, and no argument, select tar_char as victim (fighting)
   bit 9 : If no argument, select self, if argument check that it IS self.
 
*/

const int SPELL_TYPE_SPELL = 0; 
const int SPELL_TYPE_POTION = 1; 
const int SPELL_TYPE_WAND = 2; 
const int SPELL_TYPE_STAFF = 3; 
const int SPELL_TYPE_SCROLL = 4; 

/* Attacktypes with grammar */

struct attack_hit_type
{
	const char *singular;
	const char *plural;
};


#define ASPELL(spellname) \
void	spellname(Character *ch, Character *victim, Object *obj, char *argument, Weave* weave)

#define MANUAL_SPELL(spellname)	spellname(caster, cvict, ovict, argument, weave);

ASPELL( spell_create_water );
ASPELL( spell_teleport );
ASPELL( spell_gate );
ASPELL( spell_locate_object );
ASPELL( spell_ward_item );
ASPELL( spell_ward_weapon );
ASPELL( spell_regen );
ASPELL( spell_locate_life );
ASPELL( spell_tornado );
ASPELL( spell_restore );
ASPELL( spell_bond );
ASPELL( spell_eavesdrop );
ASPELL( spell_circle_of_silence );
ASPELL( spell_flare );
ASPELL( spell_delve );
ASPELL( spell_quicksand );
/* basic magic calling functions */

int find_zone_slope( int zone_obj, int zone_char );
int mag_damage(Character *ch, Character *victim, int spellnum);
int mag_mana_damage(Character *ch, Character *victim, int spellnum);
void mag_points( Character *ch, Character *victim, int spellnum );
void mag_affects( Character *ch, Character *victim, int spellnum );
void mag_groups( Character *ch, int spellnum );
void mag_masses( Character *ch, int spellnum );
void mag_areas( Character *ch, int spellnum );
void mag_unaffects( Character *ch, Character *victim, int spellnum );
void mag_alter_objs( Character *ch, Object *obj, int spellnum );
void mag_creations( Character *ch, int spellnum );
std::string DistanceString( int dist );
void DisplayWeaveMessages(Weave *weave, Character *ch, Character *victim);

#endif

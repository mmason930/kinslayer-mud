/* ************************************************************************
*   File: class.c                                       Part of CircleMUD *
*  Usage: Source file for class-specific code                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * This file attempts to concentrate most of the code which must be changed
 * in order for new classes to be added.  If you're adding a new class,
 * you should go through this entire file from beginning to end and add
 * the appropriate new special cases for your new class.
 */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "db.h"
#include "spells.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "clans.h"
#include "screen.h"
#include "olc.h"
#include "stats.h"
#include "constants.h"
#include "weaves.h"
#include "zones.h"
#include "StringUtil.h"
#include "MiscUtil.h"

extern int siteok_everyone;

/* local functions */
int parse_class( char arg );
long find_class_bitvector( char arg );

void check_autowiz( Character *ch );
int level_exp( int level );

bool isInArena(Character *ch);
int JS_getArenaStartRoomVnum( int race );
int getArenaTeam( Character *ch );
bool JS_isArenaInFFAMode();

std::string Character::RaceName( )
{
	return Character::RaceName(GET_CLASS(this),GET_RACE(this),GET_LEVEL(this));
}

std::string Character::ClassName()
{
	return Character::ClassName(GET_CLASS(this),GET_RACE(this),GET_LEVEL(this));
}
std::string Character::BuildRaceAndClassNameList() {

	if( IS_NPC(this) )
		return std::string("");

	std::list<std::string> NameList;
	
	if( isInArena(this) )
	{
		NameList.push_back("arena");
		
		if( !JS_isArenaInFFAMode() ) {
			if( getArenaTeam(this) == ARENA_BLUE )
				NameList.push_back("blue");
			else if( getArenaTeam(this) == ARENA_RED )
				NameList.push_back("red");
			else if( getArenaTeam(this) == ARENA_YELLOW )
				NameList.push_back("yellow");
			else if( getArenaTeam(this) == ARENA_GREEN )
				NameList.push_back("green");
		}
	}
	else {
		if( IS_HUMAN(this) )
			NameList.push_back("human");
		if( IS_AIEL(this) )
			NameList.push_back("aiel");
		if( IS_SEANCHAN(this) )
			NameList.push_back("seanchan");
		if( IS_TROLLOC(this) ) {
			NameList.push_back("trolloc");
			NameList.push_back("dark");
		}
	}
	if( IS_DREADLORD(this) ) {
		NameList.push_back("dreadlord");
		NameList.remove("trolloc");
	}
	if( IS_DREADGUARD(this) ) {
		NameList.push_back("dreadguard");
		NameList.remove("trolloc");
	}
	if( IS_FADE(this) ) {
		NameList.push_back("fade");
		NameList.push_back("myrddraal");
		NameList.remove("trolloc");
	}
	if( IS_GREYMAN(this) ) {
		NameList.push_back("greyman");
		NameList.remove("trolloc");
	}
	if( IS_BLADEMASTER(this) ) {
		NameList.push_back("blademaster");
	}
	if( IS_OGIER(this) ) {
		NameList.push_back("ogier");
	}
	return StringUtil::implode< std::list<std::string> >( NameList, " " );
}
std::string Character::RaceName( const int &chclass, const int &race, const int &level)
{
	if ( chclass == CLASS_DREADLORD )
		return "dreadlord";
	else if ( chclass == CLASS_FADE )
		return "myrddraal";
	else if ( chclass == CLASS_GREYMAN )
		return "greyman";
	else if( chclass == CLASS_DREADGUARD )
		return "dreadguard";
	else if( chclass == CLASS_BLADEMASTER )
		return "blademaster";
	else if( chclass == CLASS_OGIER )
		return "ogier";
	else if ( race == RACE_HUMAN )
		return "human";
	else if ( race == RACE_TROLLOC )
		return "trolloc";
	else if ( race == RACE_SEANCHAN )
		return "seanchan";
	else if ( race == RACE_AIEL )
		return "aiel";
	else if ( race == RACE_ANIMAL )
		return "animal";
	else if ( race == RACE_OTHER )
		return "other";
	else
	{
		MudLog(BRF, LVL_GOD, TRUE,
			"Invalid race string returned by RaceName(): Class: %d, Race: %d", chclass, race );
		return "";
	}
}
std::string Character::ClassName(const int &chclass, const int &race, const int &level)
{
	if(!Character::WhoisShowClass(chclass,race,level))
		return "";
	else if(chclass == CLASS_WARRIOR)
		return "warrior";
	else if(chclass == CLASS_THIEF)
		return "thief";
	else if(chclass == CLASS_RANGER)
		return "ranger";
	else if(chclass == CLASS_CHANNELER)
		return "channeler";
	else if(chclass == CLASS_BLADEMASTER)
		return "blademaster";
	else if(chclass == CLASS_OGIER)
		return "ogier";
	else
	{
		MudLog(BRF, LVL_GOD, TRUE,
			"Unknown class string returned by ClassName(): Class: %d, Race: %d",chclass,race);
		return "";
	}
}
float Character::TotalWeight()
{
	return ( GET_WEIGHT( this ) + this->CarriedWeight() + this->WornWeight() );
}
float Character::CarriedWeight()
{
	float weight = 0.f;
	for(Object *obj = this->carrying;obj;obj = obj->next_content)
	{
		weight += obj->Weight();
	}
	return weight;
}
float Character::WornWeight()
{
	float weight = 0.f;
	for(int i = 0;i < NUM_WEARS;++i)
	{
		if(GET_EQ(this, i))
		{
			weight += GET_EQ(this, i)->Weight();
		}
	}
	return weight;
}

int race_alias( Character *ch, char *alias )
{

	if ( !ch || !alias )
		return 0;

	if ( GET_LEVEL( ch ) >= LVL_IMMORT || IS_NPC( ch ) )
		return 0;

	if ( IS_FADE( ch ) && (!str_cmp( alias, "dark" ) || !str_cmp( alias, "fade" )) )
		return 1;

	if ( IS_DREADLORD( ch ) && (!str_cmp( alias, "dark" ) || !str_cmp( alias, "dreadlord" )) )
		return 1;

	if( IS_DREADGUARD(ch) && (!str_cmp( alias, "dark" ) || !str_cmp( alias, "dreadguard" )) )
		return 1;

	if ( IS_GREYMAN( ch ) && (!str_cmp( alias, "dark" ) || !str_cmp( alias, "greyman" )) )
		return 1;

	if ( IS_BLADEMASTER( ch ) && (!str_cmp( alias, "human" ) || !str_cmp( alias, "blademaster" )) )
		return 1;

	if ( IS_OGIER( ch ) && (!str_cmp( alias, "human" ) || !str_cmp( alias, "ogier" )) )
		return 1;

	if ( IS_TROLLOC( ch ) && !IS_FADE( ch ) && !IS_DREADLORD( ch ) && !IS_GREYMAN( ch ) &&
	        ( !str_cmp( alias, "dark" ) || !str_cmp( alias, "trolloc" ) ) )
		return 1;

	if ( IS_HUMAN( ch ) && !PLR_FLAGGED( ch, PLR_DARKFRIEND ) && !str_cmp( alias, "human" ) && GET_LEVEL( ch ) > 15 )
		return 1;

	if ( GET_LEVEL( ch ) <= 15 && !str_cmp( alias, "newbie" ) )
		return 1;

	return 0;
}

const char *race_abbrevs[] =
    {
        "Hum",
        "Tro",
        "Aie",
        "\n"
    };

const char *pc_race_types[] =
    {
        "human",
        "trolloc",
        "seanchan",
        "aiel",
        "animal",
        "other",
        "\n"
    };

/* The menu for choosing a race in interpreter.c: */
const char *race_menu =
    "\r\n"
    "Select a race:\r\n"
    "  [H]uman          : ** RECOMMENDED FOR NEW PLAYERS **\r\n"
	"                     Mainly located in the Wetlands, the 'humans' maintain control\r\n"
    "                     of all land from the Borderlands to the north, to the Sea of\r\n"
    "                     Storms to the south. They are also the most numerous, containing\r\n"
	"                     a great number of clans and nations across the world.\r\n\n"
    "  [T]rolloc        : ** RECOMMENDED EXPERIENCED PLAYERS ONLY **\r\n"
	"                     The beastly servants of the Great Lord of the Dark, Shai'tan,\r\n"
    "                     the Trollocs live in the cruel land of the Blight north of the\r\n"
    "                     Borderlands. Using their night-vision, keen hearing and smelling,\r\n"
	"                     and immense endurance, they set out to conquer the land of the\r\n"
	"                     humans according to the Great Lord's will while satisfying their bloodlust\r\n\n"
    "  [S]eanchan       : Soon to Come!\r\n\n"
    "  [A]iel           : Soon to Come!\r\n\n"
    ;

int parse_race( char arg )
{
	arg = LOWER( arg );

	switch ( arg )
	{
			case 'h':
			return RACE_HUMAN;
			case 't':
			return RACE_TROLLOC;
			case 's':
			return RACE_SEANCHAN;
			case 'a':
			return RACE_AIEL;
			case 'w':
			return RACE_ANIMAL;
			case 'o':
			return RACE_OTHER;
			default:
			return RACE_UNDEFINED;
	}
}
/////////////Classes///////////////

const char *class_abbrevs[] =
    {
        "Wa",
        "Th",
        "Ra",
        "Ch",
        "My",
        "Dr",
        "Bm",
        "Gr",
        "Dk",
		"Dg",
        "\n"
    };

const char *pc_class_types[] =
    {
        "warrior",
        "thief",
        "ranger",
        "channeler",
        "myrddraal",
        "dreadlord",
        "blademaster",
        "greyman",
        "draghkar",
		"dreadguard",
		"ogier",
        "\n"
    };


/* The menu for choosing a class in interpreter.c: */
const char *human_class_menu =
    "\r\n"
    "Select a class:\r\n"
#if defined(WARRIOR_SELECT)
    "  [W]arrior         : The warriors are the tanks of battle, specializing in the\r\n"
    "                      use of heavy weapons and armor.\r\n\n"
#endif
#if defined(THIEF_SELECT)
    "  [T]hief           : The surreptitious thieves use their keen skills of deceit, \r\n"
    "                      striking most painfully when least expected.\r\n\n"
#endif
#if defined(RANGER_SELECT)
    "  [R]anger          : Roaming most comfortably in the countryside and forests, the\r\n"
	"                      rangers are most proficient in their ability to track their foe\r\n"
	"                      while surviving in the uncivilized land.\r\n\n"
#endif
#if defined(CHANNELER_SELECT)
	"  [C]hanneler       : Although gifted with the ability to wield the One Power, the\r\n"
	"                      channeler is physically the weakest. Their power, however, is\r\n"
	"                      both unique and crucial for the well-being of the Light.\r\n\n"
#endif
    ;

const char *other_class_menu =
    "\r\n"
    "Select a class:\r\n"
#if defined(WARRIOR_SELECT)
    "  [W]arrior         : The warriors are the tanks of battle, specializing in the\r\n"
    "                    use of heavy weapons and armor.\r\n\n"
#endif
#if defined(THIEF_SELECT)
    "  [T]hief           : The surreptitious thieves use their keen skills of deceit, \r\n"
    "                      striking most painfully when least expected.\r\n\n"
#endif
#if defined(RANGER_SELECT)
    "  [R]anger          : Roaming most comfortably in the countryside and forests, the\r\n"
	"                      rangers are most proficient in their ability to track their foe\r\n"
	"                      while surviving in the uncivilized land.\r\n\n"
#endif
#if defined(CHANNELER_SELECT)
	"  [D]readguard      : Fearsome users of the One Power, these humans gave their souls\r\n"
	"                      to the Dark One in exchange for eternal life. Their ability to\r\n"
	"                      channel is invaluable to the efforts of the Shadow.\r\n\n"
#endif
	;


/*
 * The code to interpret a class letter -- used in interpreter.c when a
 * new character is selecting a class and by 'set class' in Act.wizard.c.
 */

int parse_class( char arg )
{
	arg = LOWER( arg );

	switch ( arg )
	{
		case 'w':
		return CLASS_WARRIOR;
		case 't':
		return CLASS_THIEF;
		case 'r':
		return CLASS_RANGER;
		case 'c':
		return CLASS_CHANNELER;
		case 'f':
		return CLASS_FADE;
		case 'd':
		return CLASS_DREADLORD;
		case 'b':
		return CLASS_BLADEMASTER;
		case 'g':
		return CLASS_GREYMAN;
		case 'u':
		return CLASS_DREADGUARD;
		case 'o':
			return CLASS_OGIER;
		default:
		return CLASS_UNDEFINED;
	}
}

/*
 * bitvectors (i.e., powers of two) for each class, mainly for use in
 * do_users.  Add new classes at the end so that all classes
 * use sequential powers of two (1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4,
 * 1 << 5, etc.
 */

long find_class_bitvector( char arg )
{
	arg = LOWER( arg );

	switch ( arg )
	{
			case 'w':
			return ( 1 << CLASS_WARRIOR );
			case 't':
			return ( 1 << CLASS_THIEF );
			case 'r':
			return ( 1 << CLASS_RANGER );
			case 'c':
			return ( 1 << CLASS_CHANNELER );
			default:
			return 0;
	}
}

/*
 * These are definitions which control the guildmasters for each class.
 *
 * The first field (top line) controls the highest percentage skill level
 * a character of the class is allowed to attain in any skill.  (After
 * this level, attempts to practice will say "You are already learned in
 * this area."
 *
 * The second line controls the maximum percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out higher than this number, the gain will only be
 * this number instead.
 *
 * The third line controls the minimu percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out below this number, the gain will be set up to
 * this number.
 *
 * The fourth line simply sets whether the character knows 'spells'
 * or 'skills'.  This does not affect anything except the message given
 * to the character when trying to practice (i.e. "You know of the
 * following spells" vs. "You know of the following skills"
 */

const int SPELL = 0;
const int SKILL = 1;

bool Character::NeedsReset()
{
	if (Conf->play.reset_time.before(DateTime()))
	{
		if ( this->reset_time.before(Conf->play.reset_time) &&
		        GET_LEVEL( this ) <= LVL_IMMORT && !PLR_FLAGGED( this, PLR_NODELETE ) )
			return true;
	}
	return false;
}

bool Character::NeedsRestat()
{
	if ( Conf->play.restat_time.before(DateTime()) )
	{
		if ( this->restat_time.before(Conf->play.restat_time) && ( IS_CHANNELER( this ) ||
		        IS_WARRIOR( this ) || IS_RANGER( this ) || IS_THIEF( this ) ) &&
		        GET_LEVEL( this ) <= LVL_IMMORT && !PLR_FLAGGED( this, PLR_NODELETE ) )
			return true;
	}
	return false;
}

int Character::NumOfStats()
{
	switch ( GET_CLASS( this ) )
	{
			case CLASS_WARRIOR:
				return 300;
			case CLASS_RANGER:
				return 400;
			case CLASS_CHANNELER:
			case CLASS_DREADGUARD:
				return 410;
			case CLASS_FADE:
			case CLASS_THIEF:
			case CLASS_GREYMAN:
			return 420;
	}
	return 0;
}

int Character::SkillAdd( int skill )
{
	int perc = 0, stat = 0, before = 0, add = 0;
	unsigned int i = 0;
	std::vector<int> segment( 5 );
	Weave* weave = WeaveManager::GetManager().GetWeave( skill );

	if( !weave )
		return 0;

	segment[ 0 ] = 45;
	segment[ 1 ] = 72;
	segment[ 2 ] = 85;
	segment[ 3 ] = 92;
	segment[ 4 ] = 99;

	perc = GET_SKILL( this, skill );

	if ( weave->classSet( CLASS_WARRIOR ) )
		stat = STAT_STR;
	else if ( weave->classSet( CLASS_THIEF ) )
		stat = STAT_DEX;
	else if ( weave->classSet( CLASS_RANGER ) )
		stat = STAT_INT;
	else if ( weave->classSet( CLASS_CHANNELER ) )
		stat = STAT_WIS;
	else
		stat = STAT_INT;

	if( weave->classSet( "Trade" ) )
		return 10;
	else {
		add = 	(int) (( (float) this->GetStat( stat ) * .60 ) +
				( (float) this->GetStat( STAT_INT ) * .40 ));
	}
	//add = 8 - 5

	//Not a weave element
	if ( !weave->isElement() )
	{
		while ( add > 0 )
		{
			for ( i = 0;i < segment.size();++i )
			{
				if ( perc < segment[ i ] )
					break;
			}
			if ( i == 0 ) 		//45
				add = (int) (add * 1);	//Does nothing, I know.
			else if ( i == 1 ) 	//72
				add = (int) (add * .75);
			else if ( i == 2 ) 	//85
				add = (int) (add * .50);
			else if ( i == 3 ) 	//92
				add = (int) (add * .25);
			else			//99
				add = 1;

			before = perc;
			perc = MIN( segment[ i ], perc + add );
			//If there's a remainder, we want to apply the segment malus to it.
			if ( add > perc - before )
					add -= perc - before;
			else
				break;
		}
	}
	//Elements work differently. We want to increment one level precisely.
	else
	{
		if( this->GetSkillLevel(skill) < 10 )
			perc = GET_SKILL(this, skill) + 1;
		else
			return 0;
	}

	return perc - GET_SKILL( this, skill );
}

byte Character::GetStat( int stat )
{
	static byte no_stat = 0;

	switch ( stat )
	{
	case STAT_STR:
		return aff_abils.str;
		break;
	case STAT_INT:
		return aff_abils.intel;
		break;
	case STAT_WIS:
		return aff_abils.wis;
		break;
	case STAT_DEX:
		return aff_abils.dex;
		break;
	case STAT_CON:
		return aff_abils.con;
		break;
	}
	return ( no_stat );
}

int Character::StatMaximum( int stat )
{
	if ( GET_RACE( this ) != RACE_TROLLOC )
	{
		if ( GET_CLASS( this ) == CLASS_WARRIOR )
		{
			if ( stat == STAT_STR )
				return 20;
		}
		else if ( GET_CLASS( this ) == CLASS_RANGER )
		{
			if ( stat == STAT_CON )
				return 20;
		}
		else if ( GET_CLASS( this ) == CLASS_THIEF )
		{
			if ( stat == STAT_DEX )
				return 20;
		}
		else if ( GET_CLASS( this ) == CLASS_CHANNELER )
		{
			if ( stat == STAT_WIS || stat == STAT_INT )
				return 20;
			else if ( stat == STAT_STR )
				return 16;
			else if ( stat == STAT_DEX )
				return 18;
			else if ( stat == STAT_CON )
				return 16;
		}
		else
		{
			MudLog( BRF, LVL_APPR, TRUE,
			        "Invalid class type of %d(?) in StatMaximum() for %s.", stat, this->player.name.c_str() );
			return 0;
		}
		return 19;
	}
	else
	{
		if ( GET_CLASS( this ) == CLASS_WARRIOR )
		{
			if ( stat == STAT_STR )
				return 21;
		}
		else if ( GET_CLASS( this ) == CLASS_RANGER )
		{
			if ( stat == STAT_CON )
				return 20;
		}
		else if ( GET_CLASS( this ) == CLASS_THIEF )
		{
			if ( stat == STAT_DEX )
				return 20;
		}
		else if( GET_CLASS( this ) == CLASS_DREADGUARD )
		{
			if ( stat == STAT_WIS || stat == STAT_INT )
				return 20;
			else if ( stat == STAT_STR )
				return 16;
			else if ( stat == STAT_DEX )
				return 18;
			else if ( stat == STAT_CON )
				return 16;
		}
		else
		{
			MudLog( BRF, LVL_APPR, TRUE,
			        "Invalid class type of %d(?) in StatMaximum() for %s.", stat, this->player.name.c_str() );
			return 0;
		}
		if ( stat == STAT_INT )
			return 19;
		if ( stat == STAT_WIS )
			return 19;
		if ( stat == STAT_STR )
			return 20;	//Darkside strength specialization
		return 19;
	}

	MudLog( BRF, LVL_APPR, TRUE, "%s managed to not return a stat maximum with a class of %s, stat type %d.",
	        GET_NAME( this ), pc_class_types[ ( int ) GET_CLASS( this ) ], stat );
	return 0;
}

int Character::StatMinimum( int stat )
{
	if ( GET_RACE( this ) != RACE_TROLLOC )
	{
		if ( stat == STAT_INT )
			return 12;
		if ( stat == STAT_WIS )
			return 12;
		return 10;
	}
	else
	{
		return 10;
	}

	MudLog( BRF, LVL_APPR, TRUE, "%s managed to not return a stat minimum with a class of %s, stat type %d.",
	        GET_NAME( this ), pc_class_types[ ( int ) GET_CLASS( this ) ], stat );
	return 0;
}

int StatCost[ NUM_CLASSES ][ 6 ] =
    {
        //S,  I,  W,     D,  C
        { 10, 20, 20, 0, 30, 20},  //Warriors
        { 20, 10, 20, 0, 10, 20},  //Thieves
        { 20, 15, 20, 0, 20, 10},  //Rangers
        { 30, 10, 10, 0, 20, 30},  //Channelers
        { 30, 10, 10, 0, 20, 30},  //Fade
        { 30, 10, 10, 0, 20, 30},  //Dreadlord
        { 30, 10, 10, 0, 20, 30},  //Blademaster
        { 30, 10, 10, 0, 20, 30},  //Greyman
        { 30, 10, 10, 0, 20, 30},  //Draghkar
        { 30, 10, 10, 0, 20, 30},  //Dreadguard
		{ 10, 20, 20, 0, 30, 20}   //Ogier
    };

int Character::CostPerStat( int stat )
{
	if ( GET_CLASS( this ) <= CLASS_DREADGUARD )
		return StatCost[ ( int ) GET_CLASS( this ) ][ stat ];
	else
		return 0;
}

int Character::RollHealth( int level )
{
	int num = 0;

	if ( IS_WARRIOR( this ) && level <= 30 )
		++num;

	if ( level == 1 )
	{
		num += MiscUtil::random( 20, 30 );
		num += GetCon() / 2;
	}
	else if ( level <= 30 )
		num += MiscUtil::random( GetCon() - 8, GetCon() - 6 );
	else
		num += MiscUtil::random( 1, 2 );

	return num;
}

int Character::RollMana( int level )
{
	if ( level > 1 && level <= 30 && this->ChannelingAbility())
		return MiscUtil::random( GetWis() / 6, GetWis() / 4 );
	else if ( level == 1 )
		return 100;

	return 0;
}

void Character::ResetHitRolls( bool pts )
{
	int c = 0, roll = 0;
	unsigned int i;

	this->points.HitRolls.resize( GET_LEVEL( this ) );

	if ( !( this->points.HitRolls.size() ) )
		return ;

	if ( pts )
		GET_MAX_HIT( this ) = 0;

	for ( i = 0; i < this->points.HitRolls.size(); ++i )
	{
		roll = this->RollHealth( i + 1 );
		c += roll;
		this->points.HitRolls[ i ] = roll;
	}

	if ( !pts )
	{
		c -= GET_MAX_HIT( this );
		this->points.HitRolls[ 0 ] -= c;
	}
	else
		GET_MAX_HIT( this ) = c;

	GET_HIT( this ) = MIN( GET_HIT( this ), GET_MAX_HIT( this ) );
}

void Character::ResetManaRolls( bool pts )
{
	int c = 0, roll = 0;
	unsigned int i;

	this->points.ManaRolls.resize( GET_LEVEL( this ) );

	if ( !( this->points.ManaRolls.size() ) )
		return ;

	if ( pts )
		GET_MAX_MANA( this ) = 0;

	for ( i = 0; i < this->points.ManaRolls.size(); ++i )
	{
		roll = this->RollMana( i + 1 );
		c += roll;
		this->points.ManaRolls[ i ] = roll;
	}

	if ( !pts )
	{
		c -= GET_MAX_MANA( this );
		this->points.ManaRolls[ 0 ] -= c;
	}
	else
		GET_MAX_MANA( this ) = c;
}

void Character::RollStats()
{}

/*
 * This function controls the change to maxmove, maxmana, and maxhp for
 * each class every time they gain a level.
 */
void Character::GainLevel( int show )
{
	int i, tmp;

	if ( ( GET_LEVEL( this ) - 1 ) >= ( int ) this->points.HitRolls.size() )
	{
		tmp = this->RollHealth( GET_LEVEL( this ) );
		this->points.HitRolls.resize( GET_LEVEL( this ) );
		this->points.HitRolls[ GET_LEVEL( this ) - 1 ] = tmp;
	}
	else
		tmp = this->points.HitRolls[ GET_LEVEL( this ) - 1 ];

	GET_MAX_HIT( this ) += tmp;

	if ( ( GET_LEVEL( this ) - 1 ) >= ( int ) this->points.ManaRolls.size() )
	{
		tmp = this->RollMana( GET_LEVEL( this ) );
		this->points.ManaRolls.resize( GET_LEVEL( this ) );
		this->points.ManaRolls[ GET_LEVEL( this ) - 1 ] = tmp;
	}
	else
		tmp = this->points.ManaRolls[ GET_LEVEL( this ) - 1 ];

	GET_MAX_MANA( this ) += tmp;
	this->PlayerData->skillpracs += this->PracticeGain( GET_LEVEL( this ) );
	this->PlayerData->spellpracs += this->SPracticeGain(GET_LEVEL( this ) );
	this->PlayerData->tradepracs += this->TPracticeGain(GET_LEVEL( this ) );

	if ( GET_LEVEL( this ) >= LVL_IMMORT )
	{
		for ( i = 0; i < 3; ++i )
			this->PlayerData->conditions[ i ] = ( char ) - 1;

		SET_BIT_AR( PRF_FLAGS( this ), PRF_HOLYLIGHT );
	}

	if ( GET_LEVEL( this ) == 5 && !PRF_FLAGGED( this, PRF_STATTED ) )
	{
		SET_BIT_AR( PRF_FLAGS( this ), PRF_STATTED );
		this->RollMoves();
		this->body_structure = MiscUtil::random( STRUCT_LIGHT, STRUCT_HEAVY );
		this->RollWeightAndHeight();
		if( IS_FADE( this ) )
			this->RollShadow();

		this->NewbieTip( "Your skills have reset! You may now practice different"
		                 " skills at the trainers. This is the only time this will happen for free." );
		this->ResetAllSkills();
		StatManager::GetManager().RollStats( this );
		this->Save();//Important enough that we save for this big of an event.

		//	GET_LUC(this) = MiscUtil::random(0, 20);


		GET_MAX_HIT(this) = 0;
		GET_MAX_MANA(this) = 0;
		this->points.HitRolls.resize(5);
		this->points.ManaRolls.resize(5);

		for(i = 1;i < 5;++i)
		{
			tmp = this->RollHealth(i);
			GET_MAX_HIT(this) += tmp;
			this->points.HitRolls[i - 1] = tmp;

			tmp = this->RollMana(i);
			GET_MAX_MANA(this) += tmp;
			this->points.ManaRolls[i - 1] = tmp;
		}
	}

	check_autowiz( this );

	if ( show )
		MudLog( CMP, MAX( LVL_GOD, GET_INVIS_LEV( this ) ), TRUE, "%s advanced to level %d", GET_NAME( this ), GET_LEVEL( this ) );
}

void Character::DropLevel()
{
	if((int)this->points.HitRolls.size() >= GET_LEVEL(this))
		GET_MAX_HIT( this ) -= this->points.HitRolls[ GET_LEVEL( this ) - 1 ];
	if((int)this->points.ManaRolls.size() >= GET_LEVEL(this))
		GET_MAX_MANA( this ) -= this->points.ManaRolls[ GET_LEVEL( this ) - 1 ];

	this->PlayerData->skillpracs -= this->PracticeGain( GET_LEVEL( this ) );
	this->PlayerData->spellpracs -= this->SPracticeGain( GET_LEVEL( this ) );
	this->PlayerData->tradepracs -= this->TPracticeGain(GET_LEVEL( this ) );

	--GET_LEVEL( this );
	check_autowiz( this );
}

int inches( int centimeters )
{
	return ( ( int ) ( centimeters / 2.54 ) );
}
int feet( int centimeters )
{
	return inches( centimeters ) / 12;
}
int centimeters( int inches )
{
	return ( ( int ) ( inches * 2.54 ) );
}

/*
 * This is the exp given to implementors -- it must always be greater
 * than the exp required for immortality, plus at least 20,000 or so.
 */
const int EXP_MAX = 10000000;

/* Function to return the exp required for each class/level */
int level_exp( int level )
{
	switch ( level )
	{
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 2000;
	case 3:
		return 5000;
	case 4:
		return 10000;
	case 5:
		return 20000;
	case 6:
		return 40000;
	case 7:
		return 70000;
	case 8:
		return 100000;
	case 9:
		return 150000;
	case 10:
		return 225000;
	case 11:
		return 325000;
	case 12:
		return 525000;
	case 13:
		return 825000;
	case 14:
		return 1225000;
	case 15:
		return 1725000;
	case 16:
		return 2325000;
	case 17:
		return 3025000;
	case 18:
		return 3825000;
	case 19:
		return 4725000;
	case 20:
		return 5725000;
	case 21:
		return 7000000;
	case 22:
		return 9000000;
	case 23:
		return 12000000;
	case 24:
		return 16000000;
	case 25:
		return 21000000;
	case 26:
		return 27000000;
	case 27:
		return 34000000;
	case 28:
		return 42000000;
	case 29:
		return 51000000;
	case 30:
		return 61000000;
	case 31:
		return 71000000;
	case 32:
		return 81000000;
	case 33:
		return 91000000;
	case 34:
		return 101000000;
	case 35:
		return 116000000;
	case 36:
		return 131000000;
	case 37:
		return 146000000;
	case 38:
		return 161000000;
	case 39:
		return 176000000;
	case 40:
		return 196000000;
	case 41:
		return 216000000;
	case 42:
		return 236000000;
	case 43:
		return 256000000;
	case 44:
		return 276000000;
	case 45:
		return 296000000;
	case 46:
		return 316000000;
	case 47:
		return 336000000;
	case 48:
		return 356000000;
	case 49:
		return 376000000;
	case 50:
		return 396000000;
	default:
		return 999999999;
	}

	/*
	 * This statement should never be reached if the exp tables in this function
	 * are set up properly.  If you see exp of 123456 then the tables above are
	 * incomplete -- so, complete them!
	 */
	Log("SYSERR: XP tables not set up correctly in class.cpp!" );
	return 123456;
}

/* If this person is in an Aes Sedai clan, return true */
bool Character::AES_SEDAI()
{
	if ( this->getUserClan( 17 ) || this->getUserClan( 18 ) || this->getUserClan( 19 ) || this->getUserClan( 20 ) || this->getUserClan( 21 ) ||
	        this->getUserClan( 22 ) || this->getUserClan( 23 ) )
		return true;

	return false;
}

int BodyMassIndex( int w, int h )
{
	float result = ( float ) w / ( ( float ) h * ( float ) h );
	return ( int ) ( result * 703 );
}
int Character::BMI()
{
	return BodyMassIndex( GET_WEIGHT( this ), GET_HEIGHT( this ) );
}

bool Character::ChannelingAbility()
{
	if ( IS_DREADLORD( this ) || IS_CHANNELER( this ) || IS_DREADGUARD( this ) )
		return true;
	return false;
}

std::string Character::HeightToFeetAndInches()
{
	char str[ 1024 ];

	sprintf( str, "%d feet, %d inches", GET_HEIGHT( this ) / 12, GET_HEIGHT( this ) % 12 );

	return str;
}

void Character::RollWeightAndHeight()
{
	int inches = 0;
	float pounds = 0.0f;

	//HEIGHT
	switch ( this->player.race )
	{
			case RACE_HUMAN:
			case RACE_SEANCHAN:
			inches	= MiscUtil::random( 56, 78 );
			break;
			case RACE_TROLLOC:
			case RACE_AIEL:
			inches	= MiscUtil::random( 73, 88 );
			break;
	}

	if ( GET_SEX( this ) == SEX_FEMALE )
		inches -= MiscUtil::random( 0, 4 );
	GET_HEIGHT( this ) = inches;

	for ( ;inches;--inches )
	{
		if ( IS_TROLLOC( this ) && !IS_DREADLORD( this ) && !IS_DREADGUARD(this) )
		{
			if ( this->body_structure == STRUCT_LIGHT )
				pounds += ( ( float ) MiscUtil::random( 250, 300 ) / 100 );
			else if ( this->body_structure == STRUCT_MEDIUM )
				pounds += ( ( float ) MiscUtil::random( 300, 350 ) / 100 );
			else if ( this->body_structure == STRUCT_HEAVY )
				pounds += ( ( float ) MiscUtil::random( 350, 400 ) / 100 );
			else
			{
				this->body_structure = MiscUtil::random( STRUCT_LIGHT, STRUCT_HEAVY );
				++inches;
			}
		}
		else
		{
			if ( this->body_structure == STRUCT_LIGHT )
				pounds += ( ( float ) MiscUtil::random( 180, 230 ) / 100 );
			else if ( this->body_structure == STRUCT_MEDIUM )
				pounds += ( ( float ) MiscUtil::random( 230, 280 ) / 100 );
			else if ( this->body_structure == STRUCT_HEAVY )
				pounds += ( ( float ) MiscUtil::random( 280, 330 ) / 100 );
			else
			{
				this->body_structure = MiscUtil::random( STRUCT_LIGHT, STRUCT_HEAVY );
				++inches;
			}
		}
	}

	GET_WEIGHT( this ) = ( int ) pounds;
}


int Character::GetSkillLevel( int skill )
{
	return WeaveManager::GetManager().GetWeave(skill)->isElement() ? (GET_SKILL(this, skill)) : (GET_SKILL( this, skill ) / 12);
}

Room *Character::StartRoom()
{
	if ( GET_LEVEL( this ) >= LVL_IMMORT )
		return Conf->room_nums.immortal_start.ptr;
	else if ( PLR_FLAGGED( this, PLR_FROZEN ) )
		return Conf->room_nums.frozen_start.ptr;
	else if ( IS_HUMAN( this ) )
		return Conf->room_nums.human_start.ptr;
	else if ( IS_TROLLOC( this ) )
		return Conf->room_nums.trolloc_start.ptr;
	else if ( IS_SEANCHAN( this ) )
		return Conf->room_nums.seanchan_start.ptr;
	else if ( IS_AIEL( this ) )
		return Conf->room_nums.aiel_start.ptr;
	else
		return World[ 0 ];
}

Room *Character::ArenaStartRoom()
{
	// Get the room vnum. Vnums are stored in the array arenaStartRooms in script -150. 
	int vnum = JS_getArenaStartRoomVnum( ( JS_isArenaInFFAMode() ? GET_RACE(this) : ( getArenaTeam( this ) - 1 ) ) );

	Room *room = FindRoomByVnum(vnum);

	if(room == NULL) {

		room = FindRoomByVnum(500);
	}

	return room;
}

bool Character::CanTaint()
{
	if ( !( IS_DREADLORD( this ) || GET_SEX( this ) == SEX_FEMALE || PLR_FLAGGED( this, PLR_DARKFRIEND ) || IS_DREADGUARD(this) ) )
		return true;
	return false;
}

sh_int Character::GetTotalTaint()
{
	return (this->points.taint + this->points.temp_taint);
}

void Character::RandomTaintSocial()
{
	int size = 0, social = 0;
	std::string RandomSocials[] =
	    {
	        "frolic",
	        "giggle",
	        "snicker",
	        "smile",
	        "cackle",
	        "cough",
	        ""
	    };

	for ( size = 0;!RandomSocials[ size ].empty();++size )
		;

	if ( this->GetTotalTaint() >= TaintData.TaintForRandomSocial )
	{
		social = MiscUtil::random( 0, size );
		this->InterpCommand( "%s", RandomSocials[ social ].c_str() );
	}
}

void Character::RandomTaintVoices()
{
	if(this->GetTotalTaint() < TaintData.TaintForRandomVoice)
		return;

	int voice = 0, count = 0;

	std::map<std::string, short int> RandomVoices;

	RandomVoices["You hear something grumbling inside of your head.\r\n"] = 0;
	RandomVoices["A voice in your head screams at you.\r\n"] = 0;
	RandomVoices["A voice in your head begins weeping painfully.\r\n"] = 0;
	RandomVoices["You giggle as a voice in your head tells a joke.\r\n"] = 0;
	RandomVoices["A voice in your head begins laughing insanely.\r\n"] = 0;
	RandomVoices["A voice quietly whispers, 'Why do we keep running from our fate?'\r\n"] = 0;
	RandomVoices["A voice says to you, 'We are the destroyers of the world.'\r\n"] = 0;
	RandomVoices["A voice asks you, 'Do we no longer feel guilt for our evil deeds?'\r\n"] = 0;
	RandomVoices["A voice screams, 'KILL! KILL THEM ALL!'.\r\n"] = POS_FIGHTING;
	RandomVoices["You hear the voice in your head ranting about death and pain.\r\n"] = POS_FIGHTING;
	RandomVoices["The voice in your head begins weeping from the sight of battle.\r\n"] = POS_FIGHTING;
	RandomVoices["A voice in your head whispers, 'We all die someday.'\r\n"] = POS_FIGHTING;
	RandomVoices["A voice in your head asks you, 'Is this the day we perish?'\r\n"] = POS_FIGHTING;
	RandomVoices["A voice in your head begins screaming madly about revenge.\r\n"] = POS_FIGHTING;

	std::map<std::string, short int>::iterator Msg;
	do
	{
		++count;
		voice = MiscUtil::random( 0, RandomVoices.size()-1 );
		for( Msg = RandomVoices.begin();voice;++Msg, --voice );
	}
	while ( count < 1000 && !( GET_POS( this ) != (*Msg).second || (*Msg).second ) );
	this->Send( ((*Msg).first).c_str() );
}

void Character::ShowZones( Character *ch )
{
	int i = 0, n = 0;
	Zone *zone;

	for ( i = 0;(zone = ZoneManager::GetManager().GetZoneByRnum(i));++i )
	{
		if ( ( isname( this->player.name, zone->GetBuilders() ) ) )
		{
			if ( !n )
			{
				ch->Send( "%s is working on the following zones:\r\n\n", GET_NAME( this ) );
				ch->Send( "Vnum              Name\r\n" );
				ch->Send( "--------------------------------------------\r\n" );
			}
			ch->Send( "%-5d      %s%s%s%s\r\n", zone->getVnum(), COLOR_BOLD( ch, CL_COMPLETE ),
			          COLOR_GREEN( ch, CL_COMPLETE ), zone->getName().c_str(),
			          COLOR_NORMAL( ch, CL_COMPLETE ) );

			++n;
		}
	}

	if ( !n )
		ch->Send( "%s is not currently assigned to any zones.\r\n", GET_NAME( this ) );
}

byte RaceByString( const std::string &str)
{
    for(int i = 0;*pc_race_types[i] != '\n';++i)
    {
        if(!str_cmp(str, pc_race_types[i]))
            return i;
    }
    return RACE_UNDEFINED;
}

byte ClassByString( const std::string &str)
{
    for(int i = 0;*pc_class_types[i] != '\n';++i)
    {
        if(!str_cmp(str, pc_class_types[i]))
            return i;
    }
    return CLASS_UNDEFINED;
}

byte SexByString( const std::string &str)
{
    for(int i = 0;i < NUM_GENDERS;++i)
    {
        if(!str_cmp(str, genders[i]))
            return i;
    }
    return SEX_UNDEFINED;
}

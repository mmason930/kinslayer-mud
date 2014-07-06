/* ************************************************************************
*   File: fight.c                                       Part of CircleMUD *
*  Usage: Combat system                                                   *
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
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "screen.h"
#include "clans.h"
#include "weaves.h"
#include "mobs.h"
#include "fight.h"
#include "dg_event.h"
#include "js.h"

#include "MiscUtil.h"
#include "ClanUtil.h"
#include "DatabaseUtil.h"
#include "MiscUtil.h"
#include "rooms/Room.h"
#include "rooms/Exit.h"

Clock individualHitClockDamage;
#define PERFORM_VIOLENCE_LOGGING 0

/* Structures */
Character *combat_list = NULL;	/* head of l-list of fighting chars */
Character *next_combat_list = NULL;

/* External structures */
extern struct GameTime time_info;

extern struct MeleeMessageList fight_messages[ MAX_MESSAGES ];
extern Object *object_list;
extern int auto_save;		/* see config.c -- not used in this file */
extern int max_exp_gain;	/* see config.c */
extern int max_exp_loss;	/* see config.c */
extern const char *findSuffix( int number );
extern const char *month_name[];
extern Room *FindTargetRoom( Character * ch, char * rawroomstr );
extern const char* body_parts[];

int move_damage( Character *ch, Character *vict, int low, int high );
int mana_damage( Character *ch, Character *vict, int damage );

/* External procedures */
char *fread_action( FILE * fl, int nr );
ACMD( do_flee );
int RandomBodyPart();
/* local functions */
void check_fighting( void );
void perform_group_gain( Character * ch, int base, Character * victim );
void dam_message( int dam, Character * ch, Character * victim, int w_type, int BodyPart );
void load_messages( void );
void group_gain( Character * ch, Character * victim );
void solo_gain( Character * ch, Character * victim );
char *replace_string( const char *str, const char *weapon_singular, const char *weapon_plural, int BodyPart );
void perform_violence( void );
void gain_wp( Character *ch, Character *vict );
void check_legend( Character *ch );
int level_exp( int level );

EVENT(BashStand);

//TODO: Turn into method.
bool JS_isArenaZone(int zoneVnum);

bool isInArena(Character *ch)
{
	if(ch && ch->in_room && JS_isArenaZone(ch->in_room->getZone()->getVnum()))
		return true;

	return false;
}

/************ PK Manager & Co *************/
PKManager *PKManager::Self = (NULL);

PKManager::PKManager()
{
	// We need to determine the current top kill ID
	sql::Query query;
	std::string topKillIdString;
	try
	{
		topKillIdString = DatabaseUtil::getSingleResultFromQuery(gameDatabase, "SELECT MAX(kill_id) FROM userPlayerKill");
	}
	catch(sql::QueryException e)
	{
		e.report();
		MudLog(BRF, TRUE, LVL_APPR, "Unable to grab top kill ID upon PKManager construction.");
		return;
	}
	
	if(topKillIdString == "NULL")
		this->nextKillID = 0;
	else
		this->nextKillID = MiscUtil::Convert<int>(topKillIdString);
}
PKManager::~PKManager() {}

PKManager &PKManager::GetManager()
{
	if( Self == NULL ) Self = new PKManager();
	return (*Self);
}
void PKManager::Free()
{
	if( Self != NULL ) delete (Self);
}

void PKManager::RegisterKill( Character *Killer, Character *Victim, const unsigned int &wtrans, const time_t &when, const int kill_id )
{
	std::stringstream QueryText;

	QueryText	<< " INSERT INTO userPlayerKill("
				<< "   `kill_id`,"
				<< "   `killer_id`,"
				<< "   `victim_id`,"
				<< "   `wp_transfer`,"
				<< "   `time_of_death`"
				<< " ) VALUES ("
				<< kill_id << ","
				<< Killer->player.idnum << ","
				<< Victim->player.idnum << ","
				<< wtrans << ","
				<< sql::encodeQuoteDate(when)
				<< ")";

	try {
		gameDatabase->sendRawQuery( QueryText.str() );
	} catch(sql::QueryException e) {
		e.report();
		MudLog(BRF, MAX(LVL_APPR,MAX(GET_INVIS_LEV(Killer),GET_INVIS_LEV(Victim))), TRUE,
			"Unable to add player kill. Killer: %s, Victim: %s : %s", GET_NAME(Killer),GET_NAME(Victim),
			e.getMessage().c_str());
	}
}
const unsigned int PKManager::GetNextKillID()
{
	return (++nextKillID);
}
const unsigned int PKManager::GetCurrKillID()
{
	return (nextKillID);
}

/************ End PKManager & Co ************/

/* Galnor 2005-02-10
Update the list by creating a temporary list to put into order by greatest to least,
then converting it back to the standard list.
*/

void UpdateLegend()
{//TODO: Move to script
	std::list<Legend *> TempList;
	std::list<Legend *>::iterator iter, l;
	bool added = false;
	int i;

	//Begin by going through the list and ordering it onto a temporary list.
	for ( l = Legends.begin();l != Legends.end(); )
	{
		added = false;
		for ( i = 0, iter = TempList.begin();iter != TempList.end() && i < 16;iter++, ++i )
		{
			if ( ( *l )->wp > ( *iter )->wp )
			{
				added = true;
				TempList.insert( iter, ( *l ) );
				l = Legends.erase(l);
				break;
			}
		}
		if ( !added && i < 16 )
		{
			TempList.push_back( ( *l ) );
			l = Legends.erase(l);
		}
		else if( added == false )
			l++;
	}

	//Galnor - 04/27/2009 - This was a huge memory leak...
	while( !Legends.empty() )
	{
		delete Legends.front();
		Legends.pop_front();
	}

	Legends = TempList;
}

//Returns true if this is allowed to be listed on the Legend list
bool Character::CanBeLegend()
{//TODO: Move to script
	if ( GET_LEVEL( this ) >= LVL_IMMORT || PLR_FLAGGED( this, PLR_NO_WEAVE ) || !this->points.weave )
		return false;
	return true;
}

void Character::UpdateLegendEntry()
{//TODO: Move to script
	std::list<Legend *>::iterator entry;
	//Begin by removing all entries for this player

	for ( entry = Legends.begin();entry != Legends.end(); )
	{
		if ( !str_cmp( ( *entry ) ->name, this->player.name ) )
		{
			delete ( *entry );
			entry = Legends.erase( entry );
		} else {
			++entry;
		}
	}

	if ( !this->CanBeLegend() )
		return ;

	Legend *new_entry = new Legend();
	new_entry->name = this->player.name;
	new_entry->wp = this->points.weave;

	Legends.push_front( new_entry );

	UpdateLegend();
	if( this->LoggedIn() )
		this->save();
}
void Character::InterruptTimer()
{	
	char dComm[MAX_INPUT_LENGTH];
	if( !this->delayed_command.empty() )
		strcpy(dComm, this->delayed_command.c_str());
	if( strlen(dComm) > 0 )
	{
		char *s = strtok( dComm, "'");
		s = strtok( s, "'" );
		int cost = 0;
		if( WeaveManager::GetManager().GetWeave(s) && WeaveManager::GetManager().GetWeave(s)->hasAttribute("Cost"))
		{
			cost = MiscUtil::Convert<int>(WeaveManager::GetManager().GetWeave(s)->getAttribute("Cost"));
			cost = cost * 2 / 3;
			if( cost > 0)
				GET_MANA(this) = MAX(0, GET_MANA(this) - cost);
		}
	}
	this->CancelTimer( false );

	//if ( this->ShieldBlock )
	//{
	//	this->ShieldBlock = false;
	//	act("You lower your shield, having failed to anticipate an attack.", FALSE, this, NULL, NULL, TO_CHAR);
	//	act("$n lowers $s shield, becoming vulnerable once more.", FALSE, this, NULL, NULL, TO_ROOM);
	//	WAIT_STATE(this, PULSE_VIOLENCE / 2);
	//}
	//else
		this->send( "You are interrupted and stop what you are doing.\r\n" );
}

void update_slew( Character *ch, Character *victim )
{//TODO: Rewrite, possibly ditch slew variable
	const char * suf = findSuffix( time_info.day + 1 );

	sprintf( GET_SLEW_MESSAGE( ch ), "Slew %s on the %d%s Day of the %s, Year %d.",
	          GET_NAME( victim ), time_info.day + 1, suf, month_name[ ( int ) time_info.month ], time_info.year );
}

//GetWeaveLoss & gainWeave re-written by Galnor: 2006-11-17
//weaveGroupDistribution also newly added on the same date.
//Re-wrote to better organize the system of Weave Point loss & gain.
sh_int Character::GetWeaveLoss()
{
	return (GET_WP( this ) / 5);
}
sh_int Character::GetSamesideWeaveLoss()
{
	return (GET_WP( this ) / 10);
}
void Character::gainWeave( sh_int wp )
{
	this->points.weave += wp;
	this->send("Your fame around the world has increased.\r\n");
}
bool Character::canGainWeave( Character* victim )
{
	if( !victim || victim == this || this->player.level >= LVL_IMMORT || PLR_FLAGGED(this, PLR_NO_WEAVE)

	|| PLR_FLAGGED(victim, PLR_NO_WEAVE) || IS_NPC(this) || IS_NPC(victim) /*||
	GET_RACE(this) == GET_RACE(victim)*/)
		return false;
	return true;
}

void Character::weaveGroupDistribution( Character* victim )
{
	int min_weave_gain = 5;
	int min_sameside_weave_gain = 3;
	int wp = 0;
	if(!this->canGainWeave( victim ))
		return;

	if(GET_RACE(this) == GET_RACE(victim))
		wp = victim->GetSamesideWeaveLoss();
	else
		wp = victim->GetWeaveLoss();

	int num_followers = 1;
	Character* leader = (this->master && AFF_FLAGGED(this, AFF_GROUP)) ? this->master : this;
	Follower* f;

	victim->points.weave -= wp * Conf->play.WpLossMultiplier;
	victim->send("Your fame has been diminished.\r\n");

	if(victim->player.level >= 30)
	{
		if (GET_RACE(this) == GET_RACE(victim))
			wp += min_sameside_weave_gain; //smaller contribution for same race kills
		else
			wp += min_weave_gain;//leader's contribution
		for(f = leader->followers;f;f = f->next)
		{
			//Only those in the room for the kill qualify for distribution gain.
			if(f->follower->in_room == this->in_room && AFF_FLAGGED(f->follower, AFF_GROUP)
			&& f->follower->canGainWeave( victim ))
			{
				++num_followers;
				if(GET_RACE(this) == GET_RACE(victim))
					wp += min_sameside_weave_gain;
				else
					wp += min_weave_gain;//follower's contribution
			}
		}
	}

	wp *= Conf->play.WpGainMultiplier;		//increase wps based on the multiplier

	//The reason we don't simply do the weave gaining in the loop above is that
	//we may want to add more weave point bonuses later on. We can just do all of
	//that above here and take care of distribution on the lower half instead of
	//confusingly integrating it in the wp gain calculations above. We also had to
	//figure out how many people were legitely grouped first in order to determine
	//the distribution factor.

	//wp now has all the weave points which will be distributed to the group.
	bool bNextPlayerKillIDObtained=false;
	if(leader->in_room->getZoneNumber() == this->in_room->getZoneNumber())
	{
		leader->gainWeave( wp / num_followers );
		leader->UpdateLegendEntry();
		update_slew(leader, victim);

		/* Player Kills occur when weave points have been rewarded */
		PKManager::GetManager().RegisterKill(leader,victim,(wp/num_followers),time(0),PKManager::GetManager().GetNextKillID());
		bNextPlayerKillIDObtained=true;
	}
	for(f = leader->followers;f;f = f->next)
	{
		if (f->follower->in_room->getZoneNumber() == this->in_room->getZoneNumber() && AFF_FLAGGED(f->follower, AFF_GROUP)
			&& f->follower->canGainWeave( victim ))
		{
			f->follower->gainWeave( wp / num_followers );
			f->follower->UpdateLegendEntry();
			update_slew(f->follower, victim);

			if( bNextPlayerKillIDObtained == false ) {
				PKManager::GetManager().GetNextKillID();
				bNextPlayerKillIDObtained = true;
			}
			PKManager::GetManager().RegisterKill(f->follower,victim,(wp/num_followers),time(0),
				PKManager::GetManager().GetCurrKillID());
		}
	}

	victim->UpdateLegendEntry();
}

/* Weapon attack texts */
struct attack_hit_type attack_hit_text[] =
    {//TODO: Database
	    {"hit", "hits"
	    }
	    , 			/* 0 */
	    {"sting", "stings"},
	    {"whip", "whips"},
	    {"slash", "slashes"},
	    {"bite", "bites"},
	    {"bludgeon", "bludgeons"}, 		/* 5 */
	    {"crush", "crushes"},
	    {"pound", "pounds"},
	    {"claw", "claws"},
	    {"maul", "mauls"},
	    {"thrash", "thrashes"}, 		/* 10 */
	    {"pierce", "pierces"},
	    {"blast", "blasts"},
	    {"punch", "punches"},
	    {"stab", "stabs"},
	    {"smash", "smashes"}, 			/* 15 */
	    {"strike", "strikes"},
	    {"hack", "hacks"},
	    {"smite", "smites"},
	    {"slice", "slices"},
	    {"cleave", "cleaves"}, 			/* 20 */
	    {"chop", "chops"},
	    {"clash", "clashes"},
	    {"flail", "flails"},
	    {"clobber", "clobbers"},
	    {"clout", "clouts"}, 			/* 25 */
	    {"swat", "swats"},
	    {"thump", "thumps"},
	    {"whack", "whacks"},
	    {"glance", "glances"},
	    {"jab", "jabs"}, 			/* 30 */
	    {"prick", "pricks"},
	    {"penetrate", "penetrates"},
	    {"lacerate", "lacerates"},
		{"slit","slits"}				/* 34 */
    };

#define IS_WEAPON(type) (((type) >= TYPE_HIT) && ((type) < TYPE_SUFFERING))

/* The Fight related routines */

void Character::Appear()
{
	REMOVE_BIT_AR( AFF_FLAGS( this ), AFF_INVISIBLE );

	if ( GET_LEVEL( this ) < LVL_IMMORT )
		Act( "A bright flash of light appears as $n steps in.", FALSE, this, 0, 0, TO_ROOM );
}

void load_messages( void )
{
	sql::Query query = gameDatabase->sendQuery("SELECT * FROM skillMessage");

	int fightMessageIndex = 0;

	for ( fightMessageIndex = 0; fightMessageIndex < MAX_MESSAGES; ++fightMessageIndex )
	{
		fight_messages[ fightMessageIndex ].a_type = 0;
		fight_messages[ fightMessageIndex ].number_of_attacks = 0;
		fight_messages[ fightMessageIndex ].msg = 0;
	}

	fightMessageIndex = 0;

	while(query->hasNextRow()) {

		sql::Row row = query->getRow();

		fight_messages[fightMessageIndex].a_type = row.getInt("type");
		++fight_messages[fightMessageIndex].number_of_attacks;

		MeleeMessageType *meleeMessageType = new MeleeMessageType();

		meleeMessageType->die_msg.attacker_msg = str_dup(row.getString("die_attacker_text").c_str());
		meleeMessageType->die_msg.room_msg = str_dup(row.getString("die_room_text").c_str());
		meleeMessageType->die_msg.victim_msg = str_dup(row.getString("die_victim_text").c_str());
		
		meleeMessageType->miss_msg.attacker_msg = str_dup(row.getString("miss_attacker_text").c_str());
		meleeMessageType->miss_msg.room_msg = str_dup(row.getString("miss_room_text").c_str());
		meleeMessageType->miss_msg.victim_msg = str_dup(row.getString("miss_victim_text").c_str());

		meleeMessageType->hit_msg.attacker_msg = str_dup(row.getString("hit_attacker_text").c_str());
		meleeMessageType->hit_msg.room_msg = str_dup(row.getString("hit_room_text").c_str());
		meleeMessageType->hit_msg.victim_msg = str_dup(row.getString("hit_victim_text").c_str());
		
		meleeMessageType->god_msg.attacker_msg = str_dup(row.getString("god_attacker_text").c_str());
		meleeMessageType->god_msg.room_msg = str_dup(row.getString("god_room_text").c_str());
		meleeMessageType->god_msg.victim_msg = str_dup(row.getString("god_victim_text").c_str());

		fight_messages[fightMessageIndex].msg = meleeMessageType;

		++fightMessageIndex;
	}
}

void update_pos( Character * victim )
{//TODO: Purge
	if ( GET_HIT( victim ) > 0 )
	{
		if ( !victim->points.is_bashed )
		{
			if ( FIGHTING( victim ) )
				GET_POS( victim ) = POS_FIGHTING;
			else if( IS_NPC(victim) )
				GET_POS( victim ) = victim->MobData->default_pos;
			else
				GET_POS( victim ) = POS_STANDING;
		}
	}
	else if ( GET_HIT( victim ) <= -3 )
		GET_POS( victim ) = POS_DEAD;
	else if ( GET_HIT( victim ) <= -2 )
		GET_POS( victim ) = POS_MORTALLYW;
	else if ( GET_HIT( victim ) <= -1 )
		GET_POS( victim ) = POS_INCAP;
	else
		GET_POS( victim ) = POS_STUNNED;
}

/* start one char fighting another (yes, it is horrible, I know... )  */
void Character::SetFighting( Character *vict )
{//TODO: Rewrite(redo combat list)
	if ( !vict || this == vict || this->in_room != vict->in_room )
		return ;

	if ( FIGHTING( this ) )
	{
		core_dump();
		return ;
	}

	next_fighting = combat_list;
	combat_list = this;

	if ( AFF_FLAGGED( this, AFF_SLEEP ) )
		affect_from_char( this, SPELL_SLEEP, 0 );
	if(IS_NPC(this))
	{
		if ( MOB_FLAGGED( this, MOB_MEMORY ) )
			this->Remember( vict );
		if ( MOB_FLAGGED( this, MOB_TRACK ) )
			this->player.hunting = vict;
	}
	FIGHTING( this ) = vict;
	if ( !IS_BASHED( this ) )
		GET_POS( this ) = POS_FIGHTING;

	if ( !FIGHTING( vict ) )
		vict->SetFighting( this );
}

/* remove a char from the list of fighting chars */
void Character::StopFighting()
{
	Character *temp;

	if ( this == next_combat_list )
		next_combat_list = next_fighting;

	REMOVE_FROM_LIST( this, combat_list, next_fighting );
	next_fighting = NULL;

	temp = FIGHTING( this );
	FIGHTING( this ) = NULL;
//	if ( temp && FIGHTING( temp ) == this )
//		temp->StopFighting();

	update_pos( this );
}

float Object::BashRating()
{
	return	(	((GetTotalOffense() * BashData.O_OBMultiplier) +
				(GetTotalWeight() * BashData.O_WeaponWeightMultiplier)) *
				(  ((float)GetTotalVal4() / 100) )
			);
}

void Character::SetNewFighting()
{//Todo: Review
	Character *vict;

	if(FIGHTING(this))
		this->StopFighting();
	for(vict = this->in_room->people;vict;vict = vict->next_in_room)
	{
		if(FIGHTING(vict) == this)
		{
			this->SetFighting(vict);
			break;
		}
	}
}

Clock corpseClock1, corpseClock2, corpseClock3, corpseClock4, corpseClock5;
void Character::MakeCorpse()
{//TODO: Review
	Object *corpse, *o, *obj;
	int i, x, y;
	const char *type = " ";

	if ( MOB_FLAGGED( this, MOB_GHOST ) )
		return ;//No corpse for ghosts

	if ( !desc && !IS_NPC( this ) )
		type = " torn up ";

	corpseClock1.turnOn();
	corpse = create_obj();
	corpseClock1.turnOff();

	corpse->item_number = NOTHING;
	corpse->in_room = 0;
	corpse->name = str_dup( "corpse" );

	sprintf( buf2, "The%scorpse of %s is lying here.", type, GET_NAME( this ) );
	corpse->description = str_dup( buf2 );

	sprintf( buf2, "the corpse of %s", GET_NAME( this ) );
	corpse->short_description = str_dup( buf2 );
	sprintf( buf2, "%s", GET_NAME( this ) );

	corpse->scalp = new ScalpData;

	if ( !IS_NPC( this ) )
		corpse->scalp->player_scalp = true;
	corpse->scalp->is_scalp = (false);

	if ( this->GetPrototype() != NULL && this->GetPrototype()->MobData->Food )
	{
		corpse->scalp->Food				= new FoodUnit;
		corpse->scalp->Food->quantity	= this->GetPrototype()->MobData->Food->quantity;
		corpse->scalp->Food->vnum		= this->GetPrototype()->MobData->Food->vnum;
	}
	else
		corpse->scalp->Food = NULL;

	if( this->GetPrototype() != NULL && this->GetPrototype()->MobData->Skin )
	{
		corpse->scalp->Skin             = new SkinningUnit;
		corpse->scalp->Skin->required   = this->GetPrototype()->MobData->Skin->required;
		corpse->scalp->Skin->vnum       = this->GetPrototype()->MobData->Skin->vnum;
		corpse->scalp->Skin->skinned    - this->GetPrototype()->MobData->Skin->skinned;
	}
	else
		corpse->scalp->Skin = NULL;

	if(this->GetPrototype() != NULL && GET_RACE(GetPrototype()) != RACE_OTHER) {

		corpse->scalp->blood = MAX(70,GET_WEIGHT(GetPrototype()) / 5);
	}

	corpse->scalp->race		= this->player.race;
	corpse->scalp->name		= GET_NAME( this );
	corpse->scalp->level	= IS_NPC( this ) ? 30 : GET_LEVEL( this );
	corpse->scalp->scalped	= (false);

	corpse->setType(ITEM_CONTAINER);
	GET_OBJ_EXTRA(corpse) = 0;

	corpse->obj_flags.wear_flags = 0;

	SET_BITK( GET_OBJ_WEAR( corpse ), ITEM_WEAR_TAKE );
	GET_OBJ_VAL( corpse, 0 ) = 0;	/* You can't store stuff in a corpse */
	GET_OBJ_VAL( corpse, 3 ) = 1;	/* corpse identifier */
	GET_OBJ_WEIGHT( corpse ) = GET_WEIGHT( this ) + this->CarriedWeight();
	GET_OBJ_RENT( corpse ) = this->points.weave * this->player.level;
	corpse->Money	= this->points.gold;
	this->points.gold	= 0;

	if ( IS_NPC( this ) )
		GET_OBJ_TIMER( corpse ) = CONFIG_MAX_NPC_CORPSE_TIME;
	else
		GET_OBJ_TIMER( corpse ) = CONFIG_MAX_PC_CORPSE_TIME;

	/* transfer character's inventory to the corpse */
	corpse->contains = carrying;

	for ( o = corpse->contains; o != NULL; o = o->next_content )
		o->in_obj = corpse;

	corpseClock2.turnOn();
	object_list_new_owner( corpse, NULL );
	corpseClock2.turnOff();

	corpseClock4.turnOn();
	/* transfer character's equipment to the corpse */
	for ( i = 0; i < NUM_WEARS;++i )
	{
		if ( GET_EQ( this, i ) )
		{
			obj = GET_EQ( this, i );
			obj_to_obj( unequip_char( this, i ), corpse );
		}
	}
	corpseClock4.turnOff();

	carrying = NULL;
	IS_CARRYING_N( this ) = 0;

	corpseClock5.turnOn();
	corpse->MoveToRoom( in_room );
	corpseClock5.turnOff();
	if(!ROOM_FLAGGED(in_room, ROOM_VAULT))
	{
		corpseClock3.turnOn();
		in_room->corpseSave();
		corpseClock3.turnOff();
	}
}

void Character::DeathCry()
{//TODO: Move to script
	int door;
	Room *was_in;

	Act( "Your blood freezes as you hear $n's death cry.", FALSE, this, 0, 0, TO_ROOM );
	was_in = this->in_room;

	for ( door = 0; door < NUM_OF_DIRS;++door )
	{
		if ( EXIT( this, door ) )
		{
			if ( CAN_GO( this, door ) )
			{
				in_room = was_in->dir_option[ door ] ->getToRoom();
				Act( "Your blood freezes as you hear someone's death cry.", FALSE, this, 0, 0, TO_ROOM );
				this->in_room = was_in;
			}
		}
	}
}

Clock dieClock1, dieClock2, dieClock3, dieClock4, dieClock5;
Clock dieClock6, dieClock7, dieClock8, dieClock9, dieClock10;
Clock dieClock11, dieClock12, dieClock13, dieClock14, dieClock15;
Clock dieClock16;

Clock dieClock14A, dieClock14B, dieClock14C;
void Character::Die( Character *killer )
{//TODO: Review

	if(killer == NULL)
		killer = this;

	if(killer != NULL)
	{
		dieClock1.turnOn();
		if ( (killer != this) && (IS_NPC(this) || this->desc) && !isInArena(this) && !isInArena(killer) )
		{
			if ( AFF_FLAGGED( killer, AFF_GROUP ) )
				group_gain( killer, this );
			else
				solo_gain( killer, this );
		}
		dieClock1.turnOff();
		dieClock2.turnOn();
		if ( !IS_NPC( this ) )
		{
			MudLog( BRF, (MAX(LVL_IMMORT,GET_LEVEL(this))), TRUE, "%s killed by %s at %s", GET_NAME( this ), GET_NAME( killer ), this->in_room->getName() );
			if ( MOB_FLAGGED( killer, MOB_MEMORY ) )
				killer->Forget(this);
		}
		dieClock2.turnOff();
		dieClock3.turnOn();
		if(!isInArena(this) && !isInArena(killer)) {

			killer->weaveGroupDistribution( this );
		}

		if(!isInArena(this)) {

			if(IS_NPC(killer) ) {

				gain_exp( this, -( level_exp( this->player.level ) - level_exp( this->player.level - 1 ) ) / 2 );
			}

			while( !IS_NPC( this ) && GET_EXP( this ) < level_exp( GET_LEVEL( this ) ) )
			{
				if ( GET_LEVEL( this ) <= 1 || GET_LEVEL( this ) >= LVL_IMMORT )
					break;

				this->DropLevel();
				this->send( "Ugh! You lost a level!\r\n" );
				MudLog( NRM, LVL_GRGOD, TRUE, "%s dropped to level %d.", GET_NAME( this ), GET_LEVEL( this ) );
			}
		}
		dieClock3.turnOff();
		dieClock4.turnOn();
		if ( !IS_NPC( killer ) && ( GET_LEVEL( this ) < LVL_IMMORT ) && ( killer != this ) && !isInArena(this))
			GET_DEATH_WAIT( this ) = 10;
		else
			GET_DEATH_WAIT( this ) = 0;	//Reset if somehow redied, clear up possible screwup

		if(!isInArena(this)) {
			if ( !IS_NPC( killer ) )
			{
				if ( GET_LEVEL( this ) >= 20 && !IS_NPC( this ) )
					++killer->pks;
				++this->pk_deaths;
			}
			else
				++this->mob_deaths;
		}

		if(!isInArena(this)) {
			//Added by Seronis in 2004.
			killer->kill_list[ GET_NAME( this ) ] ++;//Trophy incrementation by name
		}

		killer->StopFighting();
		dieClock4.turnOff();
	}//END OF if(killer != NULL)
	dieClock5.turnOn();
	js_death_trigger( this, killer );
	dieClock5.turnOff();

	dieClock6.turnOn();
	while ( this->affected )
		affect_remove( this, this->affected );
	dieClock6.turnOff();

	dieClock7.turnOn();
	if ( FIGHTING( this ) && FIGHTING( FIGHTING( this ) ) )
		FIGHTING( this ) ->StopFighting();
	dieClock7.turnOff();

	dieClock8.turnOn();
	this->CancelTimer( false );
	dieClock8.turnOff();

	dieClock9.turnOn();
	this->Dismount();
	dieClock9.turnOff();

	dieClock10.turnOn();
	this->StopFighting();
	dieClock10.turnOff();

	dieClock11.turnOn();
	this->DeathCry();
	dieClock11.turnOff();

	dieClock12.turnOn();
	this->RemoveSource();
	dieClock12.turnOff();

	dieClock13.turnOn();
	ShieldManager::GetManager().KillShieldsByTarget(this);
	dieClock13.turnOff();

	dieClock14.turnOn();
	dieClock14A.turnOn();
	this->send( "You are dead! Sorry...\r\n" );

	if ( MOB_FLAGGED( this, MOB_GHOST ) )
		Act( "$n howls in pain and slowly fades away.", TRUE, this, 0, this, TO_NOTVICT );
	else
		Act( "$n is dead! R.I.P.", FALSE, this, 0, 0, TO_ROOM );

	dieClock14A.turnOff();
	if(!isInArena(this)) {
		dieClock14B.turnOn();
		this->MakeCorpse();
		dieClock14B.turnOff();
		dieClock14C.turnOn();
		this->itemSave();
		dieClock14C.turnOff();
	}
	dieClock14.turnOff();

	if ( IS_NPC( this ) )
	{
		dieClock15.turnOn();
		this->StopFighting();
		this->Extract();
		dieClock15.turnOff();
		return;
	}
	else
	{
		dieClock16.turnOn();
		this->PlayerData->mood = MOOD_WIMPY;
		GET_HIT( this ) = 1;
		GET_MOVE( this ) = 1;
		GET_MANA( this ) = 1;
		GET_POS( this ) = POS_RESTING;
		FLEE_LAG( this ) = 0;

		if( !isInArena(this) ) {
			this->RemoveFromRoom();
			this->MoveToRoom( this->StartRoom() );
		}
		else {
			this->RemoveFromRoom();
			this->MoveToRoom( this->ArenaStartRoom() );
		}
		
		look_at_room( this, 0 );

		if ( GET_LEVEL( this ) <= 5 )//Allow those level 5 or under to request another kit.
			REMOVE_BIT_AR( PRF_FLAGS( this ), PRF_KIT );

		Act( "$n suddenly arrives from the midst of the air.", FALSE, this, 0, 0, TO_ROOM );
		if( !isInArena(this) )
			this->send( "\nYou feel completely drained of all of your strength..." );

		this->save();
		dieClock16.turnOff();
	}
}

void perform_group_gain( Character *ch, int base, Character *victim )
{//TODO: Rewrite
	int share;

	share = MIN( max_exp_gain, MAX( 1, base ) );
	ch->send( "You receive your share of experience.\r\n" );

	gain_exp( ch, share );
}

void group_gain( Character * ch, Character * victim )
{//TODO: Rewrite
	int tot_members, base, tot_gain;
	Character *k;
	struct Follower *f;

	if ( !( k = ch->master ) )
		k = ch;

	if ( AFF_FLAGGED( k, AFF_GROUP ) && ( k->in_room == ch->in_room ) )
		tot_members = 1;
	else
		tot_members = 0;

	for ( f = k->followers; f; f = f->next )
	{
		if ( AFF_FLAGGED( f->follower, AFF_GROUP ) && f->follower->in_room == ch->in_room )
		{
			++tot_members;
		}
	}

	//Total exp gained
	if ( tot_members == 2 )
		tot_gain = GET_EXP( victim );
	else
		tot_gain = ( GET_EXP( victim ) / ( ( tot_members != 1 ? tot_members : 2 ) - 1 ) );

	/* prevent illegal xp creation when killing players */
	if ( !IS_NPC( victim ) )
		tot_gain = MIN( GET_LEVEL( victim ) * 10000, tot_gain );

	base = MAX( 1, tot_gain / tot_members );

	if ( AFF_FLAGGED( k, AFF_GROUP ) && k->in_room == ch->in_room )
		perform_group_gain( k, base, victim );

	for ( f = k->followers; f; f = f->next )
	{
		if ( AFF_FLAGGED( f->follower, AFF_GROUP ) && f->follower->in_room == ch->in_room )
		{
			perform_group_gain( f->follower, base, victim );
		}
	}
}

void solo_gain( Character * ch, Character * victim )
{//TODO: Rewrite
	int exp;

	if ( GET_LEVEL( ch ) >= 20 )
		exp = MIN( max_exp_gain, ( int ) ( GET_EXP( victim ) * .7 ) );
	else
		exp = MIN( max_exp_gain, GET_EXP( victim ) );
	/* Calculate level-difference bonus */
	if ( IS_NPC( ch ) )
		exp += MAX( 0, ( exp * MIN( 4, ( GET_LEVEL( victim ) - GET_LEVEL( ch ) ) ) ) / 8 );
	else
		exp += MAX( 0, ( exp * MIN( 8, ( GET_LEVEL( victim ) - GET_LEVEL( ch ) ) ) ) / 8 );

	exp = MAX( exp, 1 );

	sprintf( buf2, "You receive some experience points!\r\n" );
	ch->send( buf2 );

	if ( GET_RACE( ch ) == GET_RACE( victim ) && !IS_NPC( victim ) )
		gain_exp( ch, 1000 );
	else
		gain_exp( ch, exp );

}

char *replace_string( const char *str, const char *weapon_singular, const char *weapon_plural, int BodyPart )
{//TODO: Review
	static char buf[ 256 ];
	char *cp = buf;
	std::string BodyString = body_parts[ BodyPart ];
	int i = 0;

	for ( ; *str; str++ )
	{
		if ( *str == '#' )
		{
			switch ( *( ++str ) )
			{
					case 'W':
					for ( ; *weapon_plural; *( cp++ ) = *( weapon_plural++ ) )
						;
					break;
					case 'w':
					for ( ; *weapon_singular; *( cp++ ) = *( weapon_singular++ ) )
						;
					break;
					case 'b':
					for ( i = 0; i < ( int ) BodyString.size();*( cp++ ) = BodyString[ i++ ] )
						;
					break;
					default:
					*( cp++ ) = '#';
					break;
			}
		}

		else
			*( cp++ ) = *str;

		*cp = 0;
	}				/* For */

	return ( buf );
}

/* message for doing damage with a weapon */
void dam_message( int dam, Character * ch, Character * victim, int w_type, int BodyPart )
{//TODO: Rewrite
	char * buf;
	int msgnum;

	static struct dam_weapon_type
	{
		const char *to_room;
		const char *to_char;
		const char *to_victim;
	}
	dam_weapons[] =
	    {

	        /* use #w for singular (i.e. "slash") and #W for plural (i.e. "slashes") */

	        {
	            "$n tries to #w $N's #b, but misses.", 	/* 0: 0     */
	            "You try to #w $N's #b, but miss.",
	            "$n tries to #w your #b, but misses."
	        },

	        {
	            "$n barely tickles $N's #b with $s #w,", 	/* 7: 19..23 */
	            "You barely tickle $N's #b with your #w.",
	            "$n barely tickles your #b with $s #w."
	        },

	        {
	            "$n tickles $N's #b as $e #W $M.", 	/* 1: 1..2  */
	            "You tickle $N's #b as you #w $M.",
	            "$n tickles your #b as $e #W you."
	        },

	        {
	            "$n barely #W $N's #b.", 		/* 2: 3..4  */
	            "You barely #w $N's #b.",
	            "$n barely #W your #b."
	        },

	        {
	            "$n #W $N's #b.", 			/* 3: 5..6  */
	            "You #w $N's #b.",
	            "$n #W your #b."
	        },

	        {
	            "$n #W $N's #b hard.", 			/* 4: 7..10  */
	            "You #w $N's #b hard.",
	            "$n #W your #b hard."
	        },

	        {
	            "$n #W $N's #b very hard.", 		/* 5: 11..14  */
	            "You #w $N's #b very hard.",
	            "$n #W your #b very hard."
	        },

	        {
	            "$n #W $N's #b extremely hard.", 	/* 6: 15..19  */
	            "You #w $N's #b extremely hard.",
	            "$n #W your #b extremely hard."
	        },


	        {
	            "$n #W $N's #b into bloody fragments!", 	/* 8: > 23   */
	            "You #w $N's #b into bloody fragments!",
	            "$n #W your #b into bloody fragments!"
	        }
	    };


	w_type -= TYPE_HIT;		/* Change to base of table with text */

	if ( dam == 0 )
		msgnum = 0;
	else if ( dam <= 2 )
		msgnum = 1;
	else if ( dam <= 4 )
		msgnum = 2;
	else if ( dam <= 6 )
		msgnum = 3;
	else if ( dam <= 10 )
		msgnum = 4;
	else if ( dam <= 14 )
		msgnum = 5;
	else if ( dam <= 19 )
		msgnum = 6;
	else if ( dam <= 23 )
		msgnum = 7;
	else
		msgnum = 8;

	/* damage message to onlookers */
	buf = replace_string( dam_weapons[ msgnum ].to_room,
	                      attack_hit_text[ w_type ].singular, attack_hit_text[ w_type ].plural, BodyPart );
	Act( buf, FALSE, ch, NULL, victim, TO_NOTVICT, NULL, true );

	/* damage message to damager */
	ch->send( COLOR_GREEN( ch, CL_COMPLETE ) );
	buf = replace_string( dam_weapons[ msgnum ].to_char,
	                      attack_hit_text[ w_type ].singular, attack_hit_text[ w_type ].plural, BodyPart );
	Act( buf, FALSE, ch, NULL, victim, TO_CHAR, COLOR_GREEN(ch, CL_COMPLETE), true );
	ch->send( COLOR_NORMAL( ch, CL_COMPLETE ) );

	/* damage message to damagee */
	victim->send( (msgnum == 0 ? "" : COLOR_RED( victim, CL_COMPLETE )) );
	buf = replace_string( dam_weapons[ msgnum ].to_victim,
	                      attack_hit_text[ w_type ].singular, attack_hit_text[ w_type ].plural, BodyPart );
	Act( buf, FALSE, ch, NULL, victim, TO_VICT | TO_SLEEP, (msgnum == 0 ? "" : COLOR_RED( victim, CL_COMPLETE )), true );
	victim->send( COLOR_NORMAL( victim, CL_COMPLETE ) );
}

/*
 * message for doing damage with a spell or skill
 *  C3.0: Also used for weapon damage on miss and death blows
 */
// Small fixes by Serai for "dodge/parry" and multiple messages sometimes.
int skill_message( int dam, Character * ch, Character * vict, int attacktype, int BodyPart )
{//TODO: Rewrite
	int i, j, nr, db, pb;
	struct MeleeMessageType *msg;
	Object *weap = GET_EQ( ch, WEAR_WIELD );
	char mesg[ 1024 ], *temp;

	for ( i = 0; (size_t)i < MAX_MESSAGES; i++ )
	{
		if ( fight_messages[ i ].a_type == attacktype )
		{
			nr = MiscUtil::dice( 1, fight_messages[ i ].number_of_attacks );

			for ( j = 1, msg = fight_messages[ i ].msg; ( j < nr ) && msg; j++ )
				msg = msg->next;

			if ( !IS_NPC( vict ) && ( GET_LEVEL( vict ) >= LVL_IMMORT ) )
			{
				Act( msg->god_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR );
				Act( msg->god_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT );
				Act( msg->god_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT );

				return ( 1 );
			}

			else if ( dam != 0 )
			{
				/*
				 * Don't send redundant color codes for TYPE_SUFFERING & other types
				 * of damage without attacker_msg.
				 */


				if ( GET_POS( vict ) == POS_DEAD )
				{
					if ( msg->die_msg.attacker_msg )
					{
						ch->send( COLOR_GREEN( ch, CL_COMPLETE ) );
						Act( msg->die_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR,COLOR_GREEN( ch, CL_COMPLETE ) );
						ch->send( COLOR_NORMAL( ch, CL_COMPLETE ) );
					}

					vict->send( COLOR_RED( vict, CL_COMPLETE ) );
					Act( msg->die_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP, COLOR_RED( vict, CL_COMPLETE ) );
					vict->send( COLOR_NORMAL( vict, CL_COMPLETE ) );

					Act( msg->die_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT );
				}


				else
				{
					if ( msg->hit_msg.attacker_msg )
					{
						ch->send( COLOR_GREEN( ch, CL_COMPLETE ) );
						Act( msg->hit_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR, COLOR_GREEN( ch, CL_COMPLETE ) );
						ch->send( COLOR_NORMAL( ch, CL_COMPLETE ) );
					}

					vict->send( COLOR_RED( vict, CL_COMPLETE ) );
					Act( msg->hit_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP, COLOR_RED( vict, CL_COMPLETE ) );
					vict->send( COLOR_NORMAL( vict, CL_COMPLETE ) );

					Act( msg->hit_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT );
				}
			}

			else if ( ch != vict )
			{	/* Dam == 0 */
				if ( IS_WEAPON( attacktype ) )
				{
					attacktype -= TYPE_HIT;

					pb = vict->Parry();
					db = vict->Dodge();
					nr = MiscUtil::random( 0, ( pb + db ) );
					temp = replace_string( "you try to #w $N's #b, but $E %s the attack.",
					                       attack_hit_text[ attacktype ].singular, attack_hit_text[ attacktype ].plural, BodyPart );
					sprintf( mesg, temp, ( nr >= pb ? "dodges" : "parries" ) );
					Act( mesg, FALSE, ch, weap, vict, TO_CHAR );

					temp = replace_string( "$n tries to #w your #b, but you %s the attack.",
					                       attack_hit_text[ attacktype ].singular, attack_hit_text[ attacktype ].plural, BodyPart );
					sprintf( mesg, temp, ( nr >= pb ? "dodge" : "parry" ) );
					Act( mesg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP );

					temp = replace_string( "$n tries to #w $N's #b, but $E %s the attack.",
					                       attack_hit_text[ attacktype ].singular, attack_hit_text[ attacktype ].plural, BodyPart );
					sprintf( mesg, temp, ( nr >= pb ? "dodges" : "parries" ) );
					Act( mesg, FALSE, ch, weap, vict, TO_NOTVICT );
				}

				else
				{
					if ( msg->miss_msg.attacker_msg )
					{
						ch->send( COLOR_GREEN( ch, CL_COMPLETE ) );
						Act( msg->miss_msg.attacker_msg, FALSE, ch, weap, vict, TO_CHAR, COLOR_GREEN( ch, CL_COMPLETE ) );
						ch->send( COLOR_NORMAL( ch, CL_COMPLETE ) );
					}

					vict->send( COLOR_RED( vict, CL_COMPLETE ) );

					Act( msg->miss_msg.victim_msg, FALSE, ch, weap, vict, TO_VICT | TO_SLEEP, COLOR_RED( vict, CL_COMPLETE ) );
					vict->send( COLOR_NORMAL( vict, CL_COMPLETE ) );

					Act( msg->miss_msg.room_msg, FALSE, ch, weap, vict, TO_NOTVICT );
				}

				return ( 1 );
			}
		}
	}

	return ( 0 );
}

int move_damage( Character *ch, Character *vict, int low, int high )
{//TODO: Rewrite

	if ( !vict || !ch )
		return 0;

	GET_MOVE( vict ) -= MiscUtil::random( low, high );

	if ( GET_MOVE( vict ) < 0 )
		GET_MOVE( vict ) = 0;

	return 1;
}

int mana_damage( Character *ch, Character *vict, int damage )
{//TODO: Rewrite

	if ( !vict || !ch)
		return 0;

	GET_MANA( vict ) -= damage;

	if ( GET_MANA( vict ) < 0 )
		GET_MANA( vict ) = 0;

	return damage;
}

/*
 * Alert: As of bpl14, this function returns the following codes:
 *	< 0	Victim died.
 *	= 0	No damage.
 *	> 0	How much damage done.
 */
Clock damageClock1, damageClock2, damageClock3, damageClock4, damageClock5, damageClock6, damageClock7;
int damage( Character * ch, Character * victim, int dam, int attacktype, int BodyPart )
{//TODO: Needs rewrite
	if ( GET_POS( victim ) <= POS_DEAD )
	{
		Log("SYSERR: Attempt to damage corpse '%s' in room #%d by '%s'.", GET_NAME(victim), victim->in_room->getVnum(), GET_NAME(ch));
		victim->Die( ch );
		return 0;			// -je, 7/7/92
	}

	// peaceful rooms
	if ( ch != victim && ROOM_FLAGGED( ch->in_room, ROOM_PEACEFUL ) )
	{
		ch->send( "This room just has such a peaceful, easy feeling...\r\n" );
		return 0;
	}

	// You can't damage an immortal!
	if ( !IS_NPC( victim ) && ( GET_LEVEL( victim ) >= LVL_IMMORT ) )
		dam = 0;

	damageClock1.turnOn();
	if ( victim != ch && attacktype != SPELL_DECAY && attacktype != SPELL_BLAZE && attacktype != SPELL_DARK_PLAGUE && attacktype != SKILL_MISC)
	{
		// Start the attacker fighting the victim
		if ( GET_POS( ch ) > POS_STUNNED && !FIGHTING( ch ) && attacktype != SKILL_COMPEL )
			ch->SetFighting( victim );

		// Start the victim fighting the attacker
		if ( GET_POS( victim ) > POS_STUNNED && !FIGHTING( victim ) && attacktype != SKILL_COMPEL )
			victim->SetFighting( ch );
	}
	damageClock1.turnOff();

	damageClock2.turnOn();
	// If you attack a pet, it hates your guts
	if ( victim->master == ch && attacktype != SKILL_COMPEL)
		victim->stopFollowing();

	// If the attacker is invisible, he becomes visible
	if ( AFF_FLAGGED( ch, AFF_INVISIBLE ) )
		ch->Appear();

	// Cut damage if victim has sanct, to a minimum 1
	if ( AFF_FLAGGED( victim, AFF_SANCTUARY ) && dam >= 2 )
		dam = ( int ) ( ( double ) dam * atof(WeaveManager::GetManager().GetWeave("Sanctuary")->getAttribute("DmgFactor").c_str()) );
	damageClock2.turnOff();

	damageClock3.turnOn();
	// Fades and Ogier take less damage from weaves
	if ( attacktype < 130 && IS_FADE(ch) )
	{
		dam *= .8;
	}

	if (attacktype < 130 && IS_OGIER(ch) )
	{
		dam *= (100 - ch->Absorb() * .4) / 100;
	}

	if ( IS_FADE(ch) && GET_SKILL(ch, SKILL_SHADOW_RAGE) && AFF_FLAGGED(ch, AFF_SHADOW_RAGE) )
		dam = (dam + 1) * .6;
	
	// Set the maximum damage per round and subtract the hit points
	dam = MAX( MIN( dam, 5000 ), 0 );
	GET_HIT( victim ) -= dam;

	// Gain exp for the hit

	if ( ch != victim )
	{
		int max_hit_exp = ( level_exp( GET_LEVEL( ch ) + 1 ) - level_exp( GET_LEVEL( ch ) ) ) / MAX( 1, Conf->play.max_hit_exp );
		gain_exp( ch, MIN( max_hit_exp, GET_LEVEL( victim ) * dam * Conf->play.hit_exp_mult ) );
	}
	update_pos( victim );
	damageClock3.turnOff();

	damageClock4.turnOn();
	if ( dam > 0 )
	{
		if ( (victim->timer > 0 && victim->desc && ( !str_cmp( victim->Command, "channel" )
		        || !str_cmp( victim->Command, "backstab" ) || !str_cmp( victim->Command, "charge" ) 
				|| !str_cmp( victim->Command, "skewer" ) ) ) && attacktype != SPELL_BLAZE )
			victim->InterruptTimer();
		if ( dam > 0 )
			REMOVE_BIT_AR( AFF_FLAGS( victim ), AFF_HIDE );
	}

	if ( attacktype == SKILL_PARRY && dam > 0)
	{
		victim->setBashState( PULSE_VIOLENCE );
	}

	if ( attacktype == SKILL_BASH && dam > 0 )
	{
		victim->setBashState( PULSE_VIOLENCE * 2 );
	}

	if ( attacktype == SKILL_PULVERIZE && dam > 0)
	{
		victim->setBashState( PULSE_VIOLENCE );
	}
	damageClock4.turnOff();
	/*
	 * skill_message sends a message from the messages file in lib/misc.
	 * dam_message just sends a generic "You hit $n extremely hard.".
	 * skill_message is preferable to dam_message because it is more
	 * descriptive.
	 *
	 * If we are _not_ attacking with a weapon (i.e. a spell), always use
	 * skill_message. If we are attacking with a weapon: If this is a miss or a
	 * death blow, send a skill_message if one exists; if not, default to a
	 * dam_message. Otherwise, always send a dam_message.
	 */

	damageClock5.turnOn();
	if( attacktype == SKILL_MISC )
	{//Raw damage - Galnor 12/19/2009
		//...
	}
	else if ( !IS_WEAPON( attacktype ) )
	{
		if( (attacktype < 130) && WeaveManager::GetManager().GetWeave(attacktype)->invertable() && ch->InvertNextWeave)
			DisplayWeaveMessages(WeaveManager::GetManager().GetWeave(attacktype), ch, victim);
		else
			skill_message( dam, ch, victim, attacktype, BodyPart );
	}
	else
	{
		if ( GET_POS( victim ) == POS_DEAD || dam == 0 )
		{
			if ( !skill_message( dam, ch, victim, attacktype, BodyPart ) )
				dam_message( dam, ch, victim, attacktype, BodyPart );
		}
		else
		{
			dam_message( dam, ch, victim, attacktype, BodyPart );
		}
	}
	damageClock5.turnOff();
	// Use send() Act() doesn't send message if you are DEAD.

	damageClock6.turnOn();
	switch ( GET_POS( victim ) )
	{
	case POS_MORTALLYW:
		Act( "$n is mortally wounded, and will die soon, if not aided.", TRUE, victim, 0, 0, TO_ROOM );
		victim->send( "You are mortally wounded, and will die soon, if not aided.\r\n" );
		break;
	case POS_INCAP:
		Act( "$n is incapacitated and will slowly die, if not aided.", TRUE, victim, 0, 0, TO_ROOM );
		victim->send( "You are incapacitated an will slowly die, if not aided.\r\n" );
		break;
	case POS_STUNNED:
		Act( "$n is stunned, but will probably regain consciousness again.", TRUE, victim, 0, 0, TO_ROOM );
		victim->send( "You're stunned, but will probably regain consciousness again.\r\n" );
		break;
	default: 			// >= POSITION SLEEPING
		if ( dam > ( GET_MAX_HIT( victim ) / 4 ) )
		{
			Act( "That really did HURT!", FALSE, victim, 0, 0, TO_CHAR );
			Act( "$n grimaces in pain.", FALSE, victim, 0, 0, TO_ROOM );
		}
		if ( GET_HIT( victim ) < ( GET_MAX_HIT( victim ) / 4 ) )
		{
			if ( dam > 0 )
			{
				victim->send( "You wish that your wounds would stop BLEEDING so much!\r\n" );
			}

			if ( ch != victim && MOB_FLAGGED( victim, MOB_WIMPY ) )
				do_flee( victim, NULL, 0, 0 );
		}
		if ( !IS_NPC( victim ) && victim->PlayerData->wimp_level && ( victim != ch ) &&
		        GET_HIT( victim ) < victim->PlayerData->wimp_level && GET_HIT( victim ) > 0 )
		{
			victim->send( "You wimp out, and attempt to flee!\r\n" );
			do_flee( victim, NULL, 0, 0 );
		}
		break;
	}

	// stop someone from fighting if they're stunned or worse
	if ( ( GET_POS( victim ) == POS_DEAD && FIGHTING( victim ) ) )
		victim->StopFighting();
	damageClock6.turnOff();
	// Uh oh. Victim died.
	if ( GET_POS( victim ) == POS_DEAD )
	{
		damageClock7.turnOn();
		if( attacktype == SPELL_DECAY )
			victim->Die( victim->DecayedBy ? (victim->DecayedBy) : (ch) );
		else if( attacktype == SPELL_BLAZE )
			victim->Die( victim->BurnedBy ? (victim->BurnedBy) : (ch) );
		else if( attacktype == SPELL_DARK_PLAGUE )
			victim->Die( victim->PlaguedBy ? (victim->PlaguedBy) : (ch) );
		else
			victim->Die( ch );
		damageClock7.turnOff();
		return -1;

	}

	return dam;
}

int Character::CalculateDamage( Character *victim, int Type, int BodyPart )
{
	int dam = 0;
	Object *wielded = GET_EQ( this, WEAR_WIELD );
	float ABS = 0.00f;

	/*This was moved up, and is returned here so that abs does not affect damage on a stab */
	if ( Type == SKILL_BACKSTAB )
	{
		dam = MiscUtil::random( wielded->GetTotalVal5() , wielded->GetTotalVal6() );
		//dam += MiscUtil::random( 1, GET_LEVEL( this ) );

		if ( IS_OBJ_STAT( wielded, ITEM_WARD ) )
			dam = int( float( dam ) * MeleeData.WardMultiplier );

		if ( !AFF_FLAGGED(this, AFF_HIDE) )
			dam /= 2;

		dam -= ( ( 20 - this->GetDex() ) * 25 );
	}

	else if ( Type == SKILL_CHARGE || Type == SKILL_SKEWER )
	{
		dam = MiscUtil::random( wielded->GetTotalVal8(), wielded->GetTotalVal9() );

		if ( IS_OBJ_STAT( wielded, ITEM_WARD ) )
			dam = int( float( dam ) * MeleeData.WardMultiplier );

		dam -= ( ( 20 - this->GetStr() ) *  25);
	}

	else if ( Type == SKILL_BASH || Type == SKILL_PARRY || Type == SKILL_HAMSTRING )
	{
		return 1;
	}

	else if ( Type == SKILL_KICK )
	{
		return (GET_LEVEL(this) / 4);
	}

	//	Choke
	else if ( Type == SKILL_BAREHANDS )
	{
		return MiscUtil::random(5, 15);
	}

	else
	{
		/* Start with the damage bonuses: the damroll and strength apply */
		dam += GET_DAMROLL( this );

		if ( wielded )
		{
			int min = wielded->GetTotalVal1();
			int max = wielded->GetTotalVal2();

			if ( this->isInClan( GET_OBJ_CLAN( wielded ) ) )
			{
				min += GET_OBJ_CL_DMG1( wielded );
				max += GET_OBJ_CL_DMG2( wielded );
			}
			dam += MiscUtil::random( min, max );

			if ( IS_OBJ_STAT( wielded, ITEM_WARD ) )
				dam = int( float( dam ) * MeleeData.WardMultiplier );
		}
		else
		{
			/* If no weapon, add bare hand damage instead */
			if ( IS_NPC( this ) )
			{
				dam += MiscUtil::random( this->MobData->damnodice, this->MobData->damsizedice );
			}
			else
			{
				dam += MiscUtil::random( 0, GetStr() / 4 );	/* Max 2 bare hand damage for players */
			}
		}

		// Boost up damage for Warders VS trollocs
		if ( IS_WARDER( this ) && IS_TROLLOC( victim ) )
			dam = ( int ) ( dam * 1.2 );

		if ( GET_EQ( victim, BodyPart ) )
		{
			ABS = ( float ) GET_EQ( victim, BodyPart )->GetTotalAbsorb() / 100.00f;
			if( IS_OGIER(this) )
				ABS += .05;
		}
		dam -= int( float( dam ) * ABS );

		/* at least 1 hp damage min per hit */
		dam = MAX( 1, dam );
	}

	/* Damage multiplier */
	if ( GET_POS( victim ) <= POS_STUNNED )
		dam *= 5;	//Give them a hard finishing hit
	else if ( GET_POS( victim ) < POS_FIGHTING )
		dam = ( int ) ( ( float ) dam * ( 1.0f + (float)( POS_FIGHTING - GET_POS( victim ) ) * 0.5f ) );
	return dam;
}

int RandomBodyPart()
{//TODO: Revise
	int p = 0, rand = MiscUtil::random( 1, 100 ), count = 0;

	for ( count = 0, p = 0;p < ( int ) BodyPercents.size();++p )
	{
		count += BodyPercents[ p ];

		if ( rand <= count )
			return p;
	}
	MudLog( BRF, LVL_APPR, TRUE, "Hrm! Error in RandomBodyParts(), random number of %d!", rand );
	return 0;
}

int Character::OffenseMeleeRoll()
{
	int ob = 0;
	ob = MiscUtil::random( (int) (this->Offense() * 0.5f), (int)(this->Offense() * 1.5f) );

	return ob;
}

int Character::DefenseMeleeRoll()
{
	int pb = this->Parry(), i = 0;
	Character *temp;

	for ( i = 0, temp = this->in_room->people;temp;temp = temp->next_in_room )
	{
		if ( FIGHTING( temp ) == this && GET_POS( temp ) == POS_FIGHTING )
		{
			if ( i )
				if (IS_BLADEMASTER(this))
					pb = ( int ) ( pb * (MeleeData.ParrySplit + 0.05) );
				else
					pb = ( int ) ( pb * MeleeData.ParrySplit );
			i = 1;
		}
	}
	return (int)( (pb * MeleeData.ParryMultiplier) + (this->Dodge() * MeleeData.DodgeMultiplier) );
}

void Character::GainNoQuit( Character *victim )
{//TODO: Revise

	int noquit = 0;
	affected_type af, *a;

	if ( IS_NPC( victim ) || IS_NPC( this ) )
		noquit = 1;
	else
		noquit = 5;

	af.type = SKILL_SNEAK;
	af.duration = noquit;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_NOQUIT;

	for ( a = this->affected;a;a = a->next )
	{
		if ( a->bitvector== AFF_NOQUIT )
		{
			a->duration = MAX( a->duration, noquit );
			return ;
		}
	}
	if ( !a )
		affect_to_char( this, &af );
}

int hit( Character * ch, Character * victim, int type )
{//TODO: Rewrite
	Object * wielded = GET_EQ( ch, WEAR_WIELD );
	int finalDamageCalculation;

	int w_type, dam, bp = ( ( ch->ps_tgt != -1 ) ? ch->ps_tgt : RandomBodyPart() );

	if ( !ch || !victim )
		return 0;

	if ( ( ( victim->NumFighting() >= 4 && !IS_NPC( ch ) ) ||
	        ( ROOM_FLAGGED( ch->in_room, ROOM_TUNNEL ) && victim->NumFighting() >= CONFIG_TUNNEL_SIZE ) )
	        && FIGHTING( ch ) != victim && FIGHTING( victim ) != ch )
	{
		ch->send( "You'd probably get killed before you got there, it is so crowded!\r\n" );
		return 0;
	}

	if ( IS_NPC( victim ) && GET_RACE( victim ) == GET_RACE( ch ) )
	{
		for(auto userClanIter = victim->userClans.begin();userClanIter != victim->userClans.end();++userClanIter)
		{
			UserClan *userClan = (*userClanIter);
			if ( ClanUtil::getClan( userClan->getClanId() ) )
				SET_BIT_AR( GET_WARRANTS( ch ), ClanUtil::getClan( userClan->getClanId() )->warrant );
		}
	}

	if ( MOUNT( ch ) == victim )
		ch->Dismount();

	/* Do some sanity checking, in case someone flees, etc. */
	if ( !victim || ( victim->in_room != ch->in_room ) )
		return 0;

	//Draw if no weapon wielded
	if ( !GET_EQ( ch, WEAR_WIELD ) )
		ch->Draw();

	/* Find the weapon type (for display purposes only) */
	if (wielded && wielded->getType() == ITEM_WEAPON)
		w_type = GET_OBJ_VAL( wielded, 3 ) + TYPE_HIT;

	else
	{
		if ( IS_NPC( ch ) && ( ch->MobData->attack_type != 0 ) )
			w_type = ch->MobData->attack_type + TYPE_HIT;
		else
			w_type = TYPE_HIT;
	}

	ch->GainNoQuit( victim );
	victim->GainNoQuit( ch );

	dam = ch->CalculateDamage( victim, type, bp );

	//Unhide character
	if ( AFF_FLAGGED( ch, AFF_HIDE ) )
		REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_HIDE );

	//	If it's a combat skill, we already have everything we need. No reason to check each skill separately.
	if ( type < TYPE_HIT && type > MAX_SPELLS )
	{
		individualHitClockDamage.turnOn();
		finalDamageCalculation = damage( ch, victim, dam, type, -1 );
		individualHitClockDamage.turnOff();
		return finalDamageCalculation;
	}

	/* decide whether this is a hit or a miss */
	int roll = MiscUtil::random(1, 20);
	if(AFF_FLAGGED(ch, AFF_DARK_PLAGUE))
	{
		Weave* weave = WeaveManager::GetManager().GetWeave("Dark Plague");
		if( weave )
			roll += MiscUtil::Convert<int>(weave->getAttribute("HitRollModifier"));
	}

	if ( !(roll <= 1) && ( (roll == 20 && !IS_NPC(ch)) || ch->OffenseMeleeRoll() > victim->DefenseMeleeRoll() ) )
	{
		js_hitpercent_triggers(ch, victim);
		individualHitClockDamage.turnOn();
		finalDamageCalculation = damage( ch, victim, dam, w_type, bp );
		individualHitClockDamage.turnOff();
	}
	else
	{// the attacker missed the victim
		individualHitClockDamage.turnOn();
		finalDamageCalculation = damage( ch, victim, 0, w_type, bp );
		individualHitClockDamage.turnOff();
	}

	return finalDamageCalculation;
}

/* Old, crappy coding from 2004ish for chain weapon type */
void Character::HitAllFighting()
{//TODO: Rewrite
	Character * vict, *temp;

	for ( vict = in_room->people;vict;vict = temp )
	{
		temp = vict->next_in_room;
		if ( FIGHTING( vict ) == this || vict == FIGHTING( this ) )
		{
			hit( this, vict, TYPE_UNDEFINED );
		}
	}
}

void Character::PerformFear( Character *victim )
{
	struct affected_type af;

	if ( victim == NULL )
		return ;

	if ( AFF_FLAGGED( victim, AFF_PARANOIA ) || victim->getUserClan( CLAN_BLADEMASTERS ) )
		return ;

	Act( "You stare into $n's eyeless face, unable to pull your eyes away as you feel your bones freeze.",
	     TRUE, this, NULL, victim, TO_VICT );
	Act( "You stare into $N's eyes and watch $M shake with fear.", TRUE, this, NULL, victim, TO_CHAR );
	Act( "$N begins to shiver in paranoia as $E stares into the eyeless face of $n.", TRUE, this, NULL, victim, TO_NOTVICT );

	af.type = SKILL_NOTICE;
	af.duration = 2;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_PARANOIA;
	affect_to_char( victim, &af );
}

void Character::PerformPoison( Character *victim )
{
	struct affected_type af;

	if ( !victim )
		return ;

	if ( AFF_FLAGGED( victim, AFF_POISON ) )
		return ;

	if ( this->in_room != victim->in_room )
		return ;

	Act( "You slash into $N's body deeply, injecting poison from your weapon into $S body!", TRUE, this, NULL, victim, TO_CHAR );
	Act( "$n slashes into deeply into your body, injecting a tainted poison inside of you!", TRUE, this, NULL, victim, TO_VICT );
	Act( "$n slashes into $N deeply, injecting poison into $S body!", TRUE, this, NULL, victim, TO_NOTVICT );

	af.type = SKILL_NOTICE;
	af.duration = 2;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_POISON;
	affect_to_char( victim, &af );
}

void check_fighting( void )
{//TODO: Rewrite
	Character * ch;
	Object *weapon;

	for ( ch = combat_list;ch;ch = next_combat_list )
	{
		next_combat_list = ch->next_fighting;

		if ( !FIGHTING( ch ) )
			continue;

		weapon = GET_EQ( ch, WEAR_WIELD );

		if ( weapon && IS_OBJ_STAT( weapon, ITEM_POISON ) && MiscUtil::random( 1, 40 ) == 1 )
			ch->PerformPoison( FIGHTING( ch ) );

		/*if ( IS_FADE( ch ) && MiscUtil::random( GET_SKILL( ch, SKILL_FEAR ), 2000 ) <= GET_SKILL( ch, SKILL_FEAR ) && FIGHTING( ch ) )
			ch->PerformFear( FIGHTING( ch ) );*/
	}
}
/* control the fights going on.  Called every 4 seconds from comm.c. */
void perform_violence( void )
{
	Character * ch;
	Object *wielded;
	int percent, prob;

	Clock loopClock, triggerClock, hitClock, specialClock;
	individualHitClockDamage.reset(false);

	loopClock.turnOn();
	
	damageClock1.reset(false); damageClock2.reset(false); damageClock3.reset(false); damageClock4.reset(false); damageClock5.reset(false); damageClock6.reset(false); damageClock7.reset(false);

	dieClock1.reset(false);
	dieClock2.reset(false);
	dieClock3.reset(false);
	dieClock4.reset(false);
	dieClock5.reset(false);
	dieClock6.reset(false);
	dieClock7.reset(false);
	dieClock8.reset(false);
	dieClock9.reset(false);
	dieClock10.reset(false);
	dieClock11.reset(false);
	dieClock12.reset(false);
	dieClock13.reset(false);
	dieClock14.reset(false);
	dieClock15.reset(false);
	dieClock16.reset(false);

	dieClock14A.reset(false);
	dieClock14B.reset(false);
	dieClock14C.reset(false);

	corpseClock1.reset(false);
	corpseClock2.reset(false);
	corpseClock3.reset(false);
	corpseClock4.reset(false);
	corpseClock5.reset(false);

	for ( ch = combat_list; ch; ch = next_combat_list )
	{
		next_combat_list = ch->next_fighting;
		
		triggerClock.turnOn();
		js_fight_triggers(ch, FIGHTING(ch));
		triggerClock.turnOff();
		/* Checks the characters timer to see whether to pass the round or go on to hit() *Galnor* */
		if ( CHECK_WAIT( ch ) )
			continue;

		if ( ch->timer )
			continue;

		if ( GET_POS( ch ) < POS_FIGHTING && GET_POS( ch ) > POS_STUNNED )
			GET_POS( ch ) = POS_FIGHTING;

		if ( GET_POS( ch ) < POS_FIGHTING )
			continue;

		if ( !FIGHTING( ch ) || ch->in_room != FIGHTING( ch ) ->in_room )
		{
			ch->StopFighting();
			if ( FIGHTING( ch ) )
				FIGHTING( ch ) ->StopFighting();
			continue;
		}

		if ( IS_NPC( ch ) && MOB_FLAGGED( ch, MOB_NOFIGHT ) )
			continue;

		wielded = GET_EQ( ch, WEAR_WIELD );

		hitClock.turnOn();
		if ( wielded && IS_OBJ_STAT( wielded, ITEM_CHAIN ) )
			ch->HitAllFighting();
		else
		{
			if ( !FIGHTING( ch ) )
				return ;
			hit( ch, FIGHTING( ch ), TYPE_UNDEFINED );
		}
		hitClock.turnOff();

		if ( wielded && FIGHTING( ch ) )
		{
			if ( GET_OBJ_VAL( wielded, 0 ) == WEAPON_SHORT_BLADE || AFF_FLAGGED( ch, AFF_HASTE ) /*|| IS_FADE( ch ) || ch->getUserClan( CLAN_BLADEMASTERS )
			        || IS_WARDER( ch ) */)
			{
				percent = MiscUtil::random( 1, 150 );	/* random for attack */

				if ( AFF_FLAGGED( ch, AFF_HASTE ) )
					prob = 99;
				else
					prob = GET_SKILL( ch, SKILL_ATTACK );
				hitClock.turnOn();
				if ( percent < prob && FIGHTING( ch ) != NULL )
					hit( ch, FIGHTING( ch ), TYPE_UNDEFINED );
				hitClock.turnOff();

			}
			else if ( IS_FADE(ch) && GET_SKILL(ch, SKILL_SHADOW_RAGE) && AFF_FLAGGED(ch, AFF_SHADOW_RAGE) )
			{
				if( GET_SHADOW(ch) <= 1 ) {
					ch->send( "Your anger abates.\n\r" );
					affect_from_char( ch, 0, AFF_SHADOW_RAGE );
					continue;
				}
				hitClock.turnOn();
				percent = MiscUtil::random( 1, 100 );
				prob = ch->GetSkillLevel(SKILL_SHADOW_RAGE) * 13;
				if ( percent < prob && FIGHTING(ch) != NULL )
					hit( ch, FIGHTING(ch), TYPE_UNDEFINED );
				GET_SHADOW(ch) = MAX(GET_SHADOW(ch) - 2, 0);
				hitClock.turnOff();
			}

		}
		/* XXX: Need to see if they can handle "" instead of NULL. */
		if ( MOB_FLAGGED( ch, MOB_SPEC ) && MobManager::GetManager().GetIndex((u_int)ch->nr)->func != NULL ) {
			char sEmpty[12];
			*sEmpty = '\0';
			specialClock.turnOn();
			( MobManager::GetManager().GetIndex((u_int)ch->nr)->func ) ( ch, ch, 0, sEmpty );
			specialClock.turnOff();
		}
	}


#if(PERFORM_VIOLENCE_LOGGING == 1)
	std::cout << "===================================================" << std::endl;
	std::cout << "Perform Combat    :"; loopClock.print();
	std::cout << "Combat Trigger    :"; triggerClock.print();
	std::cout << "Combat Hit        :"; hitClock.print();
	std::cout << "Hit Damage        :"; individualHitClockDamage.print();
	std::cout << "Damage1           :"; damageClock1.print();
	std::cout << "Damage2           :"; damageClock2.print();
	std::cout << "Damage3           :"; damageClock3.print();
	std::cout << "Damage4           :"; damageClock4.print();
	std::cout << "Damage5           :"; damageClock5.print();
	std::cout << "Damage6           :"; damageClock6.print();
	std::cout << "Damage7           :"; damageClock7.print();
	std::cout << "Special           :"; specialClock.print();
	
	
	std::cout << "Die clock1        :"; dieClock1.print();
	std::cout << "Die clock2        :"; dieClock2.print();
	std::cout << "Die clock3        :"; dieClock3.print();
	std::cout << "Die clock4        :"; dieClock4.print();
	std::cout << "Die clock5        :"; dieClock5.print();
	std::cout << "Die clock6        :"; dieClock6.print();
	std::cout << "Die clock7        :"; dieClock7.print();
	std::cout << "Die clock8        :"; dieClock8.print();
	std::cout << "Die clock9        :"; dieClock9.print();
	std::cout << "Die clock10       :"; dieClock10.print();
	std::cout << "Die clock11       :"; dieClock11.print();
	std::cout << "Die clock12       :"; dieClock12.print();
	std::cout << "Die clock13       :"; dieClock13.print();
	std::cout << "Die clock14       :"; dieClock14.print();
	std::cout << "Die clock14A      :"; dieClock14A.print();
	std::cout << "Die clock14B      :"; dieClock14B.print();
	std::cout << "Die clock14C      :"; dieClock14C.print();
	std::cout << "Die clock15       :"; dieClock15.print();
	std::cout << "Die clock16       :"; dieClock16.print();

	std::cout << "Corpse clock1     :"; corpseClock1.print();
	std::cout << "Corpse clock2     :"; corpseClock2.print();
	std::cout << "Corpse clock3     :"; corpseClock3.print();
	std::cout << "Corpse clock4     :"; corpseClock4.print();
	std::cout << "Corpse clock5     :"; corpseClock5.print();
	std::cout << "===================================================" << std::endl << std::endl << std::endl;
#endif
}

/* Galnor: 8-3-2005, For the second time(hard drive ate this the first time), return random fighting target. */
Character *Character::RandomFighting()
{
	Character * ch;
	int n = 0;

	if ( FIGHTING( this ) )
		return NULL;
	else
		n = MiscUtil::random( 1, this->NumFighting() );

	for ( ch = this->in_room->people;ch && n;ch = ch->next_in_room )
	{
		if ( FIGHTING( ch ) == this )
			--n;
	}

	return ch;
}

/* Added by Galnor in October of 2003. Counts the number of people on character. */
int Character::NumFighting()
{
	int count = 0;
	Character *victim;

	for ( victim = this->in_room->people;victim;victim = victim->next_in_room )
	{
		if ( FIGHTING( victim ) == this )
			++count;
	}

	return count;
}

/* Added 6/1/09 by Fogel - Takes the name of a body part and returns its index. */
int FindBodyPart(const char *bp)
{/* changed char to const char to keep original string unaltered - 12/06/2009 */

//	for (int i = 0; i < MAX_STRING_LENGTH; i++)
//		part[i] = LOWER(part[i]);

	//str_cmp for case insensitivity.
	if (str_cmp(bp, "head") == 0)
		return WEAR_HEAD;
	else if (str_cmp(bp, "body") == 0)
		return WEAR_BODY;
	else if (str_cmp(bp, "shoulders") == 0)
		return WEAR_SHOULDERS;
	else if (str_cmp(bp, "arms") == 0)
		return WEAR_ARMS;
	else if (str_cmp(bp, "waist") == 0)
		return WEAR_WAIST;
	else if (str_cmp(bp, "hands") == 0)
		return WEAR_HANDS;
	else if (str_cmp(bp, "legs") == 0)
		return WEAR_LEGS;
	else if (str_cmp(bp, "feet") == 0)
		return WEAR_FEET;
	else
		return -1;
}

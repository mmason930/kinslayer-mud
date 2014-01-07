/* ************************************************************************
*   File: Act.offensive.c                               Part of CircleMUD *
*  Usage: player-level commands of an offensive nature                    *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, copyright (C) 1990, 1991.               *
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
#include "constants.h"
#include "screen.h"
#include "dg_event.h"
#include "weaves.h"
#include "MiscUtil.h"
#include "Descriptor.h"

/* extern variables */

extern Descriptor *descriptor_list;
extern Character *character_list;
extern std::list< std::pair< Character*, event_info* > * > BashStandQueue;

/* local functions */
ACMD(do_assist);
ACMD(do_backstab);
ACMD(do_bash);
ACMD(do_choke);
ACMD(do_flee);
ACMD(do_hamstring);
ACMD(do_hit);
ACMD(do_kick);
ACMD(do_kill);
ACMD(do_order);
ACMD(do_precisestrike);
ACMD(do_release);
ACMD(do_rescue);
ACMD(do_shadowstep);
ACMD(do_shieldblock);
ACMD(do_source);
ACMD(do_whirlwind);
ACMD(do_pulverize);


int too_scared(Character *ch, int skill);
int room_visibility(Character *ch, Character *vict);
int move_damage(Character *ch, Character *vict, int low, int high);
int held_and_back(Character *ch);
int can_move(Character *ch, int dir, int need_specials_check, bool flee);
void apply_fear(Character *ch, Character *vict);
void apply_hamstring(Character *ch, Character *vict);
void apply_shieldblock(Character *ch);
void apply_severewound(Character *ch, Character *vict);
void dmg(Character *ch, Character *vict, int low, int high, int same_room);
void save_player_clan(Character *ch, int number);
void performFlee(Character *ch);
void perform_hamstring(Character *ch, Character *vict);
void perform_shadowstep(Character *ch, Character *vict);
void perform_shieldblock(Character *ch, Character *vict, int attack_type, int subcmd);
void perform_whirlwind(Character *ch);
void perform_pulverize(Character *ch);
int mag_manacost( Character * ch, int spellnum );
int FindBodyPart(const char *bp);
int RandomBodyPart();

/************
 *    Galnor 12/22/2009 - Force player into standing position once the bash has expired.
 *    We can't simply check for wait pulse to hit zero because there my be other lag
 *    that has built up over time that pushed the bash beyond the normal two combat
 *    rounds, extending the bash. We need a separate event handler to take care of this.
 **********/
EVENT(BashStand)
{//This event should ONLY ever be called if the Character is valid.
	std::pair< Character*, event_info* > *myTuple;

	myTuple = reinterpret_cast< std::pair< Character*, event_info* > * >( info );

	myTuple->first->points.is_bashed = false;
	update_pos( myTuple->first );

	delete myTuple;
	BashStandQueue.remove( myTuple );
}
/*** Galnor 12/22/2009 - Place character in a bashed state for x pulses ***/
void Character::setBashState( const int nrPulses, bool cancelTimer, bool makeSit )
{
	if( makeSit )
		GET_POS( this ) = POS_SITTING;
	this->WaitState( this->wait + nrPulses );
	IS_BASHED( this ) = true;
	if( cancelTimer ) {//Set to true by default.
		this->CancelTimer( false );
	}

	/*** Determine if there is a bash event in the queue already. ***/
	std::list< std::pair<Character*,event_info*> *>::iterator eIter;
	for( eIter = BashStandQueue.begin(); eIter != BashStandQueue.end() ; ++eIter )
	{
		if( (*eIter)->first == this ) {
			//Yes, there is. We NEVER want more than one event in the queue per char.
			//We'll accumulate the pulses(can be changed later).
			(*eIter)->second->time_remaining += nrPulses;
			(*eIter)->second->time_remaining = MAX(0, (*eIter)->second->time_remaining);
			return;//And we are done.
		}
	}

	/*** There is no event in the queue. We'll need to add a callback to force the user to stand. ***/
	std::pair<Character*,event_info*> *eInfo = new std::pair<Character*,event_info*>;
	eInfo->first = this;
	eInfo->second = add_event( nrPulses, BashStand, eInfo );
	BashStandQueue.push_back( eInfo );//Scanned in Character's destructor to kill the event if it still exists.
}

std::string MoodString(int mood);

int too_scared(Character *ch, int skill)
{
	// phasing this out, just return 0 until this gets removed
	return 0;

	if((GET_TARGET(ch) && !IS_FADE(GET_TARGET(ch))) || (FIGHTING(ch) && !IS_FADE(FIGHTING(ch))))
		return 0;

	switch(skill)
	{
	case SKILL_BASH:
		if(MiscUtil::random(0, GET_SKILL(GET_TARGET(ch), SKILL_FEAR)) > 79)
			return 1;
		break;
	case SKILL_CHARGE:
		if(MiscUtil::random(0, GET_SKILL(GET_TARGET(ch), SKILL_FEAR)) > 74)
			return 1;
		break;
	case SKILL_BACKSTAB:
		if(MiscUtil::random(0, GET_SKILL(GET_TARGET(ch), SKILL_FEAR)) > 49)
			return 1;
		break;
	case SKILL_KICK:
		if(MiscUtil::random(0, GET_SKILL(GET_TARGET(ch), SKILL_FEAR)) > 79)
			return 1;
		break;
	case SKILL_PRECISESTRIKE:
		if(MiscUtil::random(0, GET_SKILL(GET_TARGET(ch), SKILL_FEAR)) > 79)
			return 1;
		break;
	case SKILL_PARRY:
		if(MiscUtil::random(0, GET_SKILL(FIGHTING(ch), SKILL_FEAR)) > 79)
			return 1;
		break;
	default:
		break;
	}

	return 0;
}

Character *faceoff(Character *attacker, Character *prey)
{
	Character *target;

	if(!FIGHTING(prey) || !AFF_FLAGGED(prey, AFF_GROUP))
		return prey;

	for(target = prey->in_room->people;target;target = target->next_in_room)
	{
		if( (target->master == NULL && prey->master == NULL) || !AFF_FLAGGED(target,AFF_GROUP) )
			continue;

		if(CAN_SEE(attacker, target) && (target->master == prey->master || target == prey->master || target->master == prey))
		{
			if(target->NumFighting() < prey->NumFighting()
			        && !MOB_FLAGGED(target, MOB_MOUNT) && !MOB_FLAGGED(target, MOB_SHADOW_MOUNT) && !MOB_FLAGGED(target, MOB_OGIER_MOUNT))
			{
				return target;
			}
		}
	}

	return prey;
}

bool Bash::BashFailWeight( Character *Basher, Character *Victim )
{
	float chance = ((float)Victim->TotalWeight()) / (MAX(Basher->TotalWeight(), 1));
	return ( MiscUtil::random(0, 10000) < (BashData.WeightDifferenceMultiplier * chance * 100) );

}
bool Bash::BashFailLevel( Character *Basher, Character *Victim )
{
	if(GET_LEVEL(Basher) >= 30 && GET_LEVEL(Victim) >= 30)
		return false;
	float chance = ((float)Victim->player.level) / (MAX(Basher->player.level, 1));
	return ( MiscUtil::random(0, 10000) < (BashData.LevelDifferenceMultiplier * chance * 100) );
}
bool Bash::BashSucceedWeapon( Character *Basher, Character *Victim )
{
	float WeightDifference = GET_EQ(Basher, WEAR_WIELD)->GetTotalWeight() - GET_EQ(Victim, WEAR_WIELD)->GetTotalWeight();
	return ( MiscUtil::random(0, 10000) < (BashData.WeaponDifferenceMultiplier * WeightDifference * 100) );
}
bool Character::rollBash( Character *Victim, bool BashFaceoff )
{
	int chance = MiscUtil::random(1,20);

	if ( chance == 20 )		// Fogel - "critical hit", guaranteed landing
		return true;
	if ( chance == 1 )		// "critical miss", guaranteed failure
		return false;

	// Makes tied bashes less biased against people with low dodge bonus and a heavy weapon.
	if (BashFaceoff && Bash::BashSucceedWeapon(this, Victim))
	{
		return true;
	}
	if (this->BashOffenseRoll() < Victim->BashDefenseRoll() ||
	Bash::BashFailWeight( this, Victim ) ||
	Bash::BashFailLevel( this, Victim ))
	{
		return false;
	}
	if ( ( IS_CHANNELER(this) || IS_DREADGUARD(this) ) && MiscUtil::random(1, 2) == 2 )
		return false;

	return true;
}

bool Character::rollBash( Character *Victim, int &off, int &def )
{
	int chance = MiscUtil::random(1,20);
	off = this->BashOffenseRoll();
	def = Victim->BashDefenseRoll();
	
	if ( chance == 20 )		// Fogel - "critical hit", guaranteed landing
		return true;
	if( chance == 1 )		// "critical miss", guaranteed failure
		return false;

	if (off < def ||
	Bash::BashFailWeight( this, Victim ) ||
	Bash::BashFailLevel( this, Victim ) )
	{
		return false;
	}

	if ( ( IS_CHANNELER(this) || IS_DREADGUARD(this) ) && MiscUtil::random(1, 2) == 2 )
		return false;

	return true;
}

int Character::BashOffenseRoll()
{
	float off = 0.0f, ob = 0.0f, weight = 0.0f, bmi = 0.0f;
	int weap_type = 0;

	if(!GET_EQ(this, WEAR_WIELD) || MiscUtil::random(1, 100) > GET_EQ(this, WEAR_WIELD)->GetTotalVal4())
		return -500;

	weap_type = GET_OBJ_VAL(GET_EQ(this, WEAR_WIELD), 0);

	bmi		= this->BMI() * BashData.O_BMIMultiplier + BashData.O_BMIOffset;
	weight	= GET_EQ(this, WEAR_WIELD)->Weight() * BashData.O_WeaponWeightMultiplier;
	ob		= this->Offense() * BashData.O_OBMultiplier;
	ob      *= ((float)GET_SKILL(this, SKILL_BASH) / 100.00f);
	ob      *= ((float)weapon_skill(this,  weap_type) / 100.00f);

	off  =  ob + weight + bmi;

	off +=  (MiscUtil::random((int) -(off * .30), (int) (off * .30)));

	off -=  3 * (20 - this->GetStr());

	return ((int) off);
}

int Character::BashDefenseRoll()
{
	float def = 0.00f, bmi = 0.0f, dodge = 0.0f;

	def += BashData.D_BaseBonus;

	dodge	=	(this->Dodge() * BashData.D_DBMultiplier);
//	bmi		=	(this->BMI()   * BashData.D_BMIMultiplier);

	def += (bmi * BashData.D_BMIMultiplier) + dodge;
	def *= (float)GET_SKILL(this, SKILL_DODGE) / 100.00f;

	return ((int) def);
}
void perform_bash(Character *ch, Character *vict)
{
	Character *p = 0;
	char dCommand[MAX_INPUT_LENGTH];
	int i = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	if(!vict || vict->IsPurged() || vict->in_room != ch->in_room)
	{
		ch->send("They must have already left...\r\n");
		return;
	}
	if( ch->IsPurged() )
		return;

	ch->GainNoQuit(vict);
	vict->GainNoQuit(ch);

	strcpy(dCommand, ch->delayed_command.c_str());
	//Both basher and bashee land bash on the same pulse...
	if( (ch->CanBash(OneArgument(dCommand, arg))) && (GET_TARGET(vict) == ch)
	        && vict->timer <= ((float)2 / PASSES_PER_SEC) && !str_cmp(GetCommand(dCommand), "bash")
	        && vict->CanBash(dCommand))
	{
		vict->CancelTimer(false);
		ch->CancelTimer(false);

		//Do 100 rolls, whoever wins first gets the bash.
		for(i = 100;i;--i)
		{
			bool cLand = ch->rollBash( vict, true );
			bool vLand = vict->rollBash( ch, true );
			//Both landed! Try again...
			if(cLand && vLand)
				continue;
			else if(cLand)
			{
				damage(vict, ch, 0, SKILL_BASH, -1);
				damage(ch, vict, 1, SKILL_BASH, -1);
				return;
			}
			else if(vLand)
			{
				damage(ch, vict, 0, SKILL_BASH, -1);
				damage(vict, ch, 1, SKILL_BASH, -1);
				return;
			}
			else
			{
				//Both missed. What a shame!
				damage(ch, vict, 0, SKILL_BASH, -1);
				damage(vict, ch, 0, SKILL_BASH, -1);
				return;
			}
		}
		//Wow! 100 bashes and they both landed every time! I suppose both get to sit down.
		hit(ch, vict, SKILL_BASH);
		hit(vict, ch, SKILL_BASH);
		return;
	}
	else
	{
		if (ch->rollBash( vict, false ))
		{
			if (damage(ch, vict, 1, SKILL_BASH, -1) == -1)
				return;
		}
		else
		{
			damage(ch, vict, 0, SKILL_BASH, -1);
			ch->WaitState(PULSE_VIOLENCE / 2);
		}
	}
	if(FIGHTING(ch) && (p = faceoff(ch, vict)) && !PreFighting)
	{
		ch->StopFighting();
		ch->SetFighting(p);
	}
}

Character *Character::CanBash(char *argument)
{
	Character *vict = 0;
	OneArgument(argument, arg);

	if (str_cmp(this->Command, "bash"))
		return 0;
	if (!GET_SKILL(this, SKILL_BASH))
	{
		this->send("You have no idea how.\r\n");
		return 0;
	}
	if (ROOM_FLAGGED(this->in_room, ROOM_PEACEFUL))
	{
		this->send("This room just has such a peaceful, easy feeling...\r\n");
		return 0;
	}
	if (!GET_EQ(this, WEAR_WIELD))
	{
		this->send("You need to wield a weapon to make it a success.\r\n");
		return 0;
	}
	if(MOUNT(this) && this->GetSkillLevel(SKILL_RIDE) < 6)
	{
		this->send("Your riding skills are too low to do this while mounted.\r\n");
		return 0;
	}
	if(!*arg && FIGHTING(this) && this->in_room == FIGHTING(this)->in_room && !GET_TARGET2(this))
		vict = FIGHTING(this);
	else if(this->command_ready && GET_TARGET(this))
		vict = GET_TARGET(this);
	else if(this->command_ready && GET_TARGET2(this))
		vict = GET_TARGET2(this);
	else if (!*arg || !(vict = get_char_room_vis(this, arg)))
	{
		this->send("Bash who?\r\n");
		this->WaitState(MAX(2, this->wait + 1));
		return 0;
	}
	// Requested by Someone (Jonlin?), next few lines of code by Serai - 02/10/04
	// Galnor: 02-17-2004 : Fixed a target bug dealing with bashing without a name.
	if (FIGHTING(this) && FIGHTING(this) != vict && FIGHTING(vict) != this)
	{
		this->send("You can't bash someone else while engaged!\r\n");
		return 0;
	}
	if(!vict || !room_visibility(this, vict))
	{ //Checks to see if ch can see vict in ch's room //
		this->send("They must have left already...\r\n");
		return 0;
	}
	if (vict == this)
	{
		this->send("Aren't we funny today...\r\n");
		return 0;
	}
	if(IS_BASHED(vict))
	{
		this->send("They already seem stunned!\r\n");
		return 0;
	}
	if(GET_POS(vict) == POS_FLYING)
	{
		send("They're too high off the ground to reach!\r\n");
		return 0;
	}
	return vict;
}

ACMD(do_bash)
{
	Character *vict, *temp, *multivict = 0;

	if((vict = ch->CanBash(argument)))
	{
		if(!ch->command_ready)
		{	
			GET_TARGET(ch) = vict;
			if (GET_SKILL(ch, SKILL_DUALBASH) && !GET_TARGET2(ch))	//Fogel 8/22 - Clan bonus, two target multibash
			{
				for ( multivict = ch->in_room->people; multivict; multivict = temp )
				{
					temp = multivict->next_in_room;
					if ( FIGHTING( multivict ) == ch && multivict != FIGHTING( ch ) && multivict != vict)
					{
						if ( IS_BASHED( multivict ) )
							continue;
						GET_TARGET2(ch) = multivict;
						break;
					}
				}
			}
			if(too_scared(ch, SKILL_BASH))
			{
				Act("You lock eyes with $N and take a moment to shake off your fear before swinging your weapon!", TRUE, ch, NULL, vict, TO_CHAR);
				Act("$n pauses briefly before continuing $s attack as you stare at $m mockingly!", TRUE, ch, NULL, vict, TO_VICT);
				Act("$N turns $S eyeless gaze to $n, who panics briefly before continuing $s attack!", TRUE, ch, NULL, vict, TO_NOTVICT);
				ch->timer += .5f;
			}
			ch->command_ready = true;
		}
		else
		{
			if(vict->ShieldBlock)
				perform_shieldblock(vict, ch, SKILL_BASH, 0);
			else if(GET_SKILL(ch, SKILL_DUALBASH) && GET_TARGET2(ch) && (GET_TARGET2(ch))->ShieldBlock)
				perform_shieldblock(GET_TARGET2(ch), ch, SKILL_BASH, 0);
			else
			{
				perform_bash(ch, GET_TARGET(ch));
				GET_TARGET(ch) = NULL;
				if(GET_SKILL(ch, SKILL_DUALBASH) && GET_TARGET2(ch) && !IS_BASHED(GET_TARGET2(ch)) && FIGHTING(GET_TARGET2(ch)) == ch && GET_TARGET2(ch)->in_room == ch->in_room)
				{
					perform_bash(ch, GET_TARGET2(ch));
					GET_TARGET2(ch) = NULL;
				}
			}
		}
	}
}

bool Character::BackstabLanded(Character *victim)
{
	int fate_roll = 0, off_range = 0, lev_dif = 0, no_quit_mod = 5;

	if(!GET_EQ(this, WEAR_WIELD))
		return false;
	if(MiscUtil::random(1, 100) > GET_EQ(this, WEAR_WIELD)->GetTotalVal7() || (IS_NPC(victim) && MOB_FLAGGED(victim, MOB_AWARE)))
		return false;

	fate_roll = MiscUtil::random(0, 200);
	off_range = GET_SKILL(this, SKILL_BACKSTAB) / 2;
	off_range += GET_SKILL(this, SKILL_SHORT_BLADE) / 2;
	off_range += AFF_FLAGGED(this, AFF_HIDE) ? (GET_SKILL(this, SKILL_HIDE) / 2) : 0;

	//Status contributors
	off_range += GetDex();
	off_range -= GetLuck();

	//Notice factor
	if(AFF_FLAGGED(victim, SKILL_NOTICE))
		off_range -= victim->GetSkillLevel(SKILL_NOTICE);

	//No quit factor
	if(!IS_NPC(this))
	{
		for(affected_type *aff = this->affected; aff; aff = aff->next)
		{
			if( aff->bitvector == AFF_NOQUIT )
				no_quit_mod -= aff->duration;
		}
		no_quit_mod *= 10;
		off_range += no_quit_mod;
	}

	if(GET_POS(victim) <= POS_SLEEPING)
		off_range += 100;
	else if(GET_POS(victim) <= POS_SITTING)
		off_range += 50;

	//Level factor
	lev_dif = GET_LEVEL(this) - GET_LEVEL(victim);
	if( (lev_dif <= 0 && IS_NPC(victim)) || !IS_NPC(victim) )
		off_range += lev_dif;

	if(fate_roll > off_range)
		return false;
	if(IS_HORSE(victim) && RIDDEN_BY(victim) && MiscUtil::random(0, 100) < 50)
		return false;
	return true;
}

void perform_backstab(Character *ch, Character *vict)
{
	Character *p = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	if(!vict)
	{
		ch->send("They must have already left...\r\n");
		return;
	}

	if (!ch->BackstabLanded(vict))
	{
		if(too_scared(ch, SKILL_BACKSTAB))
		{
			Act("You fall to the ground in fear as $N's eyeless stare falls upon you!", TRUE, ch, NULL, vict, TO_CHAR);
			Act("You take a long look at $n, who falls to the ground convulsing.", TRUE, ch, NULL, vict, TO_VICT);
			Act("$n drops to the ground shaking violently as $N locks eyes with $m.", TRUE, ch, NULL, vict, TO_NOTVICT);

			ch->setBashState( PULSE_VIOLENCE * 2, false );
		}
		else
		{
			damage(ch, vict, 0, SKILL_BACKSTAB, -1);
			WAIT_STATE(ch, PULSE_VIOLENCE * 2);
		}
	}
	else
	{
		hit(ch, vict, SKILL_BACKSTAB);
		if(room_visibility(ch, vict))
			apply_severewound(ch, vict);
		return;
	}
	GET_TARGET(ch) = NULL;
	if((p = faceoff(ch, vict)) && FIGHTING(ch) && !PreFighting)
	{
		ch->StopFighting();
		ch->SetFighting(p);
	}
}

void perform_assist(Character *ch, Character *helpee)
{
	Character *opponent = 0;

	if(!ch || !helpee || FIGHTING(ch))
		return;

	if (FIGHTING(helpee))
		opponent = FIGHTING(helpee);

	else
	{
		for (opponent = ch->in_room->people;
		        opponent && (FIGHTING(opponent) != helpee);
		        opponent = opponent->next_in_room)
			;
	}

	if (!opponent)
		Act("But nobody is fighting $M!", FALSE, ch, 0, helpee, TO_CHAR);

	else if (!CAN_SEE(ch, opponent))
		Act("You can't see who is fighting $M!", FALSE, ch, 0, helpee, TO_CHAR);

	else
	{
		opponent = faceoff(ch, opponent);
		ch->send("You join the fight!\r\n");
		Act("$N assists you!", 0, helpee, 0, ch, TO_CHAR);
		Act("$n assists $N.", FALSE, ch, 0, helpee, TO_NOTVICT);

		if(( (opponent->NumFighting() >= 3 && !IS_NPC(ch)) ||
		        (ROOM_FLAGGED(ch->in_room, ROOM_TUNNEL) && opponent->NumFighting() >= 2)))
		{
			ch->send("You'd probably get killed before you reached the fight because it is so crowded!\r\n");
			return;
		}
		ch->SetFighting(opponent);
	}
}

int Character::ChargeDefenseRoll(int skill)
{
	int defense = 0;

	defense += ChargeData.D_BaseBonus;
	//defense -= this->BMI()   * ChargeData.D_BMIMultiplier;
	defense += (int)(this->Dodge() * ChargeData.D_DBMultiplier);

	return defense;
}

int Character::ChargeOffenseRoll(int skill)
{
	float offense = 0.0f;

	if(!GET_EQ(this, WEAR_WIELD) || MiscUtil::random(1, 100) > GET_EQ(this, WEAR_WIELD)->GetTotalVal10())
		return -5000;

	offense += (this->Offense() * ChargeData.O_OBMultiplier);
	offense += (this->BMI() * ChargeData.O_BMIMultiplier);
	offense -= (GET_EQ(this, WEAR_WIELD) ?
		(GET_EQ(this, WEAR_WIELD)->Weight() * ChargeData.O_WeaponWeightMalus) : (0) );
	offense += MiscUtil::random( -((int) (offense * ChargeData.O_NegativeSway) ),
						((int) (offense * ChargeData.O_PositiveSway) )
					 );

	switch (SECT(this->in_room))
	{
		case SECT_CITY:
			offense -= 10;
			break;
		case SECT_FOREST:
			offense -= 20;
			break;
		case SECT_HILLS:
			offense -= 30;
			break;
		case SECT_MOUNTAIN:
			offense -= 40;
			break;
	}
	
	offense *= ((float)GET_SKILL(this, skill) / 100);

	return (int)offense;
}

void perform_charge(Character *ch, Character *victim, int type)
{
	Character *p = 0;
	int defense = 0, offense = 0, skill = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	if(!victim || victim->IsPurged())
	{
		ch->send("They must have left already...\r\n");
		return;
	}
	if( ch->IsPurged() )
		return;

	skill = (type == SCMD_CHARGE ? SKILL_CHARGE : SKILL_SKEWER);

	/* Find the defense of the victim */
	offense = ch->ChargeOffenseRoll(skill);
	defense = victim->ChargeDefenseRoll(skill);
	GET_TARGET(ch) = NULL;

	/* The attacker fails. */
	if(defense > offense || (IS_NPC(victim) && MOB_FLAGGED(victim, MOB_AWARE)))
	{
		damage(ch, victim, 0, skill, -1);
		WAIT_STATE(ch, ChargeData.O_PulsesLagOnMiss);
	}

	/* The attacker succeeds. */
	else
	{
		WAIT_STATE(ch, ChargeData.O_PulsesLagOnLand);
		WAIT_STATE(victim, ChargeData.D_PulsesLagOnLand);
		if(hit(ch, victim, skill) == -1)
			return;
	}
	if((p = faceoff(ch, victim)) && FIGHTING(ch) && !PreFighting)
	{
		ch->StopFighting();
		ch->SetFighting(p);
	}
}


ACMD(do_charge)
{
	Character *victim = 0, *p = 0;
	char arg[MAX_INPUT_LENGTH];
	bool PreFighting = FIGHTING(ch) ? true : false;

	OneArgument(argument, arg);

	if(IS_TROLLOC(ch) && subcmd == SCMD_CHARGE && !IS_FADE(ch) && !IS_DREADLORD(ch))
	{
		ch->send("You can't charge! You would need a mount for that.\r\n");
		return;
	}

	if(((!IS_TROLLOC(ch) && !ch->getUserClan(CLAN_WOLFBROTHER)) || IS_FADE(ch)) && (subcmd == SCMD_SKEWER))
	{
		ch->send("You can't run fast enough to do that.\r\n");
		return;
	}

	if ( (!*argument || !(victim = get_char_room_vis(ch, arg)) || victim->IsPurged()) && !ch->command_ready )
	{
		ch->send("To whom do you wish to do this?\r\n");
		ch->WaitState(MAX(4, ch->wait + 1));
		return;
	}

	if(ch->command_ready)
		victim = GET_TARGET(ch);

	if(!room_visibility(ch, victim))
	{ //Checks to see if ch can see vict in ch's room //
		ch->send("They must have left already...\r\n");
		return;
	}

	if(MOUNT(ch) && ch->GetSkillLevel(SKILL_RIDE) < 6)
	{
		ch->send("Your riding skills are too low to do this while mounted.\r\n");
		return;
	}

	if(GET_POS(victim) == POS_FLYING)
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}

	if(victim == MOUNT(ch))
	{
		ch->send("You cannot charge your own mount!\r\n");
		return;
	}

	if(ch->command_ready)
		victim = GET_TARGET(ch);

	if(subcmd == SCMD_CHARGE && !MOUNT(ch))
	{
		ch->send("You will need to be riding to even attempt this.\r\n");
		return;
	}

	/* Fogel 5/7/09 - Added to disallow trollocs from skewering indoors and humans from charging indoors should they get ported indoors while mounted*/
	if(SECT( ch->in_room ) == SECT_INSIDE)
	{
		ch->send("There isn't enough room for you to do that.\r\n");
		return;
	}
	/* End of changes from 5/7/09 */

	if (victim == ch)
	{
		ch->send("You want to try to charge yourself?\r\n");
		return;
	}

	if(FIGHTING(ch) && ((!ch->command_ready && !ChargeData.CanStartWhileFighting)
	|| (ch->command_ready && !ChargeData.CanFinishWhileFighting) ))
	{
		ch->send("No way! You're fighting for your life here!\r\n");
		return;
	}

	/* Fogel 5/7/09 - Added code so the CanFinishWhileFighting variable actually works */
	if(FIGHTING(ch) && !ch->command_ready  && ChargeData.CanFinishWhileFighting)
		ch->CannotFinishCharge = true;

	if(!FIGHTING(ch) && ChargeData.CanFinishWhileFighting && ch->command_ready)
		ch->CannotFinishCharge = false;

	if(ch->command_ready && ch->CannotFinishCharge)
	{
		ch->send("No way! You're fighting for your life here!\r\n");
		ch->CannotFinishCharge = false;
		return;
	}
	/* End of changes from 5/7/09 */

	if (!GET_EQ(ch, WEAR_WIELD))
	{
		ch->send("You need to wield a weapon to make it a success.\r\n");
		return;
	}

	/* Fogel 5/7/09 - Changed to allow spears and lances to both charge and skewer */
	if ((subcmd == SCMD_CHARGE || subcmd == SCMD_SKEWER) && ((GET_EQ(ch, WEAR_WIELD)->GetTotalVal0() != WEAPON_LANCE) && (GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 0) != WEAPON_SPEAR)))
	{
		ch->send("You will need a lance or spear to even attempt this.\r\n");
		return;
	}
	/* End of changes from 5/7/09 */

	if(FIGHTING(victim) && (!ChargeData.CanStartOnFightingVict || ch->command_ready))
	{
		ch->send("You might hit the wrong target!\r\n");
		return;
	}

	if( FIGHTING(victim) && !FIGHTING(ch) && ChargeData.CanStartOnFightingVict )
	{
		for(affected_type *aff = ch->affected; aff; aff = aff->next)
		{
			if( aff->bitvector == AFF_NOQUIT && aff->duration > 3 && !IS_NPC(FIGHTING(victim)))
			{
				ch->send("You might hit the wrong target!\r\n", GET_NAME(victim));
				return;
			}
		}
	}

	if(IS_BASHED(victim))
	{
		ch->send("%s is stunned... You can't seem to reach the right spot!\r\n", GET_NAME(victim));
		return;
	}

	GET_TARGET(ch) = victim;

	if(!ch->command_ready)
	{
		Act("You begin charging towards $N.", TRUE, ch, NULL, victim, TO_CHAR);
		Act("$n begins charging towards you.", TRUE, ch, NULL, victim, TO_VICT);
		Act("$n begins charging towards $N.", TRUE, ch, NULL, victim, TO_NOTVICT);
		ch->command_ready = true;
		ch->timer = ChargeData.Timer;
	}
	else
	{
		if(too_scared(ch, SKILL_CHARGE))
		{
			Act("Suddenly $N stares deeply into the eyes of your horse, who veers off course out of fear!", TRUE, ch, NULL, victim, TO_CHAR);
			Act("You turn your eyeless gaze to $n's horse, causing it to sharply change course!", TRUE, ch, NULL, victim, TO_VICT);
			Act("$N locks eyes with the horse of $n, who bucks wildly and runs in a different direction!", TRUE, ch, NULL, victim, TO_NOTVICT);
			WAIT_STATE(ch, ChargeData.O_PulsesLagOnMiss);
			if((p = faceoff(ch, victim)) && FIGHTING(ch) && !PreFighting)
			{
				ch->StopFighting();
				ch->SetFighting(p);
			}
			return;
		}
		if(victim->ShieldBlock)
			perform_shieldblock(victim, ch, SKILL_CHARGE, (subcmd == SCMD_CHARGE) ? SKILL_CHARGE : SKILL_SKEWER );
		else
			perform_charge(ch, victim, subcmd);
	}
}


/*	Galnor: 4-9-2004. Basically here we have two types of bow and arrow shooting.
 *	You can shoot a target one room away for limited damage and less accuracy or
 *	shoot someone in the same room for more damage
 */

int find_first_step( Room *src, Room *target );

void Character::RemoveTaintDizzy()
{
	this->send("The vileness of the taint slowly fades away.\r\n");
	this->dizzy_time = 0;
}

EVENT(SendTaintDizzyMessage)
{
	Character *ch;
	long idnum;

	idnum = (long)info;
	for(ch = character_list;ch;ch = ch->next)
	{
		if(!ch->IsPurged() && idnum == ch->player.idnum && ch->dizzy_time <= 0)
		{
			ch->send("A strong wave of dizziness hits you. You feel as if you are about to vomit!\r\n");
			ch->dizzy_time += (ch->points.taint / 10);
		}
		else if(ch->dizzy_time > 0)
			ch->dizzy_time += (ch->points.taint / 10);
	}
}

/* Connecting to the True Source in order to channel. */
ACMD(do_source)
{
	Character *victim;

	if( !CAN_CHANNEL(ch) && GET_LEVEL(ch) < LVL_GRGOD)
	{
		if(subcmd == SCMD_SEIZE)
			ch->send("You come out empty handed.\r\n");
		else if(subcmd == SCMD_EMBRACE)
			ch->send("There is nothing for you to embrace!\r\n");
		return;
	}

	if(subcmd == SCMD_EMBRACE && GET_SEX(ch) == SEX_MALE)
	{

		ch->send("If you did that, saidin would burn you alive!\r\n");
		return;
	}
	if(subcmd == SCMD_SEIZE && GET_SEX(ch) == SEX_FEMALE)
	{
		ch->send("You have to embrace saidar, not seize it!\r\n");
		return;
	}
	if( AFF_FLAGGED(ch, AFF_SHIELD) || ShieldManager::GetManager().ShieldedBy(ch) != NULL)
	{
		ch->send("You fumble for access to the True Source, but you're blocked off!\r\n");
		return;
	}
	if(PRF_FLAGGED(ch, PRF_SOURCE))
	{
		ch->send("You are in touch with the True Source already. Can you not feel the surge of power through you?\r\n");
		return;
	}
	if( ROOM_FLAGGED(ch->in_room, ROOM_NOSOURCE) )
	{
		ch->send("The True Source seems to be unreachable here!\r\n");
		return;
	}

	if(subcmd == SCMD_EMBRACE)
		ch->send("You surrender yourself to saidar as you embrace the True Source.\r\n");
	else if(subcmd == SCMD_SEIZE)
	{
		ch->send("You grasp for saidin and feel it surge through your body.\r\n");
		if(ch->CanTaint())
		{
			ch->send("You feel the filth of the taint seep through your body.\r\n");
/*
			if(ch->points.taint >= TaintData.TaintForSeizeDizziness)
			{
				add_event(MAX(1, ((TAINT_MAX / 100) - (ch->points.taint / 100)) * PASSES_PER_SEC), SendTaintDizzyMessage, &GET_ID(ch));
				ch->AddStrain(TaintData.SeizeCost / 2);*/
// 			}
		}
	}
	else
	{
		ch->send("What's that? How did you get here? Please report this!\r\n");
		return;
	}

	SET_BIT_AR(PRF_FLAGS(ch), PRF_SOURCE);

	for(victim = ch->in_room->people;victim;victim = victim->next_in_room)
	{
		if(GET_SEX(ch) == GET_SEX(victim) && victim != ch &&
		        (victim->ChannelingAbility() || GET_LEVEL(victim) >= LVL_GRGOD))
		{
			victim->send("You feel a strange sensation from somewhere nearby.\r\n");
		}
	}
}

void Character::RemoveSource()
{
	Character *victim = 0;

	if( !IS_SET_AR(PRF_FLAGS(this), PRF_SOURCE) ) return;

	if(this->CanTaint())
	{
		this->send("You feel the taint slowly fade away.\r\n");
		this->dizzy_time /= 2;
	}

	REMOVE_BIT_AR(PRF_FLAGS(this), PRF_SOURCE);
	this->KillAllGates();
	ShieldManager::GetManager().KillShieldsByCaster( this );

	for(victim = this->in_room->people;victim;victim = victim->next_in_room)
	{
		if(GET_SEX(this) == GET_SEX(victim) && victim != this &&
		        (victim->ChannelingAbility() || GET_LEVEL(victim) >= LVL_GRGOD))
		{
			victim->send("A strange sensation from nearby gets weaker.\r\n");
		}
	}
}

ACMD(do_release)
{
	if( !ch->ChannelingAbility() && GET_LEVEL(ch) < LVL_GRGOD)
	{
		ch->send("And you want to release what?\r\n");
		return;
	}

	if(!PRF_FLAGGED(ch, PRF_SOURCE))
	{
		ch->send("You aren't even connected to the True Source.\r\n");
		return;
	}

	if(ch->Eavesdropping)
		ch->stopEavesdropping();
	if(ch->in_room->EavesWarder == ch)
		ch->stopWarding();
	if(ch->InvertNextWeave)
		ch->InvertNextWeave = false;

	ch->send("You release the One Power.\r\n");
	ch->RemoveSource();

}

ACMD(do_assist)
{
	Character *helpee;

	if (FIGHTING(ch))
	{
		ch->send("You're already fighting!  How can you assist someone else?\r\n");
		return;
	}

	OneArgument(argument, arg);

	if (!*arg)
		ch->send("Whom do you wish to assist?\r\n");

	else if (!(helpee = get_char_room_vis(ch, arg)))
		ch->send(NOPERSON);

/*	else if(too_scared(ch))
		return;
*/
	else if (helpee == ch)
		ch->send("You can't help yourself any more than this!\r\n");

	else
		perform_assist(ch, helpee);
}


ACMD(do_hit)
{
	Character *vict;

	OneArgument(argument, arg);

	if (!*arg)
		ch->send("Hit who?\r\n");
	else if (!(vict = get_char_room_vis(ch, arg)))
		ch->send("They don't seem to be here.\r\n");
/*	else if(too_scared(ch))
		return;
*/	else if (vict == ch)
	{
		ch->send("You hit yourself...OUCH!.\r\n");
		Act("$n hits $mself, and says OUCH!", FALSE, ch, 0, vict, TO_ROOM);
	}
	else if( GET_POS(vict) == POS_FLYING )
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}
	else
	{
		vict = faceoff(ch, vict);

		if ((GET_POS(ch) == POS_STANDING) &&  !FIGHTING(ch))
			hit(ch, vict, TYPE_UNDEFINED);
		else if (GET_SKILL(ch, SKILL_SWITCHTARGET) && (GET_POS(ch) == POS_FIGHTING) && (FIGHTING(vict) == ch) && (FIGHTING(ch) != vict) && str_cmp(arg, "human") && str_cmp(arg, "dark") && str_cmp(arg, "trolloc") && str_cmp(arg, "seanchan") && str_cmp(arg, "aiel") )
		{
			FIGHTING(ch) = vict;
			Act("You are now attacking $N.", FALSE, ch, 0, vict, TO_CHAR);
			Act("$n	turns towards you and starts to attack!", FALSE, ch, 0, vict, TO_VICT);
			Act("$n squares up with $N and begins to attack $M.", FALSE, ch, 0, vict, TO_NOTVICT);
		}
		else
			ch->send("You do the best you can!\r\n");
	}
}


ACMD(do_kill)
{
	Character *vict;
	Object *weap = 0;
	char arg2[MAX_INPUT_LENGTH];

	if ((GET_LEVEL(ch) < LVL_IMPL && !IS_GAY(ch) && !IS_AWESOME(ch) && !IS_QUEEN(ch) && !IS_KING(ch)) || IS_NPC(ch))
	{
		do_hit(ch, argument, cmd, subcmd);
		return;
	}

	TwoArguments(argument, arg, arg2);

	if (!*arg)
		ch->send("Kill who?\r\n");
	else
	{
		if (!(vict = get_char_room_vis(ch, arg)))
			ch->send("They aren't here.\r\n");
		else if (ch == vict)
			ch->send("Your mother would be so sad.. :(\r\n");
		else if(IS_KING(vict))
		{
			ch->send("You can't strike the dictator! He is just much more... superb than you!\r\n"); //We know it's true! :)
			return;
		}
		else if( GET_POS(vict) == POS_FLYING )
		{
			ch->send("They're too high off the ground to reach!\r\n");
			return;
		}
		else
		{
			if(!str_cmp(arg2, "backstab"))
			{
				weap = GET_EQ(ch, WEAR_WIELD);
				if(!weap || !IS_SHORT_BLADE(weap))
				{
					ch->send("You need to be wielding a dagger to do this.\r\n");
					return;
				}
				ch->send("You blick %s!\r\n", GET_NAME(vict));
				vict->send("%sSuddenly Someone stabs you in the back, R.I.P....%s\r\n", COLOR_RED(vict, CL_COMPLETE),
				           COLOR_NORMAL(vict, CL_COMPLETE));
				vict->Die(ch);
				return;
			}
			else
			{
				Act("You chop $M to pieces!  Ah!  The blood!", FALSE, ch, 0, vict, TO_CHAR);
				Act("$N chops you to pieces!", FALSE, vict, 0, ch, TO_CHAR);
				Act("$n brutally slays $N!", FALSE, ch, 0, vict, TO_NOTVICT);
			}
			vict->Dismount();
			vict->Die(ch);
		}
	}
}

ACMD(do_backstab)
{
	Character *vict;

	OneArgument(argument, buf);

	if ( (!*argument || !(vict = get_char_room_vis(ch, buf))) && !ch->command_ready )
	{
		ch->send("Backstab who?\r\n");
		ch->WaitState(MAX(4, ch->wait + 1));
		return;
	}

	if(ch->command_ready)
		vict = GET_TARGET(ch);

	if(!room_visibility(ch, vict))
	{ //Checks to see if ch can see vict in ch's room //
		ch->send("They must have left already...\r\n");
		return;
	}

	if(MOUNT(ch))
	{
		ch->send("You can't do that while riding!\r\n");
		return;
	}

	if (vict == ch)
	{
		ch->send("How can you sneak up on yourself?\r\n");
		return;
	}

	if(FIGHTING(ch) && !ch->command_ready)
	{
		ch->send("No way! You're fighting for your life here!\r\n");
		return;
	}

	if (!GET_EQ(ch, WEAR_WIELD))
	{
		ch->send("You need to wield a weapon to make it a success.\r\n");
		return;
	}

	if (GET_OBJ_VAL(GET_EQ(ch, WEAR_WIELD), 0 ) != WEAPON_SHORT_BLADE)
	{
		ch->send("Only short blades weapons can be used for backstabbing.\r\n");
		return;
	}

	if(FIGHTING(vict) && (ch->command_ready))
	{
		ch->send("You can't sneak up on a fighting target.\r\n", GET_NAME(vict));
		return;
	}

	//Prevents character from starting a backstab on something engaged on a player if he has too much no quit
	if(FIGHTING(vict))
	{
		for(affected_type *aff = ch->affected; aff; aff = aff->next)
		{
			if( aff->bitvector == AFF_NOQUIT && aff->duration > 3 && !IS_NPC(FIGHTING(vict)))
			{
				ch->send("You can't sneak up on a fighting target.\r\n", GET_NAME(vict));
				return;
			}
		}
	}

	if(IS_BASHED(vict))
	{
		ch->send("%s is stunned... You can't seem to reach the right spot!\r\n", GET_NAME(vict));
		return;
	}
	if(GET_POS(vict) == POS_FLYING)
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}

	GET_TARGET(ch) = vict;

	if(!ch->command_ready)
		ch->command_ready = true;
	else
		perform_backstab(ch, GET_TARGET(ch));
}

const char *orders[] =
    {
        "follow",
        "\n",
    };

int order_command(const char *order)
{
	int i = 0;

	for(i = 0;orders[i] != "\n";i++)
		if(!str_cmp(order, orders[i]))
			return 1;

	return 0;
}


int order_ok(Character *ch, Character *victim, char *order)
{

	int i = 0;

	if(victim->master != ch)
		return 0;

	if(!IS_NPC(victim))
		return 0;

	for(i = 0;complete_cmd_info[i].command[0] != '\n';i++)
	{
		if(!strn_cmp(complete_cmd_info[i].command, order, strlen(order)))
			if(!order_command(complete_cmd_info[i].command.c_str()))
			{
				continue;
			}

			else
				return 1;
	}

	return 0;
}

ACMD(do_order)
{
	char name[MAX_INPUT_LENGTH], message[MAX_INPUT_LENGTH];
	bool found = FALSE;
	Room *org_room;
	Character *vict;
	struct Follower *k;

	HalfChop(argument, name, message);

	if (!*name || !*message)
		ch->send("Order who to do what?\r\n");
	else if (!(vict = get_char_room_vis(ch, name)))
		ch->send("That person isn't here.\r\n");
	else if (ch == vict)
		ch->send("You obviously suffer from skitzofrenia.\r\n");
	else
	{
		if (vict)
		{
			strcpy(message, ch->ScrambleSpeech(message, vict).c_str());
			vict->send("%s orders you to '%s'", GET_NAME(ch), message);
			Act("$n gives $N an order.", FALSE, ch, 0, vict, TO_ROOM);

			if (!order_ok(ch, vict, message))
				Act("$n has an indifferent look.", FALSE, vict, 0, 0, TO_ROOM);

			else
			{
				ch->send(OK);
				CommandInterpreter(vict, message);
			}
		}

		else
		{			/* This is order "followers" */
			sprintf(buf, "$n issues the order '%s'.", message);
			Act(buf, FALSE, ch, 0, vict, TO_ROOM);

			org_room = ch->in_room;

			for (k = ch->followers; k; k = k->next)
			{
				if (org_room == k->follower->in_room)
				{
					found = TRUE;
					CommandInterpreter(k->follower, message);
				}
			}

			if (found)
				ch->send(OK);
			else
				ch->send("Nobody here is a loyal subject of yours!\r\n");
		}
	}
}

int Room::NumExits()
{
	int num = 0;
	for( int i = 0; i < NUM_OF_DIRS; i++ )
		if( this->GetNeighbor(i) )
			num++;
	return num;
}

void performFlee(Character *ch)
{
	int attempt, loss;
	Character *was_fighting, *vict;

	if( ch->IsPurged() )
		return;

	FLEE_GO(ch) = false;

	if (GET_POS(ch) < POS_FIGHTING)
	{
		ch->send("You are in pretty bad shape, unable to flee!\r\n");
		return;
	}
	attempt = GET_DIRECTION(ch);	/* Select a random direction */

	if (CAN_GO(ch, attempt) && !ROOM_FLAGGED(EXIT(ch, attempt)->to_room, ROOM_DEATH))
	{
		vict = FIGHTING(ch);
		was_fighting = FIGHTING(ch);

		if (ch->SimpleMove(attempt, FALSE, true))
		{
			ch->send("You flee head over heels.\r\n");

			if (was_fighting)
			{
				if(!IS_NPC(ch))
				{
					loss = GET_MAX_HIT(was_fighting) - GET_HIT(was_fighting);
					gain_exp(ch, -loss);
				}
				if(was_fighting->points.is_bashed && !FIGHTING(was_fighting))
					was_fighting->setBashState( -PULSE_VIOLENCE );
			}
		}

		else
			Act("$n tries to flee, but can't!", TRUE, ch, 0, 0, TO_ROOM);

		return;
	}

	ch->send("PANIC!  You couldn't escape!\r\n");
}

ACMD(do_flee)
{
	int num_exits = ch->in_room->NumExits(), i = 0;
	bool room_found = FALSE;
	char arg1[MAX_INPUT_LENGTH];

	*arg1 = '\0';

	if(argument)
		OneArgument(argument, arg1);

	if(ch->timer > 0.0f)
		ch->CancelTimer(true);

	if(!IS_NPC(ch) && ch->PlayerData->mood == MOOD_BERSERK)
	{
		ch->send("You are BERSERK! Can you not feel the MADNESS?!");
		return;
	}
	if(GET_DEATH_WAIT(ch) > 0 && ch->in_room == ch->StartRoom())
	{
		ch->send("You must wait %d more minutes to regain strength.\r\n", GET_DEATH_WAIT(ch));
		return;
	}

	Act("$n panics, and attempts to flee!", TRUE, ch, 0, 0, TO_ROOM);

	/************ BEGIN OLD FLEE IMPLEMENATATION *******************/
	//if(!IS_NPC(ch))
	//{
	//	if(ch->PlayerData->mood == MOOD_BRAVE)
	//		chances = 1;
	//	else if(ch->PlayerData->mood == MOOD_MILD)
	//		chances = 2;
	//	else if(ch->PlayerData->mood == MOOD_WIMPY)
	//		chances = 3;
	//}
	//if(FLEE_GO(ch) == false)
	//{
	//	for (i = 0; i <= chances; ++i)
	//	{
	//		GET_DIRECTION(ch) = NUM_OF_DIRS;
	//		if( GET_DIRECTION(ch) == NUM_OF_DIRS )
	//			GET_DIRECTION(ch) = MiscUtil::random(0, NUM_OF_DIRS - 1);	/* Select a random direction */

	//		if (CAN_GO(ch, GET_DIRECTION(ch)) && !ROOM_FLAGGED(EXIT(ch, GET_DIRECTION(ch))->to_room, ROOM_DEATH))
	//		{
	//			//We'll check specially to see if they cannot move due to movement points being too low.
	//			if(GET_MOVE(ch) < ch->NeededToMove(GET_DIRECTION(ch)))
	//			{
	//				ch->send("You attempt to flee, but are too exhausted to move.\r\n");
	//				Act("$n attempts to flee, but is too exhausted to move.", TRUE, ch, 0, 0, TO_ROOM);
	//				return;
	//			}
	//			if(can_move(ch, GET_DIRECTION(ch), FALSE, true))
	//			{
	//				room_found = TRUE;
	//				break;
	//			}
	//		}
	//	}
	//}
	/********************* END OLD FLEE IMPLEMENTATION ********************/
	if( FLEE_GO(ch) == FALSE && num_exits > 0 )
	{
		while( room_found == FALSE )
		{
			GET_DIRECTION(ch) = MiscUtil::random(0, NUM_OF_DIRS - 1);
			int fleeDir = GET_DIRECTION(ch);
			if( CAN_GO(ch, GET_DIRECTION(ch)) )
			{
				// Prevent player from fleeing into a deathtrap.
				if( ROOM_FLAGGED(EXIT(ch, GET_DIRECTION(ch))->to_room, ROOM_DEATH) )
					break;
				// We'll check specially to see if they cannot move due to movement points being too low.
				if( GET_MOVE(ch) < ch->NeededToMove(fleeDir) )
				{
					ch->send("You attempt to flee, but are too exhausted to move.\r\n");
					Act("$n attempts to flee, but is too exhausted to move.", TRUE, ch, 0, 0, TO_ROOM);
					return;
				}
				if( MOUNT(ch) && GET_MOVE(MOUNT(ch)) < MOUNT(ch)->NeededToMove(fleeDir) )
				{
					Act("You attempt to flee, but $N is too exhausted to move.", false, ch, NULL, MOUNT(ch), TO_CHAR);
					Act("$n attempts to flee, but $N is too exhausted to move.", false, ch, NULL, MOUNT(ch), TO_ROOM);
					return;
				}
				if( can_move(ch, fleeDir, FALSE, true) )
					room_found = TRUE;
			}
			// Bail out if the exit is valid but blocked by a door.
			if( EXIT(ch, GET_DIRECTION(ch)) && EXIT(ch, GET_DIRECTION(ch))->to_room && 
				!EXIT(ch, GET_DIRECTION(ch))->IsDisabled() && IS_SET(EXIT(ch, GET_DIRECTION(ch))->exit_info, EX_CLOSED) )
				break;

			// Insurance policy in case something goes wrong.
			i++;
			if( i > 100 )
			{
				break;
			}
		}
	}
	if(FLEE_GO(ch) || (!FIGHTING(ch) && room_found == TRUE))
	{
		performFlee(ch);
		return;
	}

	if(room_found == TRUE)
	{
		ch->send("You panic and attempt to flee!\r\n");
		FLEE_GO(ch) = true;

		if(FLEE_LAG(ch) <= 0)
			FLEE_LAG(ch) += FleeData.PulsesForFirstFlee;
		else if(FIGHTING(ch))
			FLEE_LAG(ch) += FleeData.PulsesPerEngagedFlee;
		else
			FLEE_LAG(ch) += FleeData.PulsesPerUnengagedFlee;

		FLEE_LAG(ch) = MIN(FleeData.MaxFleeLag, FLEE_LAG(ch)); //Set the max flee lag.

		WAIT_STATE(ch, FLEE_LAG(ch) + MAX(0, 4 - num_exits) * FleeData.PulsesPerExit);
		return;
	}

	ch->send("PANIC!  You couldn't escape!\r\n");
}

ACMD(do_choke)
{
	Character* vict = 0;
	OneArgument( argument, arg );

	if( !GET_SKILL(ch, SKILL_CHOKE) )
	{
		ch->send("You would, but you don't know how!\r\n");
		return;
	}
	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL))
	{
		ch->send("This room just has such a peaceful, easy feeling...\r\n");
		return;
	}
	if(!*arg && FIGHTING(ch) && ch->in_room == FIGHTING(ch)->in_room)
		vict = FIGHTING(ch);
	else if (!*arg || !(vict = get_char_room_vis(ch, arg)))
	{
		ch->send("Choke who?\r\n");
		ch->WaitState(MAX(2, ch->wait + 1));
		return;
	}
	if (FIGHTING(ch) && FIGHTING(ch) != vict && FIGHTING(vict) != ch)
	{
		ch->send("You can't choke someone else while engaged!\r\n");
		return;
	}
	if(!vict || !room_visibility(ch, vict))
	{ //Checks to see if ch can see vict in ch's room 
		ch->send("They must have left already...\r\n");
		return;
	}
	if (vict == ch)
	{
		ch->send("And what would that accomplish?\r\n");
		return;
	}
	if(GET_POS(vict) == POS_FLYING)
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}
	if( !ch->command_ready )
		ch->command_ready = true;
	else
	{
		int roll = MiscUtil::random(1,100) - (GET_LEVEL(vict) - GET_LEVEL(ch));
		if( roll > 33 )
		{
			//Success.
			hit(ch, vict, SKILL_BAREHANDS);

			if( !FIGHTING(ch) )
				ch->SetFighting( vict );

			if( !AFF_FLAGGED(vict, AFF_SHIELD) && vict->ChannelingAbility() )
			{
				affected_type af;
				af.type = 0;
				af.duration = 0;
				af.modifier = 0;
				af.location = APPLY_NONE;
				af.bitvector= AFF_SHIELD;
				affect_to_char( vict, &af );

				vict->send("You stumble and lose your connection to the True Source!\r\n");
				vict->RemoveSource();
			}
		}
		else
		{
			//Failure.
			damage(ch, vict, 0, SKILL_BAREHANDS, -1);
			WAIT_STATE(ch, PULSE_VIOLENCE/2);
		}
	}

}

ACMD(do_rescue)
{
	Character *vict, *tmp_ch;
	int percent, prob;

	OneArgument(argument, arg);

	if (!(vict = get_char_room_vis(ch, arg)))
	{
		ch->send("Whom do you want to rescue?\r\n");
		return;
	}

	if(GET_RACE(vict) != GET_RACE(ch))
	{
		ch->send("Why would you rescue someone that is not of your kind?\r\n");
		return;
	}

	if (vict == ch)
	{
		ch->send("How about fleeing instead?\r\n");
		return;
	}

	if (FIGHTING(ch) == vict)
	{
		ch->send("How can you rescue someone you are trying to kill?\r\n");
		return;
	}

/*	if(too_scared(ch))
		return;
*/
	for (tmp_ch = ch->in_room->people; tmp_ch &&
	        (FIGHTING(tmp_ch) != vict); tmp_ch = tmp_ch->next_in_room)
		;

	if (!tmp_ch)
	{
		Act("But nobody is fighting $M!", FALSE, ch, 0, vict, TO_CHAR);
		return;
	}

	if (!GET_SKILL(ch, SKILL_RESCUE))
		ch->send("But you have no idea how!\r\n");

	else
	{
		percent = MiscUtil::random(1, 101);	/* 101% is a complete failure */
		prob = GET_SKILL(ch, SKILL_RESCUE);

		if (percent > prob)
		{
			ch->send("You fail the rescue!\r\n");
			return;
		}

		ch->send("Banzai!  To the rescue...\r\n");
		Act("You are rescued by $N, you are confused!", FALSE, vict, 0, ch, TO_CHAR);
		Act("$n heroically rescues $N!", FALSE, ch, 0, vict, TO_NOTVICT);

		if (FIGHTING(vict) == tmp_ch)
			vict->StopFighting();

		if (FIGHTING(tmp_ch))
			tmp_ch->StopFighting();

		if (FIGHTING(ch))
			ch->StopFighting();

		ch->SetFighting(tmp_ch);
		if(!FIGHTING(tmp_ch))
			tmp_ch->SetFighting(ch);

		WAIT_STATE(vict, 2 * PULSE_VIOLENCE);
	}
}


void perform_kick(Character *ch, Character *vict)
{
	Character *p = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;
	int off = 0, def = 0;

	if(!vict || !ch)
		return;

	vict = faceoff(ch, vict);
	off = MiscUtil::random(0, GET_SKILL(ch, SKILL_KICK));
	def = vict->Dodge() / 4 + GET_LEVEL(vict);

	if ( def > off )
		damage(ch, vict, 0, SKILL_KICK, -1);

	else
	{
		move_damage(ch, vict, GET_SKILL(ch, SKILL_KICK) / 5  * GET_MOVE(vict) / (GET_MAX_MOVE(vict) + 1), GET_SKILL(ch, SKILL_KICK) / 4  * GET_MOVE(vict) / (GET_MAX_MOVE(vict) + 1));
		hit(ch, vict, SKILL_KICK);
		if(vict->timer && room_visibility(ch, vict))
			vict->InterruptTimer();
	}

	GET_TARGET(ch) = NULL;
	WAIT_STATE(ch, PULSE_VIOLENCE);

	if(FIGHTING(ch) && (p = faceoff(ch, vict)) && !PreFighting)
	{
		ch->StopFighting();
		ch->SetFighting(p);
	}
}

ACMD(do_kick)
{
	Character *vict = 0, *p = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	if (!GET_SKILL(ch, SKILL_KICK))
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	OneArgument(argument, arg);

	/*Added by Fogel 5/1/09 to fix targeting*/
	if ((!*arg && !FIGHTING(ch)) || (!get_char_room_vis(ch, arg) && !FIGHTING(ch)))
	{
		ch->send("Kick who?\r\n");
		ch->WaitState(ch->wait + 1);
		return;
	}

	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL))
	{
		ch->send("This room just has such a peaceful, easy feeling...\r\n");
		return;
	}

	if(!*arg && FIGHTING(ch) && ch->in_room == FIGHTING(ch)->in_room)
		vict = FIGHTING(ch);
	else
		vict = get_char_room_vis(ch, arg);

	GET_TARGET(ch) = vict;

	if (vict == ch)
	{
		ch->send("Aren't we funny today...\r\n");
		return;
	}
	/* Added by Galnor on 05/07/2009 - fixes some crashes we had */
	if( vict == NULL )
	{
		ch->send(NOPERSON);
		return;
	}
	if( GET_POS(vict) >= POS_FLYING )
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}

	if (FIGHTING(ch) && FIGHTING(ch) != vict && FIGHTING(vict) != ch)
	{
		ch->send("You can't kick someone you aren't fighting!\r\n");
		return;
	}

	if(too_scared(ch, SKILL_KICK))
	{
		Act("Your eyes meet $N's as you attempt to kick $M, unnerving you and causing you to fall down!", TRUE, ch, NULL, vict, TO_CHAR);
		Act("You stare down $n as $e attempts to kick you, causing $m to fall to the ground!", TRUE, ch, NULL, vict, TO_VICT);
		Act("$n tries to kick $N, but changes $s mind after making eye contact!", TRUE, ch, NULL, vict, TO_NOTVICT);
		WAIT_STATE(ch, PULSE_VIOLENCE);
		if((p = faceoff(ch, vict)) && FIGHTING(ch) && !PreFighting)
		{
			ch->StopFighting();
			ch->SetFighting(p);
		}
		return;
	}

	if(!vict->ShieldBlock)
		perform_kick(ch, vict);
	else
		perform_shieldblock(vict, ch, SKILL_KICK, 0);
	
} 

/* 6/1/09 by Fogel - Precise Strike, a combat skill for thieves. */
void perform_precisestrike(Character *ch, Character *vict)
{
	Character *p = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	if(!vict || !ch)
		return;
	
	if( !FIGHTING(ch) )
		vict = faceoff(ch, vict);

	GET_TARGET(ch) = NULL;
	
	if (!ch->rollPrecStrike( vict ))
		damage(ch, vict, 0, SKILL_PRECISESTRIKE, -1);
	else
	{
		hit(ch, vict, SKILL_PRECISESTRIKE);
		ch->WaitState( PULSE_VIOLENCE / 3 );
	}

	if(FIGHTING(ch) && (p = faceoff(ch, vict)) && !PreFighting)
	{
		ch->StopFighting();
		ch->SetFighting(p);
	}
}

bool Character::rollPrecStrike( Character *Victim )
{
	int chance = MiscUtil::random(1,20);

	if ( chance == 20 )		// "critical hit", guaranteed landing
		return true;
	if ( chance == 1 )		// "critical miss", guaranteed failure
		return false;
	if ( Victim->PrecStrikeDefenseRoll() > this->PrecStrikeOffenseRoll() )
		return false;

	return true;
}

bool Character::rollPrecStrike( Character *Victim, int &off, int &def )
{
	int chance = MiscUtil::random(1,20);
	off = this->PrecStrikeOffenseRoll();
	def = Victim->PrecStrikeDefenseRoll();
	
	if ( chance == 20 )		// "critical hit", guaranteed landing
		return true;
	if ( chance == 1 )		// "critical miss", guaranteed failure
		return false;
	if ( def > off )
		return false;

	return true;
}

int Character::PrecStrikeOffenseRoll()
{
	float off = 0.00f;

	off += this->Offense() * PreciseStrikeData.O_OBMultiplier;
	off *= MiscUtil::random( 1, PreciseStrikeData.O_OffenseRollFactor ) / PreciseStrikeData.O_OffenseRollFactor;
	off *= (float)GET_SKILL(this, SKILL_PRECISESTRIKE) / 100;
	off += PreciseStrikeData.O_WeightMultiplier * GET_EQ(this, WEAR_WIELD)->Weight();
	off *= BodyPercents[this->ps_tgt] / BodyPercents[WEAR_BODY];

	return off;
}

int Character::PrecStrikeDefenseRoll()
{
	float def = 0.00f;

	def += PreciseStrikeData.D_BaseDB;
	def += this->Dodge() * PreciseStrikeData.D_DBMultiplier;
	def += this->Parry() * PreciseStrikeData.D_PBMultiplier;
	
	//if( IS_BASHED(this) )
	//	def *= PreciseStrikeData.D_BashedMultiplier;

	return def;
}

ACMD(do_precisestrike)
{//TODO: Clean it up
	Character *vict = 0;
	int q = 0;	
	bool BodyPartSpecified = false;

	if (!GET_SKILL(ch, SKILL_PRECISESTRIKE))
	{
		ch->send("You aren't skilled enough to do this!\r\n");
		return;
	}

	if (!GET_EQ(ch, WEAR_WIELD))
	{
		ch->send("How do you plan on doing this without a weapon?!\r\n");
		return;
	}

	if (ROOM_FLAGGED(ch->in_room, ROOM_PEACEFUL))
	{
		ch->send("This room just has such a peaceful, easy feeling...\r\n");
		return;
	}

	TwoArguments(argument, buf, buf1);

	if(!*buf && !*buf1 && !FIGHTING(ch))
	{
		ch->send(NOPERSON);
		ch->ps_tgt = -1;
		return;
	}
	else if(!*buf && !*buf1 && FIGHTING(ch) && ch->in_room == FIGHTING(ch)->in_room)
	{
		vict = FIGHTING(ch);
		ch->ps_tgt = RandomBodyPart();
	}
	else if(!*buf1 && FIGHTING(ch) && ch->in_room == FIGHTING(ch)->in_room && (q = FindBodyPart(buf)) != -1)
	{
		vict = FIGHTING(ch);
		ch->ps_tgt = q;
	}
	else if(!*buf1 && FIGHTING(ch) && (q = FindBodyPart(buf)) == -1)
	{
		vict = get_char_room_vis(ch, buf);
		ch->ps_tgt = RandomBodyPart();
	}
	else if(FIGHTING(ch) && ch->in_room == FIGHTING(ch)->in_room && (q = FindBodyPart(buf1)) != -1)
	{
		vict = get_char_room_vis(ch, buf);
		ch->ps_tgt = q;
	}
	else if(!*buf1 && !FIGHTING(ch) && (q = FindBodyPart(buf)) == -1)
	{	
		vict = get_char_room_vis(ch, buf);
		ch->ps_tgt = RandomBodyPart();
	}
	else if(!FIGHTING(ch) && (q = FindBodyPart(buf1)) != -1)
	{
		vict = get_char_room_vis(ch, buf);
		ch->ps_tgt = q;
	}

	if( GET_TARGET(ch) )
		vict = GET_TARGET(ch);
	else
		GET_TARGET(ch) = vict;

	if(!room_visibility(ch, vict))
	{
		ch->send("They must have left already...\r\n");
		ch->ps_tgt = -1;
		return;
	}

	if( vict == NULL )
	{
		ch->send(NOPERSON);
		ch->ps_tgt = -1;
		return;
	}

	if (vict == ch)
	{
		ch->send("You can only strike others who are in the room!\r\n");
		ch->ps_tgt = -1;
		return;
	}

	if (ch->ps_tgt == -1)
	{
		ch->send("That spot is impossible to hit!\r\n");
		ch->ps_tgt = -1;
		return;
	}

	if (FIGHTING(ch) && FIGHTING(ch) != vict && FIGHTING(vict) != ch)
	{
		ch->send("You can't strike someone you aren't fighting!\r\n");
		ch->ps_tgt = -1;
		return;
	}
	if(GET_POS(vict) == POS_FLYING)
	{
		ch->send("They're too high off the ground to reach!\r\n");
		ch->ps_tgt = -1;
		return;
	}

	if(!ch->command_ready)
	{
		if(too_scared(ch, SKILL_PRECISESTRIKE))
		{
			Act("You prepare to strike at $N, but briefly hesitate in fear as you see $S eyeless gaze upon you!", TRUE, ch, NULL, vict, TO_CHAR);
			Act("$n begins to approach you, but hesitates briefly as you stare $m down!", TRUE, ch, NULL, vict, TO_VICT);
			Act("$n begins to attack $N, but delays $s approach out of fear from $N's eyeless stare!", TRUE, ch, NULL, vict, TO_NOTVICT);
			ch->timer += .5f;
		}
		ch->command_ready = true;
	}
	else
	{
		perform_precisestrike(ch, vict);
		ch->ps_tgt = -1;
	}
}

/* 6/8/09 by Fogel - Shield Block, a defensive skill for those practiced in Shield Parry */
//	Defense for Shield Block is rolled by the attacker, as he is trying to defend from Shield Block
int Character::ShieldBlockDefenseRoll( int attack_skill )
{
	int def = 0;

	switch( attack_skill )
	{
	case SKILL_CHARGE:
		//	ChargeOffenseRoll will, in most situations, give us a number between 100-200. This is pretty close
		//	to the range for ShieldBlockOffenseRoll, so we'll use it and approximate a 50% success rate.
		def += this->ChargeOffenseRoll( attack_skill );
		break;
	case SKILL_BASH:
		//	Shield Block's offense roll is a bit more than half of a BashOffenseRoll from a good weapon, so we
		//	use a fraction of BashOffenseRoll as this character's saving roll
		def += this->BashOffenseRoll() *.65;
		break;
	case SKILL_KICK:
		//	We want to use kick as a counter to Shield Block, so we give it a fairly high roll.
		def += GET_SKILL(this, SKILL_KICK);
		def += MiscUtil::random(50, 150);
		break;
	default:
		//	This shouldn't ever be reached
		MudLog(CMP, LVL_GOD, TRUE, "Default case reached in Shield Block, notify a coder!");
		break;
	}
	return def;
}

//	Offense is rolled by the person using Shield Block
int Character::ShieldBlockOffenseRoll()
{
	int off = 0;

	off += GET_EQ(this, WEAR_SHIELD)->GetTotalParry();
	off += GET_EQ(this, WEAR_SHIELD)->GetTotalDodge();
	off += MIN( GET_LEVEL(this), 30 );
	off += MiscUtil::random( 0, 100 );
	off *= (GET_SKILL(this, SKILL_PARRY) / 100.0);

	if( AFF_FLAGGED(this, AFF_SHIELD_BLOCK) )
		for(affected_type *aff = this->affected; aff; aff = aff->next)
			if( aff->bitvector == AFF_SHIELD_BLOCK )
				off -= 10 * aff->modifier;

	return off;
}

bool Character::rollShieldBlock( Character *attacker, int attack_type )
{
	if( this->ShieldBlockOffenseRoll() > attacker->ShieldBlockDefenseRoll( attack_type ) )
		return true;

	return false;
}

void perform_shieldblock(Character *ch, Character *vict, int attack_type, int subcmd)
{
	Character *p = 0;
	int	skill = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	ch->ShieldBlock = false;

	if( ch->rollShieldBlock( vict, attack_type ) )
	{//	Landed. Varied effects may be added later, but for now vict will always have a bashed state of PULSE_VIOLENCE applied in damage()
		if( !AFF_FLAGGED( ch, AFF_SHIELD_BLOCK ) )
			apply_shieldblock( ch );
		else
			//We always want SHIELD BLOCK's modifier to be greater than the duration, so that we don't have a tic where the effect is present with no modifier.
			for(affected_type *aff = ch->affected; aff; aff = aff->next)
				if( aff->bitvector == AFF_SHIELD_BLOCK )
				{
					aff->modifier++;
					aff->duration = ceil( aff->modifier / 2.0 ) - 1;
				}
		ch->CancelTimer( false );
		hit(ch, vict, SKILL_PARRY);

		if(FIGHTING(ch) && (p = faceoff(ch, vict)) && !PreFighting)
		{
			ch->StopFighting();
			ch->SetFighting(p);
		}
	}
	else
	{//	Missed. Varied effects may be added later, but for now the user will just eat the skill they were trying to block.
		ch->CancelTimer( false );
		hit(vict, ch, attack_type);
	}
}

ACMD(do_shieldblock)
{
	Character *vict = 0;	

	if(!GET_EQ(ch, WEAR_SHIELD))
	{
		ch->send("You need to be using a shield for this to work.\r\n");
		return;
	}

	if(!ch->GetSkillLevel(SKILL_PARRY))
	{
		ch->send("You don't know how to do this!\r\n");
		return;
	}

/*	if(too_scared(ch))
		return;
*/
	if(!ch->command_ready)
	{
		ch->command_ready = true;
		ch->ShieldBlock = true;
		Act("You raise your shield and brace yourself in preparation for an oncoming attack.", FALSE, ch, NULL, vict, TO_CHAR);
		Act("$n hefts $s shield and takes up a defensive stance.", FALSE, ch, NULL, vict, TO_NOTVICT);
	}
	else
	{
		ch->ShieldBlock = false;
		Act("You lower your shield, having failed to anticipate an attack.", FALSE, ch, NULL, vict, TO_CHAR);
		Act("$n lowers $s shield, becoming vulnerable once more.", FALSE, ch, NULL, vict, TO_NOTVICT);
		WAIT_STATE(ch, PULSE_VIOLENCE / 2);
	}
}

/* 6/8/09 by Fogel - Applies the "SHIELD BLOCK" status effect, which controls the negative effects accompanying use of Shield Block */
void apply_shieldblock(Character *ch)
{
	struct affected_type af;

	af.type = SKILL_PARRY;
	af.duration = 0;
	af.modifier = 1;
	af.location = APPLY_NONE;
	af.bitvector= AFF_SHIELD_BLOCK;
	affect_to_char( ch, &af );
}

/* 6/9/09 by Fogel - Applies the "SEVERE WOUND" status effect, which multiplies the victim's parry and dodge bonuses by .75 */
void apply_severewound(Character *ch, Character *vict)
{
	struct affected_type af;

	af.type = SKILL_BACKSTAB;
	af.duration = 2;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_SEVERE_WOUND;

	if( AFF_FLAGGED(vict, AFF_SEVERE_WOUND) )
		affect_from_char(vict, 0, AFF_SEVERE_WOUND);
	affect_to_char( vict, &af );

	Act("You have been severely wounded, you can no longer move around as quickly!", FALSE, vict, NULL, ch, TO_CHAR);
	Act("$n has been heavily wounded, noticeably slowing $s movement speed!", FALSE, vict, NULL, ch, TO_VICT);
	Act("$N deeply wounds $n, who appears to no longer move as quickly!", FALSE, vict, NULL, ch, TO_NOTVICT);
}

/* 6/28/09 by Fogel - Hamstring, a skill for Blademasters. Decreases the victim's dodge bonus and movement regen. */
ACMD(do_hamstring)
{
	Character *vict = 0;

	if (!GET_SKILL(ch, SKILL_HAMSTRING) || !IS_BLADEMASTER(ch))
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	if (GET_EQ(ch, WEAR_WIELD) && GET_OBJ_VAL( GET_EQ(ch, WEAR_WIELD), 0 ) != WEAPON_LONG_BLADE)
	{
		ch->send("You need to be wielding a sword to do that!\r\n");
		return;
	}

	OneArgument(argument, arg);

	if ((!*arg && !FIGHTING(ch)) || (!get_char_room_vis(ch, arg) && !FIGHTING(ch)))
	{
		ch->send("Who do you wish to do this to?\r\n");
		ch->WaitState(ch->wait + 1);
		return;
	}

	if (!*arg && FIGHTING(ch))
		vict = FIGHTING(ch);
	else
		vict = get_char_room_vis(ch, arg);

	if (vict == ch)
	{
		ch->send("Why would you want to hamstring yourself?\r\n");
		return;
	}
	
	if( vict == NULL )
	{
		ch->send(NOPERSON);
		return;
	}
	if( GET_POS(vict) >= POS_FLYING )
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}

/*	if(too_scared(ch))
		return;
*/
	if(!ch->command_ready)
		ch->command_ready = true;
	else
		perform_hamstring(ch, vict);
}

void perform_hamstring(Character *ch, Character *vict)
{
	Character *p = 0;
	bool PreFighting = FIGHTING(ch) ? true : false;

	if(MiscUtil::random(0, vict->Dodge()) < GET_SKILL(ch, SKILL_HAMSTRING))
	{
		apply_hamstring(ch, vict);
		hit(ch, vict, SKILL_HAMSTRING);
		ch->GainNoQuit(vict);
		vict->GainNoQuit(ch);
		if(vict->timer && room_visibility(ch, vict))
			vict->InterruptTimer();
	}
	else
		damage(ch, vict, 0, SKILL_HAMSTRING, -1);

	if(FIGHTING(ch) && (p = faceoff(ch, vict)) && !PreFighting)
	{
		ch->StopFighting();
		ch->SetFighting(p);
	}
}

/* 6/28/09 by Fogel - Applies the new Hamstring effect */
void apply_hamstring(Character *ch, Character *vict)
{
	struct affected_type af;

	af.type = SKILL_HAMSTRING;
	af.duration = 3;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_HAMSTRING;
	if( AFF_FLAGGED(vict, AFF_HAMSTRING) )
		affect_from_char(vict, 0, AFF_HAMSTRING);
	affect_to_char( vict, &af );
}

ACMD(do_whirlwind)
{
	if (!GET_SKILL(ch, SKILL_WHIRLWIND) || !IS_BLADEMASTER(ch))
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	if (GET_EQ(ch, WEAR_WIELD) && GET_OBJ_VAL( GET_EQ(ch, WEAR_WIELD), 0 ) != WEAPON_LONG_BLADE)
	{
		ch->send("You need to be wielding a sword to do that!\r\n");
		return;
	}

	if (MOUNT(ch))
	{
		ch->send("You can't do this while mounted!\r\n");
		return;
	}

	OneArgument(argument, arg);

	if (!FIGHTING(ch))
	{
		ch->send("You spin around, making yourself quite dizzy.\r\n");
		return;
	}

	
/*	if(too_scared(ch))
		return;
*/
	if(!ch->command_ready)
		ch->command_ready = true;
	else
		perform_whirlwind(ch);
}

void perform_whirlwind(Character *ch)
{
	Character *vict = 0, *temp;

	if (GET_SKILL(ch, SKILL_WHIRLWIND) > MiscUtil::random(0, 125))
	{
		Act( "You perform Apple Blossoms in the Wind, quickly slashing all of your opponents!", TRUE, ch, NULL, vict, TO_CHAR );
		Act( "$n spins quickly as $e lashes out at $s opponents with Apple Blossoms in the Wind.", TRUE, ch, NULL, vict, TO_NOTVICT );
		for ( vict = ch->in_room->people; vict; vict = temp )
		{
			temp = vict->next_in_room;
			if ( FIGHTING( vict ) == ch || vict == FIGHTING( ch ) )
			{
				Act( "$n quickly slashes at you before pivoting to face $s next opponent.", TRUE, ch, NULL, vict, TO_VICT );
				damage(ch, vict, 5, SKILL_WHIRLWIND, -1);
				/*if(vict->timer && room_visibility(ch, vict) && MiscUtil::random(0, 200) > GET_SKILL(ch, SKILL_WHIRLWIND))*/
				vict->InterruptTimer();
				ch->GainNoQuit(vict);
				vict->GainNoQuit(ch);
			}
		}
	}
	else
	{
		Act( "You spin around attempting to perform Apple Blossoms in the Wind, but fall flat on your face!", TRUE, ch, NULL, vict, TO_CHAR );
		Act( "$n falls to the ground as $s spinning attack fails to hit anyone!", TRUE, ch, NULL, vict, TO_ROOM );
		WAIT_STATE(ch, PULSE_VIOLENCE * .5);
	}

}

ACMD(do_invert)
{
	Weave *weave = WeaveManager::GetManager().GetWeave("Invert");

	if( !ch->ChannelingAbility() && !IS_NPC(ch) && GET_LEVEL(ch) < LVL_APPR )
	{
		ch->send("You have no idea how to channel the One Power.\r\n");
		return;
	}

	if( !weave || GET_SKILL(ch, weave->getVnum()) <= 0 )
	{
		ch->send("You don't know how to do that.\r\n");
		return;
	}
	
	if( !PRF_FLAGGED(ch, PRF_SOURCE) )
	{
		ch->send("You are not in touch with the True Source.\r\n");
		return;
	}

	if( AFF_FLAGGED(ch, AFF_SHIELD) || ShieldManager::GetManager().ShieldedBy(ch) != NULL )
	{
		ch->send("Your connection to the True Source is blocked!\r\n");
		return;
	}

	if( ch->InvertNextWeave )
	{
		Act( weave->getAttribute( "ToCharFail" ).c_str(), TRUE, ch, NULL, ch, TO_CHAR);
		return;
	}

	if(!ch->command_ready)
		ch->command_ready = true;
	else
	{
		if( MiscUtil::random(0,105) > GET_SKILL(ch, weave->getVnum()) )
		{
			ch->send("You lose your concentration.\r\n");
			return;
		}
		ch->InvertNextWeave = true;
		ch->points.mana = MAX(0, ch->points.mana - mag_manacost(ch, WeaveManager::GetManager().GetWeave("Invert")->getVnum()));
		Act( weave->getAttribute( "ToChar" ).c_str(), TRUE, ch, NULL, ch, TO_CHAR);
	}
}

ACMD(do_shadowstep)
{
	Character *vict = 0;

	if( GET_CLASS(ch) != CLASS_GREYMAN )
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	OneArgument(argument, arg);

	if ( !get_char_room_vis(ch, arg) )
	{
		ch->send("Who do you wish to do this to?\r\n");
		ch->WaitState(ch->wait + 1);
		return;
	}
	
	if( !*arg )
		vict = 0;
	else
		vict = get_char_room_vis(ch, arg);

	if (vict == ch)
	{
		ch->send("What would be the point of that?\r\n");
		return;
	}
	
	if( vict && GET_POS(vict) >= POS_FLYING )
	{
		ch->send("They're too high off the ground to reach!\r\n");
		return;
	}

	if(!ch->command_ready)
	{
		ch->timer += (FLEE_LAG(ch) / 4);
		ch->command_ready = true;
	}
	else
		perform_shadowstep(ch, vict);
}

void perform_shadowstep(Character *ch, Character *vict)
{
	Character *temp;

	if( !vict )
	{
		if( ch->IsPurged() )
			return;

		if ( GET_POS(ch) < POS_FIGHTING )
		{
			ch->send("You are in pretty bad shape, unable to flee!\r\n");
			return;
		}

		if ( FIGHTING(ch) )
		{
			ch->StopFighting();
			for( temp = ch->in_room->people; temp; temp = temp->next_in_room )
			{
				if( FIGHTING(temp) == ch )
					temp->SetNewFighting();
			}
		}

		Act( "You step back into the shadows.", TRUE, ch, NULL, vict, TO_CHAR );
		Act( "$n steps into the shadows, disengaging from combat.", TRUE, ch, NULL, ch, TO_NOTVICT );
	
		if ( MiscUtil::random( 1, 101 ) < GET_SKILL( ch, SKILL_HIDE ) && !ROOM_FLAGGED(ch->in_room, ROOM_NOHIDE))
			SET_BIT_AR( AFF_FLAGS( ch ), AFF_HIDE );
		else
			ch->send("You can't find a good place to hide!\r\n");
	}
	else
	{
		FIGHTING(ch) = vict;
		damage(ch, vict, 0, SKILL_SHADOWSTEP, -1);
		Act("You slip through the shadows and begin to attack $N.", FALSE, ch, 0, vict, TO_CHAR);
		Act("$n	pops out of the shadows and begins to attack you!", FALSE, ch, 0, vict, TO_VICT);
		Act("$n slips through the shadows and begins to attack $N!", FALSE, ch, 0, vict, TO_NOTVICT);

	}

	FLEE_LAG(ch) += (PULSE_VIOLENCE * 2);
	//FLEE_LAG(ch) += FleeData.PulsesPerEngagedFlee;
	//FLEE_LAG(ch) = MIN(FleeData.MaxFleeLag, FLEE_LAG(ch));
}

ACMD(do_pulverize)
{
	if (!GET_SKILL(ch, SKILL_PULVERIZE) || !IS_OGIER(ch))
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	if( ch->pulverizeCooldown > 0 )
	{
		std::stringstream Buffer;
		Buffer << "You must wait another " << ch->pulverizeCooldown << " seconds before you can do that again.\r\n";
		ch->send(Buffer.str());
		return;
	}

	OneArgument(argument, arg);
	
/*	if(too_scared(ch))
		return;
*/
	if(!ch->command_ready)
		ch->command_ready = true;
	else
		perform_pulverize(ch);
}

void perform_pulverize(Character *ch)
{
	Character *vict = 0, *temp;

	if (GET_SKILL(ch, SKILL_PULVERIZE) > MiscUtil::random(0, 200))
	{
		Act( "You violently stomp the ground, stunning nearby opponents!", TRUE, ch, NULL, vict, TO_CHAR );
		Act( "$n violently stomps the ground, sending $s opponents flying!", TRUE, ch, NULL, vict, TO_NOTVICT );
		for ( vict = ch->in_room->people; vict; vict = temp )
		{
			temp = vict->next_in_room;
			if ( FIGHTING( vict ) == ch || vict == FIGHTING( ch ) )
			{
				Act( "$n violently stomps the ground, the force of which sends you sprawling!", TRUE, ch, NULL, vict, TO_VICT );
				damage(ch, vict, 1, SKILL_PULVERIZE, -1);
				/*if(vict->timer && room_visibility(ch, vict) && MiscUtil::random(0, 200) > GET_SKILL(ch, SKILL_WHIRLWIND))*/
				FLEE_LAG(vict) += FleeData.PulsesPerEngagedFlee * 2;
				vict->InterruptTimer();
				ch->GainNoQuit(vict);
				vict->GainNoQuit(ch);
				ch->pulverizeCooldown = 5.0f;
			}
		}
	}
	else
	{
		Act( "You try to stomp the ground, but your opponents jump out of the way!", TRUE, ch, NULL, vict, TO_CHAR );
		Act( "You jump out of the way as $n tries to stun you by stomping the ground in front of $m!", TRUE, ch, NULL, vict, TO_VICT );
		Act( "$n stomps the ground, but doesn't manage to stun anyone around $m.", TRUE, ch, NULL, vict, TO_NOTVICT);
		WAIT_STATE(ch, PULSE_VIOLENCE * .5);
	}

}

/****** Melee Functions Below ******/

/* Generate the character's total 'difficulty' -- Galnor 07/20/2009 */
int Character::Difficulty()
{
	/**********
	 *
	 *	Let's have one defense(dodge or parry) be the base, here. It has a value of one.
	 *  Everything else will be based upon that.
	 *
	 **********/

	float fOffense = 0.0f, fDefense = 0.0f, fAvgDam;
	float fAbsFactor    = 2.5;
	float fMaxHitFactor = 6.5;
	float fOffenseFactor= 1.75;//This is a very hard one because it is inconsistent.
	float fDamageFactor = 4.0;

	fDefense = (this->Dodge() + this->Parry());
	fDefense+= (this->Absorb() * fAbsFactor);
	fDefense+= (this->points.max_hit * fMaxHitFactor);

	//fAvgDam is the average melee damage that will be dealt by the mob. This is an estimation.
	fAvgDam = this->points.damroll;
	if( this->equipment[WEAR_WIELD] ) {
		fAvgDam += ((equipment[WEAR_WIELD]->GetTotalVal1() + equipment[WEAR_WIELD]->GetTotalVal2()) / 2);
	}
	else {
		if( IS_MOB(this) )
			fAvgDam += ( ((float)this->MobData->damnodice * this->MobData->damsizedice) / 2 );
		else
			fAvgDam += ( ((float)this->GetStr() / 4) / 2 );
	}

	/******
	 *	0 offensive bonus & 25 avg damage is weak. Similar if reversed.
	 *	We'll want a product to combine them properly.
	 *
	 *	Similarly, we want fOffense to be proportional to fDefense.
	 *
	 *  150 offense & 23 avg damage = 3450
	 *  
	 *	OB is more valuable than defense because it affects bash, which is a
	 *     very commonly used offensive skill.
	******/

	fOffense = (this->Offense() * fOffenseFactor) * static_cast<int>(fAvgDam*fDamageFactor);

	return fOffense + fDefense;
}

/* Find Dodging Bonus -- Galnor */
int Character::Dodge()
{
	int dodge = 0;
	int i;
	int add;
	float divide, db, result, lPercent;

	if(GET_POS(this) <= POS_SLEEPING)
		return 0;	//Ain't goin anywhere, bud!

	add = (GetDex() - 10) * 3;

	dodge = GET_DB(this);
	dodge = DB_MAX_BASE;
	lPercent = .45f + ((float)player.level / (float)30) * 0.55f;
	lPercent = MIN(1.00f, lPercent);

	dodge += GET_DB(this);
	dodge += add;
	dodge -= int((held_and_back(this) + this->CarriedWeight()) / 10);
	dodge += MoodDodge();

	/* Get equipment bonuses */
	for(i = 0;i < NUM_WEARS;++i)
	{
		if(GET_EQ(this, i))
		{
			if( this->isInClan( GET_OBJ_CLAN( GET_EQ(this, i) ) ) )
				dodge += GET_OBJ_CL_DB(GET_EQ(this, i));
			//Galnor 11/16/2009 - Requested by Koridin. Prevent weapons from providing bonuses to non-wielded slots.
			if( GET_OBJ_TYPE(GET_EQ(this, i)) == ITEM_WEAPON && i != WEAR_WIELD )
				continue;
			dodge += GET_EQ(this, i)->GetTotalDodge();
		}
	}

	/* Chop off some for those who are sitting or below. */
	if(GET_POS(this) < POS_FIGHTING)
		dodge = (int) ((double) dodge * DB_SITTING_MALUS);

	/* Now we chop it up according to the player's skill in dodge */
	divide = (float)dodge;
	db = GET_SKILL(this, SKILL_DODGE);
	result = db / 100;

	dodge = (int) (divide * result);

	if(MeleeData.PoundsPerDodge)
		dodge -= int( (this->CarriedWeight() + this->WornWeight()) / MeleeData.PoundsPerDodge);
	dodge = MAX(dodge, 0);

	if(GET_LEVEL(this) == 1 && IS_NPC(this))
		dodge = 0;

	dodge *= lPercent;

	if(AFF_FLAGGED(this, AFF_AGILITY))
	{
		Weave* weave = WeaveManager::GetManager().GetWeave("Agility");
		if( weave )
			dodge += atoi(weave->getAttribute("DodgeBonus").c_str());
	}

	if( AFF_FLAGGED(this, AFF_DAZE) )
		dodge -= atoi( WeaveManager::GetManager().GetWeave("Daze")->getAttribute("DodgeDrop").c_str() );

	if (AFF_FLAGGED(this, AFF_SEVERE_WOUND))
		return dodge * .80;

	if (AFF_FLAGGED(this, AFF_HAMSTRING))
		return dodge - 5;
	else
		return dodge;
}


/* Find Offense Bonus -- Galnor */
int Character::Offense()
{
	int offense = 0;
	int i;
	float divide, ob, result, lPercent;
	Object *wielded;

	if(GET_POS(this) <= POS_SLEEPING)
		return 0;

	if(IS_NPC(this))
		offense += GET_OB(this);

	//This should have no effect on players at or above level 30,
	//however it should hinder players under that level to some extent.
	offense += OB_MAX_BASE;
	lPercent = .45f + ((float)player.level / (float)30) * 0.55f;
	lPercent = MIN(1.00f, lPercent);
	/****
		if(GET_LEVEL(this) >= 30)
			offense += OB_MAX_BASE;
		else
			offense += GET_LEVEL(this);
	****/

	for(i = 0;i < NUM_WEARS;++i)
	{
		if(GET_EQ(this, i))
		{
			//Galnor 11/16/2009 - Requested by Koradin. Prevent weapons from providing bonuses to non-wielded slots.
			if( GET_OBJ_TYPE(GET_EQ(this, i)) == ITEM_WEAPON && i != WEAR_WIELD )
				continue;
			offense += GET_EQ(this, i)->GetTotalOffense();
			if( this->isInClan( GET_OBJ_CLAN( GET_EQ(this, i) ) ) )
				offense += GET_OBJ_CL_OB(GET_EQ(this, i));
		}
	}

	offense += GetStr();

	offense -= int((held_and_back(this) + this->CarriedWeight()) / 10);

	/* From here we find the value by taking their skill in their weapon class and applying that */
	divide = (float)offense;
	wielded = GET_EQ(this, WEAR_WIELD);

	if(wielded)
		ob = (float)weapon_skill(this, GET_OBJ_VAL(wielded, 0));
	else
		ob = GET_SKILL(this, SKILL_BAREHANDS);

	result = ob / 100;

	/* And now we finish it off...*/

	offense = (int) (divide * result);

	if(MeleeData.PoundsPerOffense)
		offense -= int( (this->CarriedWeight() + this->WornWeight()) / MeleeData.PoundsPerOffense);
	offense = MAX(offense, 0);
	offense += MoodOffense();

	if((IS_FADE(this) && this->followers && this->check_if_pc_in_group() && this->group_members_in_zone()) || (!IS_NPC(this) && this->master && IS_FADE(this->master) && (this->in_room->GetZone() == this->master->in_room->GetZone())))
		offense += 5;

	offense *= lPercent;

	if(AFF_FLAGGED(this, AFF_CHILL))
		offense -= atoi( WeaveManager::GetManager().GetWeave("Chill")->getAttribute("OffenseDrop").c_str() );

	if(AFF_FLAGGED(this, AFF_FEAR))
		return offense * .9;
	else
		return offense;
}

int Character::MoodParry()
{
	int a = 0;
	int mood = MOOD_WIMPY;
	if(!IS_NPC(this))
		mood = this->PlayerData->mood;

	switch(mood)
	{
		case MOOD_WIMPY:
			a += PB_WIMPY;
			break;
		case MOOD_MILD:
			a += PB_MILD;
			break;
		case MOOD_BERSERK:
			a += PB_BERSERK;
			a += (int) ( ( 1 - (float)GET_HIT(this) / (float)GET_MAX_HIT(this) ) * PB_BERSERK * 2 );
			break;
	}

	if (IS_BLADEMASTER(this))
	{
		switch(this->stance)
		{
			case STANCE_DEFENSIVE:
				a += PB_STANCE_DEFENSIVE;
				break;
			case STANCE_NORMAL:
				a += PB_STANCE_NORMAL;
				break;
			case STANCE_OFFENSIVE:
				break;
		}
	}

//	if(IsMaster())
//		a += PB_MASTER;

	return a;
}

int Character::MoodDodge()
{
	int a = 0;
	int mood = MOOD_WIMPY;
	if(!IS_NPC(this))
		mood = this->PlayerData->mood;

	switch(mood)
	{
		case MOOD_BRAVE:
			a -= DB_BRAVE_MALUS;
			break;
		case MOOD_BERSERK:
			a -= DB_BERSERK_MALUS;
			a -= (int) ( ( 1 - (float)GET_HIT(this) / (float)GET_MAX_HIT(this) ) * DB_BERSERK_MALUS );
			break;
	}

	if(this->player.mount)
		a -= DB_RIDING_MALUS;

	if(GET_POS(this) <= POS_SITTING)
		a = (int) (a * DB_SITTING_MALUS);

//	if(IsMaster())
//		a += PB_MASTER;

	return a;
}

int Character::MoodOffense()
{
	int a = 0;
	int mood = MOOD_WIMPY;
	if(!IS_NPC(this))
		mood = this->PlayerData->mood;

	switch(mood)
	{
		case MOOD_MILD:
			a += OB_MILD;
			break;
		case MOOD_BRAVE:
			a += OB_BRAVE;
			break;
		case MOOD_BERSERK:
			a += OB_BERSERK;
			a += (int) ( ( 1 - (float)GET_HIT(this) / (float)GET_MAX_HIT(this) ) * 25 );
			break;
	}

	if (IS_BLADEMASTER(this))
	{	
		switch(this->stance)
		{
			case STANCE_DEFENSIVE:
				break;
			case STANCE_NORMAL:
				a += OB_STANCE_NORMAL;
				break;
			case STANCE_OFFENSIVE:
				a += OB_STANCE_OFFENSIVE;
				break;
		}
	}

	if(this->player.mount || this->getUserClan( CLAN_WOLFBROTHER ))
		a += OB_RIDING_BONUS;

	if(IS_TROLLOC(this) && !IS_FADE(this))
		a += OB_RIDING_BONUS / 2;

//	if(IsMaster())
//		a += OB_MASTER;

	return a;
}

/* Find Parrying Bonus -- Galnor */
int Character::Parry()
{
	int i;
	float parry = 0.0f, lPercent;
	Object *item;

	if(GET_POS(this) <= POS_SLEEPING)
		return 0;

	parry = GET_PB(this);
	lPercent = .45f + ((float)player.level / (float)30) * 0.55f;
	lPercent = MIN(1.00f, lPercent);

	/**** Galnor, 1-31-2005, Fixed this finally! ****/
	for(i = 0;i < NUM_WEARS;++i)
	{
		if((item = GET_EQ(this, i)))
		{
			//Galnor 11/16/2009 - Requested by Koradin. Prevent weapons from providing bonuses to non-wielded slots.
			if( GET_OBJ_TYPE(GET_EQ(this, i)) == ITEM_WEAPON && i != WEAR_WIELD )
				continue;
			if( this->isInClan( GET_OBJ_CLAN( GET_EQ(this, i) ) ) )
				parry += GET_OBJ_CL_PB(GET_EQ(this, i));
			if(i == WEAR_WIELD)
				parry += (item->GetTotalParry() * ((float) weapon_skill(this, GET_OBJ_VAL(item, 0)) / 100));
			else if(i == WEAR_SHIELD)
				parry += (item->GetTotalParry() * ((float) GET_SKILL(this, SKILL_PARRY) / 100)) + (this->GetStr()-17)*4;
			else
				parry += item->GetTotalParry();
		}
	}


	if(this->GetStr() < 19 && MeleeData.PoundsPerParry)
		parry -= int( (20 - this->GetStr()) * (this->CarriedWeight() + .5 * this->WornWeight()) / MeleeData.PoundsPerParry);
	else if(MeleeData.PoundsPerParry)
		parry -= int( (this->CarriedWeight() + this->WornWeight()) / MeleeData.PoundsPerParry);

	if(AFF_FLAGGED(this, AFF_BURN))
		parry -= atoi( WeaveManager::GetManager().GetWeave("Blaze")->getAttribute("ParryDrop").c_str() );

	//	Using Shield Block repeatedly decreases your parry
	if( AFF_FLAGGED(this, AFF_SHIELD_BLOCK) )
		for(affected_type *aff = this->affected; aff; aff = aff->next)
			if( aff->bitvector == AFF_SHIELD_BLOCK )
				parry -= 7 * aff->modifier;

	parry += MoodParry();
	parry += (GetStr() - 2) * 2;
	if(GET_EQ(this, WEAR_SHIELD))
		parry += GetDex() / 2;

	parry *= lPercent;
	if(GET_POS(this) < POS_FIGHTING)
		parry *= PB_SITTING_MALUS;
	parry = MAX(parry, 0);

	if (AFF_FLAGGED(this, AFF_SEVERE_WOUND))
		return (int)parry * .80;
	else
		return (int)parry;
}

float Object::AverageAbsorb(int i, Character *ch)
{
	float abs = (float)GetTotalAbsorb();
	if (ch->isInClan(GET_OBJ_CLAN(this)))
	{
		abs += GET_OBJ_CL_ABS(this);
	}
	if(IS_OGIER(ch))
		abs += 5;
	return (float)(abs * ((float)BodyPercents[i] / 100.00f));
}

/* Find Absorb Bonus -- Galnor */
int Character::Absorb()
{
	float abs = 0.00f;
	int i;

	for(i = 0;i < NUM_WEARS;++i)
	{
		if(GET_EQ(this, i))
		{
			abs += GET_EQ(this, i)->AverageAbsorb(i, this);
		}
	}

	return (int)(abs);
}

/* ************************************************************************
*   File: spell_parser.c                                Part of CircleMUD *
*  Usage: top-level magic routines; outside points of entry to magic sys. *
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
#include "interpreter.h"
#include "handler.h"
#include "comm.h"
#include "db.h"
#include "constants.h"
#include "weaves.h"
#include "clans.h"
#include "MiscUtil.h"
#include "Descriptor.h"
#include "rooms/Room.h"

#include "ClanUtil.h"

#include "items/ItemUtil.h"

/* local functions */
int mag_manacost( Character * ch, int spellnum );
//int mag_damage(int level, Character *ch, Character *victim, int spellnum, int savetype);
int room_visibility( Character *ch, Character *vict );




Character *randomTarget( Character *ch );
void sendElementMessages( Character *ch, Weave *weave );




// // Obsolete the day after it was made :p
// Character * FindTargetFromWeaveArgument(Character * ch, std::string arguments, Weave * w)
// {
// 	for (Character * v = ch->in_room->people; v;  v = v->next_in_room)
// 	{
// 		if (v->player.name == arguments)
// 			{
// 				return v;
// 			}
// 	}
// 	return 0;
// }

int mag_manacost( Character * ch, int spellnum )
{
	static float mana = 0.00f;
	Object *angreal;
	Weave* W = WeaveManager::GetManager().GetWeave(spellnum);

	if( !W ) return 0;

	mana = (float)atof(W->getAttribute("Cost").c_str());

	/* If player is carrying an angreal, subtract the mana cost. */
	if ( ( angreal = GET_EQ( ch, WEAR_HOLD ) ) )
	{
		if ( angreal->getType() == ITEM_ANGREAL && angreal->GetTotalVal2() != 0 )
		{
			/* Turn this into a percent. */
			mana *= ( ( float ) ( ( float ) angreal->GetTotalVal0() / 100 ) );
		}
	}

	return ( int ) mana;
}

int PerformWeave( Character* caster, Character* cvict, Object* ovict, char* argument, int spellnum )
{
	Weave* weave = WeaveManager::GetManager().GetWeave(spellnum);
	if ( !weave )
	{
		Log( "SYSERR: cast_spell trying to call spellnum %d.\n", spellnum );
		return 0;
	}
	if ( GET_POS( caster ) < weave->minPosition() )
	{
		switch ( GET_POS( caster ) )
		{
			case POS_SLEEPING:
				caster->send( "You dream about great magical powers.\r\n" );
				break;
			case POS_RESTING:
				caster->send( "You cannot concentrate while resting.\r\n" );
				break;
			case POS_SITTING:
				caster->send( "You can't do this sitting!\r\n" );
				break;
			case POS_FIGHTING:
				caster->send( "Impossible!  You can't concentrate enough!\r\n" );
				break;
			default:
				caster->send( "You can't do much of anything like this!\r\n" );
				break;
		}

		return 0;
	}

	//Does the caster fail the weave?
	if( MiscUtil::random(0,100) > GET_SKILL(caster, weave->getVnum()) )
	{
		caster->send("You lose your concentration.\r\n");
		return 0;
	}

	if ( ( cvict != caster ) && ( weave->targetSet("SelfOnly") ) )
	{
		caster->send( "You can only channel this spell upon yourself!\r\n" );
		return 0;
	}

	if ( ( cvict == caster ) && weave->targetSet("NotSelf") )
	{
		caster->send( "You cannot channel this spell upon yourself!\r\n" );
		return 0;
	}

	if ( weave->routineSet("Groups") && !AFF_FLAGGED( caster, AFF_GROUP ) )
	{
		caster->send( "You can't channel this spell if you're not in a group!\r\n" );
		return 0;
	}
	if (ROOM_FLAGGED(caster->in_room, ROOM_NOMAGIC))
	{
		caster->send("Your magic fizzles out and dies.\r\n");
		Act("$n's weave fizzles out and dies.", FALSE, caster, 0, 0, TO_ROOM);
		return 0;
	}
  
	if (ROOM_FLAGGED(caster->in_room, ROOM_PEACEFUL) &&
      (weave->isViolent() || weave->routineSet("Damage")))
	{
		caster->send( "A flash of white light fills the room, dispelling your violent weave!\r\n");
		Act("White light from no particular source suddenly fills the room, then vanishes.", 0, caster, 0, 0, TO_ROOM);
		return 0;
	}

	caster->send( OK );

	if( weave->routineSet("Damage") )
		mag_damage(caster, cvict, spellnum);
	if ( weave->routineSet("ManaDamage") )
		mag_mana_damage(caster, cvict, spellnum);
	if( weave->routineSet("Affects") )
		mag_affects(caster, cvict, spellnum);
	if( weave->routineSet("Points") )
		mag_points(caster, cvict, spellnum);
	if( weave->routineSet("Unaffects") )
		mag_unaffects(caster, cvict, spellnum);
	if( weave->routineSet("Creations") )
		mag_creations(caster, spellnum);
	if( weave->routineSet("AlterObjects") )
		mag_alter_objs(caster, ovict, spellnum);
	if( weave->routineSet("Areas") )
		mag_areas(caster, spellnum);
	if( weave->routineSet("Manual") )
	{
		if( weave->getName() == "Locate Object" )
		{
			MANUAL_SPELL(spell_locate_object);
		}
		else if( weave->getName() == "Locate Life" )
		{
			MANUAL_SPELL(spell_locate_life);
		}
		else if( weave->getName() == "Ward Weapon" )
		{
			MANUAL_SPELL(spell_ward_weapon);
		}
		else if( weave->getName() == "Gate" )
		{
			MANUAL_SPELL(spell_gate);
		}
		else if( weave->getName() == "Tornado" )
		{
			MANUAL_SPELL(spell_tornado);
		}
		else if( weave->getName() == "Regen" )
		{
			MANUAL_SPELL(spell_regen);
		}
		else if( weave->getName() == "Create Water" )
		{
			MANUAL_SPELL(spell_create_water);
		}
		else if( weave->getName() == "Eavesdrop" )
		{
			MANUAL_SPELL(spell_eavesdrop);
		}
		else if( weave->getName() == "Circle of Silence" )
		{
			MANUAL_SPELL(spell_circle_of_silence);
		}
		else if( weave->getName() == "Flare" )
		{
			MANUAL_SPELL(spell_flare);
		}
		else if( weave->getName() == "Delve" )
		{
			MANUAL_SPELL(spell_delve);
		}
		else if( weave->getName() == "Quicksand" )
		{
			MANUAL_SPELL(spell_quicksand);
		}
	}

	caster->points.mana = MAX(0, caster->points.mana - mag_manacost(caster, spellnum));

	return 1;
}

CommandHandler  do_cast  = DEFINE_COMMAND
{
	char* s;
	Character* cvict = 0;
	Object* ovict = 0;
	int spellnum;
	bool target = false;
	Weave* weave;

	s = strtok( argument, "'" );
	s = strtok( 0, "'" );//Used to figure out which weave is being channeled.

	if( !ch->ChannelingAbility() && !IS_NPC(ch) && GET_LEVEL(ch) < LVL_APPR )
	{
		ch->send("You have no idea how to channel the One Power.\r\n");
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

	if( !*argument )
	{
		ch->send("Channel what and where?\r\n");
		return;
	}
	if( !s )
	{
		ch->send("You must type the weave name surrounded by single quotes:\r\nchannel 'weave name' <target>\r\n");
		return;
	}

	weave = WeaveManager::GetManager().GetWeave(s);
	if( weave ) spellnum = weave->getVnum();

	if( !weave || GET_SKILL(ch, spellnum) <= 0 )
	{
		ch->send("You are unfamiliar with that weave.\r\n");
		return;
	}

	if( weave->disabled() )
	{
		ch->send("This weave has been disabled by the administration.\r\n");
		return;
	}

	if( ch->points.mana < MiscUtil::convert<int>(weave->getAttribute("Cost")) )
	{
		ch->send("You are too drained to complete the weave.\r\n");
		return;
	}

	s = strtok(0, "'");
	if(s) skip_spaces( &s );

	if( weave->targetSet("Ignore") ) //Weave requires no target.
		target = true;

	/*** Channeler's timer is up. Make sure that the target is still available. ***/
	else if( ch->command_ready )
	{
		cvict = ch->player.target;
		ovict = ch->player.otarget;

		if( cvict != NULL )
		{
			if( weave->targetSet("CharRoom") && ch->in_room == cvict->in_room && CAN_SEE(ch, cvict) )
				target = true;
			else if( weave->targetSet("CharWorld") && CAN_SEE(ch, cvict))
				target = true;
		}
		else if( ovict != NULL )
		{
			if( weave->targetSet("ObjInv") && ovict->carried_by == ch )
				target = true;
			else if( weave->targetSet("ObjRoom") && ovict->in_room == ch->in_room )
				target = true;
		}
	}
	/*** Attempt to find the weave target. ***/
	else if( s && !target ) //Did the user provide a target name?
	{
		char TName[MAX_INPUT_LENGTH];
		
		OneArgument( s, TName );

		if( weave->targetSet("CharRoom") && !target )
			target = (cvict = get_char_room_vis(ch, TName));
		if( weave->targetSet("CharWorld") && !target )
			target = (cvict = get_char_vis(ch, TName));
		if( weave->targetSet("ObjInv") && !target )
			target = (ovict = ItemUtil::get()->getObjectInListVis(ch, TName, ch->carrying));
		if( weave->targetSet("ObjEquip") && !target )
			target = (ovict = ItemUtil::get()->getObjectInEquipmentList(ch, TName));
		if( weave->targetSet("ObjRoom") && !target && ch->in_room )
			target = (ovict = ItemUtil::get()->getObjectInListVis(ch, TName, ch->in_room->contents));
		if( weave->targetSet("ObjWorld") && !target)
			target = (ovict = ItemUtil::get()->getObjectVis(ch, TName));
	}
	else //Weave needs a target, but no specification was given.
	{
		if( weave->targetSet("FightSelf") && FIGHTING(ch) && !target)
			target = (cvict = ch);
		if( weave->targetSet("FightVict") && FIGHTING(ch) && !target)
			target = (cvict = FIGHTING(ch));

		if( !target )
		{
			ch->send("Who do you wish to channel this weave upon?\r\n");
			return;
		}
		if( cvict == ch && weave->isViolent() )
		{
			ch->send("You want to use that weave on yourself? Are you mad?!\r\n");
			return;
		}
	}

	if( !target )
	{
		ch->send("Your target could not be found.\r\n");
		return;
	}

	if(weave->getName() == "Quicksand" && IS_BASHED(cvict))
	{
		ch->send("They already seem to be stunned!\r\n");
		return;
	}

	if (weave->getName() == "Eavesdrop" && (ch->in_room->getDistanceToRoom(cvict->in_room) > atoi(WeaveManager::GetManager().GetWeave("Eavesdrop")->getAttribute("MaxDistance").c_str())))
	{
		ch->send("Your target could not be found.\r\n");
		return;
	}
	/*** And that should be it! The target should either be found, or any conflict taken care of. ***/


	/** All prerequisites passed. Time to get on with the channeling. ***/
	if( !ch->command_ready )
	{
		ch->send("You begin channeling the appropriate flows...\r\n");
		sendElementMessages(ch, weave);
		ch->command_ready = true;
		ch->timer = MiscUtil::convert<float>(weave->getAttribute("Timer"));
		if(weave->getName() == "Gate")
			ch->setGateTimer();
		ch->player.target = cvict;
		ch->player.otarget = ovict;
		return;
	}
	else
	{
		cvict = ch->player.target;
		ovict = ch->player.otarget;
		if( cvict && AFF_FLAGGED(cvict, AFF_SLICE) )
		{
			Act(WeaveManager::GetManager().GetWeave("Slice")->getAttribute("SliceToChar").c_str(), TRUE, cvict, NULL, ch, TO_CHAR);
			Act(WeaveManager::GetManager().GetWeave("Slice")->getAttribute("SliceToVict").c_str(), TRUE, cvict, NULL, ch, TO_VICT);
			Act(WeaveManager::GetManager().GetWeave("Slice")->getAttribute("SliceToRoom").c_str(), TRUE, cvict, NULL, ch, TO_NOTVICT);
			affect_from_char(cvict, 0, AFF_SLICE);
			return;
		}
		PerformWeave(ch, cvict, ovict, s, spellnum);
	}
};

void sendElementMessages( Character *ch, Weave *weave )
{
	for( Character *temp = ch->in_room->people; temp; temp = temp->next_in_room )
		if( temp->ChannelingAbility() && ( GET_SEX(ch) == GET_SEX(temp) ) && ( ch != temp ) && !ch->InvertNextWeave )
		{
			std::string elements("You sense $N begin to channel a weave containing ");

			if(atoi(weave->getAttribute("Air").c_str()) > 0)
				elements.append( "Air, " );
			if(atoi(weave->getAttribute("Earth").c_str()) > 0)
				elements.append( "Earth, " );
			if(atoi(weave->getAttribute("Fire").c_str()) > 0)
				elements.append( "Fire, " );
			if(atoi(weave->getAttribute("Spirit").c_str()) > 0)
				elements.append( "Spirit, " );
			if(atoi(weave->getAttribute("Water").c_str()) > 0)
				elements.append( "Water, " );

			elements.erase( elements.length() - 2, 2 );
			elements.push_back( '.' );
			
			int pos = elements.find_last_of(',');
			if(  pos > 0 )
				elements.replace( pos, 1, " and" );

			Act(elements.c_str(), FALSE, temp, NULL, ch, TO_CHAR);
		}
}

const char *randomChannel[] =
    {
        "fireball",  /* 0 */
        "call lightning",
        "air scythe" /* 2 */
    };

int numberOfSpells = 2;

Character *randomTarget( Character *ch )
{
	int target, count = 0;
	Character *temp;

	for ( temp = ch->in_room->people; temp; temp = temp->next_in_room )
	{
		if ( room_visibility( ch, temp ) )
		{
			++count;
		}
	}
	target = MiscUtil::random( 1, count );

	for ( temp = ch->in_room->people; temp; temp = temp->next_in_room )
	{
		if ( room_visibility( ch, temp ) && --target == 0 )
		{
			return ( temp );
		}
	}

	return ( NULL ); /* should never get here */
}




void Character::AddStrain( int strain )
{
	if ( this->CanTaint() )
	{
		this->points.strain += strain;
		this->points.strain = MAX( 0, (int)this->points.strain );
	}
}

void Character::AddChannelingStrain( bool Failure, int spell )
{
	Weave* weave = WeaveManager::GetManager().GetWeave(spell);
	if( weave == NULL )
	{
		MudLog(BRF, LVL_APPR, TRUE, "SYSERR - Character::AddChannelingStrain(): Weave #%d does not exist.", spell);
		return;
	}

	// The command started successfully.  If MC add taint.  -Serai
	// The longer they channel (spell timer), the more taint.
	if ( this->CanTaint() )
	{
		int strain = 0;

		//Add strain dependant on mana cost and spell timer.
		strain += int( ( weave->getTimer() * TaintData.TimerCost ) + ( mag_manacost( this, spell ) * TaintData.ManaCost ) );
		strain *= int( Failure ? TaintData.FailureCost : 1 );

		if ( this->PlayerData->conditions[ FULL ] <= 4 || this->PlayerData->conditions[ THIRST ] <= 4 )
			strain += TaintData.HungerThirstCost;

		if ( this->PlayerData->conditions[ DRUNK ] >= 12 )
			strain += TaintData.HungerThirstCost;

		if ( this->getUserClan( CLAN_BLACK_TOWER ) )
			strain = int( float( strain ) * TaintData.BlackTowerBonus );

		this->AddStrain( strain );
	}
}

void Character::setGateTimer()
{
	UserClan *userClan;

	if(userClans.empty())
		this->timer += 20;
	else if( userClan = this->getUserClan( CLAN_WHITE_TOWER ) )
		this->timer += MAX(((5 - userClan->getRank()) * 4), 0);
	else if( userClan = this->getUserClan( CLAN_BLACK_TOWER ) )
		this->timer += MAX(((6 - userClan->getRank()) * 4), 0);
	else if( userClan = this->getUserClan( CLAN_DREADGUARDS ) )
		this->timer += MAX(((6 - userClan->getRank()) * 4), 0);
	else if( userClan = this->getUserClan( CLAN_ALGHOL ) )
		this->timer += MAX(((8 - userClan->getRank()) * 4), 0);
	else
		this->timer += 20;

	this->timer += (FLEE_LAG(this));
}

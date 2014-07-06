/* ************************************************************************
*   File: magic.c                                       Part of CircleMUD *
*  Usage: low-level functions for magic; spell template code              *
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
#include "db.h"
#include "interpreter.h"
#include "weaves.h"
#include "dg_event.h"

#include "js_functions.h"
#include "MiscUtil.h"
#include "rooms/Room.h"

extern Object *object_list;
extern Character *character_list;
extern Descriptor *descriptor_list;
extern Index *obj_index;
extern struct default_mobile_stats *mob_defaults;
extern struct apply_mod_defaults *apmd;
extern const char *spell_wear_off_msg[];

byte saving_throws(int class_num, int type, int level); /* class.c */
void weight_change_object(Object *obj, int weight);
void add_follower(Character * ch, Character * leader);
int mana_damage( Character *ch, Character *vict, int damage );
int GetAffNumber(Weave *weave);
int move_damage(Character *ch, Character *vict, int low, int high);
int PerformPeriodicEffects(Character *ch, affected_type *af);

/* local functions */
int mag_savingthrow(Character *ch, int type);
void affect_update(void);

ACMD(do_untie);

/*
 * Saving throws are now in class.c (bpl13)
 */

int mag_savingthrow(Character *ch, int type)
{
	int save;

	save = GET_LEVEL(ch);

	/* throwing a 0 is always a failure */
	if (MAX(1, save) < MiscUtil::random(0, 99))
		return TRUE;
	else
		return FALSE;
}

/* affect_update: called from comm.c (causes spells to wear off) */
void affect_update(void)
{
	struct affected_type *af, *next;
	Weave* weave;
	Character *i, *i_next;

	for (i = character_list; i; i = i_next)
	{
		i_next = i->next;
		if( i->IsPurged() ) continue;
		for (af = i->affected; af; af = next)
		{
			next = af->next;

			if (af->duration >= 1)
			{
				--af->duration;
				if( af->bitvector == AFF_SHIELD_BLOCK )
					af->modifier = MAX(0, af->modifier - 2);
			}
			else if (af->duration == -1)	/* No action */
				af->duration = -1;	/* GODs only! unlimited */
			else
			{
				if ((!af->next || (af->next->type != af->type) || (af->next->duration > 0)))
				{
					if((weave = WeaveManager::GetManager().GetWeave( af->type )) != NULL)
					{
						std::string WearOffMsg = weave->getAttribute("WearOffMsg");
						if( WearOffMsg != "<None>" )
							i->send("%s\r\n", WearOffMsg.c_str());
						if( af->bitvector == AFF_INSANE )
							i->points.temp_taint = 0;
					}
					else if(af->bitvector == AFF_DISORIENT)
					{//TODO: we should have a map or something cleaner for this.
						i->send("The world stops spinning around you as you regain your bearings.\r\n");
						Act("$n appears to regain $s bearings.", true, i, NULL, NULL, TO_ROOM);
					}
				}
				if(i->SlowedBy)
				{
					i->SlowedBy = 0;
				}
				affect_remove(i, af);
				continue;
			}
			//If i dies, we'll break out of the loop.
			if( PerformPeriodicEffects(i, af) == -1 )
				break;
		}
	}
}

/*
 * Every spell that does damage comes through here. This calculates the
 * amount of damage, adds in any modifiers, determines what the saves are,
 * tests for save and calls damage().
 *
 * -1 = dead, otherwise the amount of damage done.
 */
int mag_damage(Character *ch, Character *victim, int spellnum)
{
	int dam = 0;
	Object *angreal;
	Weave *weave = WeaveManager::GetManager().GetWeave(spellnum);

	if (victim == NULL || ch == NULL || weave == (NULL))
		return 0;

	/* Galnor 08/04/2009 - We had if statements below checking each weave by vnum, which actually is very unnnecessary.
	 *  If we just grab the weave by vnum and check to see if it has the relevant attributes, we can do it all
	 *  much more swiftly.
	 */
	if( weave->hasAttribute("DmgLow") && weave->hasAttribute("DmgHigh") )
	{
		int low = MiscUtil::Convert<int>(weave->getAttribute("DmgLow"));
		int high = MiscUtil::Convert<int>(weave->getAttribute("DmgHigh"));
		dam = MiscUtil::random(low,high);
	}

	if( weave->hasAttribute("Execute") && weave->hasAttribute("ExecuteBelow") )
	{
		if( GET_HIT(victim) <= GET_MAX_HIT(victim) * MiscUtil::Convert<float>(weave->getAttribute("ExecuteBelow")) )
		{
			dam *= MiscUtil::Convert<float>(weave->getAttribute("Execute"));
		}
	}

	if ( (angreal = GET_EQ(ch, WEAR_HOLD)) != NULL)
	{
		if (angreal->getType() == ITEM_ANGREAL && angreal->GetTotalVal2() != 0)
		{
			/* Turn this into a percent. */
			dam = (int) ((float) dam * ((float) ((float)angreal->GetTotalVal1() / 100)));
		}
	}

	/* and finally, inflict the damage */
	return damage(ch, victim, dam, spellnum, -1);
}

int mag_mana_damage(Character *ch, Character *victim, int spellnum)
{
	int dam = 0;
	Object *angreal;
	Weave *weave = WeaveManager::GetManager().GetWeave(spellnum);

	if (victim == NULL || ch == NULL || weave == (NULL))
		return 0;

	if( weave->hasAttribute("ManaDmgLow") && weave->hasAttribute("ManaDmgHigh") )
	{
		int low = MiscUtil::Convert<int>(weave->getAttribute("ManaDmgLow"));
		int high = MiscUtil::Convert<int>(weave->getAttribute("ManaDmgHigh"));
		dam = MiscUtil::random(low,high);
	}

	if ( (angreal = GET_EQ(ch, WEAR_HOLD)) != NULL)
	{
		if (angreal->getType() == ITEM_ANGREAL && angreal->GetTotalVal2() != 0)
		{
			/* Turn this into a percent. */
			dam = (int) ((float) dam * ((float) ((float)angreal->GetTotalVal1() / 100)));
		}
	}

	DisplayWeaveMessages(weave, ch, victim);
	dam = mana_damage(ch, victim, dam);

	if( !weave->getName().compare("Drain") && GET_MAX_MANA(victim) > 0 && victim->ChannelingAbility() )
		GET_MANA(ch) = MIN(GET_MANA(ch) + dam * atoi(weave->getAttribute("ManaRegFactor").c_str()), GET_MAX_MANA(ch));

	return 1;
}

/*
 * Every spell that does an affect comes through here.  This determines
 * the effect, whether it is added or replacement, whether it is legal or
 * not, etc.
 *
 * affect_join(vict, aff, add_dur, avg_dur, add_mod, avg_mod)
*/

const int MAX_SPELL_AFFECTS = 5; // change if more needed

void mag_affects(Character *ch, Character *victim, int spellnum)
{
	struct affected_type af[MAX_SPELL_AFFECTS];
	struct affect_type_not_saved *al;
	Weave *weave = WeaveManager::GetManager().GetWeave(spellnum);
	Character *temp;
	bool accum_affect = FALSE, accum_duration = FALSE;
	int i;

	if (victim == NULL || ch == NULL)
		return;

	for (i = 0; i < MAX_SPELL_AFFECTS; i++)
	{
		af[i].type = spellnum;
		af[i].bitvector= 0;
		af[i].modifier = 0;
		af[i].location = APPLY_NONE;
	}
	if( !weave->getName().compare("Blindness") )
	{
		if (MOB_FLAGGED(victim,MOB_NOBLIND) || WeaveManager::GetManager().SavingRoll(ch, victim) )
		{
			ch->send("You fail to blind %s.\r\n", GET_NAME(victim));
			return;
		}
	}
	else if( !weave->getName().compare("Shield") )
	{
		if(!victim->ChannelingAbility() || WeaveManager::GetManager().SavingRoll(ch, victim) ||
			(AFF_FLAGGED(victim, AFF_SHIELD) || ShieldManager::GetManager().ShieldedBy(victim) != NULL) )
		{
			ch->send("You fail to cut %s off from the True Source.\r\n", GET_NAME(victim));
			return;
		}

		ShieldManager::GetManager().AddShield( new Shield(ch, victim) );
		victim->RemoveSource();
	}
	else if( !weave->getName().compare("Sleep") )
	{
		if(GET_LEVEL(victim) >= LVL_IMMORT)
		{
			temp = victim;
			victim = ch;
			ch = temp;

			Act("You ward off an attempt to put you to sleep!", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n rebounds the attempt to put $s asleep!", TRUE, ch, 0, 0, TO_ROOM);
		}

		if(FIGHTING(victim))
		{
			ch->send("%s is fighting... You might hit the wrong target!\r\n");
			return;
		}
		if (MOB_FLAGGED(victim, MOB_NOSLEEP) || WeaveManager::GetManager().SavingRoll(ch, victim))
		{
			ch->send("You fail to put %s to sleep.\r\n", GET_NAME(victim));
			return;
		}

		if (GET_POS(victim) > POS_SLEEPING)
		{
			Act("You feel very sleepy...  Zzzz......", FALSE, victim, 0, 0, TO_CHAR);
			Act("$n goes to sleep.", TRUE, victim, 0, 0, TO_ROOM);
			GET_POS(victim) = POS_SLEEPING;
		}
	}
	else if( !weave->getName().compare("Slow") )
	{
		//if( IS_HORSE(victim) )
		//{
		//	GET_MOVE(victim) = 0;
		//	DisplayWeaveMessages(weave, ch, victim);
		//	return;
		//}
		victim->SlowedBy = ch;
	}
	else if( !weave->getName().compare("Decay") )
	{
		victim->DecayedBy = ch;
	}
	else if( !weave->getName().compare("Blaze") )
	{
		victim->BurnedBy = ch;
	}
	else if( !weave->getName().compare("Dark Plague") )
	{
		victim->PlaguedBy = ch;
	}
	else if( !weave->getName().compare("Insanity") )
		victim->points.temp_taint = abs(atoi(weave->getAttribute("InitTaint").c_str()) - victim->points.taint);

	if( weave->hasAttribute("Duration") )
		af[0].duration = atoi( weave->getAttribute("Duration").c_str() );
	if( int num = GetAffNumber(weave) )
		af[0].bitvector = num;
	if( weave->hasAttribute("StrModifier") )
	{
		af[0].modifier = atoi( weave->getAttribute("StrModifier").c_str() );
		af[0].location = APPLY_STR;
	}
	if( weave->hasAttribute("DexModifier") )
	{
		af[0].modifier = atoi( weave->getAttribute("DexModifier").c_str() );
		af[0].location = APPLY_DEX;
	}
	if( weave->hasAttribute("IntModifier") )
	{
		af[0].modifier = atoi( weave->getAttribute("IntModifier").c_str() );
		af[0].location = APPLY_INT;
	}
	if( weave->hasAttribute("WisModifier") )
	{
		af[0].modifier = atoi( weave->getAttribute("WisModifier").c_str() );
		af[0].location = APPLY_WIS;
	}
	if( weave->hasAttribute("ConModifier") )
	{
		af[0].modifier = atoi( weave->getAttribute("ConModifier").c_str() );
		af[0].location = APPLY_CON;
	}
	if( !af[0].location )
		af[0].location = APPLY_NONE;

	if( ch->InvertNextWeave && weave->invertable() )
		af[0].inverted = true;

	/*
	 * If this is a mob that has this affect set in its mob file, do not
	 * perform the affect.  This prevents people from un-sancting mobs
	 * by sancting them and waiting for it to fade, for example.
	 */

	if (IS_NPC(victim) && !affected_by_spell(victim, spellnum))
		for (i = 0; i < MAX_SPELL_AFFECTS; ++i)
			if (AFF_FLAGGED(victim, af[i].bitvector))
			{
				ch->send(NOEFFECT);
				return;
			}

	/*
	 * If the victim is already affected by this spell, and the spell does
	 * not have an accumulative effect, then fail the spell.
	 */

	if (affected_by_spell(victim,spellnum) && !(accum_duration||accum_affect))
	{
		ch->send(NOEFFECT);
		return;
	}

	/* Send the messages for the spell. */

	DisplayWeaveMessages(weave, ch, victim);

	for (i = 0; i < MAX_SPELL_AFFECTS; i++)
	{
		if (af[i].bitvector|| (af[i].location != APPLY_NONE))
		{
			affect_join(victim, af+i, accum_duration, FALSE, accum_affect, FALSE);
		}
	}


	/* Add the non-saving affection to the non-saving affection list. */
	al = new affect_type_not_saved;
	al->caster = ch;
	al->affect = af;
	add_affection_to_list(victim, al);
}

/*
 * Every spell that affects the group should run through here
 * perform_mag_groups contains the switch statement to send us to the right
 * magic.
 *
 * group spells affect everyone grouped with the caster who is in the room,
 * caster last.
 *
 * To add new group spells, you shouldn't have to change anything in
 * mag_groups -- just add a new case to perform_mag_groups.
 */

void mag_groups(Character * ch, int spellnum)
{
	Character *k;

	if (ch == NULL)
		return;

	if (!AFF_FLAGGED(ch, AFF_GROUP))
		return;

	if (ch->master != NULL)
		k = ch->master;

	else
		k = ch;
}

/*
 * mass spells affect every creature in the room except the caster.
 *
 * No spells of this class currently implemented as of Circle 3.0.
 */

void mag_masses(Character * ch, int spellnum)
{
	Character *tch, *tch_next;

	for (tch = ch->in_room->people; tch; tch = tch_next)
	{
		tch_next = tch->next_in_room;

		if (tch == ch)
			continue;

	}
}
/*
 * Every spell that affects an area (room) runs through here.  These are
 * generally offensive spells.  This calls mag_damage to do the Actual
 * damage -- all spells listed here must also have a case in mag_damage()
 * in order for them to work.
 *
 *  area spells have limited targets within the room.
*/

void mag_areas(Character * ch, int spellnum)
{
	struct affected_type af;
	Character *tch, *next_tch;
	Weave *weave = WeaveManager::GetManager().GetWeave(spellnum);
	
	if (ch == NULL)
		return;

	/*
	 * to add spells to this fn, just add the message here plus an entry
	 * in mag_damage for the damaging part of the spell.
	 */

	for (tch = ch->in_room->people; tch; tch = next_tch)
	{
		next_tch = tch->next_in_room;

		/*
		 * The skips: 1: the caster
		 *            2: immortals
		 *            3: if no pk on this mud, skips over all players
		 *            4: pets (charmed NPCs)
		 *            5: NPCs flagged as INVISIBLE
		 * players can only hit players in CRIMEOK rooms 4) players can only hit
		 * charmed mobs in CRIMEOK rooms
		 */

		if (tch == ch)
			continue;

		if ( tch == MOUNT(ch) )
			continue;

		if (!IS_NPC(tch) && GET_LEVEL(tch) >= LVL_IMMORT)
			continue;

		if (IS_NPC(tch) && MOB_FLAGGED(tch, MOB_INVIS))
			continue;
		//Flying mobs can't be quaked. what if we have a mag_areas that CAN hit flyers, though??
		if( GET_POS(tch) >= POS_FLYING )
			continue;

		DisplayWeaveMessages(weave, ch, tch);
		if ( spellnum == WeaveManager::GetManager().GetWeaveVnum( "Thunderclap" ) && MiscUtil::random(0, 99) < atoi(WeaveManager::GetManager().GetWeave("Thunderclap")->getAttribute("AffChance").c_str()) )
		{
			af.duration = atoi( WeaveManager::GetManager().GetWeave("Thunderclap")->getAttribute("AffDuration").c_str() );
			af.bitvector= AFF_DEAF;
			affect_to_char( tch, &af );

			Act(WeaveManager::GetManager().GetWeave( "Thunderclap" )->getAttribute( "AffToVict" ).c_str(), FALSE, ch, NULL, tch, TO_VICT);
			Act(WeaveManager::GetManager().GetWeave( "Thunderclap" )->getAttribute( "AffToVictRoom" ).c_str(), FALSE, ch, NULL, tch, TO_ROOM);
		}

		/* Doesn't matter if they die here so we don't check. -gg 6/24/98 */
		mag_damage(ch, tch, spellnum);
	}
}

void mag_points(Character * ch, Character * victim, int spellnum)
{
	int hit = 0;
	int move = 0;

	if (victim == NULL)
		return;

	if (victim == ch)
	{
		ch->send("You can't heal yourself!\r\n");
		return;
	}

	Weave* weave = WeaveManager::GetManager().GetWeave(spellnum);

	if(	weave->hasAttribute("HpsLow") && weave->hasAttribute("HpsHigh") )
	{
		int low = MiscUtil::Convert<int>(weave->getAttribute("HpsLow"));
		int high = MiscUtil::Convert<int>(weave->getAttribute("HpsHigh"));
		hit = MiscUtil::random(low, high);
	}

	DisplayWeaveMessages(weave, ch, victim);
	GET_HIT(victim) = MIN(GET_MAX_HIT(victim), GET_HIT(victim) + hit);
	GET_MOVE(victim) = MIN(GET_MAX_MOVE(victim), GET_MOVE(victim) + move);
	update_pos(victim);
}

void mag_unaffects(Character * ch, Character * victim, int spellnum)
{
	int aff;

	if (victim == NULL)
		return;
	Weave* weave = WeaveManager::GetManager().GetWeave( spellnum );

	if( !weave ) //Weave doesn't exist?
		return;

	if( !weave->getName().compare("Cure Blind") )
		aff = AFF_BLIND;
	else if( !weave->getName().compare("Remove Poison") )
		aff = AFF_POISON;
	else if( !weave->getName().compare("Remove Silence") )
		aff = AFF_SILENCE;
	else if( !weave->getName().compare("Remove Deafness") )
		aff = AFF_DEAF;
	else if( !weave->getName().compare("Cure Weakness") )
		aff = AFF_WEAKEN;
	else if( !weave->getName().compare("Cure Cripple") )
		aff = AFF_CRIPPLE;
	else if( !weave->getName().compare("Remove Slow") )
		aff = AFF_SLOW;
	else if( !weave->getName().compare("Cure Decay") )
		aff = AFF_DECAY;
	else if( !weave->getName().compare("Cure Insanity") )
	{
		aff = AFF_INSANE;
		victim->points.temp_taint = 0;
	}
	else if( !weave->getName().compare("Heal Burn") )
		aff = AFF_BURN;
	else if( !weave->getName().compare("Remove Chill") )
		aff = AFF_CHILL;
	else if( !weave->getName().compare("Remove Daze") )
		aff = AFF_DAZE;
	else
	{
			Log("SYSERR: unknown spellnum %d passed to mag_unaffects.", spellnum);
			return;
	}

	if ( !AFF_FLAGGED(victim, aff) )
	{
		ch->send(NOEFFECT);
		return;
	}

	affect_from_char(victim, 0, aff);

	DisplayWeaveMessages(weave, ch, victim);
}

void mag_alter_objs(Character * ch, Object * obj, int spellnum)
{
	const char *to_char = NULL, *to_room = NULL;

	if (obj == NULL)
		return;

	switch (spellnum)
	{
		case SPELL_POISON:

			if (((obj->getType() == ITEM_DRINKCON) ||
				(obj->getType() == ITEM_FOUNTAIN) ||
				(obj->getType() == ITEM_FOOD)) && !obj->GetTotalVal3())
			{
				GET_OBJ_VAL(obj, 3) = 1;
				to_char = "$p steams briefly.";
			}

			break;

		case SPELL_REMOVE_POISON:

			if (((obj->getType() == ITEM_DRINKCON) ||
				(obj->getType() == ITEM_FOUNTAIN) ||
				(obj->getType() == ITEM_FOOD)) && obj->GetTotalVal3())
			{
				GET_OBJ_VAL(obj, 3) = 0;
				to_char = "$p steams briefly.";
			}

			break;
	}

	if (to_char == NULL)
		ch->send(NOEFFECT);

	else
		Act(to_char, TRUE, ch, obj, 0, TO_CHAR);

	if (to_room != NULL)
		Act(to_room, TRUE, ch, obj, 0, TO_ROOM);

	else if (to_char != NULL)
		Act(to_char, TRUE, ch, obj, 0, TO_ROOM);

}

void mag_creations(Character * ch, int spellnum)
{
	byte level = ch->player.level;
	Object *tobj;
	int z;
	Weave* weave = WeaveManager::GetManager().GetWeave(spellnum);

	if (ch == NULL || weave == NULL)
		return;

	level = MAX(MIN(level, LVL_IMPL), 1);

	if( weave->getName() == "Create Food" )
	{
		z = atoi(weave->getAttribute("FoodVnum").c_str());
	}
	else if( weave->getName() == "Fiery Sword" )
	{
		z = atoi(weave->getAttribute("SwordVnum").c_str());
		
	}
	else
	{
		ch->send("Spell unimplemented, it would seem.\r\n");
		return;
	}

	if (!(tobj = read_object(z, VIRTUAL, true)))
	{
		ch->send("I seem to have goofed.\r\n");
		Log("SYSERR: spell_creations, spell %d, obj %d: obj not found",
		    spellnum, z);
		return;
	}

	sprintf(tobj->creator, "Magic Creation(%s) by %s", weave->getName().c_str(), GET_NAME(ch));

	obj_to_char(tobj, ch);
	Act("$n creates $p.", FALSE, ch, tobj, 0, TO_ROOM);
	Act("You create $p.", FALSE, ch, tobj, 0, TO_CHAR);
	
	if( !tobj->IsPurged() ) {
		js_load_triggers(tobj);
	}
}

void Character::KillAllGates()
{
	GateManager::GetManager().KillAllGates( this );
}

ACMD(do_untie)
{
	int n = 0;
	char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	bool untie = false;
	bool anything_tied = false;
	std::list<int>::iterator jj;

	TwoArguments(argument, arg, arg2);

	if(!*arg)
	{
		ch->send("Currently tied to you: \r\n");
		std::list<Gate*> TempGateList = GateManager::GetManager().GetGatesByCreator(ch);

		n = 1;
		for(std::list<Gate*>::iterator gIter = TempGateList.begin();gIter != TempGateList.end();++gIter, ++n)
		{
			anything_tied = true;
			ch->send("%d) a gate\r\n", n);
		}
		if( anything_tied == false )
			ch->send("...Nothing is tied to you...\r\n");
		else
			ch->send("\r\nIf you wish to untie something, type: untie <item> <number>\r\n");
		return;
	}
	if(!*arg2)
	{
		ch->send("Which do you wish to untie? Supply a number or \"all\".\r\n");
		return;
	}

	if(isdigit(arg2[0]))
		n = atoi(arg2) - 1;
	else if(!str_cmp(arg2, "all"))
		n = -1;
	else
	{
		ch->send("You must supply a number or \"all\" when untying something.\r\n");
		return;
	}

	if(!(strn_cmp(arg, "gate", strlen(arg))))
	{
		if(n == -1)
			ch->KillAllGates();
		else
		{
			std::list<Gate*> TempGateList = GateManager::GetManager().GetGatesByCreator( ch );
			for(std::list<Gate *>::iterator g = TempGateList.begin();g != TempGateList.end();++g, --n)
			{
				if(!n)//Gate has been found.
				{
					(*g)->Close();
					delete (*g);
					ch->send("Successfully untied.\r\n");
					untie = true;
					break;
				}
			}
			if(!untie)//Gate was not found.
				ch->send("Which gate is that?\r\n");
		}
	}
	else
		ch->send("You can untie the following: Gate");
}

void DisplayWeaveMessages(Weave *weave, Character *ch, Character *victim)
{
	std::string InvertedToChar, InvertedToVict, InvertedToRoom,
		ToChar, ToRoom, ToVict, ToNotVict, ToRoomChIsVict, ToCharChIsVict;

	if( weave->hasAttribute("InvToChar") )
		InvertedToChar = weave->getAttribute("InvToChar");
	if( weave->hasAttribute("InvToVict") )
		InvertedToVict = weave->getAttribute("InvToVict");
	if( weave->hasAttribute("InvToRoom") )
		InvertedToRoom = weave->getAttribute("InvToRoom");
	if( weave->hasAttribute("ToChar") )
		ToChar = weave->getAttribute("ToChar");
	if( weave->hasAttribute("ToRoom") )
		ToRoom = weave->getAttribute("ToRoom");
	if( weave->hasAttribute("ToVict") )
		ToVict = weave->getAttribute("ToVict");
	if( weave->hasAttribute("ToNotVict") )
		ToNotVict = weave->getAttribute("ToNotVict");
	if( weave->hasAttribute("ToCharChIsVict") )
		ToCharChIsVict = weave->getAttribute("ToCharChIsVict");
	if( weave->hasAttribute("ToRoomChIsVict") )
		ToRoomChIsVict = weave->getAttribute("ToRoomChIsVict");

	if(weave->invertable() && ch->InvertNextWeave)
	{
		ch->InvertNextWeave = false;

		if(!InvertedToChar.empty())
			Act(InvertedToChar.c_str(), FALSE, ch, NULL, victim, TO_CHAR);
		if(!InvertedToVict.empty())
			Act(InvertedToVict.c_str(), FALSE, ch, NULL, victim, TO_VICT);
		if(!InvertedToRoom.empty())
			Act(InvertedToRoom.c_str(), TRUE, ch, NULL, victim, TO_NOTVICT);
	}
	else if( ch == victim )
	{
		if( !ToCharChIsVict.empty() )
			Act(ToCharChIsVict.c_str(), FALSE, ch, NULL, victim, TO_CHAR);
		if( !ToRoomChIsVict.empty() )
			Act(ToRoomChIsVict.c_str(), FALSE, ch, NULL, victim, TO_NOTVICT);
	}
	else
	{
		if(!ToChar.empty())
			Act(ToChar.c_str(), FALSE, ch, NULL, victim, TO_CHAR);
		if(!ToVict.empty() && ch != victim)
			Act(ToVict.c_str(), FALSE, ch, NULL, victim, TO_VICT);
		if(!ToRoom.empty())
			Act(ToRoom.c_str(), TRUE, ch, NULL, victim, TO_ROOM);
		if(!ToNotVict.empty())
			Act(ToNotVict.c_str(), TRUE, ch, NULL, victim, TO_NOTVICT);
//		else if(!ToRoom.empty())
//			Act(ToRoom.c_str(), TRUE, ch, NULL, victim, TO_CHAR);
	}
}

int GetAffNumber(Weave *weave)
{
	if( !weave->getName().compare("Blindness") )
		return AFF_BLIND;
	else if( !weave->getName().compare("Sanctuary") )
		return AFF_SANCTUARY;
	else if( !weave->getName().compare("Poison") )
		return AFF_POISON;
	else if( !weave->getName().compare("Sleep") )
		return AFF_SLEEP;
	else if( !weave->getName().compare("Night Vision") )
		return AFF_NIGHT_VISION;
	else if( !weave->getName().compare("Haste") )
		return AFF_HASTE;
	else if( !weave->getName().compare("Shield") )
		return AFF_SHIELD;
	else if( !weave->getName().compare("Agility") )
		return AFF_AGILITY;
	else if( !weave->getName().compare("Strength") )
		return AFF_STRENGTH;
	else if( !weave->getName().compare("Insanity") )
		return AFF_INSANE;
	else if( !weave->getName().compare("Silence") )
		return AFF_SILENCE;
	else if( !weave->getName().compare("Chill") )
		return AFF_CHILL;
	else if( !weave->getName().compare("Deafen") )
		return AFF_DEAF;
	else if( !weave->getName().compare("Weaken") )
		return AFF_WEAKEN;
	else if( !weave->getName().compare("Cripple") )
		return AFF_CRIPPLE;
	else if( !weave->getName().compare("Slow") )
		return AFF_SLOW;
	else if( !weave->getName().compare("Decay") )
		return AFF_DECAY;
	else if( !weave->getName().compare("Slice") )
		return AFF_SLICE;
	else if( !weave->getName().compare("Blaze") )
		return AFF_BURN;
	else if( !weave->getName().compare("Chill") )
		return AFF_CHILL;
	else if( !weave->getName().compare("Daze") )
		return AFF_DAZE;
	else if( !weave->getName().compare("Dark Plague") )
		return AFF_DARK_PLAGUE;
	else
		return 0;

}

int PerformPeriodicEffects(Character *ch, affected_type *af)
{
	std::string CastToChar, CastToRoom, CastToVict;
	Weave *weave;

	int rv = 0;

	if( af->bitvector == AFF_SLOW )
	{
		int dist,dmg;
		int iMaxDist = atoi(WeaveManager::GetManager().GetWeave("Slow")->getAttribute("MaxDist").c_str());

		if( ch->SlowedBy )
			dist = MIN(ch->in_room->getDistanceToRoom(ch->SlowedBy->in_room), iMaxDist);
		else
			dist = iMaxDist + 1;
		dmg = dist / atoi(WeaveManager::GetManager().GetWeave("Slow")->getAttribute("DistFactor").c_str());
		dmg *= atoi(WeaveManager::GetManager().GetWeave("Slow")->getAttribute("MovesPerDist").c_str());
//		dmg = (dmg * GET_MOVE(ch)) / (GET_MAX_MOVE(ch) + 1);
		dmg = (dmg / 100) * GET_MAX_MOVE(ch);
		move_damage(ch, ch, dmg, dmg);
		if( ch->SlowedBy && dist >= iMaxDist )
			affect_remove(ch, af);
	}
	if( af->bitvector == AFF_DECAY )
	{
		int dmg,dist;
		int iMaxDist = atoi(WeaveManager::GetManager().GetWeave("Decay")->getAttribute("MaxDist").c_str());

		if( ch->DecayedBy )
			dist = MIN(ch->in_room->getDistanceToRoom(ch->DecayedBy->in_room), iMaxDist);
		else
			dist = iMaxDist + 1;
		dmg = MiscUtil::random(atoi(WeaveManager::GetManager().GetWeave("Decay")->getAttribute("DmgLow").c_str()), atoi(WeaveManager::GetManager().GetWeave("Decay")->getAttribute("DmgHigh").c_str()));

		//Do this first, then damage. Dying takes all the ch->affects away.
		if( ch->DecayedBy && dist >= iMaxDist )
			affect_remove(ch, af);
		rv = damage(ch, ch, dmg, SPELL_DECAY, -1);
//		mag_damage(ch->DecayedBy, ch, SPELL_DECAY);
	}
	if( af->bitvector == AFF_INSANE )
	{
		ch->points.temp_taint -= abs(atoi(WeaveManager::GetManager().GetWeave("Insanity")->getAttribute("InitTaint").c_str()) - ch->points.taint) / (atoi(WeaveManager::GetManager().GetWeave("Insanity")->getAttribute("Duration").c_str()) + 1);
		ch->send("You feel a bit more sane.\r\n");
	}
	return rv;//-1 is ch has died.
}



/*** Galnor 12/29/2009 - This will handle all weave callbacks to ensure that they are still valid ***/
EVENT( effMainCallback )
{
	wTuple *Data = reinterpret_cast< wTuple* >( info );
	Character *ch = (*Data).first;
	affected_type *af = (*Data).second, *aIter;
	bool hasEffect = false;
	EVENT(*cb);
	int cbTime;

	if( !Data->first || !Data->second )
	{
		delete Data;
		return;
	}

	//Check to see if the effect persists.
	for( affected_type *aIter = ch->affected; aIter != 0; aIter = aIter->next )
	{
		if( aIter == af )
		{
			hasEffect = true;
			break;
		}
	}

	if( !hasEffect )
	{
		//Cleanup.
		delete Data;
		return;
	}
	if( !af->callBack )
	{
		WeaveCallback wc = WeaveManager::GetManager().GetCallback( af->bitvector );
		cbTime = wc.callTime;
		cb = wc.callBack;
	}
	else
	{
		cbTime = af->callTime;
		cb = af->callBack;
	}

	//The effect persists. Determine if there is a callback. If so we will run it.
	if( cb && cbTime > 0 )
		cb( Data );
	add_event( cbTime, effMainCallback, Data );
}

EVENT( effPerformBurn )
{
	wTuple *Data = static_cast< wTuple* >( info );

//	if( !FIGHTING(Data->first) )
//		return;

	//Burn the shit out of them.
	damage( Data->first, Data->first, 1, SPELL_BLAZE, -1 );
}

EVENT( effPerformDarkPlague )
{
	wTuple *Data = static_cast< wTuple* >( info );

	damage( Data->first, Data->first, 5, SPELL_DARK_PLAGUE, -1 );
}

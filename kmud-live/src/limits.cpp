/* ************************************************************************
*   File: limits.c                                      Part of CircleMUD *
*  Usage: limits & gain funcs for HMV, exp, hunger/thirst, idle time      *
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
#include "db.h"
#include "handler.h"
#include "screen.h"
#include "weather.h"
#include "zones.h"

#include "UserLogoutType.h"
#include "Descriptor.h"

extern Character *character_list;
extern Object *object_list;

/* local functions */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6);
void check_autowiz(Character * ch);
int level_exp(int level);
void update_char_objects(Character * ch);	/* handler.c */
extern class Wizard *wizlist;
extern struct Index *obj_index;
extern Room *FindTargetRoom(Character * ch, char * rawroomstr);
bool isInArena(Character *ch);

/*
 * The hit_limit, mana_limit, and move_limit functions are gone.  They
 * added an unnecessary level of complexity to the internal structure,
 * weren't particularly useful, and led to some annoying bugs.  From the
 * players' point of view, the only difference the removal of these
 * functions will make is that a character's age will now only affect
 * the HMV gain per tick, and _not_ the HMV maximums.
 */

/* manapoint gain pr. game hour */

void Character::RollMoves()
{
	/* First, determine the offset given by equipment bonuses */
	int offset = GET_MAX_MOVE(this) - this->BaseMoves();
	if(IS_GREYMAN(this))
		GET_MAX_MOVE(this) = MiscUtil::random(220, 260);
	else if(IS_HUMAN(this) || IS_FADE(this) || IS_DREADLORD(this))
		GET_MAX_MOVE(this) = MiscUtil::random(120, 145);
	else if(GET_RACE(this) == RACE_AIEL)
		GET_MAX_MOVE(this) = MiscUtil::random(165, 230);
	else if(IS_TROLLOC(this))
		GET_MAX_MOVE(this) = MiscUtil::random(210, 250);
	else
	{
		MudLog(NRM, LVL_IMMORT, TRUE,
		       "Error with rolling moves. Character %s does not have a valid class.\r\n", GET_NAME(this));
		GET_MAX_MOVE(this) = MiscUtil::random(100, 150);
	}
	GET_MAX_MOVE(this) += offset;
}

void Character::RollShadow()
{
	if(IS_FADE(this))
		GET_MAX_SHADOW(this) = 100 + this->GetWis() * 2;
	else
	{
		MudLog(NRM, LVL_IMMORT, TRUE,
		       "Error with rolling shadow points. Character %s is not a Fade.\r\n", GET_NAME(this));
		GET_MAX_SHADOW(this) = 0;
	}
}

sh_int Character::ShadowGain()
{

	int gain = 0;

	if(!IS_FADE(this) && !IS_DREADLORD(this))
		return 0;

	gain = 20;

	switch (GET_POS(this))
	{
		case POS_SLEEPING:
			gain *= 2;
			break;
		case POS_RESTING:
			gain += (gain / 2);	/* Divide by 2 */
			break;
		case POS_SITTING:
			gain += (gain / 4);	/* Divide by 4 */
			break;
	}

	if (!IS_NPC(this) && ((this->PlayerData->conditions[ FULL ] == 0) ||
	(this->PlayerData->conditions[ THIRST ] == 0)) )
		gain /= 4;

	if (AFF_FLAGGED(this, AFF_POISON))
		gain /= 2;	

	if (in_room && in_room->GetZone()->GetWeather()->get_sun() && !in_room->IsDark())
		gain /= 2;

	//if (AFF_FLAGGED(this, AFF_SHADOW_RAGE))
	//	gain -= 40;

	return (gain);
}

sh_int Character::ManaGain()
{
	int gain = 0;

	if (IS_NPC(this))
	{
		/* Neat and fast */
		gain = GET_LEVEL(this);
	}

	else
	{
		gain = GetWis();
		/* Class calculations */
		/* Skill/Spell calculations */
		/* Position calculations    */

		switch (GET_POS(this))
		{

			case POS_SLEEPING:
				gain *= 2;
				break;
			case POS_RESTING:
				gain += (gain / 2);	/* Divide by 2 */
				break;
			case POS_SITTING:
				gain += (gain / 4);	/* Divide by 4 */
				break;
		}

		if (IS_CHANNELER(this) || IS_DREADGUARD(this))
			gain *= 2;
		else if(IS_DREADLORD(this) || this->AES_SEDAI())
			gain *= 2.5f;
		else
			gain = 0;

		if (!IS_NPC(this) && ((this->PlayerData->conditions[ FULL ] == 0) ||
		(this->PlayerData->conditions[ THIRST ] == 0)))
			gain /= 4;
	}

	if (AFF_FLAGGED(this, AFF_POISON))
		gain /= 2;

	if(PRF_FLAGGED(this, PRF_SOURCE))
		gain /= 2;

	return (gain);
}

/* Hitpoint gain pr. game hour */
sh_int Character::HitGain()
{
	int gain;

	if (IS_NPC(this))
		gain = GET_LEVEL(this);
	else
	{
		gain = GetCon() + 3;

		/* Class/Level calculations */
		/* Skill/Spell calculations */
		/* Position calculations    */

		switch (GET_POS(this))
		{
			case POS_SLEEPING:
				gain += (gain / 2);	/* Divide by 2 */
				break;
			case POS_RESTING:
				gain += (gain / 4);	/* Divide by 4 */
				break;
			case POS_SITTING:
				gain += (gain / 8);	/* Divide by 8 */
				break;
		}
		if(AFF_FLAGGED(this, AFF_HEALING))
		{
			for(affected_type *aff = this->affected; aff; aff = aff->next)
			{
				if( aff->bitvector == AFF_HEALING )
				{
					gain += aff->modifier;
					this->Send("You feel a little stronger.\r\n");
				}
			}
		}
		if (!IS_NPC(this) && ((this->PlayerData->conditions[ FULL ] == 0) ||
		(this->PlayerData->conditions[ THIRST ] == 0)) )
			gain /= 4;
	}

	if (AFF_FLAGGED(this, AFF_POISON))
		gain /= 2;

	return (gain);
}

/* move gain pr. game hour */
sh_int Character::MoveGain()
{
	int gain;

	if (IS_NPC(this))
	{
		/* Neat and fast */
		gain = GET_LEVEL(this);
	}

	if(MOB_FLAGGED(this, IS_HORSE(this))) {
		gain = GET_MAX_MOVE(this) / 4;
		if(this->player.ridden_by)
			gain += this->player.ridden_by->GetSkillLevel(SKILL_RIDE);

	}
	else
	{
		if(IS_TROLLOC(this))
			gain = 40;   //Troll regen//
		else if(IS_GREYMAN(this))
			gain = 45;
		else if(this->GetClan(CLAN_WOLFBROTHER))
			gain = 60;
		else
			gain = 26;   //Other regen//
		
		/* Position calculations    */
		if(IS_TROLLOC(this) || this->GetClan(CLAN_WOLFBROTHER))
		{
			switch(GET_POS(this))
			{
				case POS_SLEEPING:
					gain += (gain / 4);	/* Divide by 4 */
					break;
				case POS_RESTING:
					gain += (gain / 8);	/* Divide by 8 */
					break;
				case POS_SITTING:
					gain += (gain / 12);	/* Divide by 12 */
					break;
			}
		}

		else
		{
			switch (GET_POS(this))
			{
				case POS_SLEEPING:
					gain += (gain / 2);	/* Divide by 2 */
					break;
				case POS_RESTING:
					gain += (gain / 4);	/* Divide by 4 */
					break;
				case POS_SITTING:
					gain += (gain / 8);	/* Divide by 8 */
					break;
			}
		}
		if(AFF_FLAGGED(this, AFF_REGENERATION))
		{
			for(affected_type *aff = this->affected; aff; aff = aff->next)
			{
				if( aff->bitvector == AFF_REGENERATION )
				{
					gain += aff->modifier;
					this->Send("You feel a little more refreshed.\r\n");
				}
			}
		}
		if (!IS_NPC(this) && (!this->PlayerData->conditions[ FULL ]
		|| !this->PlayerData->conditions[ THIRST ] ) )
			gain /= 4;
		
		gain += this->GetSkillLevel(SKILL_SURVIVAL);
	}

	if (AFF_FLAGGED(this, AFF_HAMSTRING))
		gain -= 8;

	if (AFF_FLAGGED(this, AFF_POISON))
		gain /= 2;

	return gain;
}

void Character::SetTitle(std::string title)
{
	if(title.size() > MAX_TITLE_LENGTH - 1)
		return;

	this->player.title = title;
}

void check_autowiz(Character * ch)
{
	class Wizard *temp, *cur;

	if(IS_NPC(ch))
		return;

	for(cur = wizlist;cur;cur = cur->next)
	{
		if(cur->name == GET_NAME(ch))
		{
			REMOVE_FROM_LIST(cur, wizlist, next);
		}
	}

	if(GET_LEVEL(ch) >= LVL_IMMORT)
	{
		cur = new Wizard;
		cur->name = GET_NAME(ch);
		cur->level = GET_LEVEL(ch);
		cur->next = wizlist;
		wizlist = cur;
	}
}

void gain_exp(Character * ch, int gain, bool enforceMax)
{
	bool is_altered = false;
	int num_levels = 0;

	if (!IS_NPC(ch) && ((GET_LEVEL(ch) < 1 || GET_LEVEL(ch) >= LVL_IMMORT)))
		return;
//	if(FIGHTING(ch) && GET_RACE(ch) == GET_RACE(FIGHTING(ch)))
//		return;
	if(GET_LEVEL(ch) >= 50 && gain > 0)
		gain = 0;

	if(GET_LEVEL(ch) < 30 && enforceMax)
		gain = MIN(gain, (level_exp(GET_LEVEL(ch) + 1) - level_exp(GET_LEVEL(ch))) / 10);

	if (IS_NPC(ch))
	{
		GET_EXP(ch) += gain;
		return;
	}

	if (gain > 0)
	{
		GET_EXP(ch) += (int) (gain * Conf->play.ExpMultiplier);

		while (GET_LEVEL(ch) < LVL_IMMORT && GET_EXP(ch) >= level_exp(GET_LEVEL(ch) + 1))
		{
			++GET_LEVEL(ch);
			++num_levels;
			ch->GainLevel(TRUE);
			is_altered = true;
		}

		if (is_altered)
		{
			if (num_levels == 1)
				ch->Send("%s%sYou rise a level!\r\n%s",
				         COLOR_BOLD(ch, CL_SPARSE), COLOR_GREEN(ch, CL_SPARSE), COLOR_NORMAL(ch, CL_SPARSE));
			else
				ch->Send("%s%sYou rise %d levels!%s\r\n",
				         COLOR_BOLD(ch, CL_SPARSE), COLOR_GREEN(ch, CL_SPARSE), num_levels, COLOR_NORMAL(ch, CL_SPARSE));
		}
		is_altered = FALSE;
	}
	else
		GET_EXP(ch) = MAX(gain + GET_EXP(ch), 0);
}

void gain_exp_regardless(Character * ch, int gain)
{
	int is_altered = FALSE;
	int num_levels = 0;

	gain = MIN(gain, level_exp(GET_LEVEL(ch) + 1) / 10);
	gain = MAX(gain, -(level_exp(GET_LEVEL(ch))));

	GET_EXP(ch) += (int) (gain * Conf->play.ExpMultiplier);
	GET_EXP(ch)  = MAX((int)GET_EXP(ch), 0);

	if (!IS_NPC(ch))
	{
		while (GET_LEVEL(ch) < LVL_IMPL &&
		        GET_EXP(ch) >= level_exp(GET_LEVEL(ch) + 1))
		{
			GET_LEVEL(ch) += 1;
			++num_levels;
			ch->GainLevel(TRUE);
			is_altered = TRUE;
		}
		if (is_altered)
		{
			if (num_levels == 1)
				ch->Send("You rise a level!\r\n");
			else
			{
				ch->Send("You rise %d levels!\r\n", num_levels);
			}
			check_autowiz(ch);
		}
	}
}

void gain_condition(Character * ch, int condition, int value)
{
	bool intoxicated;

	if (IS_NPC(ch) || ch->PlayerData->conditions[ condition ] == -1)	/* No change */
		return;

	intoxicated = (ch->PlayerData->conditions[ DRUNK ] > 0);

	ch->PlayerData->conditions[ condition ] += value;

	ch->PlayerData->conditions[ condition ] = MAX(0, ch->PlayerData->conditions[ condition ]);
	ch->PlayerData->conditions[ condition ] = MIN(24, ch->PlayerData->conditions[ condition ]);

	if (ch->PlayerData->conditions[ condition ] || PLR_FLAGGED(ch, PLR_WRITING))
		return;

	switch (condition)
	{
		case FULL:
			ch->Send("You are hungry.\r\n");
			return;
		case THIRST:
			ch->Send("You are thirsty.\r\n");
			return;
		case DRUNK:
			if (intoxicated)
				ch->Send("You are now sober.\r\n");
			return;
		default:
			break;
	}
}

int check_idling(Character * ch)
{
	if(++ch->player.timer == CONFIG_IDLE_VOID)
		MudLog(NRM, MAX(GET_LEVEL(ch), LVL_GOD), TRUE, "%s has idled.", GET_NAME(ch));

	if(GET_LEVEL(ch) >= LVL_IMMORT || PLR_FLAGGED(ch, PLR_NOIDLE))
	{
		return 1;
	}

	if(ch->player.timer >= CONFIG_IDLE_RENT_TIME)
	{
		bool isLinkless = (ch->desc == NULL);
		if (ch->desc)
		{
			STATE(ch->desc) = CON_DISCONNECT;

			/*
			 * For the 'if (d->character)' test in close_socket().
			 * -gg 3/1/98 (Happy anniversary.)
			 */

			ch->desc->character = NULL;
			ch->desc = NULL;
		}

		MudLog(CMP, LVL_GOD, TRUE, "%s force-rented and extracted (idle).", GET_NAME(ch));
		ch->rentSave();
		UserLogoutType *userLogoutType = (isLinkless ? UserLogoutType::linklessIdle : UserLogoutType::idle);
		ch->Extract(userLogoutType);

		return 0;

	}

	else if(ch->player.timer >= CONFIG_IDLE_VOID)
	{
		//If they aren't idled...
		if(!GET_WAS_IN(ch))
		{
			Act("$n is idle and is pulled into the void.", TRUE, ch, 0, 0, TO_NOTVICT);
			ch->Send("You are idle and are pulled into the void.\r\n");

			if( !isInArena(ch) )
				GET_WAS_IN(ch) = ch->in_room;
			else
				GET_WAS_IN(ch) = ch->StartRoom();

			if(FIGHTING(ch))
			{
				FIGHTING(ch)->StopFighting();
				ch->StopFighting();
			}
			if(ch->in_room)
				ch->RemoveFromRoom();
			int rnum = real_room(20);
			if( rnum != -1 ) {
				ch->MoveToRoom(World[rnum]);
			}

			return 1;
		}
	}

	// Serai - 11/11/05 - I guess we'll assume no?
	// Galnor - 11/21/05 - Changed to return 1. Return 0 caused hunger/thirst to be disabled.
	return (1);
}

void Character::PointUpdate( bool login )
{
	bool increment=false;
	GET_DEATH_WAIT(this) = MAX(0, GET_DEATH_WAIT(this) - 1);
	if (GET_POS(this) >= POS_STUNNED)
	{
		if(GET_HIT(this) < GET_MAX_HIT(this))
			GET_HIT(this)		= MIN(GET_HIT(this)	+ this->HitGain()		,GET_MAX_HIT(this));

		if(GET_MOVE(this) < GET_MAX_MOVE(this))
			GET_MOVE(this)		= MIN(GET_MOVE(this)	+ this->MoveGain()		,GET_MAX_MOVE(this));

		if(GET_MANA(this) < GET_MAX_MANA(this))
			GET_MANA(this)		= MIN(GET_MANA(this)	+ this->ManaGain()		,GET_MAX_MANA(this));

		if(GET_SHADOW(this) < GET_MAX_SHADOW(this))
			GET_SHADOW(this)	= MIN(GET_SHADOW(this)	+ this->ShadowGain()	,GET_MAX_SHADOW(this));

		if(GET_STRAIN(this) >= TaintData.StrainToTaint)
			GET_TAINT(this) += 1;

		this->AddStrain(int(-( (GET_LEVEL(this) * TaintData.LevelStrainRemoval) + (this->GetWis() * TaintData.WisStrainRemoval))));
		GET_MANA(this) = MAX((int)GET_MANA(this), 0);
		GET_SHADOW(this) = MAX((int)GET_SHADOW(this), 0);
		if(GET_SHADOW(this) <= 0 && AFF_FLAGGED(this, AFF_SHADOW_RAGE))
		{
			affect_from_char( this, 0, AFF_SHADOW_RAGE );
			this->Send("Your rage leaves you as it saps the last of your energy.\r\n");
		}

		FLEE_LAG(this) = MAX(0, FLEE_LAG(this) - FleeData.PulsesRemovedPerTic);

		if (GET_POS(this) <= POS_STUNNED)
			update_pos(this);
	}
	else if (GET_POS(this) == POS_INCAP)
	{
		if (damage(this, this, 1, TYPE_SUFFERING, -1) == -1)
			return;
	}
	else if (GET_POS(this) == POS_MORTALLYW)
	{
		if (damage(this, this, 2, TYPE_SUFFERING, -1) == -1)
			return;
	}

	if( login == true ) return;

	/**** Everything below this point will not occur if the player is receiving regeneration via login ****/

	if (!IS_NPC(this))
	{
		update_char_objects(this);
		if(check_idling(this) == 0)
			return;
	}

	if(!IS_NPC(this))
	{
		increment = true;
		if((this->GetSkillLevel(SKILL_SURVIVAL) >= 7))
		{
			if(MiscUtil::random(0,2))
				increment = false;
		}
		if(increment)
		{
			gain_condition(this, FULL, -1);
			gain_condition(this, THIRST, -1);
		}
		gain_condition(this, DRUNK, -1);
	}
}

/* Update PCs, NPCs, and objects */
void point_update(void)
{
	Character *i, *next_char;
	Object *j;
	bool corpse = true;
	Clock charListClock;
	Clock objListClock;
	Clock corpseClock;

	// characters
	int nrOfCharacters = 0;
	for (i = character_list; i; i = i->next)
	{
		++nrOfCharacters;
		if( i->IsPurged() ) continue;//Mob is already extracted. Do not update points. 11/15/2009
		i->PointUpdate();//Galnor - 08/03/2009 - moved to PointUpdate()
	}

	for(j = object_list;j;j = j->next)
	{
		if(!j->IsPurged() && GET_OBJ_TIMER(j) > 0 && IS_CORPSE(j))
			--GET_OBJ_TIMER(j);
	}

	while(corpse)
	{
		corpse = false;

		/* objects */
		for (j = object_list; j; j = j->next)
		{
			if( j->IsPurged() ) continue;
			/* If this is a corpse */
			if (IS_CORPSE(j))
			{
				if (!GET_OBJ_TIMER(j))
				{
					corpse = true;

					if (j->carried_by)
					{
						Act("$p decays in your hands.", FALSE, j->carried_by, j, 0, TO_CHAR);
						obj_from_char(j);
					}

					else if ((j->in_room) && (j->in_room->people))
					{
						Act("$p rots, leaving nothing but bones.",
						    TRUE, j->in_room->people, j, 0, TO_ROOM);

						Act("$p rots, leaving nothing but bones.",
						    TRUE, j->in_room->people, j, 0, TO_CHAR);
					}

					j->Extract(true);
					break;
				}
			}

			/* If the timer is set, count it down and at 0, try the trigger */
			/* note to .rej hand-patchers: make this last in your point-update() */

			else if (GET_OBJ_TIMER(j)>0)
				--GET_OBJ_TIMER(j);
		}
	}
}

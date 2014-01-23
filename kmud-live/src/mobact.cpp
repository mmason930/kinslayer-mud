/* ************************************************************************
*   File: mobact.c                                      Part of CircleMUD *
*  Usage: Functions for generating intelligent (?) behavior in mobiles    *
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
#include "db.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "clans.h"
#include "shop.h"
#include "mobs.h"
#include "weather.h"

#include "js.h"
#include "js_interpreter.h"
#include "Descriptor.h"
#include "UserClan.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"

extern Character *character_list;

Character *faceoff(Character *attacker, Character *prey);
void perform_assist(Character *ch, Character *helpee);
int GetSharedClan(Character *p1, Character *p2);
bool isInArena(Character *ch);

ACMD(do_bash);
ACMD(do_get);
ACMD(do_hit);
ACMD(do_stand);

/* local functions */
void mobileActivity(void);

bool Character::IsGroupedWith(Character *ch)
{
	std::stringstream buffer;
	buffer << lookupName(this) << ".isGroupedWith(" << lookupName(ch) << ");";
	flusspferd::value val = JSManager::get()->executeExpression(buffer.str());

	return val.is_bool() && val.to_boolean();
}

int Character::FindAssistPriority(Character *test)
{
	int level = 0, clannum = 0;
	bool CanAggro;
	std::list<int>::iterator ai;

	if(!FIGHTING(test))
		return 0;

	//Set the boolean integer
	CanAggro = this->CanAggro(FIGHTING(test));

	//Ok, let's sum this up...

	//These guys assists are setup in medit, they assist specific mobs regardless of anything
	if(this->MobData->assists.size() && IS_NPC(test))
	{
		for(ai = this->MobData->assists.begin();ai != this->MobData->assists.end();++ai)
		{
			if(GET_MOB_VNUM(test) == (*ai))
				level = 2;
		}
	}

	//These guys assist same race, as long as they CAN aggro the opponent normally
	if(GET_RACE(test) == GET_RACE(this) && GET_RACE(this) != RACE_ANIMAL && GET_RACE(this) != RACE_OTHER && CanAggro)
		level = 3;

	//These next two if statements, aggro anything that is fighting a group member/leader. Leader is a priority.
	if(test->IsGroupedWith(this))
		level = 4;
	if(test == this->master)
		level = 5;

	//Clan member mobs assist clan members based on rank, as long as opponent is a PC or normal aggro.
	if((clannum = GetSharedClan(this, test)) && (CanAggro || !IS_NPC(FIGHTING(test))) )
	{
		level = 6 + test->getUserClan(clannum)->getRank();
	}

	//Doing this makes same rank NPCs assist eachother when fighting same ranked PC... In other words, NPC is priority.
	if(IS_NPC(test) && level)
		++level;
	return level;
}

Character *Character::FindFirstAssist()
{
	Character *buddy = NULL, *cur;

	if(!MOB_FLAGGED(this, MOB_HELPER))
		return NULL;

	for(cur = this->in_room->people;cur;cur = cur->next_in_room)
	{
		if(!FIGHTING(cur) || cur == this)
			continue;
		else if(!buddy)
		{
			if(FindAssistPriority(cur))
				buddy = cur;
			else
				continue;
		}
		else if(this->FindAssistPriority(cur) > this->FindAssistPriority(buddy))
			buddy = cur;
	}

	return buddy;
}

bool Character::CanAggro(Character *victim)
{
	if(isInArena(this))		// Mobs have special aggro handling in the arena
		return false;
	if(!IS_NPC(this) || victim == this || !victim)
		return false;
	if(IS_NPC(victim) && !GET_AGGRO(this, AGGRO_MOB))
		return false;
	if(IS_NPC(victim)
	&& std::find(MobData->assists.begin(), MobData->assists.end(), victim->getVnum()) != MobData->assists.end() )
		return false;//Prospective target is in this mob's assist list.
	if(victim->wantedByPlayer(this))
		return true;
	if(GET_AGGRO(this, AGGRO_ALL))
		return true;
	if(IS_HUMAN(victim) && GET_AGGRO(this, AGGRO_HUMAN))
		return true;
	if(IS_TROLLOC(victim) && GET_AGGRO(this, AGGRO_TROLLOC))
		return true;
	if(IS_SEANCHAN(victim) && GET_AGGRO(this, AGGRO_SEANCHAN))
		return true;
	if(IS_AIEL(victim) && GET_AGGRO(this, AGGRO_AIEL))
		return true;
	if(!GetSharedClan(this, victim) && GET_AGGRO(this, AGGRO_NON_CLAN))
		return true;

	return false;
}

void mobileActivity(void)
{
	register Character *ch, *next_ch, *vict, *prev = 0;
	Object *obj, *best_obj;
	int door, found, max;
	std::list<long>::iterator mem;

	for (ch = character_list; ch; ch = ch->next)
	{
		if (ch->IsPurged() || !IS_MOB(ch) || !ch->in_room)
			continue;

		/* Examine call for special procedure */
		if (MOB_FLAGGED(ch, MOB_SPEC))
		{
			if (!MobManager::GetManager().GetIndex((u_int)ch->nr)->func)
			{
				MudLog(NRM, LVL_APPR, TRUE, "SYSERR: %s (#%d): Attempting to call non-existing mob function.",
				    GET_NAME(ch), GET_MOB_VNUM(ch));
				REMOVE_BIT(MOB_FLAGS(ch), Q_BIT(MOB_SPEC));
			}
			else
			{
				/* XXX: Need to see if they can handle NULL instead of "". */
				char sEmpty[8];
				*sEmpty = '\0';
				if ((MobManager::GetManager().GetIndex((u_int)ch->nr)->func) (ch, ch, 0, sEmpty))
					continue;		/* go to next char */
			}
		}

		/* If the mob has no specproc, do the default actions */
		if (FIGHTING(ch) || !AWAKE(ch))
			continue;

		/* Scavenger (picking up objects) */
		if (MOB_FLAGGED(ch, MOB_SCAVENGER) && !FIGHTING(ch) && AWAKE(ch))
		{
			if (ch->in_room->contents && !MiscUtil::random(0, 10))
			{
				max = 1;
				best_obj = NULL;

				for (obj = ch->in_room->contents; obj; obj = obj->next_content)
				{
					if (CAN_GET_OBJ(ch, obj) && GET_OBJ_COST(obj) > max)
					{

						best_obj = obj;
						max = GET_OBJ_COST(obj);
					}

					if (best_obj != NULL)
					{
						best_obj->RemoveFromRoom();
						obj_to_char(best_obj, ch);
						Act("$n gets $p.", FALSE, ch, best_obj, 0, TO_ROOM);
					}
				}
			}
		}
		
		/********* Aggressive Mobs *********/
		found = FALSE;
		if( !FIGHTING(ch) )
		{
			for (vict = ch->in_room->people; vict && !found; vict = vict->next_in_room)
			{
				if (!CAN_SEE(ch, vict) || PRF_FLAGGED(vict, PRF_NOHASSLE))
					continue;

				if(ch->CanAggro(vict))
				{
					if( (!AFF_FLAGGED(vict, AFF_HIDE) && MiscUtil::random(1, 3) == 3) || MiscUtil::random(1, 4) == 5 )
					{
						vict = faceoff(ch, vict);
						hit(ch, vict, TYPE_HIT);
						found = TRUE;
					}
				}
			}
		}

		
		/* Bashing */
		if(MOB_FLAGGED(ch, MOB_BASH) && FIGHTING(ch) && MiscUtil::random(1, 3) == 3 && !ch->timer)
		{
			ch->InterpCommand("bash");
		}

		
		/* Mob Memory */
		if (MOB_FLAGGED(ch, MOB_MEMORY) && ch->MobData->memory.size())
		{
			found = FALSE;
			Character* next_vict;
			for (vict = ch->in_room->people; vict && !found; vict = next_vict)
			{
				next_vict = vict->next_in_room;
				if (IS_NPC(vict) || !CAN_SEE(ch, vict) || PRF_FLAGGED(vict, PRF_NOHASSLE))
					continue;
				for (mem = ch->MobData->memory.begin();mem != ch->MobData->memory.end();++mem)
				{
					if ((*mem) == vict->player.idnum && ((!AFF_FLAGGED(vict, AFF_HIDE) && MiscUtil::random(1, 3) == 3) || MiscUtil::random(1, 5) == 5 ))
					{
						found = TRUE;
						do_hit(ch, (char*)vict->player.name.c_str(), 0, 0);
					}
				}
			}
		}

		
		/* Helper Mobs */
		if (MOB_FLAGGED(ch, MOB_HELPER))
		{
			if((vict = ch->FindFirstAssist()))
				perform_assist(ch, vict);
		}

		
		/* Mob Movement */
		if (!ch->master && !MOB_FLAGGED(ch, MOB_SENTINEL) && (GET_POS(ch) == POS_STANDING) &&
		        ((door = MiscUtil::random(0, 42)) < NUM_OF_DIRS) && CAN_GO(ch, door) &&
		        !ROOM_FLAGGED(EXIT(ch, door)->getToRoom(), ROOM_NOMOB) &&
		        !ROOM_FLAGGED(EXIT(ch, door)->getToRoom(), ROOM_DEATH) &&
		        !IS_SET(ch->MobData->nsects, Q_BIT(EXIT(ch, door)->getToRoom()->getSector()->getValue())) &&
		        (!MOB_FLAGGED(ch, MOB_STAY_ZONE) ||
				(EXIT(ch, door)->getToRoom()->getZoneNumber() == ch->in_room->getZoneNumber())))
		{
			perform_move(ch, door, 1);
		}
		

		/* Mob Tracking */
		if(MOB_FLAGGED(ch, MOB_TRACK) && ch->player.hunting)
		{
			ch->HuntVictim();
		}
		ch->processForgets();
		
		//Code for mob checking their light source.
		if (!CAN_SEE_IN_DARK(ch))
		{
			Zone *zone = ch->in_room->getZone();
			if (!ch->equipment[WEAR_LIGHT])
			{
				if (ch->in_room->isDark() || zone->GetWeather()->getSun() == Sun::SUN_DARK || ROOM_FLAGGED(ch->in_room, ROOM_DARK))
				{
					for (Object *lightSource = ch->carrying; lightSource; lightSource = lightSource->next_content)
					{
						if (lightSource->getType() == ITEM_LIGHT)
						{
							ch->performWear(lightSource, WEAR_LIGHT);
							break;
						}
					}
				}
			}
			else
			{
				if (zone->GetWeather()->getSun() != Sun::SUN_DARK && !ROOM_FLAGGED(ch->in_room, ROOM_DARK))
				{
					ch->performRemove(WEAR_LIGHT);
				}
			}
		}

		/* Add new mobile actions here */
	}				/* end for() */
}



/* Mob Memory Routines */

/*Remember victim */
void Character::Remember(Character *victim)
{
	std::list<long>::iterator i;

	if (!IS_NPC(this) || IS_NPC(victim) || PRF_FLAGGED(victim, PRF_NOHASSLE))
		return;

	for (i = this->MobData->memory.begin();i != this->MobData->memory.end();++i)
	{
		if ( (*i) == victim->player.idnum)
		{
			return;
		}
	}

	this->MobData->memory.push_back(victim->player.idnum);
}

void Character::processForgets()
{
	if( !IS_NPC(this) ) return;
	for(std::list<long>::iterator mem = MobData->ForgetList.begin();mem != MobData->ForgetList.end();++mem)
	{
		MobData->memory.remove( (*mem) );
	}
	MobData->ForgetList.clear();
}

/* Forget victim */
void Character::Forget(Character *victim)
{
	if(IS_NPC(this))
		this->MobData->ForgetList.push_back(victim->player.idnum);
}


/* erase ch's memory */
void Character::clearMemory()
{
	if(IS_NPC(this))
		this->MobData->memory.clear();
}

/* ************************************************************************
*   File: Act.movement.c                                Part of CircleMUD *
*  Usage: movement commands, door handling, & sleep/rest/etc state        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"

#include "utils.h"
#include "spells.h"
#include "screen.h"
#include "constants.h"
#include "js/js.h"
#include "StringUtil.h"
#include "Descriptor.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"

#include "items/ItemUtil.h"

extern Character *character_list;
extern Descriptor *descriptor_list;
extern struct Index *obj_index;


void add_follower(Character *ch, Character *leader);
void do_doorcmd(Character *ch, Object *obj, int door, int scmd);
int find_eq_pos(Character *ch, Object *obj, char *arg, bool msg);
int ok_pick(Character *ch, int keynum, int pickproof, int scmd);
int find_door(Character *ch, const char *type, char *dir, const char *cmdname);
int special(Character *ch, char *cmd, char *arg);
int can_move(Character *ch, int dir, int need_specials_check, bool flee);
int level_timer(Character *ch, int level);
int find_first_step( Room *src, Room *target );
bool isInArena(Character *ch);
bool JS_isArenaInFFAMode();
int getArenaTeam( Character *ch );

int buildwalk(Character *ch, int dir);
std::list<Track *> TrackList;
const char *door_state(int state);

#define DOOR_FOUND(ch, door) ((!EXIT(ch, door)->hidden) || (EXIT(ch, door)->hidden && \
GET_SKILL(ch, SKILL_SEARCH) > EXIT(ch, door)->hidden))

Object *Character::SittingOn()
{
	return this->player.sitting_on;
}
void Character::StandFromChair()
{
	if( this->player.sitting_on != NULL )
	{
		player.sitting_on->SatOnBy = NULL;
		player.sitting_on = NULL;
	}
}
void Character::SitOnChair( Object *Chair, bool show)
{
	if( show == true )
	{
		Act("You sit down on $p.", TRUE, this, Chair, 0, TO_CHAR);
		Act("$n sits down on $p.", TRUE, this, Chair, 0, TO_ROOM, NULL, true);
	}
	Chair->SatOnBy = this;
	this->player.sitting_on = Chair;
}

const char *door_state(int state)
{
	if(state == 0)
		return "open";
	else if(state == 1)
		return "closed";
	else if(state == 2)
		return "locked";
	else
		return "Unknown state";
}

/* simple function to determine if char can walk on water */
bool Character::HasBoat()
{
	Object *obj;

	if (AFF_FLAGGED(this, AFF_WATERWALK))
		return true;

	/* non-wearable boats in inventory will do it */
	for (obj = this->carrying; obj; obj = obj->next_content)
	{
		if (obj->getType() == ITEM_BOAT && (find_eq_pos(this, obj, NULL, false) < 0))
		{
			return 1;
		}
	}
	/* and any boat you're wearing will do it too */
	for (int i = 0; i < NUM_WEARS;++i)
	{
		if (GET_EQ(this, i) && GET_EQ(this, i)->getType() == ITEM_BOAT)
		{
			return 1;
		}
	}
	return 0;
}

void Character::LayTrack(Room *room, int direction)
{
	//!this->CanTrack(room) removed from condition - Galnor 1-7-2009
	//NPC check removed from the condition - Galnor 07/31/2009
	if(!room || GET_LEVEL(this) >= LVL_IMMORT)
		return;

	new Track(this, direction, room);
}


Track *Track::Clone()
{
	Track *me = new Track();
	me->name = name;
	me->alias = alias;
	me->laytime = laytime;
	me->direction = direction;
	me->race = race;
	me->ch_class = ch_class;
	me->room = room;
	me->bloody = bloody;
	me->npc = npc;
	me->mount = mount;
	me->mount_ridden = mount_ridden;
	return (me);
}
Track::Track(Character *ch, int dir, Room *room)
{
	this->race			= ch->player.race;
	this->ch_class		= ch->player.chclass;
	this->direction		= dir;
	this->name			= GET_NAME(ch);
	if( RIDDEN_BY(ch) ) name += " (ridden)";
	this->alias			= ch->player.name;
	this->room			= room;
	this->mount			= (MOB_FLAGGED(ch,MOB_MOUNT) || MOB_FLAGGED(ch,MOB_SHADOW_MOUNT) || MOB_FLAGGED(ch, MOB_OGIER_MOUNT));
	this->mount_ridden	= (RIDDEN_BY(ch));
	this->npc			= (IS_NPC(ch));
	this->laytime		= time(0);

	room->tracks.push_front(this);
	TrackList.push_back(this);
}

/* Galnor 02-24-2005 - Track destructor. Remove from global list and room list. */
Track::~Track()
{
}

Room *FindFadingRoom(char *name)
{
	Room *room;

	if(!name)
		return 0;

	for(unsigned int i = 0;i < World.size();++i)
	{
		room = World[i];
		if(!strcmp(name, room->fadeCode()))
		{
			if(ROOM_FLAGGED(room, ROOM_NOPORT) || ROOM_FLAGGED(room, ROOM_INACCESSIBLE) || ROOM_FLAGGED(room, ROOM_GODROOM))
				return nullptr;
			else
				return room;
		}
	}

	return nullptr;
}

int fade_distance(Character *ch)
{
	int distance = 0, skill = 0;

	skill = GET_SKILL(ch, SKILL_FADE);

	if(GET_RACE(ch) >= 6)
		distance = 1;
	if(skill > 75)
		distance += 4;
	if(skill > 50)
		distance += 3;
	if(skill > 25)
		distance += 2;

	distance += 1;
	return distance;
}

void perform_actual_fade(Character *ch, Room *room)
{
	ch->send("You reach out for the shadows as they surround you completely.\r\n");
	Act("$n is surrounded by shadows and vanishes.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);

	ch->RemoveFromRoom();
	ch->MoveToRoom(room);

	Act("$n steps out from the dark shadows nearby.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);

	look_at_room(ch, 0);
}

bool Character::CanTrack(Room *room)
{
	if(ROOM_FLAGGED(room, ROOM_NOTRACK))
		return false;
	if(	room->getSector() == RoomSector::waterNoSwim || room->getSector() == RoomSector::underwater ||
		room->getSector() == RoomSector::waterSwim || room->getSector() == RoomSector::flying)
		return false;
	return true;
}
CommandHandler do_sense = DEFINE_COMMAND
{
	skip_spaces(&argument);
	std::list<Object *>::iterator obj;

	if(!strn_cmp(argument, "darkness", strlen(argument)))
	{
		if(!IS_FADE(ch))
		{
			ch->send("Sense?! What?! You want to sense something?\r\n");
			return;
		}

		if(GET_LEVEL(ch) >= LVL_IMMORT && GET_LEVEL(ch) <= LVL_GRGOD)
		{
			ch->send("Should you be trying this?\r\n");
			return;
		}

		if(ROOM_FLAGGED(ch->in_room, ROOM_NOPORT) || ROOM_FLAGGED(ch->in_room, ROOM_INACCESSIBLE))
		{
			ch->send("You try with all of your might, but you can't seem to sense the shadows here.\r\n");
			return;
		}

		if(!ch->command_ready)
		{
			ch->command_ready = true;
			ch->delayed_command = "sense ";
			ch->delayed_command += argument;
		}

		else
		{
			if(!ch->in_room->isDark())
			{
				ch->send("There is no darkness to sense here.\r\n");
				return;
			}

			ch->send("This room: [%s%s%s]\r\n", COLOR_GREEN(ch, CL_COMPLETE),
			         ch->in_room->fadeCode().c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
		}
	}
};

CommandHandler do_fade = DEFINE_COMMAND
{

	int distance = 0, cost = 0;
	Room *oldroom, *room;
	char arg[MAX_STRING_LENGTH];
	struct Follower *f;
	Character *trolloc;

	OneArgument(argument, arg);

	if(!IS_FADE(ch))
	{
		ch->send("Do you really think you can reach for the shadows?\r\n");
		return;
	}

	if(!ch->in_room->isDark() || ROOM_FLAGGED(ch->in_room, ROOM_NOPORT) || ROOM_FLAGGED(ch->in_room, ROOM_INACCESSIBLE))
	{
		ch->send("There are no shadows to reach out to.\r\n");
		return;
	}

	if(!*arg)
	{
		ch->send("Where is it you want the shadows to take you?\r\n");
		return;
	}

	if(MOUNT(ch))
	{
		ch->send("You can't do this while mounted!\r\n");
		return;
	}

	if(!ch->command_ready)
	{
		ch->timer = (float)level_timer(ch, GET_SKILL(ch, SKILL_FADE));
		ch->timer *= 1.5f;
		ch->timer += (FLEE_LAG(ch) * 2 / 3);
		ch->command_ready = true;
		ch->delayed_command = "fade ";
		ch->delayed_command += argument;
	}
	//else if(!strcmp(arg, "horse"))
	//{
	//	Character *mount;

	//	if(mount = ch->IsLeadingMount())
	//	{
	//		perform_actual_fade(ch->IsLeadingMount(), ch->in_room);
	//		GET_SHADOW(ch) -= 10;
	//	}
	//	else
	//		ch->send("But you aren't leading a mount!\r\n");
	//}
	else
	{
		// This means the room was either not found, or found and unreachable //
		if( !(room = FindFadingRoom(arg)) || ROOM_FLAGGED(room, ROOM_NOPORT) || ROOM_FLAGGED(room, ROOM_INACCESSIBLE) || ROOM_FLAGGED(room, ROOM_GODROOM))
		{
			ch->send("You begin to reach for the shadows, but realize the other end cannot be reached!\r\n");
			return;
		}

		distance = room->getZone()->Distance(ch->in_room->getZone());

		cost = 15;
		cost += distance * 3;

		GET_SHADOW(ch) -= cost;

		if(GET_SHADOW(ch) + cost < 0)
		{
			GET_SHADOW(ch) = 0;
			ch->send("You do not have enough strength to reach out for the shadows.\r\n");
			return;
		}

		if(MiscUtil::random(1, 110) > GET_SKILL(ch, SKILL_FADE))
		{
			ch->send("You pull the shadows towards you and lose control!\r\n");
			GET_SHADOW(ch) += cost * .5;
			return;
		}

		if(distance > fade_distance(ch))
		{
			ch->send("You cannot control the shadows to pull you so far.\r\n");
//			GET_SHADOW(ch) += cost * .5;
			return;
		}

		oldroom = ch->in_room;
		perform_actual_fade(ch, room);

		for (f = ch->followers; f; f = f->next)
		{

			trolloc = f->follower;

			if(trolloc->in_room == oldroom && ((GET_SHADOW(ch) - 5) >= 0))
			{
				if(!IS_HORSE(trolloc))
					GET_SHADOW(ch) -= 5;
				perform_actual_fade(trolloc, room);
			}
		}
	}
};

CommandHandler do_compel = DEFINE_COMMAND
{
	Character *vict = 0;

	OneArgument( argument, ::arg );

	if(!ch->GetSkillLevel(SKILL_COMPEL) || !IS_FADE(ch))
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	if(!*::arg || !(vict = get_char_room_vis(ch, ::arg)))
	{
		ch->send("How can you compel someone who isn't here?\r\n");
		return;
	}

	//vict = get_char_room_vis(ch, ::arg);

	if(vict->master != ch && GET_POS(vict) != POS_DEAD)
	{
		ch->send("You can only compel those who are following you!\r\n");
		return;
	}

	if(FIGHTING(ch))
	{
		ch->send("You are too concerned with your own survival to worry about them!\r\n");
		return;
	}

	if(FIGHTING(vict))
	{
		ch->send("You can't get close enough without putting yourself at risk!\r\n");
		return;
	}
	
	if(!ch->command_ready)
		ch->command_ready = true;
	else
	{
		if (GET_SKILL(ch, SKILL_COMPEL) > MiscUtil::random(0, 105) )
		{
			Act("You force $N to become less weary.", FALSE, ch, 0, vict, TO_CHAR);
			Act("You feel a sharp stab of pain as your exhaustion fades away", FALSE, ch, 0, vict, TO_VICT);
			Act("$N grimaces in pain, but appears refreshed.", FALSE, ch, 0, vict, TO_NOTVICT);
			GET_MOVE(vict) = MIN(GET_MAX_MOVE(vict), MAX(GET_MOVE(vict) + GET_MAX_MOVE(vict) * .2, .6 * GET_MAX_MOVE(vict)));
			GET_SHADOW(ch) -= 20;
			float damageMult = (IS_MOB(vict) ? .80 : .20);
			damage(ch, vict, GET_MAX_HIT(vict) * damageMult, SKILL_COMPEL, -1);
			return;
		}
		else
		{
			Act("You fail to bend $N to your will.", FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
	}
};

int can_move(Character *ch, int dir, int need_specials_check, bool flee)
{

	int need_movement;
	char sDir[64], sComm[1];
	*sComm='\0';

	/*
	 * Check for special routines (North is 1 in command list, but 0 here) Note
	 * -- only check if following; this avoids 'double spec-proc' bug
	 */

	strcpy(sDir, dirs[dir]);

	if (need_specials_check && special(ch, sDir, sComm)) /* XXX: Evaluate NULL */
	{
		return 0;
	}

	if(GET_DEATH_WAIT(ch) > 0 && ch->in_room == ch->StartRoom())
	{
		if( !flee )
			ch->send("You must wait %d more minutes to regain strength.\r\n", GET_DEATH_WAIT(ch));
		return 0;
	}

	/* if this room or the one we're going to needs a boat, check for one */
	if ((ch->in_room->getSector() == RoomSector::waterNoSwim) || (EXIT(ch, dir)->getToRoom()->getSector() == RoomSector::waterNoSwim))
	{
		if (!ch->HasBoat())
		{
			ch->send("You need a boat to go there.\r\n");
			return 0;
		}
	}

	if(MOUNT(ch) && ch->in_room->dir_option[dir]->getToRoom()->getSector() == RoomSector::inside )
	{
		if(!flee)
			ch->send("You cannot ride in there!\r\n");
		return 0;
	}

	if(GET_LEVEL(ch) >= LVL_IMMORT && GET_LEVEL(ch) <= LVL_BLDER || PLR_FLAGGED(ch, PLR_ZONE_BAN))
		if(!ch->in_room->dir_option[dir]->getToRoom()->getZone()->CanEdit(ch)
		&& ch->in_room->dir_option[dir]->getToRoom()->getZoneNumber() != 0)
		{
			ch->send("You must be higher level to leave your zone!\r\n");
			return 0;
		}

	/* move points needed is avg. move loss for src and destination sect type */
	need_movement = ch->in_room->getSector()->getMovementLoss() + EXIT(ch, dir)->getToRoom()->getSector()->getMovementLoss();

	if(IS_TROLLOC(ch) || ch->hasWolfbrotherBonuses())
		need_movement = 2;
	else
		need_movement = MIN(need_movement,5);

	//if(AFF_FLAGGED(ch, AFF_NOTICE) && !IS_TROLLOC(ch) && !ch->hasWolfbrotherBonuses() && !IS_BLADEMASTER(ch) && !IS_THIEF(ch))
		//need_movement += 1;

	if(MOUNT(ch))
	{
		if(GET_MOVE(MOUNT(ch)) < need_movement)
		{
			if( !flee )
			{
				ch->send("Your mount is too exhausted.\r\n");
				Act("$N is too exhausted to move $n any further.", FALSE, ch, 0, MOUNT(ch), TO_NOTVICT);
			}
			return 0;
		}

		if(GET_POS(MOUNT(ch)) < POS_FIGHTING)
		{
			ch->send("Your mount is unable to move!\r\n");
			Act("$n is unable to move $s mount.", FALSE, ch, 0, 0, TO_ROOM);
			return 0;
		}

		if(!flee && GET_POS(MOUNT(ch)) == POS_FIGHTING)
		{
			ch->send("Your mount is currently engaged in combat!\r\n");
			Act("$n is unable to move $s mount.", FALSE, ch, 0, 0, TO_ROOM);
			return 0;
		}
	}

	if (GET_MOVE(ch) < need_movement && (IS_HORSE(ch) || !IS_NPC(ch)))
	{
		if (need_specials_check && ch->master)
			ch->send("You are too exhausted to follow.\r\n");
		else
			ch->send("You are too exhausted.\r\n");

		return 0;
	}

	/* Mortals cannot enter god rooms. */
	if (ROOM_FLAGGED(EXIT(ch, dir)->getToRoom(), ROOM_GODROOM) &&
	        GET_LEVEL(ch) < LVL_IMMORT)
	{
		ch->send("You aren't godly enough to use that room!\r\n");
		return 0;
	}

	return 1;

	/* Now we know we're allow to go into the room. */
}
int Character::NeededToMove(int dir)
{
	if( dir < 0 || dir >= NUM_OF_DIRS || !EXIT(this, dir) ) return 0;
	return NeededToMove(this->in_room->dir_option[dir]->getToRoom());
}
int Character::NeededToMove(Room* OtherRoom)
{
	int need_movement;

	if( !OtherRoom ) return 0;

	need_movement = (this->in_room->getSector()->getMovementLoss() + OtherRoom->getSector()->getMovementLoss()) / 2;

	//if(IS_HORSE(this) && RIDDEN_BY(this) && AFF_FLAGGED(RIDDEN_BY(this), AFF_NOTICE))
		//need_movement += 1;
	/* Only give perks to players. */
	if(!IS_NPC(this))
	{
		if(IS_WARDER(this) || IS_BLADEMASTER(this) || IS_FADE(this) || IS_GREYMAN(this) || IS_DREADLORD(this) || IS_OGIER(this))
			need_movement = 2;
		else if(this->hasWolfbrotherBonuses())
			need_movement = 2;
		else
		{
			need_movement = MIN(need_movement, 5);

			//if(AFF_FLAGGED(this, AFF_NOTICE) && !IS_THIEF(this))
			//{
				//need_movement += 1;
			//}
		}
	}
	return need_movement;
}

/* Return the character who should be penalized for this movement. Ex: Horses */
Character* Character::MovePenalizer()
{
	return (MOUNT(this) ? MOUNT(this) : this);
}
bool Character::ShouldLoveMoves()
{
	return (GET_LEVEL(this) < LVL_IMMORT || (IS_NPC(this) && !IS_HORSE(this)));
}
bool Character::SnuckOut()
{
	if( MOUNT(this) ) return false;
	return ((AFF_FLAGGED(this, AFF_EFFUSION) && MiscUtil::random(1, 125) <= GET_SKILL(this, SKILL_EFFUSION)));
}
bool Character::SnuckIn()
{
	if( MOUNT(this) ) return false;
	return	(	(AFF_FLAGGED(this, AFF_SNEAK) && MiscUtil::random(1, 125) <= GET_SKILL(this, SKILL_SNEAK)) || 
				(IS_GREYMAN(this)) /*|| (this->hasWolfbrotherBonuses())*/		);
}
bool Character::ShouldLayTrack()
{
	return (!PLR_FLAGGED(this, PLR_NOTRACK) && !MOB_FLAGGED(this, MOB_NOTRACK) && !MOB_FLAGGED(this, MOB_INVIS));
}

int Character::SimpleMove(int dir, int need_specials_check, bool flee)
{
	Room *was_in;
	Character* MovePenalizer = this->MovePenalizer();
	std::string mount_message;

	was_in = this->in_room;
	if(this->IsPurged() || !js_leave_triggers(this->in_room, this, dir) || this->IsPurged())
		return 0;
	//Script moved us! Could cause problems.
	if(this->in_room != was_in)
		return 0;
	if (EXIT(this, dir) && (!js_enter_triggers(EXIT(this, dir)->getToRoom(), this, dir) || this->IsPurged()) )
		return 0;
	//Script moved us! Could cause problems.
	if(this->in_room != was_in)
		return 0;
	if(!can_move(this, dir, need_specials_check, flee))
		return 0;

	if(MOUNT(this))
		mount_message = std::string(", riding ") + GET_NAME(MOUNT(this));

	if( !this->SnuckOut() || flee )
	{
		if(!this->player.LeaveMessage)
			sprintf(buf2, "$n leaves %s%s.", dirs[dir], mount_message.c_str());
		else
			sprintf(buf2, "$n %s %s%s.", this->player.LeaveMessage, dirs[dir], mount_message.c_str());
		Act(buf2, TRUE, this, 0, 0, TO_ROOM, NULL, true);
	}

	if (this->ShouldLayTrack() && this->in_room->isTrackable())
	{
		if( MOUNT(this) )
			MOUNT(this)->LayTrack(this->in_room,dir);
		else
			this->LayTrack(this->in_room, dir);
	}

	this->RemoveFromRoom();
	this->MoveToRoom(was_in->dir_option[dir]->getToRoom());

	if(MOUNT(this))
	{
		MOUNT(this)->RemoveFromRoom();
		MOUNT(this)->MoveToRoom(this->in_room);
	}

	if ( MovePenalizer->ShouldLoveMoves() )
		GET_MOVE(MovePenalizer) -= MovePenalizer->NeededToMove(this->in_room);

	if ( !this->SnuckIn() )
	{
		if(!this->player.ArriveMessage)
			sprintf(buf2, "$n has arrived from %s%s%s.",
			        ((dir == UP || dir == DOWN) ? "" : "the "),
			        (dir == UP ? "below": dir == DOWN ? "above" : dirs[rev_dir[dir]]), mount_message.c_str());
		else
			sprintf(buf2, "$n %s from %s%s%s.", this->player.ArriveMessage,
			        ((dir == UP || dir == DOWN) ? "" : "the "),
			        (dir == UP ? "below": dir == DOWN ? "above" : dirs[rev_dir[dir]]), mount_message.c_str());

		Act(buf2, TRUE, this, 0, 0, TO_ROOM, NULL, true);
	}

	if (this->desc != NULL)
		look_at_room(this, 0);

	//Fade Shadow Sensing
	if(IS_FADE(this) && this->in_room->isDark())
		this->send("%s\r\nYou feel the presence of shadows all around you.%s\r\n", COLOR_RED(this, CL_COMPLETE),
		           COLOR_NORMAL(this, CL_COMPLETE));

	if(ROOM_FLAGGED(was_in, ROOM_NOSOURCE) && !ROOM_FLAGGED(this->in_room, ROOM_NOSOURCE) && this->ChannelingAbility())
	{
		this->send("The True Source comes back into sight.\r\n");
	}
	/***************************/

	//Deathtraps
	if (ROOM_FLAGGED(this->in_room, ROOM_DEATH) && GET_LEVEL(this) < LVL_IMMORT)
	{
		MudLog(BRF, LVL_IMMORT, TRUE, "%s hit death trap #%d (%s)",
			GET_NAME(this), this->in_room->getVnum(), this->in_room->getName());

		if(IS_NPC(this))
			this->Extract();
		else
			this->Die();

		return 0;
	}
	return 1;
}
int perform_move(Character *ch, int dir, int need_specials_check)
{
	int ret_val = 0;
	Room *was_in;
	struct Follower *k, *next;

	if (ch == NULL || dir < 0 || dir >= NUM_OF_DIRS || FIGHTING(ch))
		return 0;
	else if(ch->PokerData || ch->PokerTable)
		ch->send("You must leave the table before you can leave the room.\r\n");
	else if ((!EXIT(ch, dir) && !buildwalk(ch, dir)) || !EXIT(ch, dir)->getToRoom())
		ch->send("Alas, you cannot go that way...\r\n");
	else if( (EXIT(ch, dir)->getHiddenLevel() > 0 && EXIT(ch, dir)->isFlagged(EX_CLOSED))
	|| EXIT(ch,dir)->isDisabled())
		ch->send("Alas, you cannot go that way...\r\n");
	else if (EXIT(ch, dir)->isFlagged(EX_CLOSED))
	{
		if (EXIT(ch, dir)->getKeywords())
		{
			ch->send("The %s seems to be closed.\r\n", fname(EXIT(ch, dir)->getKeywords()));
		}
		else
			ch->send("It seems to be closed.\r\n");
	}
	else
	{
		if (!ch->followers)
		{
			ret_val = ch->SimpleMove(dir, need_specials_check, false);
			REMOVE_BIT(PLR_FLAGS(ch), Q_BIT(PLR_NOTRACK));
			return ret_val;
		}

		was_in = ch->in_room;

		if (!ch->SimpleMove(dir, need_specials_check, false))
			return 0;

		for (k = ch->followers; k; k = next)
		{
			next = k->next;

			if ((k->follower->in_room == was_in) &&
			        (GET_POS(k->follower) >= POS_STANDING))
			{
				if(k->follower->timer)
					k->follower->CancelTimer(true);
				k->follower->send("You follow %s.\r\n", GET_NAME(ch));
				perform_move(k->follower, dir, 1);

				REMOVE_BIT(PLR_FLAGS(k->follower), Q_BIT(PLR_NOTRACK));
			}
		}
		return 1;
	}

	return 0;
}

CommandHandler do_lead = DEFINE_COMMAND
{
	int percent;
	char arg[MAX_INPUT_LENGTH];
	Character *target;

	OneArgument(argument, arg);

	if(!(target = get_char_room_vis(ch, arg)))
	{
		ch->send("You don't see them here.\r\n");
		return;
	}

	if(RIDDEN_BY(target))
	{
		sprintf(buf, "%s is already riding $M.", GET_NAME(RIDDEN_BY(target)));
		Act(buf, TRUE, ch, NULL, target, TO_CHAR);
		return;
	}
	if(target->master)
	{
		if(target->master == ch)
			Act("You are already leading $M.", FALSE, ch, 0, target, TO_CHAR);
		else
		{
			ch->send("%s is already following %s.\r\n", GET_NAME(target), GET_NAME(target->master));
			Act("$n refuses to follow $N.", TRUE, target, 0, ch, TO_NOTVICT);
		}
		return;
	}

	percent = MiscUtil::random(0, 50);

	if( (!MOB_FLAGGED(target, MOB_MOUNT) && !MOB_FLAGGED(target, MOB_SHADOW_MOUNT) && !MOB_FLAGGED(target, MOB_OGIER_MOUNT)) ||
	        ( MOB_FLAGGED(target, MOB_MOUNT) && IS_TROLLOC(ch)) || (MOB_FLAGGED(target, MOB_SHADOW_MOUNT) && !IS_TROLLOC(ch)) || (MOB_FLAGGED(target, MOB_OGIER_MOUNT) && !IS_OGIER(ch)) )
	{
		ch->send("Why would you think they would follow you?\r\n");
		return;
	}

	if(percent < GET_SKILL(ch, SKILL_RIDE))
		do_follow(target, (char*)GET_NAME(ch), 0, 0);

	else
	{
		Act("$N refuses to follow you!", FALSE, ch, 0, target, TO_CHAR);
		Act("You refuse $n's attemt to lead you.", FALSE, ch, 0, target, TO_VICT);
		Act("$N refuses $n's attempt to lead $M.", FALSE, ch, 0, target, TO_ROOM, NULL, true);
		return;
	}
};

CommandHandler do_move = DEFINE_COMMAND
{
	perform_move(ch, subcmd - 1, 0);
};

void Character::stopEavesdropping()
{
	Room *eaves = this->Eavesdropping;
	for(std::vector<Character*>::iterator eIter = eaves->eavesdropping.begin();eIter != eaves->eavesdropping.end();)
	{
		if((*eIter) == this)
			eIter = eaves->eavesdropping.erase(eIter);
		else
			++eIter;
	}
	this->Eavesdropping = 0;
	this->send("You stop eavesdropping.\r\n");
}

void Character::stopWarding()
{
	this->in_room->setEavesdroppingWarder(NULL);
	this->send("You release your ward.\r\n");
}
	
int Character::FindDoor(const char *type, char *dir, const char *cmdname)
{
	int door;
	if(!*type)
	{
		this->send("What is it you want to %s?\r\n", cmdname);
		return -1;
	}
	for(door = 0;door < NUM_OF_DIRS;++door)
	{
		if(EXIT(this, door) && EXIT(this, door)->getKeywords() && !str_cmp(EXIT(this, door)->getKeywords(), type))
			return door;
	}
	return -1;
}

bool Character::HasKey(int key)
{
	Object *o;

	for (o = this->carrying; o; o = o->next_content)
	{
		if (GET_OBJ_VNUM(o) == key)
		{
			return true;
		}
	}
	if (GET_EQ(this, WEAR_HOLD))
	{
		if (GET_OBJ_VNUM(GET_EQ(this, WEAR_HOLD)) == key)
		{
			return true;
		}
	}
	return false;
}

const int NEED_OPEN = 1;
const int NEED_CLOSED = 2;
const int NEED_UNLOCKED = 4;
const int NEED_LOCKED = 8;

const char *cmd_door[] =
    {
        "open",
        "close",
        "unlock",
        "lock",
        "pick"
    };

const int flags_door[] =
    {
        NEED_CLOSED | NEED_UNLOCKED,
        NEED_OPEN,
        NEED_CLOSED | NEED_LOCKED,
        NEED_CLOSED | NEED_UNLOCKED,
        NEED_CLOSED | NEED_LOCKED
    };

int PickReq(Object *obj, Room *r, int dir)
{
	if(!obj)
	{
		if(r->dir_option[dir])
			return r->dir_option[dir]->getPickRequirement();
		else
			return 0;
	}
	else
		return 0;
}

bool Object::IsPickProof()
{
	return (this->PickReq() == -1);
}
bool Object::IsClosed()
{
	return OBJVAL_FLAGGED(this, CONT_CLOSED);
}
bool Object::IsOpen()
{
	return !OBJVAL_FLAGGED(this, CONT_CLOSED);
}
bool Object::CanOpen()
{
	return (this->getType() == ITEM_CONTAINER && OBJVAL_FLAGGED(this, CONT_CLOSEABLE));
}
bool Object::IsContainer()
{
	return (this->getType() == ITEM_CONTAINER);
}
bool Object::IsLocked()
{
	return (OBJVAL_FLAGGED(this, CONT_LOCKED));
}
bool Object::CanLock()
{
	return ((GET_OBJ_VAL(this, 2) != -1));
}
bool Object::CanPick(Character *ch)
{
	return (GET_OBJ_VAL(this, 3) != -1 && GET_SKILL(ch, SKILL_PICK_LOCK) >= this->PickReq());
}
int Object::PickReq()
{
	if (this->getType() != ITEM_CONTAINER)
		return -1;//Can't pick a non-container!
	return GET_OBJ_VAL(this, 3);
}
int Object::KeyNum()
{
	return (GET_OBJ_VAL(this, 2));
}

CommandHandler do_gen_door = DEFINE_COMMAND
{
	int door;
	char type[MAX_INPUT_LENGTH], dir[MAX_INPUT_LENGTH];
	Object *obj = 0;
	Character *victim;
	Exit *exit = 0;

	skip_spaces(&argument);

	if (!*argument)
	{
		ch->send("%s what?\r\n", StringUtil::cap(cmd_door[subcmd]));
		return;
	}

	TwoArguments(argument, type, dir);

	/********* ROOM DOOR COMMANDS *********/
	if( (door = ch->FindDoor(type, dir, cmd_door[subcmd])) != -1)
	{
		exit = ch->in_room->dir_option[door];

		if(!exit->isOpen() && !exit->canBeSeen(ch))
		{
			ch->send("%s what?\r\n", StringUtil::cap(cmd_door[subcmd]));
			return;
		}
		if(!(exit->canOpen()))
		{
			ch->send("You can't %s that.\r\n", cmd_door[subcmd]);
			return;
		}
		switch(subcmd)
		{
			case SCMD_OPEN:
				if(exit->isLocked())
				{
					ch->send("But it's locked!\r\n");
					return;
				}
				if(exit->isOpen())
				{
					ch->send("It's already open!\r\n");
					return;
				}
				ch->in_room->removeDoorBit(door, EX_CLOSED);
				if (ch->in_room->getVnum() == 2501 || ch->in_room->getVnum() == 2502) {
					/*** NARGDOOR - DEFY ALL LOGIC!!!!! ***/
					if(ch->in_room->dir_option[door] && ch->in_room->dir_option[door]->getToRoom())
						ch->in_room->dir_option[door]->getToRoom()->setDoorBitOneSide(rev_dir[door],EX_CLOSED);
				}
				break;
			case SCMD_CLOSE:
				if(exit->isClosed())
				{
					ch->send("It's already closed!\r\n");
					return;
				}
				ch->in_room->setDoorBit(door, EX_CLOSED);
				if (ch->in_room->getVnum() == 2501 || ch->in_room->getVnum() == 2502) {
					/*** NARGDOOR - DEFY ALL LOGIC!!!!! ***/
					if(ch->in_room->dir_option[door] && ch->in_room->dir_option[door]->getToRoom())
						ch->in_room->dir_option[door]->getToRoom()->removeDoorBitOneSide(rev_dir[door],EX_CLOSED);
				}
				break;
			case SCMD_LOCK:
				if(!exit->canLock())
				{
					ch->send("You can't seem to find the keyhole.\r\n");
					return;
				}
				if(exit->isLocked())
				{
					ch->send("It's already locked.\r\n");
					return;
				}
				if(exit->isOpen())
				{
					ch->send("It needs to be closed first!\r\n");
					return;
				}
				if(!ch->HasKey(exit->getKey()))
				{
					ch->send("You don't have the proper key.\r\n");
					return;
				}
				ch->in_room->setDoorBit(door, EX_LOCKED);
				if (ch->in_room->getVnum() == 2501 || ch->in_room->getVnum() == 2502) {
					/*** NARGDOOR - DEFY ALL LOGIC!!!!! ***/
					if(ch->in_room->dir_option[door] && ch->in_room->dir_option[door]->getToRoom())
						ch->in_room->dir_option[door]->getToRoom()->removeDoorBitOneSide(rev_dir[door],EX_LOCKED);
				}
				break;
			case SCMD_UNLOCK:
				if(!exit->canLock())
				{
					ch->send("You can't seem to find the keyhole.\r\n");
					return;
				}
				if(!exit->isLocked())
				{
					ch->send("It's not even locked!\r\n");
					return;
				}
				if(!exit->isClosed())
				{
					ch->send("It needs to be closed first!\r\n");
					return;
				}
				if(!ch->HasKey(exit->getKey()))
				{
					ch->send("You don't have the proper key.\r\n");
					return;
				}

				/* Key Breaking after a certain number of uses. -Narg */
				/* The key finding code is shamelessly stolen from HasKey() :) */
				if (!IS_NPC(ch))
				{
					if (ch->HasObjectInInventory(exit->getKey()))
					{
						Object * key = ch->GetObjectFromInventory(exit->getKey());
						--GET_OBJ_VAL(key, 0);
						/* If it starts as 0, disabled, it cannot */
						/* equal after a -- operation. */
						if (GET_OBJ_VAL(key, 0) == 0)
						{
							Act("$p breaks as you attempt to remove it from the lock...", FALSE, ch, key, NULL, TO_CHAR);
							Act("$p breaks as $n attempts to remove it from the lock...", FALSE, ch, key, NULL, TO_ROOM);
							key->Extract();
						}
					}
					else
					{
						if (GET_OBJ_VNUM(GET_EQ(ch, WEAR_HOLD)) == exit->getKey() )
						{
							Object * key = GET_EQ(ch, WEAR_HOLD);
							--GET_OBJ_VAL(key, 0);
							if (GET_OBJ_VAL(key, 0) == 0)
							{
								Act("$p breaks as you attempt to remove it from the lock...", FALSE, ch, key, NULL, TO_CHAR);
								Act("$p breaks as $n attempts to remove it from the lock...", FALSE, ch, key, NULL, TO_ROOM);
								key->Extract();
							}
						}
					}
				}
				ch->in_room->removeDoorBit(door, EX_LOCKED);
				if (ch->in_room->getVnum() == 2501 || ch->in_room->getVnum() == 2502) {
					/*** NARGDOOR - DEFY ALL LOGIC!!!!! ***/
					if(ch->in_room->dir_option[door] && ch->in_room->dir_option[door]->getToRoom())
						ch->in_room->dir_option[door]->getToRoom()->setDoorBitOneSide(rev_dir[door],EX_LOCKED);
				}
				break;
			case SCMD_PICK:
				if(exit->isOpen())
				{
					ch->send("But it's open!\r\n");
					return;
				}
				if(!exit->isLocked())
				{
					ch->send("It's not even locked!\r\n");
					return;
				}
				if(exit->isPickProof())
				{
					ch->send("It resists your attempt to pick it.\r\n");
					return;
				}
				if(!exit->canPick(ch))
				{
					ch->send("You fail to pick the lock.\r\n");
					return;
				}
				ch->in_room->removeDoorBit(door, EX_LOCKED);
				ch->send("The lock quickly gives way to your skills.\r\n");
				return;
			default:
				ch->send("Do what now?\r\n");
				return;
		}
	}
	/************ END OF ROOM DOOR **************/


	/************ ITEM CONTAINERS  *************/
	else if (generic_find(type, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &victim, &obj) && obj->getType() == ITEM_CONTAINER)
	{
		switch(subcmd)
		{
			case SCMD_OPEN:
				if(obj->IsLocked())
				{
					ch->send("But it's locked!\r\n");
					return;
				}
				if(obj->IsOpen())
				{
					ch->send("It's already open!\r\n");
					return;
				}
				TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_CLOSED);
				break;
			case SCMD_CLOSE:
				if(obj->IsClosed())
				{
					ch->send("It's already closed!\r\n");
					return;
				}
				TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_CLOSED);
				break;
			case SCMD_LOCK:
				if(!obj->CanLock())
				{
					ch->send("You can't seem to find a keyhole.\r\n");
					return;
				}
				if(obj->IsOpen())
				{
					ch->send("But it's open!\r\n");
					return;
				}
				if(obj->IsLocked())
				{
					ch->send("It's already locked.\r\n");
					return;
				}
				if(!ch->HasKey(obj->KeyNum()))
				{
					ch->send("You don't have the proper key.\r\n");
					return;
				}
				TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_LOCKED);
				break;
			case SCMD_UNLOCK:
			{
				if(!obj->CanLock())
				{
					ch->send("You can't seem to find a keyhole.\r\n");
					return;
				}
				if(obj->IsOpen())
				{
					ch->send("But it's open!\r\n");
					return;
				}
				if(!obj->IsLocked())
				{
					ch->send("It's not even locked!\r\n");
					return;
				}
				if(!ch->HasKey(obj->KeyNum()))
				{
					ch->send("You don't have the proper key.\r\n");
					return;
				}

				//At this point, the unlock succeeded.
				Object *key = GET_EQ(ch, WEAR_HOLD);

				if(key == NULL || GET_OBJ_VNUM(key) != obj->KeyNum())
					key = ch->GetObjectFromInventory(obj->KeyNum());

				if(key)
				{
					--GET_OBJ_VAL(key, 0);
					if(GET_OBJ_VAL(key, 0) <= 0)
					{
						Act("$p breaks as you attempt to remove it from the lock...", FALSE, ch, key, NULL, TO_CHAR);
						Act("$p breaks as $n attempts to remove it from the lock...", FALSE, ch, key, NULL, TO_ROOM);
						key->Extract();
					}
				}

				TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_LOCKED);
				break;
			}
			case SCMD_PICK:
			{
				if(obj->IsOpen())
				{
					ch->send("But it's open!\r\n");
					return;
				}
				if(!obj->IsLocked())
				{
					ch->send("It's not even locked!\r\n");
					return;
				}
				if(!obj->CanPick(ch))
				{
					ch->send("You fail to pick the lock.\r\n");
					return;
				}
				TOGGLE_BIT(GET_OBJ_VAL(obj, 1), CONT_LOCKED);
				break;
			}
			default:
				ch->send("Do what now?\r\n");
				return;
		}
	}
	else
	{
		ch->send("You don't see a %s here.\r\n", type);
		return;
	}

	//Success!
	if( subcmd == SCMD_PICK )
		ch->send("The lock quickly gives way to your skills.\r\n");
	else
		ch->send("You %s %s%s.\r\n", cmd_door[subcmd], exit ? "the " : "", obj ? obj->GetSDesc() : exit->getKeywords());

	//Notify the room.
	sprintf(buf, "%s %ss %s%s.", GET_NAME(ch), cmd_door[subcmd], exit ? "the " : "", obj ? obj->GetSDesc() : exit->getKeywords());
	Act(buf, TRUE, ch, 0, 0, TO_ROOM, NULL, true);

	Exit *reverse_exit = 0;
	//Notify the other end of the door if it was an open/close.
	if(exit && EXIT(ch, door) && EXIT(ch, door)->getToRoom() != nullptr && EXIT(ch, door)->getToRoom() &&
	        (subcmd == SCMD_OPEN || subcmd == SCMD_CLOSE) &&
	        (reverse_exit = EXIT(ch, door)->getToRoom()->dir_option[rev_dir[door]]))
	{
		sprintf(buf, "The %s is %s from the other side.\r\n", reverse_exit->getKeywords(), subcmd == SCMD_OPEN ? "opened" : "closed");
		sendToRoom(buf, EXIT(ch, door)->getToRoom());
	}
};

CommandHandler do_enter = DEFINE_COMMAND
{
	int door, n = 0;
	unsigned int i;
	std::string num;
	std::list<Gate *>::iterator g;

	OneArgument(argument, buf);

	for(i = 0; i < strlen(buf); ++i)
	{
		if (buf[i] == '.')
			break;

		num += buf[i];
	}

	++i;

	if (i - 1 == strlen(buf))
	{
		i = 0;
		num = "1";
	}

	//Found a dot argument
	if (!str_cmp(buf + i, "Gate") && ch->in_room->getNumberOfGates())
	{
		n = atoi(num.c_str()) - 1;

		std::list<Gate*> TempGateList = ch->in_room->getGates();
		for(g = TempGateList.begin();g != TempGateList.end() && n;++g, --n);

		if(!n && g != TempGateList.end())
		{
			ch->StepThroughGate( (*g) );
		}
		else
			ch->send("Which gate is that?\r\n");
		return;
	}

	if (*buf)
	{			/* an argument was supplied, search for door
						 * keyword */
		for (door = 0; door < NUM_OF_DIRS; door++)

			if (EXIT(ch, door))

				if (EXIT(ch, door)->getKeywords())

					if (!str_cmp(EXIT(ch, door)->getKeywords(), buf))
					{
						perform_move(ch, door, 1);
						return;
					}

		ch->send("There is no %s here.\r\n", buf);
		return;
	}

	else if (ch->in_room->getSector() == RoomSector::inside)
		ch->send("You are already indoors.\r\n");

	else
	{
		/* try to locate an entrance */
		for (door = 0; door < NUM_OF_DIRS; door++)
		{
			if (EXIT(ch, door))
			{
				if (EXIT(ch, door)->getToRoom())
				{
					if (!EXIT(ch, door)->isFlagged( EX_CLOSED ) && EXIT(ch, door)->getToRoom()->getSector() == RoomSector::inside)
					{
						perform_move(ch, door, 1);
						return;
					}
				}
			}
		}
	}
	ch->send("You can't seem to find anything to enter.\r\n");

};


CommandHandler do_leave = DEFINE_COMMAND
{
	int door;

	if (ch->in_room->getSector() == RoomSector::inside)
		ch->send("You are outside.. where do you want to go?\r\n");

	else
	{
		for (door = 0; door < NUM_OF_DIRS;++door)
		{
			if (EXIT(ch, door))
			{
				if (EXIT(ch, door)->getToRoom())
				{
					if (!EXIT(ch, door)->isFlagged(EX_CLOSED) && EXIT(ch, door)->getToRoom()->getSector() == RoomSector::inside)
					{
						perform_move(ch, door, 1);
						return;
					}
				}
			}
		}
		ch->send("I see no obvious exits to the outside.\r\n");
	}
};

CommandHandler do_stand = DEFINE_COMMAND
{
	switch (GET_POS(ch))
	{
		case POS_STANDING:
			Act("You are already standing.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_SITTING:
			Act("You stand up.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n clambers to $s feet.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_STANDING;
			ch->StandFromChair();
			break;
		case POS_RESTING:
			Act("You stop resting, and stand up.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n stops resting, and clambers on $s feet.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_STANDING;
			break;
		case POS_SLEEPING:
			Act("You have to wake up first!", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_FIGHTING:
			Act("Do you not consider fighting as standing?", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_FLYING:
			Act("You fly down and land on the ground.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n swoops down and lands on the ground.", FALSE, ch, 0, 0, TO_ROOM, NULL, true);
			break;
		default:
			Act("You stop floating around, and put your feet on the ground.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n stops floating around, and puts $s feet on the ground.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_STANDING;
			break;
	}
};


CommandHandler do_sit = DEFINE_COMMAND
{
	Object *target;
	char arg1[MAX_INPUT_LENGTH];
	OneArgument(argument, arg1);

	switch (GET_POS(ch))
	{
		case POS_STANDING:
			if( *arg1 != '\0' )
			{
				if( (target = ItemUtil::get()->getObjectInListVis(ch, arg1, ch->in_room->contents)) == NULL )
				{
					ch->send("You don't see that here.\r\n");
					return;
				}
				if (target->getType() != ITEM_CHAIR)
				{
					ch->send("You can't sit on that!\r\n");
					return;
				}
				if( target->SatOnBy != NULL )
				{
					ch->send("%s is already seated there.\r\n", GET_NAME(target->SatOnBy));
					return;
				}
				ch->SitOnChair(target, true);
			}
			else
			{
				Act("You sit down.", FALSE, ch, 0, 0, TO_CHAR);
				Act("$n sits down.", FALSE, ch, 0, 0, TO_ROOM, NULL, true);
			}
			GET_POS(ch) = POS_SITTING;
			break;
		case POS_SITTING:
			ch->send("You're sitting already.\r\n");
			break;
		case POS_RESTING:
			Act("You stop resting, and sit up.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n stops resting.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_SITTING;
			break;

		case POS_SLEEPING:
			Act("You have to wake up first.", FALSE, ch, 0, 0, TO_CHAR);
			break;

		case POS_FIGHTING:
			Act("Sit down while fighting? are you MAD?", FALSE, ch, 0, 0, TO_CHAR);
			break;

		default:
			Act("You stop floating around, and sit down.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n stops floating around, and sits down.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_SITTING;
			break;
	}
};

CommandHandler do_rest = DEFINE_COMMAND
{
	switch (GET_POS(ch))
	{
		case POS_STANDING:
			Act("You sit down and rest your tired bones.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n sits down and rests.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_RESTING;
			break;

		case POS_SITTING:
			if( ch->SittingOn() != NULL )
			{
				ch->send("You cannot rest while sitting on %s.\r\n", ch->SittingOn()->GetSDesc());
				return;
			}
			Act("You rest your tired bones.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n rests.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_RESTING;
			break;
		case POS_RESTING:
			Act("You are already resting.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_SLEEPING:
			Act("You have to wake up first.", FALSE, ch, 0, 0, TO_CHAR);
			break;
		case POS_FIGHTING:
			Act("Rest while fighting?  Are you MAD?", FALSE, ch, 0, 0, TO_CHAR);
			break;
		default:
			Act("You stop floating around, and stop to rest your tired bones.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n stops floating around, and rests.", FALSE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_RESTING;
			break;
	}
};

CommandHandler do_sleep = DEFINE_COMMAND
{
	switch (GET_POS(ch))
	{
		case POS_STANDING:
		case POS_SITTING:
		case POS_RESTING:
			if( ch->SittingOn() != NULL )
			{
				ch->send("You cannot go to sleep while sitting on %s.\r\n", ch->SittingOn()->GetSDesc());
				return;
			}
			ch->send("You go to sleep.\r\n");
			Act("$n lies down and falls asleep.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_SLEEPING;
			break;
		case POS_SLEEPING:
			ch->send("You are already sound asleep.\r\n");
			break;
		case POS_FIGHTING:
			ch->send("Sleep while fighting?  Are you MAD?\r\n");
			break;

		default:
			Act("You stop floating around, and lie down to sleep.", FALSE, ch, 0, 0, TO_CHAR);
			Act("$n stops floating around, and lie down to sleep.", TRUE, ch, 0, 0, TO_ROOM, NULL, true);
			GET_POS(ch) = POS_SLEEPING;
			break;
	}
};

CommandHandler do_wake = DEFINE_COMMAND
{
	Character *vict;
	int self = 0;

	OneArgument(argument, ::arg);

	if (*::arg)
	{
		if (GET_POS(ch) == POS_SLEEPING)
			ch->send("Maybe you should wake yourself up first.\r\n");
		else if ((vict = get_char_room_vis(ch, ::arg)) == NULL)
			ch->send(NOPERSON);
		else if (vict == ch)
			self = 1;
		else if (GET_POS(vict) > POS_SLEEPING)
			Act("$E is already awake.", FALSE, ch, 0, vict, TO_CHAR);
		else if (AFF_FLAGGED(vict, AFF_SLEEP))
			Act("You can't wake $M up!", FALSE, ch, 0, vict, TO_CHAR);
		else if (GET_POS(vict) < POS_SLEEPING)
			Act("$E's in pretty bad shape!", FALSE, ch, 0, vict, TO_CHAR);
		else
		{
			Act("You wake $M up.", FALSE, ch, 0, vict, TO_CHAR);
			Act("You are awakened by $n.", FALSE, ch, 0, vict, TO_VICT | TO_SLEEP, NULL, true);
			Act("$N wakes you up.", FALSE, vict, 0, ch, TO_CHAR);
			GET_POS(vict) = POS_SITTING;
		}

		if (!self)
			return;
	}

	if (AFF_FLAGGED(ch, AFF_SLEEP))
		ch->send("You can't wake up!\r\n");
	else if (GET_POS(ch) > POS_SLEEPING)
		ch->send("You are already awake...\r\n");
	else
	{
		ch->send("You awaken, and sit up.\r\n");
		Act("$n awakens.", TRUE, ch, 0, 0, TO_ROOM);
		GET_POS(ch) = POS_SITTING;
	}
};

/* Galnor - 11/05/2009 - Knock on a door. */
CommandHandler do_knock = DEFINE_COMMAND
{
	char doorName[MAX_INPUT_LENGTH];
	Exit *dir = 0;
	int iDir = NUM_OF_DIRS;

	OneArgument(argument, doorName);

	if( !*doorName )
	{//User didn't enter a door name.
		ch->send("What door do you wish to knock on?\r\n");
		return;
	}

	//Now, look to see if the door entered by the user actually exists.
	for(unsigned int i = 0;i < NUM_OF_DIRS;++i)
	{
		if( ch->in_room->dir_option[i] && ch->in_room->dir_option[i]->canBeSeen(ch)
			&& ch->in_room->dir_option[i]->isDoor() && !str_cmp(doorName, ch->in_room->dir_option[i]->getKeywords()) )
		{
			dir = ch->in_room->dir_option[i];
			iDir = i;
			break;
		}
	}

	if( !dir || (ch->in_room->isDark() && !CAN_SEE_IN_DARK(ch)) )
	{//Door was not found / can't be seen.
		ch->send("You don't see a '%s' here.\r\n", doorName);
		return;
	}
	if( dir->isOpen() )
	{
		ch->send("The %s appears to be open.\r\n", dir->getKeywords());
		return;
	}

	ch->send("You knock on the %s.\r\n", dir->getKeywords());

	for(Character *onLooker = ch->in_room->people;onLooker;onLooker = onLooker->next_in_room)
	{//Show each person in the room that a door has been knocked on.

		if( onLooker == ch ) continue;//This is the knocker.

		std::string dirText;
		if     ( iDir == UP )	dirText = "above";
		else if( iDir == DOWN ) dirText = "below";
		else					dirText = (std::string("to the ") + dirs[iDir]);

		//If they can't see the door, show them that 'something' was knocked on.
		if( !dir->canBeSeen( onLooker ) || (ch->in_room->isDark() && !CAN_SEE_IN_DARK(onLooker)) )
			onLooker->send("%s knocks on something %s.\r\n", PERS(ch, onLooker), dirText.c_str());
		else
			onLooker->send("%s knocks on the %s.\r\n", PERS(ch, onLooker), dir->getKeywords());
	}

	Room *OtherRoom;
	if( (OtherRoom = dir->getToRoom()) != NULL )
	{//Notify everyone on the other end of the door that it was knocked on.
		int iRevDir = rev_dir[iDir];
		Exit *revDir = OtherRoom->dir_option[iRevDir];

		if( revDir && revDir->isDoor() )
		{
			for( Character *onLooker = OtherRoom->people;onLooker;onLooker = onLooker->next_in_room )
			{
				if( revDir->canBeSeen( onLooker ) && (!OtherRoom->isDark() || CAN_SEE_IN_DARK(onLooker)) )
				{
					onLooker->send("Someone knocks on the %s from the other side.\r\n", dir->getKeywords());
				}
				else
				{
					onLooker->send("A knock comes from somewhere nearby.\r\n");
				}
			}
		}
	}
};

CommandHandler do_follow = DEFINE_COMMAND
{
	Character *leader;

	OneArgument(argument, buf);

	if (*buf)
	{
		if (!(leader = get_char_room_vis(ch, buf)))
		{
			ch->send(NOPERSON);
			return;
		}
	}
	else
	{
		ch->send("Whom do you wish to follow?\r\n");
		return;
	}

	if( !isInArena(ch) && GET_RACE(leader) != GET_RACE(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_LEVEL(leader) < LVL_IMMORT && !IS_NPC(ch))
	{
		ch->send("What?! Don't you think they'd betray you?\r\n");
		return;
	}

	if(IS_NPC(leader) && !leader->desc && leader != ch && !IS_NPC(ch))
	{
		ch->send("%s doesn't want you to follow.\r\n", GET_NAME(leader));
		return;
	}

	if (ch->master == leader)
	{
		Act("You are already following $M.", FALSE, ch, 0, leader, TO_CHAR);
		return;
	}

	if( isInArena(ch) && JS_isArenaInFFAMode() )
	{
		Act("That wouldn't be wise, it's every man for himself!", FALSE, ch, 0, leader, TO_CHAR);
		return;
	}

	if( isInArena(ch) && getArenaTeam(ch) != getArenaTeam(leader) && !JS_isArenaInFFAMode() )
	{
		Act("Why would you follow someone not on your team?!", FALSE, ch, 0, leader, TO_CHAR);
		return;
	}

/*	if(IS_FADE(ch))
	{
		ch->send("Myrddraal are leaders, not followers!\r\n");
		return;
	}
*/
	else
	{			/* Not Charmed follow person */
		if (leader == ch)
		{
			if (!ch->master)
			{
				ch->send("You are already following yourself.\r\n");
				return;
			}

			ch->stopFollowing();
		}

		else
		{
			if (ch->circleFollow(leader))
			{
				Act("Sorry, but following in loops is not allowed.", FALSE, ch, 0, 0, TO_CHAR);
				return;
			}

			if (ch->master)
				ch->stopFollowing();

			REMOVE_BIT_AR(AFF_FLAGS(ch), AFF_GROUP);
			add_follower(ch, leader);
		}
	}
};

bool Character::check_if_pc_in_group()
{	
	for (Follower *f = this->followers; f; f = f->next)
	{
		if(!IS_NPC(f->follower))
			return true;
	}
	return false;
}

int Character::group_members_in_zone()
{
	int count = 0;
	
	for (Follower *f = this->followers; f; f = f->next)
	{
		if(f->follower->in_room->getZone() == this->in_room->getZone())
			count++;
	}
	return count;
}

Character *Character::IsLeadingMount()
{
	struct Follower *f;
	Character *ch;
	
	for(f = this->followers; f; f = f->next)
	{
		ch = f->follower;

		if(IS_HORSE(ch))
			return ch;
	}

	return 0;
}

/* Called when a character that follows/is followed dies */
void die_follower(Character * ch)
{
	struct Follower *j = 0, *k = 0;

	if (ch->master)
		ch->stopFollowing();

	for (k = ch->followers; k; k = j)
	{
		j = k->next;
		k->follower->stopFollowing();
	}
}

/* Do NOT call this before having checked if a circle of followers */
/* will arise. CH will follow leader                               */
void add_follower(Character * ch, Character * leader)
{
	struct Follower *k;
	if( ch->IsPurged() || leader->IsPurged() )
		return;

	if (ch->master)
	{
		core_dump();
		return;
	}

	if(!ch || !leader)
		return;

	ch->master = leader;

	k = new Follower;
	memset(k, 0, sizeof(Follower));

	k->follower = ch;
	k->next = leader->followers;
	leader->followers = k;

	Act("You now follow $N.", FALSE, ch, 0, leader, TO_CHAR);

	if (CAN_SEE(leader, ch))
		Act("$n starts following you.", TRUE, ch, 0, leader, TO_VICT);

	Act("$n starts to follow $N.", TRUE, ch, 0, leader, TO_NOTVICT);
}

void Character::stopFollowing()
{
	struct Follower *j = 0, *k = 0;

	if (this->master == NULL)
	{
		core_dump();
		return;
	}

	else
	{
		Act("You stop following $N.", FALSE, this, 0, this->master, TO_CHAR);
		Act("$n stops following $N.", TRUE, this, 0, this->master, TO_NOTVICT);
		Act("$n stops following you.", TRUE, this, 0, this->master, TO_VICT);
	}

	if (this->master->followers->follower == this)
	{	/* Head of follower-list? */
		k = this->master->followers;
		this->master->followers = k->next;
		delete (k);
	}

	else
	{			/* locate follower who is not head of list */
		for (k = this->master->followers; k->next->follower != this; k = k->next)
			;

		j = k->next;
		k->next = j->next;
		delete (j);
	}

	this->master = NULL;
	REMOVE_BIT_AR(AFF_FLAGS(this), AFF_GROUP);
}

bool Character::circleFollow(Character *target)
{
	for (Character *k = target; k; k = k->master)
	{
		if (k == this)
			return true;
	}
	return false;
}

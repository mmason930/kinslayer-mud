/* ************************************************************************
*   File: spells.c                                      Part of CircleMUD *
*  Usage: Implementation of "manual spells".  Circle 2.2 spell compat.    *
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
#include "constants.h"
#include "interpreter.h"
#include "olc.h"
#include "screen.h"
#include "shop.h"
#include "weaves.h"
#include "zones.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"

extern Object *object_list;
extern Character *character_list;
extern struct Index *obj_index;
extern Descriptor *descriptor_list;

extern struct default_mobile_stats *mob_defaults;
extern struct apply_mod_defaults *apmd;

#define NOLOC_CHILL_MESSAGE		"You feel a chill pass through your body.\r\n"

void add_follower(Character * ch, Character * leader);
int mag_savingthrow(Character * ch, int type);
void name_to_drinkcon(Object * obj, int type);
void name_from_drinkcon(Object * obj);
void sendToZone(const char *messg, int zone_rnum);

const int IS_NORTH		= 1;
const int IS_EAST		= 2;
const int IS_SOUTH		= 3;
const int IS_WEST		= 4;

const int IS_NORTHEAST	= 5;
const int IS_NORTHWEST	= 6;
const int IS_SOUTHEAST	= 7;
const int IS_SOUTHWEST	= 8;

const int IS_HERE		= 9;

int Zone::Distance( Zone *OtherZone )
{
	int x = abs(GetX() - OtherZone->GetX()),
		y = abs(GetY() - OtherZone->GetY());
	return MAX(x,y);
}

int find_zone_slope(int zone_obj, int zone_char)
{
	int loc_x = 0;	//East to West
	int loc_y = 0;	//North to South
	int loc;		//Final location

	Zone *ObjZone = ZoneManager::GetManager().GetZoneByRnum(zone_obj),
		 *CharZone = ZoneManager::GetManager().GetZoneByRnum(zone_char);
	if( !ObjZone || !CharZone ) return 10000;//Nonexisting zone is far away. Yes!

	/* First start off by finding out the basic location	*/

	if(ObjZone->GetX() > CharZone->GetX())
		loc_x = IS_EAST;
	if(ObjZone->GetX() < CharZone->GetX())
		loc_x = IS_WEST;
	if(ObjZone->GetY() > CharZone->GetY())
		loc_y = IS_NORTH;
	if(ObjZone->GetY() < CharZone->GetY())
		loc_y = IS_SOUTH;
	if(ObjZone->GetX() == CharZone->GetX() && ObjZone->GetY() == CharZone->GetY())
		return (IS_HERE);

	/* Round Two Begins! Now we find whether or not the zone is in more than one direction. */
	if(loc_y == IS_NORTH && loc_x == IS_WEST)
		loc = IS_NORTHWEST;
	else if(loc_y == IS_NORTH && loc_x == IS_EAST)
		loc = IS_NORTHEAST;
	else if(loc_y == IS_SOUTH && loc_x == IS_WEST)
		loc = IS_SOUTHWEST;
	else if(loc_y == IS_SOUTH && loc_x == IS_EAST)
		loc = IS_SOUTHEAST;
	else
	{
		if(loc_x)		loc = loc_x;
		else if(loc_y)	loc = loc_y;
		else			loc = IS_HERE;  // better than nothing
	}

	return loc;
}

/*
 * Special spells appear below.
 */

ASPELL(spell_regen)
{
	if (IS_MOB(victim))
	{
		Act("You try to reduce $N's fatigue, but nothing seems to happen.", true, ch, NULL, victim, TO_CHAR);
		return;
	}
	if (ch != victim)
	{
		float perc = atof( weave->getAttribute("PercentInc").c_str() );
		std::string ToRoom = weave->getAttribute("ToRoom"),
					ToChar = weave->getAttribute("ToChar"),
					ToVict = weave->getAttribute("ToVict");

		GET_MOVE(victim) = MIN(GET_MAX_MOVE(victim), MAX(GET_MOVE(victim), perc*GET_MAX_MOVE(victim)));
		Act(ToChar.c_str(), TRUE, ch, NULL, victim, TO_CHAR);
		Act(ToVict.c_str(), TRUE, ch, NULL, victim, TO_VICT);
		Act(ToRoom.c_str(), TRUE, ch, NULL, victim, TO_NOTVICT);
	}
	else
		ch->send("You can't cast that on yourself!\r\n");
}

ASPELL(spell_create_water)
{
//	Object* tobj;

	if( !*argument )
	{
		ch->send("Where do you wish to put the water you create?\r\n");
		return;
	}
	if( obj->getType() != ITEM_DRINKCON )
	{
		ch->send("You cannot put water in that!\r\n");
		return;
	}
	if( obj->GetTotalVal2() != LIQ_WATER )
	{
		ch->send("That container already containers another liquid.\r\n");
		return;
	}
	if( obj->GetTotalVal1() == obj->GetTotalVal0() )
	{
		ch->send("The container is already full.\r\n");
		return;
	}

	GET_OBJ_VAL(obj, 2) = LIQ_WATER;
	GET_OBJ_VAL(obj, 1) = MIN(GET_OBJ_VAL(obj, 0), GET_OBJ_VAL(obj, 1) + 18);
	GET_OBJ_VAL(obj, 3) = 0;

	ch->send("You weave flows into %s to fill it with some water.\r\n", obj->GetSDesc());
	Act("$n waves $s hands around $p, which now seems slightly heftier.", TRUE, ch, obj, NULL, TO_NOTVICT);
}

ASPELL(spell_locate_object)
{
	Room* room;
	Character *holder = 0;
	char arg[MAX_INPUT_LENGTH];
	int distance = 0, location = 0, zone = 0, found = 0;

	OneArgument(argument, arg);

	if(!*arg)
	{
		ch->send("What do you want to locate?\r\n");
		return;
	}

	//These will hold the display sent to the caster.
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	*buf = '\0';//Clear the buffer

	for( obj = object_list;obj;obj = obj->next )
	{
		if( obj->IsPurged() ) continue;
		room = obj->getRoom();
		if( !room ) continue;

		if(ROOM_FLAGGED(room, ROOM_NOPORT) || ROOM_FLAGGED(room, ROOM_NOMAGIC) || room->getZone()->IsClosed())
			continue;

		if( !is_name(arg, obj->getName()) )
			continue;
		holder = obj->FindHolder();
		if( holder && (GET_LEVEL(holder) >= LVL_IMMORT || AFF_FLAGGED(holder, AFF_INVISIBLE) || 
			(IS_NPC(holder) && MOB_FLAGGED(holder, MOB_INVIS)))
		  )
		{
			continue;
		}

		distance = room->getZone()->Distance(ch->in_room->getZone());
		location = find_zone_slope(room->getZoneNumber(), ch->in_room->getZoneNumber());

		if(distance <= GET_SKILL(ch, SPELL_LOCATE_OBJECT) / 20)
		{
			found = 1;
			std::string Dist = DistanceString(distance);
			if( Dist.size() ) Dist = " " + Dist + " ";
			else Dist = " ";

			sprintf(buf2, "You sense %s%s%s.  --  [%s%s%s]\r\n",
				obj->GetSDesc(), Dist.c_str(), loc[location], COLOR_GREEN(ch, CL_COMPLETE),
				room->gateCode().c_str(), COLOR_NORMAL(ch, CL_COMPLETE));

			//Make sure the buffer can hold this new line...
			if( strlen(buf) + strlen(buf2) < MAX_STRING_LENGTH )//We have enough space.
				strcat(buf, buf2);

		}
	}
	if(!found)
	{
		ch->send("You do not sense that anywhere nearby.\r\n");
		return;
	}
	else
		page_string(ch->desc, buf, TRUE);
}

ASPELL(spell_tornado)
{
	Character* vict, *vict_next;
	int PermDist, dist;

	if( ch->in_room->getSector() == RoomSector::inside )
	{
		ch->send("Your attempt to form a tornado indoors didn't quite go according to plan.\r\n");
		return;
	}

	sendToRoom("A huge twister moves across the room!\r\n", ch->in_room);
	PermDist =	atoi(weave->getAttribute("DistFactor").c_str()) *
			(GET_SKILL(ch, weave->getVnum()) / atoi(weave->getAttribute("DistThreshhold").c_str()));

	for(vict = ch->in_room->people;vict != NULL;vict = vict_next)
	{
		vict_next = vict->next_in_room;//We'll be altering the list, so grab the next in the list first.

		if( vict == ch || IS_NPC(vict) ) continue; //Tornado will not throw the caster(or MOBs 03/09/2009).
		if( RIDDEN_BY(vict) != NULL ) continue; //Skip over the horse. We'll move them when the rider gets moved.

		vict->send("The raging gusts of the tornado pick you up and throw you aside.\r\n");
		Act("$n is picked up by the raging gusts of the tornado.", TRUE, vict, NULL, NULL, TO_ROOM);

		dist = PermDist;//Constant needs changing

		Room* cur_room = ch->in_room;//This will be where the victim ultimately lands.
		for(;dist > 0;--dist)
		{
			//Before looking for another room, check to see if this room is inside. If so, we're done.
			if( cur_room->getSector() == RoomSector::inside )
				break;

			int start_exit = MiscUtil::random(0, NUM_OF_DIRS - 1);
			for(int i = start_exit + 1, n = 0;n < NUM_OF_DIRS;++i, ++n)
			{
				if( i >= NUM_OF_DIRS ) //We need to move back to the front of the exit array.
					i = 0;

				//Does this exit even exist?
				if( cur_room->dir_option[i] != NULL && cur_room->dir_option[i]->getToRoom() != NULL )
				{
					//Is the exit closed off?
					if( cur_room->dir_option[i]->isClosed() )
						continue;

					//Move to this room.
					cur_room = cur_room->dir_option[i]->getToRoom();
					break;
				}
			}
		}
		//We have a room. Now, let's move the victim.
		vict->RemoveFromRoom();
		vict->MoveToRoom( cur_room );
		vict->InterruptTimer();
		if( MOUNT(vict) != NULL )
		{
			MOUNT(vict)->RemoveFromRoom();
			MOUNT(vict)->MoveToRoom( cur_room );
		}
		look_at_room(vict, 1);

		Act("$n falls from midair, landing with a loud thud.", TRUE, vict, NULL, NULL, TO_ROOM);
	}
}

ASPELL(spell_gate)
{
	char arg[MAX_INPUT_LENGTH];

	OneArgument(argument, arg);

	if( !*arg )
	{
		ch->send("You must enter a gate code.\r\n");
		return;
	}

	if( !ROOM_FLAGGED( ch->in_room, ROOM_NOPORT ) )
	{
		for(int i = 0;i < World.size();++i)
		{
			if(	ROOM_FLAGGED(World[i], ROOM_NOPORT)  ||
				ROOM_FLAGGED(World[i], ROOM_NOMAGIC) ||
				ROOM_FLAGGED(World[i], ROOM_DEATH) ||
				World[i]->getZone()->IsClosed()
			)
			{
				continue;
			}

			if( World[i]->gateCode() == arg )
			{
				int diff = ch->in_room->getZone()->Distance(World[i]->getZone()) - (GET_SKILL(ch, weave->getVnum()) / 20);
				int failure = MiscUtil::random(0, 100) - ((100 - GET_SKILL(ch,weave->getVnum())) + 50 + (10*(diff+1)) );

				if( diff >= 0 && failure < 0 )//Gate too far, and distance factor kicked in. Need to find random room.
				{
					int n;
					const int nr_of_searches_max = 10000;
					for(n = 0;n < nr_of_searches_max;n++)//We'll try 10000 times before giving up.
					{
						i = MiscUtil::random(0, World.size()-1);//random room
						if(	ROOM_FLAGGED(World[i], ROOM_NOPORT)  ||
							ROOM_FLAGGED(World[i], ROOM_NOMAGIC) ||
							ROOM_FLAGGED(World[i], ROOM_DEATH) ||
							World[i]->getZone()->IsClosed()
						)
							continue;
						else
							break;
					}
					if( n >= nr_of_searches_max ) //failed to find room
						break;
				}

				GateManager::GetManager().AddGate( new Gate(ch->in_room, World[i], ch) );
				std::string buf = "A vertical beam of light slowly opens into a gateway, leading to somewhere afar.\r\n";
				sendToRoom(buf.c_str(), ch->in_room);
				sendToRoom(buf.c_str(), World[i]);
				return;//success
			}
		}
	}
	ch->send("Your weave collapses upon itself before it can be completed.\r\n");
}

ASPELL(spell_locate_life)
{
	Character *vict;
	char arg[MAX_INPUT_LENGTH];
	int distance = 0, location = 0, zone = 0, found = 0;

	OneArgument(argument, arg);

	if(!*arg)
	{
		ch->send("Who do you want to locate?\r\n");
		return;
	}

	for(vict = character_list; vict; vict = vict->next)
	{
		if( vict->IsPurged() )
			continue;
		if( AFF_FLAGGED(vict, AFF_HIDE) || AFF_FLAGGED(vict, AFF_INVISIBLE ) || (IS_NPC(vict) && MOB_FLAGGED(vict, MOB_INVIS)) )
			continue;
		if(!is_name(arg, (char*)GET_NAME(vict)) && !race_alias(vict, arg))
			continue;
		if(GET_LEVEL(vict) >= LVL_IMMORT)
			continue;
		if(GET_CLASS(ch) == CLASS_GREYMAN)
			continue;

		zone = vict->in_room->getZoneNumber();
		distance = vict->in_room->getZone()->Distance(ch->in_room->getZone());
		location = find_zone_slope(zone, ch->in_room->getZoneNumber());

		if(ROOM_FLAGGED(vict->in_room, ROOM_NOPORT) || ROOM_FLAGGED(vict->in_room, ROOM_NOMAGIC) || vict->in_room->getZone()->IsClosed())
			continue;

		if(distance <= GET_SKILL(ch, SPELL_LOCATE_LIFE) / 20)
		{
			found = 1;
			std::string Dist = DistanceString(distance);
			if( Dist.size() ) Dist = " " + Dist + " ";
			else Dist = " ";
			ch->send("You sense %s%s%s.  --  [%s%s%s]\r\n",
			         GET_NAME(vict), Dist.c_str(), loc[location], COLOR_GREEN(ch, CL_COMPLETE),
			         vict->in_room->gateCode().c_str(), COLOR_NORMAL(ch, CL_COMPLETE));
		}
	}

	if(!found)
	{
		ch->send("You do not sense that anywhere nearby.\r\n");
		return;
	}
}

ASPELL(spell_teleport)
{
}

ASPELL(spell_ward_weapon)
{
	if (ch == NULL || obj == NULL)
		return;

	if ((obj->getType() == ITEM_WEAPON))
	{
		ch->send("You channel flows of Spirit into %s, forming a ward around the item.\r\n", obj->GetSDesc());
		Act("$n grips $p, waving $s other hand around the item slowly.", TRUE, ch, obj, ch, TO_NOTVICT);
		IS_OBJ_STAT(obj, ITEM_WARD);
	}
	else
	{
		ch->send("You cannot ward this item.\r\n");
	}
}

ASPELL(spell_ward_item)
{
}

ASPELL(spell_eavesdrop)
{
	if( GET_LEVEL(victim) >= LVL_IMMORT && GET_LEVEL(ch) < LVL_IMMORT)
	{
		ch->send("You can't eavesdrop on a god!\r\n");
		return;
	}
	if (ch != victim && ch->in_room != victim->in_room)
	{
		if (victim->in_room->getEavesdroppingWarder() != NULL)
		{
			Act("You try to eavesdrop on $N, but the room is warded!", TRUE, ch, NULL, victim, TO_CHAR);
			victim->in_room->getEavesdroppingWarder()->send("Someone is attempting to listen in on your conversation!\r\n");
		}
		else if (ch->in_room->getDistanceToRoom(victim->in_room) <= atoi(weave->getAttribute("MaxDistance").c_str()))
		{
			ch->Eavesdropping = victim->in_room;
			victim->in_room->eavesdropping.push_back(ch);
			Act("You begin to eavesdrop on $N.", TRUE, ch, NULL, victim, TO_CHAR);
		}
		else
		{
			Act("Your target could not be found.", TRUE, ch, NULL, victim, TO_CHAR);
		}
	}
	else
	{
		ch->send("Why would you want to eavesdrop on someone in the same room as you?!\r\n");
	}
}
ASPELL(spell_circle_of_silence)
{
	if (ch->in_room->getEavesdroppingWarder())
	{
		ch->send("The room has already been warded.\r\n");
	}
	else
	{
		ch->in_room->setEavesdroppingWarder(ch);
		ch->send("You weave a ward around the area to protect against unwanted listeners.\r\n");
	}
}

ASPELL(spell_flare)
{
//	Act( weave->getAttribute( "ToChar" ).c_str(), TRUE, ch, NULL, victim, TO_CHAR);
	DisplayWeaveMessages(weave, ch, victim);

	if( IS_HUMAN( ch ) )
	{
		sendToZone( weave->getAttribute( "FromHuman" ).c_str(), ch->in_room->getZone()->GetRnum() );
		sendToZone( "\r\n", ch->in_room->getZone()->GetRnum() );
	}
	else if( IS_TROLLOC( ch ) )
	{
		sendToZone( weave->getAttribute( "FromTrolloc" ).c_str(), ch->in_room->getZone()->GetRnum() );
		sendToZone( "\r\n", ch->in_room->getZone()->GetRnum() );
	}
	else if( IS_SEANCHAN( ch ) )
	{
		sendToZone( weave->getAttribute( "FromSeanchan" ).c_str(), ch->in_room->getZone()->GetRnum() );
		sendToZone( "\r\n", ch->in_room->getZone()->GetRnum() );
	}
	else if( IS_AIEL( ch ) )
	{
		sendToZone( weave->getAttribute( "FromAiel" ).c_str(), ch->in_room->getZone()->GetRnum() );
		sendToZone( "\r\n", ch->in_room->getZone()->GetRnum() );
	}
}

ASPELL( spell_delve )
{
	DisplayWeaveMessages(weave, ch, victim);
	ch->send("%s is affected by the following status effects:\r\n", GET_NAME(victim));
	
	if(AFF_FLAGGED(victim, AFF_BLIND))
		ch->send("Blind\r\n");
	if(AFF_FLAGGED(victim, AFF_POISON))
		ch->send("Poison\r\n");
	if(AFF_FLAGGED(victim, AFF_SLEEP))
		ch->send("Sleep\r\n");
	if(AFF_FLAGGED(victim, AFF_NIGHT_VISION))
		ch->send("Night Vision\r\n");
	if(AFF_FLAGGED(victim, AFF_HASTE))
		ch->send("Haste\r\n");
	if(AFF_FLAGGED(victim, AFF_SHIELD))
		ch->send("Shield\r\n");
	if(AFF_FLAGGED(victim, AFF_AGILITY))
		ch->send("Agility\r\n");
	if(AFF_FLAGGED(victim, AFF_STRENGTH))
		ch->send("Strength\r\n");
	if(AFF_FLAGGED(victim, AFF_INSANE))
		ch->send("Insanity\r\n");
	if(AFF_FLAGGED(victim, AFF_SILENCE))
		ch->send("Silence\r\n");
	if(AFF_FLAGGED(victim, AFF_CHILL))
		ch->send("Chill\r\n");
	if(AFF_FLAGGED(victim, AFF_SHIELD_BLOCK))
		ch->send("Shield Block\r\n");
	if(AFF_FLAGGED(victim, AFF_SEVERE_WOUND))
		ch->send("Severe Wounds\r\n");
	if(AFF_FLAGGED(victim, AFF_HAMSTRING))
		ch->send("Hamstring\r\n");
	if(AFF_FLAGGED(victim, AFF_DEAF))
		ch->send("Deafness\r\n");
	if(AFF_FLAGGED(victim, AFF_WEAKEN))
		ch->send("Weakness\r\n");
	if(AFF_FLAGGED(victim, AFF_CRIPPLE))
		ch->send("Cripple\r\n");
	if(AFF_FLAGGED(victim, AFF_SLOW))
		ch->send("Slow\r\n");
	if(AFF_FLAGGED(victim, AFF_DECAY))
		ch->send("Decay\r\n");
	if(AFF_FLAGGED(victim, AFF_SLICE))
		ch->send("Slice\r\n");
	if(AFF_FLAGGED(victim, AFF_BURN))
		ch->send("Burn\r\n");
	if(AFF_FLAGGED(victim, AFF_DAZE))
		ch->send("Daze\r\n");
	if(AFF_FLAGGED(victim, AFF_DARK_PLAGUE))
		ch->send("Dark Plague\r\n");
}

ASPELL( spell_quicksand )
{
/*	if( IS_BASHED( victim ) )
	{
		ch->send("They already seem to be stunned!\r\n");
		return;
	}
*/
	int dodgeDivisor = atoi(weave->getAttribute("DodgeDivisor").c_str());
	if(MiscUtil::random(0, 100) > (dodgeDivisor ? victim->Dodge() / dodgeDivisor : victim->Dodge()) + atoi(weave->getAttribute("BaseDodge").c_str()) )
	{
		int nrPulses = ( PULSE_VIOLENCE * atof(weave->getAttribute("Duration").c_str()) );
		DisplayWeaveMessages(weave, ch, victim);
		victim->setBashState( nrPulses, true, false );
	}
	else
	{
		GET_MANA(ch) = GET_MANA(ch) + atof( weave->getAttribute("Cost").c_str() );
		Act("$N sees the ground below him begin to sink, and quickly jumps out of the way!", FALSE, ch, NULL, victim, TO_CHAR);
		Act("$N sees the ground below him begin to sink, and quickly jumps out of the way!", FALSE, ch, NULL, victim, TO_NOTVICT);
		Act("You feel the ground beneath your feet begin to shift, so you quickly jump to safer ground!", FALSE, ch, NULL, victim, TO_VICT);
	}
}

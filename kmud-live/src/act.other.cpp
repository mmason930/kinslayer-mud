/* ************************************************************************
*   File: Act.other.c                                   Part of CircleMUD *
*  Usage: Miscellaneous player-level commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __ACT_OTHER_C__

#include "conf.h"
#include "sysdep.h"
#include "stdio.h"

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "mobs.h"
#include "md5.h"
#include "MiscUtil.h"
#include "UserLogoutType.h"
#include "Descriptor.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "js.h"
#include "js_trigger.h"

#include "StringUtil.h"
#include "ForumUtil.h"
#include "CharacterUtil.h"

/* extern variables */

extern Descriptor *descriptor_list;
extern char *class_abbrevs[];
extern int max_filesize;
extern class Ideas *idea_list;
extern struct Index *obj_index;

/* extern procedures */
void list_skills( Character * ch, Character *teacher );
void perform_immort_vis( Character *ch );
SPECIAL( shop_keeper );
ACMD( do_gen_comm );

std::string MoodString( int mood );
std::string StanceString( int stance );
int perform_group( Character *ch, Character *vict );
void print_group( Character *ch );

//Declarations //
ACMD( do_say );

/* local functions */
ACMD( do_quit );
ACMD( do_save );
ACMD( do_not_here );
ACMD( do_sneak );
ACMD( do_hide );
ACMD( do_steal );
ACMD( do_effuse );
ACMD( do_practice );
ACMD( do_visible );
ACMD( do_group );
ACMD( do_ungroup );
ACMD( do_use );
ACMD( do_mark );
ACMD( do_notice );
ACMD( do_restat );
ACMD( do_ignore );
ACMD( do_wimpy );
ACMD( do_display );
ACMD( do_gen_tog );
ACMD( do_scalp );
ACMD( do_follow );
ACMD( do_self_delete );
ACMD( do_rage );


void perform_steal( Character *ch, Character *vict, Object *obj, char *obj_name )
{
	int percent, eq_pos, pcsteal = 0, ohoh = 0;

	/* 101% is a complete failure */
	percent = MiscUtil::random( 1, 101 );

	if ( GET_POS( vict ) < POS_SLEEPING )
		percent = -1;		/* ALWAYS SUCCESS */

	if ( IS_NPC( vict ) && MOB_FLAGGED( vict, MOB_AWARD ) )
	{
		char speech[64];
		strcpy(speech, "You'll pay for that!");
		do_say( vict, speech, 0, 0 );
		hit( vict, ch, TYPE_UNDEFINED );
		return ;
	}

	if ( FIGHTING( vict ) )
	{
		ch->send( "%s is fighting! He'll surely notice you trying that!\r\n", GET_NAME( vict ) );
		return ;
	}

	/* NO NO With Imp's and Shopkeepers, and if player thieving is not allowed */
	if ( GET_LEVEL( vict ) >= LVL_IMMORT || pcsteal ||
	GET_MOB_SPEC( vict ) == shop_keeper )
		percent = 101;		/* Failure */

	if ( !( obj = get_obj_in_list_vis( vict, obj_name, vict->carrying ) ) )
	{
		for ( eq_pos = 0; eq_pos < NUM_WEARS; eq_pos++ )

			if ( GET_EQ( vict, eq_pos ) &&
			        ( isname( obj_name, GET_EQ( vict, eq_pos ) ->getName() ) ) &&
			        CAN_SEE_OBJ( ch, GET_EQ( vict, eq_pos ) ) )
			{
				obj = GET_EQ( vict, eq_pos );
				break;
			}

		if ( !obj )
		{
			Act( "$E hasn't got that item.", FALSE, ch, 0, vict, TO_CHAR );
			return ;
		}

		else
		{			/* It is equipment */

			if ( ( GET_POS( vict ) > POS_STUNNED ) )
			{
				ch->send( "Steal the equipment now?  Impossible!\r\n" );
				return ;
			}

			else
			{
				Act( "You unequip $p and steal it.", FALSE, ch, obj, 0, TO_CHAR );
				Act( "$n steals $p from $N.", FALSE, ch, obj, vict, TO_NOTVICT );
				obj_to_char( unequip_char( vict, eq_pos ), ch );
			}
		}
	}

	else
	{			/* obj found in inventory */

		percent += ( int ) obj->Weight();	/* Make heavy harder */

		if ( AWAKE( vict ) && ( percent > GET_SKILL( ch, SKILL_STEAL ) ) )
		{
			ohoh = TRUE;
			Act( "Oops..", FALSE, ch, 0, 0, TO_CHAR );
			Act( "$n tried to steal something from you!", FALSE, ch, 0, vict, TO_VICT );
			Act( "$n tries to steal something from $N.", TRUE, ch, 0, vict, TO_NOTVICT );
		}

		else
		{			/* Steal the item */
			if ( ( IS_CARRYING_N( ch ) + 1 < CAN_CARRY_N( ch ) ) )
			{
				if ( ( ch->CarriedWeight() + obj->Weight() ) < CAN_CARRY_W( ch ) )
				{
					obj_from_char( obj );
					obj_to_char( obj, ch );
					ch->send( "Got it!\r\n", ch );
				}
			}

			else
				ch->send( "You cannot carry that much.\r\n" );
		}
	}

	if ( ohoh && IS_NPC( vict ) && AWAKE( vict ) )
		hit( vict, ch, TYPE_UNDEFINED );
}

void Character::AddIgnore(const std::string &name)
{
	std::string format, query;
	for(std::string::size_type i = 0;i < name.size();++i)
	{
		if(!i)
			format += UPPER(name[i]);
		else
			format += LOWER(name[i]);
	}

	this->send("You will now ignore %s.\r\n", format.c_str());
	MudLog(NRM, MAX(GET_INVIS_LEV(this), LVL_GOD), TRUE, "%s is now ignoring %s.", GET_NAME(this), format.c_str());

	this->ignores.push_back(format);

	query =	"INSERT INTO userIgnore (user_id, victim) VALUES (" + MiscUtil::Convert<std::string>(this->player.idnum) + ", '" + format + "')";

	sql::Query MyQuery;
	try {MyQuery = gameDatabase->sendQuery(query);}
	catch( sql::QueryException e )
	{
		MudLog(BRF, LVL_IMPL, TRUE, "AddIgnore : %s", e.message.c_str());
		return;
	}
}
void Character::RemoveIgnore(const std::string &name)
{
	std::list<std::string>::iterator i;
	for(i = this->ignores.begin();i != ignores.end();++i)
	{
		if(!str_cmp((*i), name))
			break;
	}
	if(i == this->ignores.end())
		return;
	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(this)), TRUE, "%s is no longer ignoring %s.",
		GET_NAME(this), (*i).c_str());

	std::string query = "DELETE FROM userIgnore WHERE user_id = " + MiscUtil::Convert<std::string>(this->player.idnum) +
		" AND victim = '" + sql::escapeString(name) + "'";

	sql::Query MyQuery;
	try {MyQuery = gameDatabase->sendQuery(query);}
	catch( sql::QueryException e )
	{
		MudLog(BRF, LVL_IMPL, TRUE, "RemoveIgnore : %s", e.message.c_str());
		return;
	}
	this->ignores.remove((*i));
}

ACMD( do_restat )
{
	if( GET_LEVEL(ch) != 5 )
	{
		ch->send("You must be level five in order to restat.\r\n");
		return;
	}
	MudLog(CMP, LVL_GRGOD, TRUE, "%s has initiated a restat.", GET_NAME(ch));
	ch->send("A bright flash of light momentarily consumes your vision. When you regain focus, you feel slightly different.\r\n");
	ch->Init();
	REMOVE_BIT_AR(PRF_FLAGS(ch), PRF_STATTED);
}

ACMD( do_ignore )
{
	char arg[ MAX_INPUT_LENGTH ];
	int count = 0;

	OneArgument( argument, arg );

	if ( !argument || !*arg )
	{
		ch->send( "You are ignoring the following people:\r\n" );

		for (std::list<std::string>::iterator i = ch->ignores.begin();i != ch->ignores.end();++i, ++count )
		{
			if ( count )
				ch->send( ", " );
			if ( !( count % 5 ) )
				ch->send( "\r\n" );
			ch->send( "%s", (*i).c_str());
		}
		return;
	}

	if(ch->IsIgnoring(arg))
	{
		ch->RemoveIgnore(arg);
		ch->send("You will no longer ignore %s.\r\n", arg);
	}
	else
		ch->AddIgnore(arg);
}


ACMD( do_self_delete )
{
	if ( !PLR_FLAGGED( ch, PLR_DELETED ) )
	{
		if ( GET_LEVEL( ch ) > 30 )
		{
			ch->send( "You must be level 30 or below in order to self delete.\r\n" );
			return ;
		}

		ch->send( "Your delete flag is now set.\r\n"
		          "Warning: Once you re-create this character, this cannot be un-done.\r\n" );
		REMOVE_BIT( PLR_FLAGS( ch ), Q_BIT(PLR_DELETED) );
	}

	else
	{
		ch->send( "Your delete flag has been removed.\r\n" );
		REMOVE_BIT( PLR_FLAGS( ch ), Q_BIT(PLR_DELETED) );
	}
}

ACMD( do_mark )
{

	Character * victim;
	char person[ MAX_INPUT_LENGTH ];

	OneArgument( argument, person );

	if ( !IS_GREYMAN( ch ) )
	{
		ch->send( "You don't seem to have that kind of power...\r\n" );
		return ;
	}

	if ( !*argument && !*person )
	{
		ch->send( "Who do you wish to mark?\r\n" );
		return ;
	}

	if ( !( victim = get_char_room_vis( ch, person ) ) )
	{
		ch->send( "There is no one by that name here.\r\n" );
		return ;
	}

	if ( GET_MARKED( ch ) )
		ch->send( "You release your focus on %s's location.\r\n", GET_NAME( GET_MARKED( ch ) ) );

	ch->send( "You now focus on %s's location.\r\n", GET_NAME( victim ) );
	GET_MARKED( ch ) = victim;
}

ACMD( do_effuse )
{

	struct affected_type af;

	if ( !GET_SKILL( ch, SKILL_EFFUSION ) )
	{
		ch->send( "You do not know that skill.\r\n" );
		return ;
	}

	if ( !AFF_FLAGGED( ch, AFF_EFFUSION ) )
		ch->send( "You will now move unseen.\r\n" );

	else if ( AFF_FLAGGED( ch, AFF_EFFUSION ) )
	{
		ch->send( "You will stop moving unnoticed.\n\r" );
		affect_from_char( ch, 0, AFF_EFFUSION );
		return ;
	}

	af.type = SKILL_EFFUSION;
	af.duration = 30;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_EFFUSION;
	affect_to_char( ch, &af );
}

/* Added by Galnor in October 2003. Command sets characters "notice" AFFECTION bit. */
ACMD( do_notice )
{

	struct affected_type af;

	if ( !AFF_FLAGGED( ch, AFF_NOTICE ) )
		ch->send( "You will now look around more carefully.\r\n" );

	else if ( AFF_FLAGGED( ch, AFF_NOTICE ) )
	{
		ch->send( "You look around less carefully.\n\r" );
		affect_from_char( ch, 0, AFF_NOTICE );
		return ;
	}

	af.type = SKILL_NOTICE;
	af.duration = 20;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_NOTICE;
	affect_to_char( ch, &af );

}

int butcher_cost( Character *ch )
{

	int skill = GET_SKILL( ch, SKILL_SURVIVAL );

	return ( 25 - ( skill / 5 ) );
}


/* Added by Galnor in September of 2003. Command gives meat from the corpse to selected player. */
ACMD( do_butcher )
{
	Object * corpse, *obj, *wielded = GET_EQ( ch, WEAR_WIELD );
	int q = 0;

	OneArgument( argument, ::arg );

	if ( !*::arg )
	{
		corpse = ch->in_room->findFirstObject([&](Object *obj) {
			return IS_CORPSE(obj) && CAN_SEE_OBJ(ch, obj);
		});
	}

	else
	{
		corpse = get_obj_in_list_vis( ch, ::arg, ch->in_room->contents );
	}

	if ( !corpse )
	{
		ch->send( "You don't see that here.\r\n" );
		if (GET_LEVEL(ch) <= 5 && subcmd == SCMD_BUTCHER) {
			get_char_cols(ch);
			ch->send("\r\nType %s%sHELP BUTCHER%s for more information on this command.\n\n", bld, cyn, nrm);
		}
		return ;
	}

	if ( !IS_CORPSE( corpse ) || !corpse->scalp )
	{
		ch->send( "That isn't a corpse!\r\n" );
		if (GET_LEVEL(ch) <= 5 && subcmd == SCMD_BUTCHER) {
			get_char_cols(ch);
			ch->send("\r\nType %s%sHELP BUTCHER%s for more information on this command.\n\n", bld, cyn, nrm);
		}
		return ;
	}

	else
	{
		if ( !wielded || GET_OBJ_VAL( wielded, 0 ) != WEAPON_SHORT_BLADE )
		{
			ch->send( "You need a short blade to do that.\r\n" );
			if (GET_LEVEL(ch) <= 5 && subcmd == SCMD_BUTCHER) {
				get_char_cols(ch);
				ch->send("\r\nType %s%sHELP BUTCHER%s for more information on this command.\n\n", bld, cyn, nrm);
			}
			return ;
		}

		if ( !corpse->scalp->Food && subcmd == SCMD_BUTCHER )
		{
			ch->send( "You can't butcher that!\r\n" );
			return ;
		}

		if ( !corpse->scalp->Skin && subcmd == SCMD_SKIN )
		{
			ch->send( "You can't skin that!\r\n" );
			return;
		}

		if ( GET_SKILL( ch, SKILL_SURVIVAL ) <= 0 && subcmd == SCMD_BUTCHER )
		{
			ch->send( "You don't know how to go about doing that.\r\n" );
			if (GET_LEVEL(ch) <= 5 && subcmd == SCMD_BUTCHER) {
				get_char_cols(ch);
				ch->send("\r\nType %s%sHELP BUTCHER%s for more information on this command.\n\n", bld, cyn, nrm);
			}
			return ;
		}

		if ( ch->GetSkillLevel( SKILL_SURVIVAL ) < 2 )
			q = 5;
		else if ( ch->GetSkillLevel( SKILL_SURVIVAL ) < 4 )
			q = 4;
		else if ( ch->GetSkillLevel( SKILL_SURVIVAL ) < 6 )
			q = 3;
		else if ( ch->GetSkillLevel( SKILL_SURVIVAL ) < 8 )
			q = 2;
		else
			q = 1;
		
		if( subcmd == SCMD_BUTCHER )
		{
			if ( real_object( corpse->scalp->Food->vnum ) == -1 )
			{
				ch->send( "Error with butchering..." );
				MudLog( BRF, LVL_APPR, TRUE, "Butcher: Attempting to load invalid food, vnum: %d, quant: %d",
						corpse->scalp->Food->vnum, corpse->scalp->Food->quantity );
				return ;
			}
			if ( q > corpse->scalp->Food->quantity )
			{
				ch->send( "You can't find enough to butcher from that.\r\n" );
				return ;
			}

			corpse->scalp->Food->quantity -= q;

			if ( !( obj = read_object( real_object( corpse->scalp->Food->vnum ), REAL, true ) ) )
			{
				MudLog( BRF, LVL_APPR, TRUE, "Failed Object Load: %s attempted to butcher %s. Attempted item load: %s(#%d).",
						GET_NAME( ch ), corpse->GetSDesc(), obj_proto[ corpse->scalp->Food->vnum ]->short_description,
						GET_OBJ_VNUM( obj_proto[ corpse->scalp->Food->vnum ] ) );
				ch->send( "You failed to butcher for unknown reasons...\r\n" );
				return ;
			}
			if (obj->getType() != ITEM_FOOD)
			{
				ch->send( "Error with butchering...\r\n" );
				delete obj;
				MudLog( BRF, LVL_APPR, TRUE, "%s attempted butchering %s, result was attempt to load item %d.",
						GET_NAME( ch ), corpse->GetSDesc(), corpse->scalp->Food->vnum );
			}

			obj_to_char( obj, ch );

			if( !obj->IsPurged() ) {
				js_load_triggers(obj);
			}
			ch->send( "You lean over and butcher %s from the corpse of %s.\r\n",
					  obj->GetSDesc(), corpse->scalp->name.c_str() );
			sprintf( buf, "%s leans over and butchers %s from the corpse of %s.", GET_NAME( ch ),
					 obj->GetSDesc(), corpse->scalp->name.c_str() );
			Act( buf, TRUE, ch, NULL, NULL, TO_ROOM, NULL, true );
		}

		if( subcmd == SCMD_SKIN )
		{
			if ( real_object( corpse->scalp->Skin->vnum ) == -1 )
			{
				ch->send( "Error with skinning..." );
				MudLog( BRF, LVL_APPR, TRUE, "Skin: Attempting to load invalid skin, vnum: %d, req: %d",
						corpse->scalp->Skin->vnum, corpse->scalp->Skin->required );
				return ;
			}
			
			if ( GET_SKILL( ch, SKILL_SKINNING ) < corpse->scalp->Skin->required )
			{
				ch->send( "You aren't skilled enough to skin that.\r\n" );
				return ;
			}
			
			if( corpse->scalp->Skin->skinned )
			{
				ch->send( "This has already been skinned!\r\n" );
				return;
			}

			if ( !( obj = read_object( real_object( corpse->scalp->Skin->vnum ), REAL, true ) ) )
			{
				MudLog( BRF, LVL_APPR, TRUE, "Failed Object Load: %s attempted to skin %s. Attempted item load: #d.", GET_NAME( ch ), corpse->GetSDesc(), corpse->scalp->Skin->vnum );
				ch->send( "You failed to skin for unknown reasons...\r\n" );
				return ;
			}
			if (obj->getType()  != ITEM_SKIN)
			{
				ch->send( "Error with skinning...\r\n" );
				delete obj;
				MudLog( BRF, LVL_APPR, TRUE, "%s attempted skinning %s, result was attempt to load item %d.",
						GET_NAME( ch ), corpse->GetSDesc(), corpse->scalp->Skin->vnum );
			}

			obj_to_char( obj, ch );

			if( !obj->IsPurged() ) {
				js_load_triggers(obj);
			}

			corpse->scalp->Skin->skinned = true;

			ch->send( "You lean over and carve %s from the corpse of %s.\r\n",
					  obj->GetSDesc(), corpse->scalp->name.c_str() );
			sprintf( buf, "%s leans over and carves %s from the corpse of %s.", GET_NAME( ch ),
					 obj->GetSDesc(), corpse->scalp->name.c_str() );
			Act( buf, TRUE, ch, NULL, NULL, TO_ROOM, NULL, true );
		}

	}
}

/* Added by Galnor in September of 2003. Command gives a scalp of selected corpse to the character. */
ACMD( do_scalp )
{
	Object * corpse, *scalpo, *wielded = GET_EQ( ch, WEAR_WIELD );
	const char *type;
	char arg[ MAX_INPUT_LENGTH ];

	OneArgument( argument, arg );

	if ( !*arg )
	{
		for ( corpse = ch->in_room->contents;corpse;corpse = corpse->next_content )
		{
			if ( IS_CORPSE( corpse ) && CAN_SEE_OBJ( ch, corpse ) )
			{
				break;
			}
		}
	}

	else
	{
		corpse = get_obj_in_list_vis( ch, arg, ch->in_room->contents );
	}

	//No object matched their argument
	if ( !corpse )
	{
		ch->send( "You do not see that here.\r\n" );
		return ;
	}

	//Object is not a corpse
	if ( !IS_CORPSE( corpse ) || !corpse->scalp )
	{
		ch->send( "That isn't a corpse!\r\n" );
		return ;
	}

	/* Only daggers can do this */
	if ( !wielded || GET_OBJ_VAL( wielded, 0 ) != WEAPON_SHORT_BLADE )
	{
		ch->send( "You must use a short blade to scalp.\r\n" );
		return ;
	}

	/* Minimum level for scalping */
	if ( corpse->scalp->level < 25 )
	{
		ch->send( "Why even bother scalping that puny thing?!\r\n" );
		return ;
	}

	/* Only do this if it has NOT been scalped yet. */
	if ( !corpse->scalp->scalped )
	{
		corpse->scalp->scalped = (true);
		scalpo	= create_obj();
		scalpo->scalp	= new ScalpData;

		scalpo->item_number	= NOTHING;
		scalpo->in_room	= 0;
		scalpo->name	= str_dup( "scalp" );

		sprintf( scalpo->creator, "Scalped by %s", GET_NAME( ch ) );

		scalpo->scalp->is_scalp	= (true);
		scalpo->scalp->race	= corpse->scalp->race;
		scalpo->scalp->level	= corpse->scalp->level;
		scalpo->scalp->Food	= NULL;
		scalpo->scalp->player_scalp = corpse->scalp->player_scalp;

		if ( corpse->scalp->level > 40 )
			type = "head";
		else
			type = "scalp";

		sprintf( buf2, "The bloody %s of %s.", type, corpse->scalp->name.c_str() );

		scalpo->description = str_dup( buf2 );

		sprintf( buf2, "a bloody %s of %s", type, corpse->scalp->name.c_str() );
		scalpo->short_description = str_dup( buf2 );

		SET_BITK( scalpo->obj_flags.wear_flags, ITEM_WEAR_TAKE );
		GET_OBJ_WEIGHT( scalpo ) = 2;

		ch->send( "You sever a bloody %s from the corpse of %s.\r\n", type, corpse->scalp->name.c_str() );
		sprintf( buf, "%s leans over and severs the bloody %s from the corpse of %s.", GET_NAME( ch ), type,
		         corpse->scalp->name.c_str() );
		Act( buf, TRUE, ch, 0, 0, TO_ROOM, NULL, true );

		if( ch->TooHeavyToPickUp( scalpo ) )
		{
			ch->send("You can't carry that much weight!\r\n");
			scalpo->MoveToRoom(ch->in_room, true);
		}
		else
		{
			obj_to_char( scalpo, ch );

			Object::saveItemToTopLevelHolder('P', MiscUtil::toString(ch->player.idnum), scalpo);
			if( corpse->in_room ) {
				corpse->in_room->itemSave();
			}
		}
	}

	else
		ch->send( "It has already been scalped!\r\n" );
}

/* Added by Galnor in September of 2003. Command mounts a character if successful. */
ACMD( do_ride )
{
	Character * victim, *riding;
	int percent;

	OneArgument( argument, ::arg );
	percent = MiscUtil::random( 1, 50 );

	if ( !*argument )
	{
		ch->send( "Ride who?\r\n" );
		return ;
	}
	REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_SNEAK );
	REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_EFFUSION );

	if( IS_DREADGUARD(ch) )
	{
		ch->send( "Ride? That thing? That would be going against your ways.\r\n");
		return;
	}
	else if ( IS_TROLLOC( ch ) && !IS_FADE( ch ) && !IS_NPC( ch ) )
	{
		ch->send( "But it looks so delicious... Those 4 legs....\r\n" );
		return ;
	}
	else
	{
		if ( !( victim = get_char_room_vis( ch, ::arg ) ) )
			ch->send( "They aren't here!\r\n" );
		else if ( victim == ch )
			ch->send( "Yeah, that'd be fun...\r\n" );
		else if ( MOUNT( ch ) )
			Act( "You're already riding $N", FALSE, ch, 0, MOUNT( ch ), TO_CHAR );
		else if ( FIGHTING( victim ) )
			ch->send( "%s is fighting! You're mad to even think about mounting %s now!\r\n",
			          HSSH( victim ), HMHR( victim ) );
		else if ( RIDDEN_BY( ch ) )
			Act( "You can't ride something else... $N is mounted on you!", FALSE, ch, 0, victim, TO_CHAR );
		else if ( RIDDEN_BY( victim ) && RIDDEN_BY( victim ) ->in_room == victim->in_room )
			ch->send( "%s is already riding it", GET_NAME( RIDDEN_BY( victim ) ) );
		else if ( !IS_HORSE( victim ) )
			Act( "You can't ride $M!", FALSE, ch, 0, victim, TO_CHAR );
		else if ( ( MOB_FLAGGED( victim, MOB_MOUNT ) && IS_TROLLOC( ch ) ) || ( MOB_FLAGGED( victim, MOB_SHADOW_MOUNT ) && !IS_TROLLOC( ch ) ) || ( MOB_FLAGGED( victim, MOB_OGIER_MOUNT ) && !IS_OGIER(ch) ) )
			Act( "$E would not like having your type on $M.", FALSE, ch, 0, victim, TO_CHAR );
		else if ( ch->hasWolfbrotherBonuses() )
			ch->send( "You wouldn't even think about getting up on that thing!\r\n" );
		else if ( ch->in_room->getSector() == RoomSector::inside )
			ch->send( "You can't ride inside!\r\n" );
		else if ( GET_SKILL( ch, SKILL_RIDE ) < percent )
			Act( "You try to mount $M and fall right off of $S back!", FALSE, ch, 0, victim, TO_CHAR );
		else
		{
			if ( AFF_FLAGGED( ch, AFF_SNEAK ) )
			{
				ch->send( "You stop sneaking.\r\n" );
				affect_from_char( ch, 0, AFF_SNEAK );
			}
			riding = victim;
			MOUNT( ch ) = riding;
			RIDDEN_BY( riding ) = ch;
			Act( "You begin riding $N.", FALSE, ch, 0, victim, TO_CHAR );
			Act( "$n begins riding $N.", FALSE, ch, 0, victim, TO_NOTVICT, NULL, true );
			Act( "$n begins to ride on your back.", FALSE, ch, 0, victim, TO_VICT );
			char str[16];
			strcpy(str,"self");
			do_follow( riding, str, 0, 0 );
		}
	}
}

/* Added by Galnor in September of 2003. Command dismounts a player. */
ACMD( do_dismount )
{
	if ( MOUNT( ch ) )
	{
		Act( "You stop riding $N.", FALSE, ch, 0, MOUNT( ch ), TO_CHAR );
		Act( "$n dismounts from you.", FALSE, ch, 0, MOUNT( ch ), TO_VICT );
		Act( "$n stops riding $N.", FALSE, ch, 0, MOUNT( ch ), TO_NOTVICT, NULL, true );
		RIDDEN_BY( MOUNT( ch ) ) = NULL;
		MOUNT( ch ) = NULL;
	}

	else
	{
		ch->send( "You aren't riding anything\r\n" );
	}
}

ACMD( do_quit )
{
	Descriptor * d, *next_d;

	if ( IS_NPC( ch ) || !ch->desc )
		return ;

	if ( AFF_FLAGGED( ch, AFF_NOQUIT ) )
	{
		ch->send( "Can't you feel the speed of your heartbeat?\r\n" );
		return ;
	}

	if ( subcmd != SCMD_QUIT && GET_LEVEL( ch ) < LVL_IMMORT )
		ch->send( "You have to type quit--no less, to quit!\r\n" );


	else if ( GET_POS( ch ) == POS_FIGHTING )
		ch->send( "No way!  You're fighting for your life!\r\n" );

	else if ( GET_POS( ch ) < POS_STUNNED )
	{
		ch->send( "You die before your time...\r\n", ch );
		ch->Die();
	}

	else
	{
		Act( "$n has left the game.", TRUE, ch, 0, 0, TO_ROOM );

		MudLog( NRM, MAX( LVL_IMMORT, GET_INVIS_LEV( ch ) ), TRUE, "%s has quit the game.", GET_NAME( ch ) );
		ch->send( "Goodbye, friend.. Come back soon!\r\n" );

		/*
		 * kill off all sockets connected to the same player as the one who is
		 * trying to quit.  Helps to maintain sanity as well as prevent duping.
		 */

		for ( d = descriptor_list; d; d = next_d )
		{
			next_d = d->next;

			if ( d == ch->desc )
				continue;

			if ( d->character && ( d->character->player.idnum == ch->player.idnum ) )
				STATE( d ) = CON_DISCONNECT;
		}

		if ( GET_LEVEL( ch ) >= LVL_IMMORT )
		{
			ch->send( "Your equipment has been saved.\r\n" );
			ch->rentSave();
			ch->Extract(UserLogoutType::quitWithRent);
		}
		else {
			ch->send( "Your equipment has NOT BEEN SAVED.\r\n" );
			ch->Extract(UserLogoutType::quit); //User is saved in extract char
			ch->itemSave();
		}
	}
}

ACMD( do_save )
{
	if ( IS_NPC( ch ) || !ch->desc )
		return ;

	/* Only tell the char we're saving if they Actually typed "save" */

	if ( cmd )
	{

		/*
		 * This prevents item duplication by two PC's using coordinated saves
		 * (or one PC with a house) and system crashes. Note that houses are
		 * still automatically saved without this enabled.
		 */
		ch->send( "Saving %s.\r\n", GET_NAME( ch ) );
	}
	ch->save();
	ch->itemSave();
}


/* generic function for commands which are normally overridden by
   special procedures - i.e., shop commands, mail commands, etc. */
ACMD( do_not_here )
{
	ch->send( "Sorry, but you cannot do that here!\r\n" );
}

ACMD( do_sneak )
{
	struct affected_type af;

	if ( !AFF_FLAGGED( ch, AFF_SNEAK ) )
		ch->send( "Okay, you'll try to move silently for a while.\r\n" );

	else if ( AFF_FLAGGED( ch, AFF_SNEAK ) )
	{
		ch->send( "Okay, you will now stop sneaking.\n\r" );
		affect_from_char( ch, 0, AFF_SNEAK );
		return ;
	}

	if ( MiscUtil::random( 1, 101 ) < GET_SKILL( ch, SKILL_SNEAK ) )
	{
		af.type = SKILL_SNEAK;
		af.duration = 25;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector= AFF_SNEAK;
		affect_to_char( ch, &af );
	}
}

ACMD( do_hide )
{
	char arg[ MAX_INPUT_LENGTH ];
	Object *obj;

	OneArgument( argument, arg );


	if ( *arg )
	{
		if ( !( obj = get_obj_in_list_vis( ch, arg, ch->carrying ) ) )
		{
			ch->send( "You are not carrying that item.\r\n" );
			return ;
		}

		obj_from_char( obj );
		obj->MoveToRoom( ch->in_room );
		obj->hidden = true;

		ch->send( "You successfully hide %s.\r\n", obj->GetSDesc() );
	}
	else
	{
		if ( MOUNT( ch ) != NULL )
		{
			ch->send( "You can't do that while riding!\r\n" );
			return ;
		}

		ch->send( "You attempt to hide yourself.\r\n" );

		if ( AFF_FLAGGED( ch, AFF_HIDE ) )
			REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_HIDE );

		if ( MiscUtil::random( 1, 101 ) < GET_SKILL( ch, SKILL_HIDE ) && !ROOM_FLAGGED(ch->in_room, ROOM_NOHIDE))
			SET_BIT_AR( AFF_FLAGS( ch ), AFF_HIDE );
		else
			ch->send("You can't find a good place to hide!\r\n");
	}
}

ACMD( do_steal )
{
	Character * vict;
	Object *obj = NULL;
	char vict_name[ MAX_INPUT_LENGTH ], obj_name[ MAX_INPUT_LENGTH ];

	if ( ROOM_FLAGGED( ch->in_room, ROOM_PEACEFUL ) )
	{
		ch->send( "This room just has such a peaceful, easy feeling...\r\n" );
		return ;
	}

	argument = OneArgument( argument, obj_name );
	OneArgument( argument, vict_name );

	if ( !( vict = get_char_room_vis( ch, vict_name ) ) )
	{
		ch->send( "Steal what from who?\r\n" );
		return ;
	}

	else if ( vict == ch )
	{
		ch->send( "Come on now, that's rather stupid!\r\n" );
		return ;
	}

	if ( !ch->command_ready )
		ch->command_ready = true;
	else
		perform_steal( ch, vict, obj, obj_name );

}

ACMD( do_practice )
{
	OneArgument( argument, ::arg );
	list_skills( ch, NULL );
}

ACMD( do_visible )
{
	if ( GET_LEVEL( ch ) >= LVL_IMMORT )
	{
		perform_immort_vis( ch );
		return ;
	}

	if AFF_FLAGGED( ch, AFF_INVISIBLE )
	{
		ch->Appear();
		ch->send( "You break the spell of invisibility.\r\n" );
	}

	else
		ch->send( "You are already visible.\r\n" );
}

int perform_group( Character *ch, Character *vict )
{
	if ( ch->IsPurged() || vict->IsPurged() || AFF_FLAGGED( vict, AFF_GROUP )
		|| !CAN_SEE( ch, vict ) || MOB_FLAGGED(vict,MOB_NOGROUP) )
		return 0;

	SET_BIT_AR( AFF_FLAGS( vict ), AFF_GROUP );

	if ( ch != vict )
		Act( "$N is now a member of your group.", FALSE, ch, 0, vict, TO_CHAR );

	Act( "You are now a member of $n's group.", FALSE, ch, 0, vict, TO_VICT );
	Act( "$N is now a member of $n's group.", FALSE, ch, 0, vict, TO_NOTVICT );
	return 1;
}

void print_group( Character *ch )
{
	Character * k;
	struct Follower *f;

	if ( !AFF_FLAGGED( ch, AFF_GROUP ) )
		ch->send( "But you are not the member of a group!\r\n" );

	else
	{
		ch->send( "Your group consists of:\r\n" );

		k = ( ch->master ? ch->master : ch );

		if ( AFF_FLAGGED( k, AFF_GROUP ) )
		{
			sprintf( buf, "     $N (Head of group)" );
			Act( buf, FALSE, ch, 0, k, TO_CHAR );
		}

		for ( f = k->followers; f; f = f->next )
		{
			if ( !AFF_FLAGGED( f->follower, AFF_GROUP ) )
				continue;

			sprintf( buf, "      $N" );
			Act( buf, FALSE, ch, 0, f->follower, TO_CHAR );
		}
	}
}

ACMD( do_group )
{
	Character * vict;
	struct Follower *f;
	int found;

	OneArgument( argument, buf );

	if ( !*buf )
	{
		print_group( ch );
		return ;
	}

	if ( ch->master )
	{
		Act( "You can not enroll group members without being head of a group.",
		     FALSE, ch, 0, 0, TO_CHAR );
		return ;
	}

	if ( !str_cmp( buf, "all" ) )
	{
		perform_group( ch, ch );

		for ( found = 0, f = ch->followers; f; f = f->next )
		{
			if ( GET_CLASS( f->follower ) == CLASS_FADE )
				continue;
			found += perform_group( ch, f->follower );
		}

		if ( !found )
			ch->send( "Everyone following you is already in your group.\r\n" );
		return ;
	}

	if ( !( vict = get_char_room_vis( ch, buf ) ) )
		ch->send( NOPERSON );

	else if ( ( vict->master != ch ) && ( vict != ch ) )
		Act( "$N must follow you to enter your group.", FALSE, ch, 0, vict, TO_CHAR );

	//else if ( GET_CLASS(vict) == CLASS_FADE && !(GET_CLASS(ch) == CLASS_FADE || GET_CLASS(ch) == CLASS_DREADLORD) )
	//	Act( "Myrddraal are leaders, not followers!\r\n", FALSE, ch, 0, vict, TO_CHAR );

	else
	{

		if ( !AFF_FLAGGED( vict, AFF_GROUP ) )
			perform_group( ch, vict );

		else
		{
			if ( ch != vict )
				Act( "$N is no longer a member of your group.", FALSE, ch, 0, vict, TO_CHAR );

			Act( "You have been kicked out of $n's group!", FALSE, ch, 0, vict, TO_VICT );
			Act( "$N has been kicked out of $n's group!", FALSE, ch, 0, vict, TO_NOTVICT );
			REMOVE_BIT_AR( AFF_FLAGS( vict ), AFF_GROUP );
		}
	}
}

ACMD( do_ungroup )
{
	struct Follower * f, *next_fol;
	Character *tch;

	OneArgument( argument, buf );

	if ( !*buf )
	{
		if ( ch->master || !( AFF_FLAGGED( ch, AFF_GROUP ) ) )
		{
			ch->send( "But you lead no group!\r\n" );
			return ;
		}

		sprintf( buf2, "%s has disbanded the group.\r\n", GET_NAME( ch ) );

		for ( f = ch->followers; f; f = next_fol )
		{
			next_fol = f->next;

			if ( AFF_FLAGGED( f->follower, AFF_GROUP ) )
			{
				REMOVE_BIT_AR( AFF_FLAGS( f->follower ), AFF_GROUP );
				f->follower->send( buf2 );

				f->follower->stopFollowing();
			}
		}

		REMOVE_BIT_AR( AFF_FLAGS( ch ), AFF_GROUP );
		ch->send( "You disband the group.\r\n" );
		return ;
	}

	if ( !( tch = get_char_room_vis( ch, buf ) ) )
	{
		ch->send( "There is no such person!\r\n" );
		return ;
	}

	if ( tch->master != ch )
	{
		ch->send( "That person is not following you!\r\n" );
		return ;
	}

	if ( !AFF_FLAGGED( tch, AFF_GROUP ) )
	{
		ch->send( "That person isn't in your group.\r\n" );
		return ;
	}

	REMOVE_BIT_AR( AFF_FLAGS( tch ), AFF_GROUP );

	Act( "$N is no longer a member of your group.", FALSE, ch, 0, tch, TO_CHAR );
	Act( "You have been kicked out of $n's group!", FALSE, ch, 0, tch, TO_VICT );
	Act( "$N has been kicked out of $n's group!", FALSE, ch, 0, tch, TO_NOTVICT );
}

ACMD( do_use )
{
	HalfChop( argument, ::arg, buf );

	if ( !*::arg )
	{
		ch->send( "What do you want to %s?\r\n",
		complete_cmd_info[cmd].command.size() ? complete_cmd_info[cmd].command.c_str() : "ERROR!!! REPORT THIS!" );
		return ;
	}

}

ACMD( do_wimpy )
{
	int wimp_lev;

	/* 'wimp_level' is a player_special. -gg 2/25/98 */
	if ( IS_NPC( ch ) )
		return ;

	OneArgument( argument, ::arg );

	if ( !*::arg )
	{
		if ( ch->PlayerData->wimp_level )
		{
			ch->send( "Your current wimp level is %d hit points.\r\n", ch->PlayerData->wimp_level );
			return ;
		}

		else
		{
			ch->send( "At the moment, you're not a wimp.  (sure, sure...)\r\n" );
			return ;
		}
	}

	if ( isdigit( *::arg ) )
	{
		if ( ( wimp_lev = atoi( ::arg ) ) )
		{
			if ( wimp_lev < 0 )
				ch->send( "Heh, heh, heh.. we are jolly funny today, eh?\r\n" );

			else if ( wimp_lev > GET_MAX_HIT( ch ) )
				ch->send( "That doesn't make much sense, now does it?\r\n" );

			else if ( wimp_lev > ( GET_MAX_HIT( ch ) / 2 ) )
				ch->send( "You can't set your wimp level above half your hit points.\r\n" );

			else
			{
				ch->send( "Okay, you'll wimp out if you drop below %d hit points.\r\n", wimp_lev );
				ch->PlayerData->wimp_level = wimp_lev;
			}
		}
		else
		{
			ch->send( "Okay, you'll now tough out fights to the bitter end.\r\n" );
			ch->PlayerData->wimp_level = 0;
		}
	}
	else
		ch->send( "Specify at how many hit points you want to wimp out at.  (0 to disable)\r\n" );
}

ACMD( do_display )
{
	size_t i;

	if ( IS_NPC( ch ) )
	{
		ch->send( "Mosters don't need displays.  Go away.\r\n" );
		return ;
	}

	skip_spaces( &argument );

	if ( !*argument )
	{
		ch->send( "Usage: prompt { { H | M | S } | all | none }\r\n" );
		return ;
	}

	if ( ( !str_cmp( argument, "on" ) ) || ( !str_cmp( argument, "all" ) ) )
	{
		SET_BIT_AR( PRF_FLAGS( ch ), PRF_DISPHP );
		SET_BIT_AR( PRF_FLAGS( ch ), PRF_DISPMANA );
		SET_BIT_AR( PRF_FLAGS( ch ), PRF_DISPMOVE );
	}

	else
	{
		REMOVE_BIT_AR( PRF_FLAGS( ch ), PRF_DISPHP );
		REMOVE_BIT_AR( PRF_FLAGS( ch ), PRF_DISPMANA );
		REMOVE_BIT_AR( PRF_FLAGS( ch ), PRF_DISPMOVE );

		for ( i = 0; i < strlen( argument ); i++ )
		{

			switch ( LOWER( argument[ i ] ) )
			{

					case 'h':
					SET_BIT_AR( PRF_FLAGS( ch ), PRF_DISPHP );
					break;

					case 's':
					SET_BIT_AR( PRF_FLAGS( ch ), PRF_DISPMANA );
					break;

					case 'm':
					SET_BIT_AR( PRF_FLAGS( ch ), PRF_DISPMOVE );
					break;

					default:
					ch->send( "Usage: prompt { { H | M | V } | all | none }\r\n" );
					return ;
					break;
			}
		}
	}

	ch->send( OK );
}

const int TOG_OFF = 0;
const int TOG_ON = 1;


ACMD( do_gen_tog )
{
	long result;

	const char *tog_messages[][ 2 ] =

	    {
	        {"Nohassle disabled.\r\n",
	         "Nohassle enabled.\r\n"},
	        {"Brief mode off.\r\n",
	         "Brief mode on.\r\n"},
	        {"Compact mode off.\r\n",
	         "Compact mode on.\r\n"},
	        {"You can now hear tells.\r\n",
	         "You are now deaf to tells.\r\n"},
	        {"You can now hear narrates.\r\n",
	         "You are now deaf to narrates.\r\n"},
	        {"You can now hear chats.\r\n",
	         "You are now deaf to chats.\r\n"},
	        {"You can now hear shouts.\r\n",
	         "You are now deaf to shouts.\r\n"},
	        {"You can now hear yells.\r\n",
	         "You are now deaf to chat yells.\r\n"},
	        {"You can now hear the Wiz-channel.\r\n",
	         "You are now deaf to the Wiz-channel.\r\n"},
	        {"You will no longer see the room flags.\r\n",
	         "You will now see the room flags.\r\n"},
	        {"You will now have your communication repeated.\r\n",
	         "You will no longer have your communication repeated.\r\n"},
	        {"HolyLight mode off.\r\n",
	         "HolyLight mode on.\r\n"},
	        {"Nameserver_is_slow changed to NO; IP addresses will now be resolved.\r\n",
	         "Nameserver_is_slow changed to YES; sitenames will no longer be resolved.\r\n"},
	        {"Autoexits disabled.\r\n",
	         "Autoexits enabled.\r\n"},
	        {"Buildwalk now OFF.\r\n",
	         "Buildwalk now ON.\r\n"},
	        {"You will no longer see the tips.\r\n",
	         "You will now see the tips.\r\n"},
            {"Autoscan is now on.\r\n", // RHOLLOR 05.15.09
			 "Autoscan is now off.\r\n"},  // RHOLLOR 05.15.09
			{"You will now see immortal speaks.\r\n",
			"You will no longer see immortal speaks.\r\n"},
			{"You will now hear the global channel.\r\n",
			"You will no longer hear the global channel.\r\n"}
	    };


	if ( IS_NPC( ch ) )
		return ;

	switch ( subcmd )
	{
			case SCMD_AUTOSCAN:                   //RHOLLOR 05.15.09
			result = PRF_TOG_CHK( ch, PRF_AUTOSCAN );   //RHOLLOR 05.15.09
			break;                              //RHOLLOR 05.15.09
			case SCMD_NOHASSLE:
			result = PRF_TOG_CHK( ch, PRF_NOHASSLE );
			break;
			case SCMD_BRIEF:
			result = PRF_TOG_CHK( ch, PRF_BRIEF );
			break;
			case SCMD_COMPACT:
			result = PRF_TOG_CHK( ch, PRF_COMPACT );
			break;
			case SCMD_NOTELL:
			result = PRF_TOG_CHK( ch, PRF_NOTELL );
			break;
			case SCMD_NONARRATE:
			result = PRF_TOG_CHK( ch, PRF_NONARR );
			break;
			case SCMD_NOCHAT:
			result = PRF_TOG_CHK( ch, PRF_NOCHAT );
			break;
			case SCMD_NOYELL:
			result = PRF_TOG_CHK( ch, PRF_NOYELL );
			break;
			case SCMD_NOWIZ:
			result = PRF_TOG_CHK( ch, PRF_NOWIZ );
			break;
			case SCMD_ROOMFLAGS:
			result = PRF_TOG_CHK( ch, PRF_ROOMFLAGS );
			break;
			case SCMD_NOREPEAT:
			result = PRF_TOG_CHK( ch, PRF_NOREPEAT );
			break;
			case SCMD_HOLYLIGHT:
			result = PRF_TOG_CHK( ch, PRF_HOLYLIGHT );
			break;
			case SCMD_AUTOEXIT:
			result = PRF_TOG_CHK( ch, PRF_AUTOEXIT );
			break;
			case SCMD_BUILDWALK:
			result = PRF_TOG_CHK( ch, PRF_BUILDWALK );
			break;
			case SCMD_NOTIPS:
			result = NEWB_TOG_CHK( ch, NEW_TIPS );
			break;
			case SCMD_NOSPEAKS:
			result = PRF_TOG_CHK( ch, PRF_NOSPEAKS );
			break;
			case SCMD_NOGLOBAL:
			result = PRF_TOG_CHK( ch, PRF_NOGLOBAL );
			break;
			default:
			Log( "SYSERR: Unknown subcmd %d in do_gen_toggle.", subcmd );
			return ;
	}

	if ( result )
		ch->send( tog_messages[ subcmd ][ TOG_ON ] );
	else
		ch->send( tog_messages[ subcmd ][ TOG_OFF ] );
	return ;
}

ACMD( do_change )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];

	skip_spaces( &argument );
	delete_doubledollar( argument );
	OneArgument( argument, arg1 );

	if ( !*argument )
	{
		ch->send( "Usage: Description, Title, Mood, Password, Spam\r\n" );
		return ;
	}

	if ( !strn_cmp( arg1, "title", strlen( arg1 ) ) )
	{
		HalfChop( argument, arg1, arg2 );

		if ( IS_NPC( ch ) )
			ch->send( "Your title is fine... go away.\r\n" );
		else if ( PLR_FLAGGED( ch, PLR_NOTITLE ) )
			ch->send( "You can't title yourself -- you shouldn't have abused it!\r\n" );
		else if ( strstr( arg2, "(" ) || strstr( arg2, ")") || strstr( arg2, "~") || strstr( arg2, "[" ) || strstr( arg2, "]" ) || strstr( arg2, "-" ) )
			ch->send( "Titles can't contain the '(', ')', '[', ']', '-' or '~' characters.\r\n" );
		else if ( strlen( arg2 ) > MAX_TITLE_LENGTH )
			ch->send( "Sorry, titles can't be longer than %d characters.\r\n", MAX_TITLE_LENGTH );
		else
		{
			ch->SetTitle( arg2 );
			ch->send( "Okay, you're now %s %s.\r\n", GET_NAME( ch ), ch->player.title.c_str() );

		}
	}
	else if ( !strn_cmp( arg1, "mood", strlen( arg1 ) ) )
	{
		if(IS_NPC(ch))
		{
			ch->send("MOBs cannot change their mood, sorry...\r\n");
			return;
		}
		TwoArguments( argument, arg1, arg2 );

		if ( ch->PlayerData->mood == MOOD_BERSERK && FIGHTING( ch ) )
		{
			ch->send( "You are too berserk to do that now!\r\n" );
			return;
		}
		if ( !str_cmp( arg2, "wimpy" ) )
			ch->PlayerData->mood = MOOD_WIMPY;
		else if ( !str_cmp( arg2, "mild" ) )
			ch->PlayerData->mood = MOOD_MILD;
		else if ( !str_cmp( arg2, "brave" ) )
			ch->PlayerData->mood = MOOD_BRAVE;
		else if ( !str_cmp( arg2, "berserk" ) )
			ch->PlayerData->mood = MOOD_BERSERK;
		else
		{
			ch->send( "Current moods are: wimpy, mild, brave, berserk.\r\n" );
			return ;
		}
		ch->send( "Mood changed to: %s\r\n", MoodString( ch->PlayerData->mood ).c_str() );
	}
	else if ( IS_BLADEMASTER(ch) && !strn_cmp( arg1, "stance", strlen( arg1 ) ) )
	{
		if(IS_NPC(ch))
		{
			ch->send("MOBs cannot change their stance, sorry...\r\n");
			return;
		}
		TwoArguments( argument, arg1, arg2 );

		if ( !str_cmp( arg2, "defensive" ) )
		{
			ch->stance = STANCE_DEFENSIVE;
			Act( "$n flows into a defensive battle stance.", TRUE, ch, NULL, 0, TO_ROOM, NULL, true );
		}
		else if ( !str_cmp( arg2, "normal" ) )
		{
			ch->stance = STANCE_NORMAL;
			Act( "$n moves into a standard battle stance.", TRUE, ch, NULL, 0, TO_ROOM, NULL, true );
		}
		else if ( !str_cmp( arg2, "offensive" ) )
		{
			ch->stance = STANCE_OFFENSIVE;
			Act( "$n takes up an offensive battle stance.", TRUE, ch, NULL, 0, TO_ROOM, NULL, true );
		}
		else
		{
			ch->send( "Possible stances are: defensive, normal, offensive.\r\n" );
			return ;
		}
		ch->send( "Stance changed to: %s\r\n", StanceString( ch->stance ).c_str() );
	}
	else if ( !strn_cmp( arg1, "password", strlen( arg1 ) ) )
	{
		//Split the argument string into a vector. Can't use TwoArguments since it kills the upper case chars.
		std::vector< std::string > vArgs = StringUtil::SplitToContainer< std::vector<std::string>, std::string >( argument, ' ' );

		//Filter out all of the empty arguments.
		for(std::vector<std::string>::iterator iter = vArgs.begin();iter != vArgs.end();) {
			if( (*iter).empty() )
				vArgs.erase( iter );
			else
				++iter;
		}

		if ( vArgs.size() < 2 )
		{
			ch->send( "Password must contain something.\r\n" );
			return ;
		}
		std::string sNewPassword = vArgs[ 1 ];
		if( sNewPassword.size() > (MAX_PWD_LENGTH-1) )
		{
			ch->send("Your password must be shorter than %d characters.\r\n");
			return;
		}
		ch->player.passwd = MD5::getHashFromString( sNewPassword.c_str() );
		CharacterUtil::changeUserPassword(gameDatabase, ch->player.idnum, ch->player.passwd);
		ForumUtil::changeUserPassword(gameDatabase, ch->player.idnum, ch->player.passwd);
		ch->send( "Your password has been changed to: %s.\r\n", sNewPassword.c_str() );
	}
	else if ( !strn_cmp( arg1, "spam", strlen( arg1 ) ) )
	{
		TwoArguments( argument, arg1, arg2 );

		if ( !str_cmp( arg2, "complete" ) )
		{
			ch->send( "Your spam has been turned to COMPLETE.\r\n" );
			SET_BIT_AR( PRF_FLAGS( ch ), PRF_SPAM );
		}
		else if ( !str_cmp( arg2, "none" ) )
		{
			ch->send( "Your spam has been turned NONE.\r\n" );
			REMOVE_BIT_AR( PRF_FLAGS( ch ), PRF_SPAM );
		}
		else
		{
			ch->send( "You can only change your spam to COMPLETE or NONE.\r\n" );
			return ;
		}
	}
	else if ( !strn_cmp( arg1, "description", strlen( arg1 ) ) )
	{
		if ( ch->desc )
		{
			Act( "$n begins editing $s description.", FALSE, ch, NULL, NULL, TO_ROOM );
			ch->send( "Enter the new text you'd like others to see when they look at you.\r\n" );
			ch->send( "(/s saves /h for help)\r\n" );
			ch->desc->str = &ch->player.description;
			ch->desc->max_str = EXDSCR_LENGTH;
		}
		else
		{
			ch->send( "You don't have this ability!\r\n" );
		}
	}
}

ACMD( do_rage )
{

	struct affected_type af;

	if ( !IS_FADE(ch) )
	{
		ch->send("What?!?\r\n");
		return;
	}
	
	if ( !GET_SKILL(ch, SKILL_SHADOW_RAGE) )
	{
		ch->send("You have no idea how.\r\n");
		return;
	}

	if ( GET_SHADOW(ch) <= 1 && !AFF_FLAGGED( ch, AFF_SHADOW_RAGE ) )
	{
		ch->send("You feel too drained!\r\n");
		return;
	}

	if ( !AFF_FLAGGED( ch, AFF_SHADOW_RAGE ) )
		ch->send( "You tap into a dark power, enhancing your attack speed.\r\n" );

	else if ( AFF_FLAGGED( ch, AFF_SHADOW_RAGE ) )
	{
		ch->send( "You release your hold on the darkness, slowing your speed.\n\r" );
		affect_from_char( ch, 0, AFF_SHADOW_RAGE );
		return;
	}

	af.type = SKILL_SHADOW_RAGE;
	af.duration = 10;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector= AFF_SHADOW_RAGE;
	affect_to_char( ch, &af );
	//GET_SHADOW(ch) = MAX( GET_SHADOW(ch) - 40, 0 );
}

bool Character::disorientRoll()
{
	return AFF_FLAGGED(this, AFF_DISORIENT) && MiscUtil::random(1, 2) == 1;
}




















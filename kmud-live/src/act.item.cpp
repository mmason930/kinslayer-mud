
/* ************************************************************************
*   File: Act.item.c                                    Part of CircleMUD *
*  Usage: object handling routines -- get/drop and container handling     *
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
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "clans.h"
#include "constants.h"
#include "shop.h"
#include "olc.h"
#include "screen.h"
#include "auction.h"
#include "StringUtil.h"
#include "MiscUtil.h"
#include "Descriptor.h"
#include "ClanUtil.h"

#include "js.h"
#include "js_functions.h"

/* extern variables */
extern sh_int donation_room_1;
#if 0
extern sh_int donation_room_2;  /* uncomment if needed! */
extern sh_int donation_room_3;  /* uncomment if needed! */
#endif
extern struct Index *obj_index;
extern std::vector<Object*> obj_proto;
extern const char *drinknames[];
extern const char *drinks[];
extern int top_of_objt;

FILE *logger;

/* local functions */
int find_eq_pos(Character *ch, Object *obj, char *arg, bool msg);
int can_take_obj( Character *ch, Object *obj );
int perform_get_from_room( Character *ch, Object *obj, bool vaultSave=true );
int perform_drop( Character *ch, Object *obj, bool vaultSave=true );
bool remort_gear_message( Character *ch, Object *obj );
void get_from_room( Character *ch, char *arg );
void perform_give( Character *ch, Character *vict, Object *obj );
void perform_put( Character *ch, Object *obj, Object *cont );
void name_from_drinkcon( Object *obj );
void get_from_container( Character *ch, Object *cont, char *arg, int mode );
void name_to_drinkcon( Object *obj, int type );
void wear_message( Character *ch, Object *obj, int where );
void perform_wear( Character *ch, Object *obj, int where );
void perform_get_from_container( Character *ch, Object *obj, Object *cont, int mode );
void perform_remove( Character *ch, int pos );
void update_chest_log( Character *ch, Object *chest, Object *obj, int type );
void lookInObject( Character * ch, char *arg, Object *obj=0);
Character *give_find_vict( Character *h, char *arg );

ACMD( do_auction );
ACMD( do_draw );
ACMD( do_get );
ACMD( do_grab );
ACMD( do_give );
ACMD( do_drink );
ACMD( do_drop );
ACMD( do_diceroll );
ACMD( do_eat );
ACMD( do_flip );
ACMD( do_pour );
ACMD( do_put );
ACMD( do_remove );
ACMD( do_sheath );
ACMD( do_wear );
ACMD( do_wield );
ACMD( do_break );
ACMD( do_show );

unsigned long int Character::Copper()
{
	return CalcCopper( this->points.gold );
}

unsigned long int Character::Silver()
{
	return CalcSilver( this->points.gold );
}

unsigned long int Character::Gold()
{
	return CalcGold( this->points.gold );
}

int Object::getVnum()
{
	if( this->item_number >= 0 && this->item_number < top_of_objt )
	{
		return obj_index[this->item_number].vnum;
	}
	return -1;
}
int Object::GetTotalDodge()
{
	return obj_flags.dodge + GetModifierInt( Object::OBJECT_MOD_DODGE );
}
int Object::GetTotalAbsorb()
{
	return obj_flags.absorb + GetModifierInt( Object::OBJECT_MOD_ABSORB );
}
int Object::GetTotalParry()
{
	return obj_flags.parry + GetModifierInt( Object::OBJECT_MOD_PARRY );
}
int Object::GetTotalOffense()
{
	return obj_flags.offensive + GetModifierInt( Object::OBJECT_MOD_OFFENSE );
}
int Object::GetTotalVal0()
{
	return obj_flags.value[ 0 ] + GetModifierInt( Object::OBJECT_MOD_VAL1 );
}
int Object::GetTotalVal1()
{
	return obj_flags.value[ 1 ] + GetModifierInt( Object::OBJECT_MOD_VAL2 );
}
int Object::GetTotalVal2()
{
	return obj_flags.value[ 2 ] + GetModifierInt( Object::OBJECT_MOD_VAL3 );
}
int Object::GetTotalVal3()
{
	return obj_flags.value[ 3 ] + GetModifierInt( Object::OBJECT_MOD_VAL4 );
}
int Object::GetTotalVal4()
{
	return obj_flags.value[ 4 ] + GetModifierInt( Object::OBJECT_MOD_VAL5 );
}
int Object::GetTotalVal5()
{
	return obj_flags.value[ 5 ] + GetModifierInt( Object::OBJECT_MOD_VAL6 );
}
int Object::GetTotalVal6()
{
	return obj_flags.value[ 6 ] + GetModifierInt( Object::OBJECT_MOD_VAL7 );
}
int Object::GetTotalVal7()
{
	return obj_flags.value[ 7 ] + GetModifierInt( Object::OBJECT_MOD_VAL8 );
}
int Object::GetTotalVal8()
{
	return obj_flags.value[ 8 ] + GetModifierInt( Object::OBJECT_MOD_VAL9 );
}
int Object::GetTotalVal9()
{
	return obj_flags.value[ 9 ] + GetModifierInt( Object::OBJECT_MOD_VAL10 );
}
int Object::GetTotalVal10()
{
	return obj_flags.value[ 10 ] + GetModifierInt( Object::OBJECT_MOD_VAL11 );
}
int Object::GetTotalVal11()
{
	return obj_flags.value[ 11 ] + GetModifierInt( Object::OBJECT_MOD_VAL12 );
}
float Object::GetTotalWeight()
{
	return obj_flags.weight + GetModifierFloat( Object::OBJECT_MOD_WEIGHT );
}

void Object::SetModifier( eObjectModifier modType, const long double ldNewValue )
{
	if( !obj_flags.mModifiers ) return;
	(*obj_flags.mModifiers)[ static_cast< byte >( modType ) ] = ldNewValue;
}
int Object::GetModifierInt( eObjectModifier modType )
{
	if( !obj_flags.mModifiers ) return 0;
	byte bModType = static_cast< byte >( modType );
	if( (*obj_flags.mModifiers).count( bModType ) != 0 )
		return static_cast< int >( (*obj_flags.mModifiers)[ bModType ] );
	return 0;
}
float Object::GetModifierFloat( eObjectModifier modType )
{
	if( !obj_flags.mModifiers ) return 0.0f;
	byte bModType = static_cast< byte >( modType );
	if( (*obj_flags.mModifiers).count( bModType ) != 0 )
		return static_cast< float >( (*obj_flags.mModifiers)[ bModType ] );
	return 0.0f;
}

int Character::Draw()
{
	Object * item;
	int i = 0;

	if ( GET_EQ( this, WEAR_WIELD ) )
	{
		return 1;
	}

	for ( i = 0;i < NUM_WEARS;++i )
	{
		if ( !( GET_EQ( this, i ) ) )
			continue;
		if ( ( ( IS_OBJ_STAT( GET_EQ( this, i ), ITEM_DAGGER_SHEATH ) )
			|| ( IS_OBJ_STAT( GET_EQ( this, i ), ITEM_SWORD_SHEATH ) )
			|| ( IS_OBJ_STAT( GET_EQ( this, i ), ITEM_SPEAR_SHEATH ) ) )
			&& GET_EQ( this, i ) ->contains)
		{
			if (!this->CanDraw( GET_EQ( this, i) ->contains))
				return 1;
			this->send( "You draw %s from %s.\r\n",
			            GET_EQ( this, i ) ->contains->GetSDesc(), GET_EQ( this, i ) ->GetSDesc() );
			Act( "$n draws $p from $P.", TRUE, this, GET_EQ( this, i ) ->contains, GET_EQ( this, i ), TO_ROOM );
			break;
		}
	}

	if ( i == NUM_WEARS )
		return 0;

	item = GET_EQ( this, i ) ->contains;
	obj_from_obj( GET_EQ( this, i ) ->contains );
	equip_char( this, item, WEAR_WIELD );

	return 1;
}

bool Character::CanDraw(Object *obj)
{
	if (GET_EQ(this, WEAR_SHIELD) && IS_OBJ_STAT(obj, ITEM_TWO_HANDED))
	{
		this->send( "You can't use that right now! It requires two hands!\r\n" );
		return false;
	}
	if ( obj->Weight() > GetStr() )
	{
		this->send( "It's too heavy for you to use.\r\n" );
		return false;
	}

	return true;
}

ACMD( do_draw )
{
	int ret_val = 0;

	if ( GET_EQ( ch, WEAR_WIELD ) )
		ch->send( "You are already wielding a weapon.\r\n" );
	else
	{
		ret_val = ch->Draw();
		if ( !ret_val )
			ch->send( "You don't seem to have anything sheathed.\r\n" );
	}
}

int Character::Sheath()
{
	Object * weapon;
	int i = 0;

	if ( !( weapon = GET_EQ( this, WEAR_WIELD ) ) )
		return -1;

	for ( i = 0;i < NUM_WEARS;++i )
	{
		if ( GET_EQ( this, i ) && !GET_EQ( this, i ) ->contains )
		{
			if ( IS_OBJ_STAT( GET_EQ( this, i ), ITEM_DAGGER_SHEATH ) && GET_OBJ_VAL( weapon, 0 ) == WEAPON_SHORT_BLADE )
			{
				this->send( "You slide %s into %s.\r\n", weapon->GetSDesc(), GET_EQ( this, i ) ->GetSDesc() );
				Act( "$n slides $p into $P.", TRUE, this, weapon, GET_EQ( this, i ), TO_ROOM );
				break;
			}
			else if ( IS_OBJ_STAT( GET_EQ( this, i ), ITEM_SWORD_SHEATH ) && GET_OBJ_VAL( weapon, 0 ) == WEAPON_LONG_BLADE )
			{
				this->send( "You sheath %s into %s.\r\n", weapon->GetSDesc(), GET_EQ( this, i ) ->GetSDesc() );
				Act( "$n sheaths $p into $P.", TRUE, this, weapon, GET_EQ( this, i ), TO_ROOM );
				break;
			}
			else if ( IS_OBJ_STAT( GET_EQ( this, i ), ITEM_SPEAR_SHEATH ) && GET_OBJ_VAL( weapon, 0 ) == WEAPON_SPEAR )
			{
				this->send( "You sheath %s into %s.\r\n", weapon->GetSDesc(), GET_EQ( this, i ) ->GetSDesc() );
				Act( "$n sheaths $p into $P.", TRUE, this, weapon, GET_EQ( this, i ), TO_ROOM );
				break;
			}
		}
	}

	if ( i == NUM_WEARS )
		return 0;

	obj_to_obj( unequip_char( this, WEAR_WIELD ), GET_EQ( this, i ) );
	return 1;
}

ACMD( do_sheath )
{
	int ret_val = 0;

	if ( !GET_EQ( ch, WEAR_WIELD ) )
		ch->send( "You are not wielding anything to sheath.\r\n" );
	else
	{
		ret_val = ch->Sheath();
		if ( !ret_val )
			ch->send( "You do not have anything to sheath your weapon into.\r\n" );
	}
}

void perform_give_extra( Character *giver, Character *receiver, Object *obj )
{
	int i = 0;
	bool sim_clan = false;

	if ( !obj || !giver || !receiver )
		return ;

	for ( Clan* C = ClanList;C;C = C->Next )
	{
		if ( giver->getUserClan( C->vnum ) && receiver->getUserClan( C->vnum ) )
		{
			sim_clan = true;
			i=C->vnum;
			break;
		}
	}

	//No similar clan, let's leave
	if ( !sim_clan )
		return ;

	if ( obj->scalp && MOB_FLAGGED( receiver, MOB_AWARD ) && obj->scalp->is_scalp && obj->scalp->player_scalp && !IS_CORPSE( obj ) && GET_RACE( giver ) != obj->scalp->race )
	{
		std::stringstream reasonBuffer;
		reasonBuffer << "Scalp `" << obj->short_description << "` turned in";

		UserClan *userClan = giver->getUserClan( CLAN_WHITE_TOWER );
		if( userClan && userClan->getRank() < 3 )
			sprintf( buf, "award %s %d 1 %s", GET_NAME( giver ), i, reasonBuffer.str().c_str() );
		else
			sprintf( buf, "award %s %d 5 %s", GET_NAME( giver ), i, reasonBuffer.str().c_str() );
		CommandInterpreter( receiver, buf );
		obj_from_char( obj );
		obj ->Extract();
		return;
	}
}

void perform_put( Character * ch, Object * obj, Object * cont )
{
	if ( !js_object_dropped(ch, obj) || ch->IsPurged() || obj->IsPurged() )
		return ;
	if ( !js_dropped_in_room( ch->in_room, obj, ch ) || ch->IsPurged() || obj->IsPurged() )
		return ;

	if ( cont->Weight() - obj->GetTotalWeight() > cont->GetTotalVal0() )
		Act( "$p won't fit in $P.", FALSE, ch, obj, cont, TO_CHAR );

	else
	{
		obj_from_char( obj );
		obj_to_obj( obj, cont );
		if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT)
		{//RHOLLOR 05.03.09 remove msg for invis items
            Act( "You put $p in $P.", FALSE, ch, obj, cont, TO_CHAR );
            Act( "$n puts $p in $P.", TRUE, ch, obj, cont, TO_ROOM );
		}

		if ( IS_OBJ_STAT( cont, ITEM_CHEST ) )
			update_chest_log( ch, cont, obj, TYPE_IN );

	}
}

ACMD( do_diceroll )
{

	int rolla = 0;
	int rollb = 0;

	rolla = MiscUtil::random( 1, 6 );
	rollb = MiscUtil::random( 1, 6 );

	ch->send( "You shake the dice, and throw them onto the ground, revealing a %d and %d on your six sided dice.\r\n",
	          rolla, rollb );


	sprintf( buf, "Two dice roll and hit the ground, thrown by $n, revealing a %d and %d on two six sided dice.",
	         rolla, rollb );
	Act( buf, TRUE, ch, 0, 0, TO_ROOM );
}

/* The following put modes are supported by the code below:

	1) put <object> <container>
	2) put all.<object> <container>
	3) put all <container>

	<container> must be in inventory or on ground.
	all objects to be put into container must be in inventory.
*/

ACMD( do_put )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	Object *obj, *next_obj, *cont;
	Character *tmp_char;
	int obj_dotmode, cont_dotmode, found = 0;

	TwoArguments( argument, arg1, arg2 );
	obj_dotmode = find_all_dots( arg1 );
	cont_dotmode = find_all_dots( arg2 );

	if ( !*arg1 )
		ch->send( "Put what in what?\r\n" );

	else if ( cont_dotmode != FIND_INDIV )
		ch->send( "You can only put things into one container at a time.\r\n" );

	else if ( !*arg2 )
	{
		ch->send( "What do you want to put %s in?\r\n", ( ( obj_dotmode == FIND_INDIV ) ? "it" : "them" ) );
	}

	else
	{
		generic_find( arg2, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP, ch, &tmp_char, &cont );

		if ( !cont )
		{
			ch->send( "You don't see %s %s here.\r\n", AN( arg2 ), arg2 );
		}

		else if ( GET_OBJ_TYPE( cont ) != ITEM_CONTAINER )
			Act( "$p is not a container.", FALSE, ch, cont, 0, TO_CHAR );

		else if ( OBJVAL_FLAGGED( cont, CONT_CLOSED ) )
			ch->send( "You'd better open it first!\r\n" );

		else
		{
			if ( obj_dotmode == FIND_INDIV )
			{	/* put <obj> <container> */
				if ( !( obj = get_obj_in_list_vis( ch, arg1, ch->carrying ) ) )
				{
					ch->send( "You aren't carrying %s %s.\r\n", AN( arg1 ), arg1 );
				}

				else if ( obj == cont )
					ch->send( "You attempt to fold it into itself, but fail.\r\n" );

				else
				{
					perform_put( ch, obj, cont );

					if ( IS_OBJ_STAT( cont, ITEM_CHEST ) )
						cont->itemSave();
				}
			}

			else
			{
				for ( obj = ch->carrying; obj; obj = next_obj )
				{
					next_obj = obj->next_content;

					if ( obj != cont && CAN_SEE_OBJ( ch, obj ) &&
					        ( obj_dotmode == FIND_ALL || isname( arg1, obj->getName() ) ) )
					{
						found = 1;
						perform_put( ch, obj, cont );
					}
				}

				if ( IS_OBJ_STAT( cont, ITEM_CHEST ) )
					cont->itemSave();

				if ( !found )
				{
					if ( obj_dotmode == FIND_ALL )
						ch->send( "You don't seem to have anything to put in it.\r\n" );

					else
					{
						ch->send( "You don't seem to have any %ss.\r\n", arg1 );
					}
				}
			}
		}
	}
}
bool Character::TooHeavyToPickUp( Object *obj )
{
	return( this->CarriedWeight() + obj->Weight() > CAN_CARRY_W( this ) && GET_LEVEL(this) < LVL_GRGOD );
}

int can_take_obj( Character * ch, Object * obj )
{
	if ( IS_CARRYING_N( ch ) >= CAN_CARRY_N( ch ) )
	{
		Act( "$p: you can't carry that many items.", FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	else if ( ch->TooHeavyToPickUp( obj ) )
	{
		Act( "$p: you can't carry that much weight.", FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	else if ( !( CAN_WEAR( obj, ITEM_WEAR_TAKE ) ) )
	{
		Act( "$p: you can't take that!", FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	return 1;
}

void perform_get_from_container( Character * ch, Object * obj, Object * cont, int mode )
{
	if ( mode == FIND_OBJ_INV || can_take_obj( ch, obj ) )
	{
		if ( IS_CARRYING_N( ch ) >= CAN_CARRY_N( ch ) )
			Act( "$p: you can't hold any more items.", FALSE, ch, obj, 0, TO_CHAR );

		else if ( js_object_get(ch, obj) && !obj->IsPurged() && !ch->IsPurged() )
		{
			if ( IS_OBJ_STAT( cont, ITEM_CHEST ) && GET_LEVEL( ch ) >= LVL_IMMORT && GET_LEVEL( ch ) < LVL_GRGOD )
			{
				ch->send( "You must be higher level to take items out of a clan chest.\r\n", GET_LEVEL( ch ) );
				return ;
			}

			obj_from_obj( obj );
			obj_to_char( obj, ch );
			if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT)
			{//RHOLLOR 05.03.09 remove message when invis...
                Act( "You get $p from $P.", FALSE, ch, obj, cont, TO_CHAR );
                Act( "$n gets $p from $P.", TRUE, ch, obj, cont, TO_ROOM );
			}

			if ( IS_OBJ_STAT( cont, ITEM_CHEST ) )
				update_chest_log( ch, cont, obj, TYPE_OUT );
		}
	}
}

void update_chest_log( Character *ch, Object *chest, Object *obj, int type )
{
	FILE * file;
	char msg[ MAX_STRING_LENGTH ];
	char *tm;
	time_t cur = time( 0 );

	if ( !ch || !chest || !obj || !obj->GetSDesc() )
		return ;

	if ( !( file = fopen( CHEST_LOG_FILE, "a+" ) ) )
	{
		MudLog( NRM, LVL_APPR, TRUE, "Cannot open chest log file for update." );
		return ;
	}

	if ( !IS_OBJ_STAT( chest, ITEM_CHEST ) )
		return ;

	tm = asctime( localtime( &cur ) );
	tm += 4;
	*( tm + 7 ) = '\0';

	if ( type == TYPE_OUT )
	{
		sprintf( msg, "%s: %s took \"%s\" out of chest vnum %d in room %d.",
		         tm, GET_NAME( ch ), obj->GetSDesc(), GET_OBJ_VNUM( chest ), ch->in_room->vnum );
	}

	else if ( type == TYPE_IN )
	{
		sprintf( msg, "%s: %s put \"%s\" in to chest vnum %d in room %d.",
		         tm, GET_NAME( ch ), obj->GetSDesc(), GET_OBJ_VNUM( chest ), ch->in_room->vnum );
	}

	else
	{
		fclose( file );
		return ;
	}

	fprintf( file, "%s\n", msg );
	fclose( file );

}

void get_from_container( Character * ch, Object * cont, char *arg, int mode )
{
	Object * obj, *next_obj;
	int obj_dotmode, found = 0;
	Character *p = 0;

	obj_dotmode = find_all_dots( arg );

	if ( OBJVAL_FLAGGED( cont, CONT_CLOSED ) )
		Act( "$p is closed.", FALSE, ch, cont, 0, TO_CHAR );
	else if ( !str_cmp( arg, "coins" ) )
	{
		if ( cont->Money )
		{
			ch->send( "You receive %s.\r\n", ch->GoldString( cont->Money ).c_str() );
			for ( p = ch->in_room->people;p;p = p->next_in_room )
			{
				if ( p != ch )
					p->send( "%s loots %s from %s.\r\n", GET_NAME( ch ), p->GoldString( cont->Money ).c_str(), cont->GetSDesc() );
			}
			ch->points.gold += cont->Money;
			cont->Money = 0;
			cont->itemSave();
		}
		else
		{
			ch->send( "There are no coins in that.\r\n" );
		}
	}
	else if ( obj_dotmode == FIND_INDIV )
	{
		if ( !( obj = get_obj_in_list_vis( ch, arg, cont->contains ) ) )
		{
			sprintf( buf, "There doesn't seem to be %s %s in $p.", AN( arg ), arg );
			Act( buf, FALSE, ch, cont, 0, TO_CHAR );
		}
		else
		{
			perform_get_from_container( ch, obj, cont, mode );

			if ( IS_OBJ_STAT( cont, ITEM_CHEST ) )
				cont->itemSave();
		}
	}
	else
	{
		if ( obj_dotmode == FIND_ALLDOT && !*arg )
		{
			ch->send( "Get all of what?\r\n" );
			return ;
		}

		for ( obj = cont->contains; obj; obj = next_obj )
		{
			next_obj = obj->next_content;

			if ( ( obj_dotmode == FIND_ALL || isname( arg, obj->getName() ) ) )
			{
				found = 1;
				perform_get_from_container( ch, obj, cont, mode );

			}
		}
		if ( obj_dotmode == FIND_ALL && cont->Money )
		{
			ch->send( "You receive %s.\r\n", ch->GoldString( cont->Money ).c_str() );
			sprintf( buf, "%s receives %s from %s.", GET_NAME( ch ), ch->GoldString( cont->Money ).c_str(),
			         cont->GetSDesc() );
			Act( buf, TRUE, ch, 0, 0, TO_ROOM );
			ch->points.gold += cont->Money;
			cont->Money = 0;
			cont->itemSave();

			found = 1;
		}

		if ( IS_OBJ_STAT( cont, ITEM_CHEST ) )
			cont->itemSave();

		if ( !found )
		{
			if ( obj_dotmode == FIND_ALL )
				Act( "$p seems to be empty.", FALSE, ch, cont, 0, TO_CHAR );

			else
			{
				sprintf( buf, "You can't seem to find any %ss in $p.", arg );
				Act( buf, FALSE, ch, cont, 0, TO_CHAR );
			}
		}
	}
}

int perform_get_from_room( Character * ch, Object * obj, bool vaultSave )
{
	if ( can_take_obj( ch, obj ) &&  !obj->IsPurged() && !ch->IsPurged()
		&& js_object_get(ch, obj) && !obj->IsPurged() && !ch->IsPurged()
		)
	{
		obj->RemoveFromRoom();
		obj_to_char( obj, ch );
		if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT)
		{//RHOLLOR 05.03.09 remove messg when invis
			Act( "You get $p.", FALSE, ch, obj, 0, TO_CHAR );
			Act( "$n gets $p.", TRUE, ch, obj, 0, TO_ROOM );
		}
		return 1;
	}

	return 0;
}

void get_from_room( Character *ch, char *arg )
{
	Object * obj, *next_obj;
	int dotmode, found = 0;

	dotmode = find_all_dots( arg );

	if ( dotmode == FIND_INDIV )
	{
		if ( !( obj = get_obj_in_list_vis( ch, arg, ch->in_room->contents ) ) || obj->hidden )
		{
			ch->send( "You don't see %s %s here.\r\n", AN( arg ), arg );
		}
		else
			perform_get_from_room( ch, obj, false );
	}
	else
	{
		if ( dotmode == FIND_ALLDOT && !*arg )
		{
			ch->send( "Get all of what?\r\n" );
			return ;
		}

		for ( obj = ch->in_room->contents; obj; obj = next_obj )
		{
			next_obj = obj->next_content;

			if ( !obj->hidden && CAN_SEE_OBJ( ch, obj ) && ( dotmode == FIND_ALL || isname( arg, obj->getName() ) ) )
			{
				found = 1;
				perform_get_from_room( ch, obj, false );
			}
		}

		if ( !found )
		{
			if ( dotmode == FIND_ALL )
				ch->send( "There doesn't seem to be anything here.\r\n" );
			else
			{
				ch->send( "You don't see any %ss here.\r\n", arg );
			}
		}
	}
	if( ROOM_FLAGGED(ch->in_room, ROOM_VAULT) )
		ch->in_room->itemSave();
}

ACMD( do_get )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];

	int cont_dotmode, found = 0, mode;
	Object *cont;
	Character *tmp_char;

	TwoArguments( argument, arg1, arg2 );

	if ( IS_CARRYING_N( ch ) >= CAN_CARRY_N( ch ) )
		ch->send( "Your arms are already full!\r\n" );
	else if ( !*arg1 )
		ch->send( "Get what?\r\n" );
	else if ( !*arg2 )
		get_from_room( ch, arg1 );
	else
	{
		cont_dotmode = find_all_dots( arg2 );

		if ( cont_dotmode == FIND_INDIV || FIND_OBJ_EQUIP )
		{
			mode = generic_find( arg2, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP, ch, &tmp_char, &cont );

			if ( !cont )
			{
				ch->send( "You don't have %s %s.\r\n", AN( arg2 ), arg2 );
			}
			else if ( GET_OBJ_TYPE( cont ) != ITEM_CONTAINER )
				Act( "$p is not a container.", FALSE, ch, cont, 0, TO_CHAR );
			else
				get_from_container( ch, cont, arg1, mode );
		}

		else
		{
			if ( cont_dotmode == FIND_ALLDOT && !*arg2 )
			{
				ch->send( "Get from all of what?\r\n" );
				return ;
			}

			for ( cont = ch->carrying; cont; cont = cont->next_content )
				if ( CAN_SEE_OBJ( ch, cont ) && ( cont_dotmode == FIND_ALL || isname( arg2, cont->getName() ) ) )
				{
					if ( GET_OBJ_TYPE( cont ) == ITEM_CONTAINER )
					{
						found = 1;
						get_from_container( ch, cont, arg1, FIND_OBJ_INV );
					}
					else if ( cont_dotmode == FIND_ALLDOT )
					{
						found = 1;
						Act( "$p is not a container.", FALSE, ch, cont, 0, TO_CHAR );
					}
				}
			for ( cont = ch->in_room->contents; cont; cont = cont->next_content )
			{
				if ( ( cont_dotmode == FIND_ALL || isname( arg2, cont->getName() ) ) )
				{
					if ( GET_OBJ_TYPE( cont ) == ITEM_CONTAINER )
					{
						get_from_container( ch, cont, arg1, FIND_OBJ_ROOM );
						found = 1;
					}

					else if ( cont_dotmode == FIND_ALLDOT )
					{
						Act( "$p is not a container.", FALSE, ch, cont, 0, TO_CHAR );
						found = 1;
					}
				}
			}

			if ( !found )
			{
				if ( cont_dotmode == FIND_ALL )
					ch->send( "You can't seem to find any containers.\r\n" );
				else
				{
					ch->send( "You can't seem to find any %ss here.\r\n", arg2 );
				}
			}
		}
	}
}

int perform_drop( Character *ch, Object *obj, bool vaultSave )
{
	if ( !js_object_dropped(ch, obj) || ch->IsPurged() || obj->IsPurged() )
		return 0;
	if ( !js_dropped_in_room( ch->in_room, obj, ch ) || ch->IsPurged() || obj->IsPurged() )
		return 0;

	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) && OBJ_FLAGGED(obj, ITEM_TEMP) )
	{
		sprintf( buf, "It would be gone the second you did that." );
		Act( buf, FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
	{
		sprintf( buf, "You can't drop $p." );
		Act( buf, FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}
    if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT) {//RHOLLOR 05.03.09 remove message when item INVIS
        sprintf( buf, "You drop $p." );
        Act( buf, FALSE, ch, obj, 0, TO_CHAR );

        sprintf( buf, "$n drops $p." );
        Act( buf, TRUE, ch, obj, 0, TO_ROOM );
    }

	obj_from_char( obj );
	obj->MoveToRoom( ch->in_room, vaultSave );
	return 0;
}

ACMD( do_drop )
{
	Object * obj, *next_obj;
	int dotmode, amount = 0;

	argument = OneArgument( argument, ::arg );

	if ( !*::arg )
	{
		ch->send( "What do you want to drop?\r\n" );
		return ;
	}

	else if ( IsNumber( ::arg ) )
	{
		amount = atoi( ::arg );
		argument = OneArgument( argument, ::arg );

		/* code to drop multiple items.  anyone want to write it? -je */
		ch->send( "Sorry, you can't do that to more than one item at a time.\r\n" );

		return ;
	}

	else
	{
		dotmode = find_all_dots( ::arg );

		if ( dotmode == FIND_ALL )
		{
			if ( !ch->carrying )
				ch->send( "You don't seem to be carrying anything.\r\n" );

			else
				for ( obj = ch->carrying; obj; obj = next_obj )
				{
					next_obj = obj->next_content;
					amount += perform_drop( ch, obj, false );
				}
		}

		else if ( dotmode == FIND_ALLDOT )
		{
			if ( !*::arg )
			{
				ch->send( "What do you want to drop all of?\r\n" );
				return ;
			}

			if ( !( obj = get_obj_in_list_vis( ch, ::arg, ch->carrying ) ) )
			{
				ch->send( "You don't seem to have any %ss.\r\n", ::arg );
			}

			while ( obj )
			{
				next_obj = get_obj_in_list_vis( ch, ::arg, obj->next_content );
				amount += perform_drop( ch, obj, false );
				obj = next_obj;
			}
		}

		else
		{
			if ( !( obj = get_obj_in_list_vis( ch, ::arg, ch->carrying ) ) )
				ch->send( "You don't seem to have %s %s.\r\n", AN( ::arg ), ::arg );
			else
				amount += perform_drop( ch, obj, false );
		}
	}
	if( ROOM_FLAGGED(ch->in_room, ROOM_VAULT) )
		ch->in_room->itemSave();
}

void perform_give( Character *ch, Character *vict, Object *obj )
{
	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || OBJ_FLAGGED(obj, ITEM_TEMP))
	{
		Act( "It would be gone the second you did that.", FALSE, ch, obj, 0, TO_CHAR );
		return ;
	}
	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
	{
		Act( "You can't let go of $p!!  Yeech!", FALSE, ch, obj, 0, TO_CHAR );
		return ;
	}

	if ( IS_CARRYING_N( vict ) >= CAN_CARRY_N( vict ) )
	{
		Act( "$N seems to have $S hands full.", FALSE, ch, 0, vict, TO_CHAR );
		return ;
	}

	if ( obj->Weight() + vict->CarriedWeight() > CAN_CARRY_W( vict ) )
	{
		Act( "$E can't carry that much weight.", FALSE, ch, 0, vict, TO_CHAR );
		return ;
	}
	if( !js_object_give(ch, obj, vict)      || obj->IsPurged() || ch->IsPurged() || vict->IsPurged() )
		return ;

	obj_from_char( obj );
	obj_to_char( obj, vict );
	if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT) //RHOLLOR 05.03.09 remove msg when item invis
		Act( "You give $p to $N.", FALSE, ch, obj, vict, TO_CHAR );
	if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(vict) >= LVL_IMMORT)
	{
		Act( "$n gives you $p.", FALSE, ch, obj, vict, TO_VICT );
		Act( "$n gives $p to $N.", TRUE, ch, obj, vict, TO_NOTVICT );
	}

	perform_give_extra( ch, vict, obj );
}

/* utility function for give */
Character *give_find_vict( Character *ch, char *arg )
{
	Character * vict;

	if ( !*arg )
	{
		ch->send( "To who?\r\n" );
		return NULL;
	}

	else if ( !( vict = get_char_room_vis( ch, arg ) ) )
	{
		ch->send( NOPERSON );
		return NULL;
	}

	else if ( vict == ch )
	{
		ch->send( "What's the point of that?\r\n" );
		return NULL;
	}

	else
		return vict;
}

ACMD( do_give )
{
	int amount = 0, dotmode = 0;
	Character *vict = 0;
	Object *obj = 0, *next_obj = 0;
	char amt[ MAX_INPUT_LENGTH ], target[ MAX_INPUT_LENGTH ], type[ MAX_INPUT_LENGTH ], col[ MAX_INPUT_LENGTH ], vcol[ MAX_INPUT_LENGTH ];

	argument = OneArgument( argument, ::arg );

	if ( !*::arg )
		ch->send( "Give what to who?\r\n" );
	else if ( MiscUtil::isInt( ::arg ) )
	{
		strcpy( amt, ::arg );
		TwoArguments( argument, type, target );

		if ( !*type )
		{
			ch->send( "Give how much of what money type?\r\n" );
			return ;
		}
		if ( !*target )
		{
			ch->send( "Give to who?\r\n" );
			return ;
		}

		if ( !( vict = get_char_room_vis( ch, target ) ) )
		{
			ch->send( NOPERSON );
			return ;
		}
		if ( GET_RACE( vict ) != GET_RACE( ch ) && GET_LEVEL( ch ) < LVL_IMMORT && GET_LEVEL( vict ) < LVL_IMMORT &&
		        !IS_NPC( ch ) )
		{
			ch->send( "%s refuses to accept that from you.\r\n", GET_NAME( vict ) );
			return ;
		}

		if ( !str_cmp( type, "copper" ) || !str_cmp( type, "coppers" ) )
		{
			amount = atoi( amt );
			sprintf( col, COLOR_YELLOW( ch, CL_COMPLETE ) );
			sprintf( vcol, COLOR_YELLOW( vict, CL_COMPLETE ) );
		}
		else if ( !str_cmp( type, "silver" ) )
		{
			amount = atoi( amt ) * 100;
			sprintf( col, "%s%s", COLOR_BOLD( ch, CL_COMPLETE ), COLOR_CYAN( ch, CL_COMPLETE ) );
			sprintf( vcol, "%s%s", COLOR_BOLD( vict, CL_COMPLETE ), COLOR_CYAN( vict, CL_COMPLETE ) );
		}
		else if ( !str_cmp( type, "gold" ) )
		{
			amount = atoi( amt ) * 10000;
			sprintf( col, "%s%s", COLOR_BOLD( ch, CL_COMPLETE ), COLOR_YELLOW( ch, CL_COMPLETE ) );
			sprintf( vcol, "%s%s", COLOR_BOLD( vict, CL_COMPLETE ), COLOR_YELLOW( vict, CL_COMPLETE ) );
		}
		else
		{
			ch->send( "You can only give multiple coin types to another person, not more than one item at a time.\r\n" );
			return ;
		}
		if ( ch->points.gold < amount )
		{
			ch->send( "You do not have that %s %s!\r\n", ( type[ strlen( type ) - 1 ] == 's' ) ? "many" : "much", type );
			return ;
		}
		if( amount <= 0 )
		{
			ch->send( "You can't give a negative amount of money!\r\n" );
			return ;
		}
		ch->send( "You give %s%d%s %s to %s.\r\n", col, atoi( amt ), COLOR_NORMAL( ch, CL_COMPLETE ), type,
		          GET_NAME( vict ) );
		vict->send( "%s gives you %s%d%s %s.\r\n", PERS( ch, vict ), vcol, atoi( amt ), COLOR_NORMAL( vict, CL_COMPLETE ), type );
		ch->points.gold	-= amount;
		vict->points.gold	+= amount;

		js_bribe_trigger( vict, ch, amount);
	}
	else
	{
		OneArgument( argument, buf1 );

		if ( !( vict = give_find_vict( ch, buf1 ) ) )
			return ;
		if ( GET_RACE( vict ) != GET_RACE( ch ) && GET_LEVEL( ch ) < LVL_IMMORT && GET_LEVEL( vict ) < LVL_IMMORT &&
		        !IS_NPC( ch ) )
		{
			ch->send( "%s refuses to accept that from you.\r\n", GET_NAME( vict ) );
			return ;
		}

		dotmode = find_all_dots( ::arg );

		if ( dotmode == FIND_INDIV )
		{
			if ( !( obj = get_obj_in_list_vis( ch, ::arg, ch->carrying ) ) )
			{
				ch->send( "You don't seem to have %s %s.\r\n", AN( ::arg ), ::arg );
			}

			else
				perform_give( ch, vict, obj );
		}

		else
		{
			if ( dotmode == FIND_ALLDOT && !*::arg )
			{
				ch->send( "All of what?\r\n" );
				return ;
			}

			if ( !ch->carrying )
				ch->send( "You don't seem to be holding anything.\r\n" );

			else
				for ( obj = ch->carrying; obj; obj = next_obj )
				{
					next_obj = obj->next_content;

					if ( ( ( dotmode == FIND_ALL || isname( ::arg, obj->getName() ) ) ) )
						perform_give( ch, vict, obj );
				}
		}
	}
}

void name_from_drinkcon( Object * obj )
{
	int i;
	char *new_name;

	for ( i = 0; ( *( ( obj->name ) + i ) != ' ' ) && ( *( ( obj->name ) + i ) != '\0' ); ++i )
		;

	if ( *( ( obj->name ) + i ) == ' ' )
	{
		new_name = str_dup( ( obj->name ) + i + 1 );


		if ( GET_OBJ_RNUM( obj ) < 0 || obj->name != obj_proto[ GET_OBJ_RNUM( obj ) ]->name )
			delete[] ( obj->name );

		obj->name = new_name;
	}
}

void name_to_drinkcon( Object * obj, int type )
{
	char * new_name;

	new_name = new char[ strlen( obj->name ) + strlen( drinknames[ type ] ) + 2 ];
	sprintf( new_name, "%s %s", drinknames[ type ], obj->name );

	if ( GET_OBJ_RNUM( obj ) < 0 || obj->name != obj_proto[ GET_OBJ_RNUM( obj ) ]->name )
		delete[] ( obj->name );

	obj->name = new_name;
}

ACMD( do_drink )
{
	Object * temp;
	struct affected_type af;
	int amount;
	int on_ground = 0;
	char arg[MAX_INPUT_LENGTH];

	OneArgument( argument, arg );

	if ( IS_NPC( ch ) )
		return ;

	if ( !*arg )
	{
		for ( temp = ch->in_room->contents;temp && GET_OBJ_TYPE( temp ) != ITEM_FOUNTAIN;temp = temp->next_content )
			;
		if ( !temp )
		{
			ch->send( "There's nothing here to drink from.\r\n" );
			return ;
		}
	}

	else if ( !( temp = get_obj_in_list_vis( ch, arg, ch->carrying ) ) )
	{
		if ( !( temp = get_obj_in_list_vis( ch, arg, ch->in_room->contents ) ) )
		{
			Act( "You can't find it!", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}

		else
			on_ground = 1;
	}

	if ( ( GET_OBJ_TYPE( temp ) != ITEM_DRINKCON ) &&
	        ( GET_OBJ_TYPE( temp ) != ITEM_FOUNTAIN ) )
	{
		ch->send( "You can't drink from that!\r\n" );
		return ;
	}

	if ( on_ground && ( GET_OBJ_TYPE( temp ) == ITEM_DRINKCON ) )
	{
		ch->send( "You have to be holding that to drink from it.\r\n" );
		return ;
	}

	if ( ( ch->PlayerData->conditions[ DRUNK ] > 10 ) && ( ch->PlayerData->conditions[ THIRST ] > 0 ) )
	{
		/* The pig is drunk */
		ch->send( "You can't seem to get close enough to your mouth.\r\n" );
		Act( "$n tries to drink but misses $s mouth!", TRUE, ch, 0, 0, TO_ROOM );
		return ;
	}

	if ( ( ch->PlayerData->conditions[ THIRST ] > 20 ) )
	{
		ch->send( "Your stomach can't contain anymore!\r\n" );
		return ;
	}

	if ( !GET_OBJ_VAL( temp, 1 ) )
	{
		ch->send( "It's empty.\r\n" );
		return ;
	}

	if ( subcmd == SCMD_DRINK )
	{
		sprintf( buf, "$n drinks %s from $p.", drinks[ GET_OBJ_VAL( temp, 2 ) ] );
		Act( buf, TRUE, ch, temp, 0, TO_ROOM );

		ch->send( "You drink the %s.\r\n", drinks[ GET_OBJ_VAL( temp, 2 ) ] );

		if ( drink_aff[ GET_OBJ_VAL( temp, 2 ) ][ DRUNK ] > 0 )
			amount = ( 25 - ch->PlayerData->conditions[ THIRST ] ) / drink_aff[ GET_OBJ_VAL( temp, 2 ) ][ DRUNK ];
		else
			amount = MiscUtil::random( 3, 10 );

	}

	else
	{
		Act( "$n sips from $p.", TRUE, ch, temp, 0, TO_ROOM );
		ch->send( "It tastes like %s.\r\n", drinks[ GET_OBJ_VAL( temp, 2 ) ] );
		amount = 1;
	}

	amount = MIN( amount, GET_OBJ_VAL( temp, 1 ) );

	gain_condition( ch, DRUNK, drink_aff[ GET_OBJ_VAL( temp, 2 ) ][ DRUNK ] * amount / 4 );

	//Blood
	if ( GET_OBJ_VAL( temp, 2 ) == 13 && GET_RACE( ch ) == RACE_TROLLOC &&
	(GET_CLASS( ch ) != CLASS_DREADLORD && GET_CLASS(ch) != CLASS_DREADGUARD) )
	{
		gain_condition( ch, THIRST, drink_aff[ 0 ][ THIRST ] * amount / 4 );
	}
	else if ( GET_OBJ_VAL( temp, 2 ) != 13 && GET_RACE( ch ) == RACE_TROLLOC
	&& GET_CLASS( ch ) != CLASS_DREADLORD && GET_CLASS(ch) != CLASS_DREADGUARD )
	{
		gain_condition( ch, THIRST, drink_aff[ 13 ][ THIRST ] * amount / 4 );
	}
	else
		gain_condition( ch, THIRST, drink_aff[ GET_OBJ_VAL( temp, 2 ) ][ THIRST ] * amount / 4 );

	if ( ch->PlayerData->conditions[ DRUNK ] > 10 )
		ch->send( "You feel drunk.\r\n" );

	if ( ch->PlayerData->conditions[ THIRST ] > 20 )
		ch->send( "You don't feel thirsty any more.\r\n" );

	if ( GET_OBJ_VAL( temp, 3 ) )
	{	/* It was poisoned */
		ch->send( "Oops, it tasted rather strange!\r\n" );
		Act( "$n chokes and utters some strange sounds.", TRUE, ch, 0, 0, TO_ROOM );

		af.type = SPELL_POISON;
		af.duration = amount * 3;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector= AFF_POISON;
		affect_join( ch, &af, FALSE, FALSE, FALSE, FALSE );
	}

	/* empty the container, and no longer poison. */
	GET_OBJ_VAL( temp, 1 ) -= amount;

	if ( !GET_OBJ_VAL( temp, 1 ) )
	{	/* The last bit */
		GET_OBJ_VAL( temp, 2 ) = 0;
		GET_OBJ_VAL( temp, 3 ) = 0;
		//	name_from_drinkcon(temp);
	}
}

ACMD( do_eat )
{
	Object * food;
	struct affected_type af;
	int amount;
	char arg[MAX_INPUT_LENGTH];

	OneArgument( argument, arg );

	if ( IS_NPC( ch ) )
		return ;

	if ( !*arg )
	{
		ch->send( "Eat what?\r\n" );
		return ;
	}

	if ( !( food = get_obj_in_list_vis( ch, arg, ch->carrying ) ) )
	{
		ch->send( "You don't seem to have %s %s.\r\n", AN( arg ), arg );
		return ;
	}

	if ( subcmd == SCMD_TASTE && ( ( GET_OBJ_TYPE( food ) == ITEM_DRINKCON ) ||
	                               ( GET_OBJ_TYPE( food ) == ITEM_FOUNTAIN ) ) )
	{
		do_drink( ch, argument, 0, SCMD_SIP );
		return ;
	}

	if ( ( GET_OBJ_TYPE( food ) != ITEM_FOOD ) && ( GET_LEVEL( ch ) < LVL_IMMORT ) )
	{
		ch->send( "You can't eat THAT!\r\n" );
		return ;
	}

	if ( ch->PlayerData->conditions[ FULL ] > 20 )
	{ /* Stomach full */
		Act( "You are too full to eat more!", FALSE, ch, 0, 0, TO_CHAR );
		return ;
	}

	if ( subcmd == SCMD_EAT )
	{
		Act( "You eat $p.", FALSE, ch, food, 0, TO_CHAR );
		Act( "$n eats $p.", TRUE, ch, food, 0, TO_ROOM );
	}

	else
	{
		Act( "You nibble a little bit of $p.", FALSE, ch, food, 0, TO_CHAR );
		Act( "$n tastes a little bit of $p.", TRUE, ch, food, 0, TO_ROOM );
	}

	amount = ( subcmd == SCMD_EAT ? GET_OBJ_VAL( food, 0 ) : 1 );

	gain_condition( ch, FULL, amount );

	if ( ch->PlayerData->conditions[ FULL ] > 20 )
		Act( "You are full.", FALSE, ch, 0, 0, TO_CHAR );

	if ( GET_OBJ_VAL( food, 3 ) && ( GET_LEVEL( ch ) < LVL_IMMORT ) )
	{
		/* The shit was poisoned ! */
		ch->send( "Oops, that tasted rather strange!\r\n" );
		Act( "$n coughs and utters some strange sounds.", FALSE, ch, 0, 0, TO_ROOM );

		af.type = SPELL_POISON;
		af.duration = amount * 2;
		af.modifier = 0;
		af.location = APPLY_NONE;
		af.bitvector= AFF_POISON;
		affect_join( ch, &af, FALSE, FALSE, FALSE, FALSE );
	}

	if ( subcmd == SCMD_EAT )
		food->Extract(true);
	else
	{
		if ( !( --GET_OBJ_VAL( food, 0 ) ) )
		{
			ch->send( "There's nothing left now.\r\n" );
			food->Extract(true);
		}
	}
}

ACMD( do_pour )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	Object *from_obj = NULL, *to_obj = NULL;
	int amount;

	TwoArguments( argument, arg1, arg2 );

	if ( subcmd == SCMD_POUR )
	{
		if ( !*arg1 )
		{		/* No arguments */
			Act( "From what do you want to pour?", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}

		if ( !( from_obj = get_obj_in_list_vis( ch, arg1, ch->carrying ) ) )
		{
			Act( "You can't find it!", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}

		if ( GET_OBJ_TYPE( from_obj ) != ITEM_DRINKCON )
		{
			Act( "You can't pour from that!", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}


		// Refilling lanterns. Requires special code, so we need to do most of the checks ourselves.
		// -Narg

		if ( ( to_obj = get_obj_in_list_vis( ch, arg2, ch->carrying ) ) )
		{

			if ( GET_OBJ_TYPE( to_obj ) == ITEM_LIGHT )
			{
				if ( !( to_obj = get_obj_in_list_vis( ch, arg2, ch->carrying ) ) )
				{
					Act( "Pour into what?", FALSE, ch, 0, 0, TO_CHAR );
					return ;
				}

				if ( to_obj == from_obj )
				{
					Act( "A most unproductive effort.", FALSE, ch, 0, 0, TO_CHAR );
					return ;
				}



				if ( GET_OBJ_VAL( from_obj, 2 ) != 16 )  // oil
				{
					ch->send( "It isn't designed to burn anything but oil.\r\n" );
					return ;
				}

				if ( GET_OBJ_VAL( to_obj, 4 ) == 0 )
				{
					ch->send( "You can't refill that.\r\n" );
					return ;
				}


				amount = GET_OBJ_VAL( from_obj, 1 );
				int current = GET_OBJ_VAL( to_obj, 2 );
				int max = GET_OBJ_VAL( to_obj, 3 );


				if ( amount == 0 )
				{
					ch->send( "It is empty...\r\n" );
					return ;
				}

				// Infinite ammount can be held.
				if ( max == 0 )
				{
					GET_OBJ_VAL( to_obj, 2 ) = current + amount;
					GET_OBJ_VAL( from_obj, 1 ) = 0;
					Act( "You carefully pour the oil from $P into $p.", FALSE, ch, to_obj, from_obj, TO_CHAR );
					Act( "$n carefully pours the oil from $P into $p.", TRUE, ch, to_obj, from_obj, TO_ROOM );
					return ;
				}

				if ( amount <= ( max - current ) )
				{
					GET_OBJ_VAL( to_obj, 2 ) = current + amount;
					GET_OBJ_VAL( from_obj, 1 ) = 0;
					Act( "You carefully pour the oil from $P into $p.", FALSE, ch, to_obj, from_obj, TO_CHAR );
					Act( "$n carefully pours the oil from $P into $p.", TRUE, ch, to_obj, from_obj, TO_ROOM );
					return ;

				}
				else if ( current == max )
				{
					ch->send( "It is full.\r\n" );
					return ;
				}
				else
				{
					int poured = ( max - current );
					GET_OBJ_VAL( to_obj, 2 ) = max;
					GET_OBJ_VAL( from_obj, 1 ) = ( amount - poured );
					Act( "You expertly top $p off with oil.", FALSE, ch, to_obj, from_obj, TO_CHAR );
					Act( "$n expertly tops $p off with oil.", TRUE, ch, to_obj, from_obj, TO_ROOM );
					return ;
				}
			}
		}

	}


	if ( subcmd == SCMD_FILL )
	{
		if ( !*arg1 )
		{		// no arguments
			ch->send( "What do you want to fill?  And what are you filling it from?\r\n" );
			return ;
		}

		if ( !( to_obj = get_obj_in_list_vis( ch, arg1, ch->carrying ) ) )
		{
			ch->send( "You can't find it!" );
			return ;
		}

		if ( GET_OBJ_TYPE( to_obj ) == ITEM_LIGHT )
		{
			ch->send( "It is not designed to be filled in that manner.\r\n" );
			return ;
		}


		if ( GET_OBJ_TYPE( to_obj ) != ITEM_DRINKCON )
		{
			Act( "You can't fill $p!", FALSE, ch, to_obj, 0, TO_CHAR );
			return ;
		}

		if ( !*arg2 )
		{		// no 2nd argument
			Act( "What do you want to fill $p from?", FALSE, ch, to_obj, 0, TO_CHAR );
			return ;
		}

		if ( !( from_obj = get_obj_in_list_vis( ch, arg2, ch->in_room->contents ) ) )
		{
			ch->send( "There doesn't seem to be %s %s here.\r\n", AN( arg2 ), arg2 );
			return;
		}

		if ( GET_OBJ_TYPE(from_obj) != ITEM_FOUNTAIN && !IS_CORPSE(from_obj) )
		{
			Act( "You can't fill something from $p.", FALSE, ch, from_obj, 0, TO_CHAR );
			return;
		}
	}

	short *fromObjectLiquidSource;

	if(IS_CORPSE(from_obj)) {

		fromObjectLiquidSource = &from_obj->scalp->blood;
	}
	else {

		fromObjectLiquidSource = &GET_OBJ_VAL(from_obj, 1);
	}

	if ( (*fromObjectLiquidSource) == 0 )
	{
		Act( "The $p is empty.", FALSE, ch, from_obj, 0, TO_CHAR );
		return ;
	}

	if ( subcmd == SCMD_POUR )
	{	// pour
		if ( !*arg2 )
		{
			Act( "Where do you want it?  Out or in what?", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}

		if ( !str_cmp( arg2, "out" ) )
		{
			Act( "$n empties $p.", TRUE, ch, from_obj, 0, TO_ROOM );
			Act( "You empty $p.", FALSE, ch, from_obj, 0, TO_CHAR );

			GET_OBJ_VAL( from_obj, 1 ) = 0;
			GET_OBJ_VAL( from_obj, 2 ) = 0;
			GET_OBJ_VAL( from_obj, 3 ) = 0;
			//	name_from_drinkcon(from_obj);

			return ;
		}

		if ( !( to_obj = get_obj_in_list_vis( ch, arg2, ch->carrying ) ) )
		{
			Act( "You can't find it!", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}

		if ( ( GET_OBJ_TYPE( to_obj ) != ITEM_DRINKCON ) &&
		        ( GET_OBJ_TYPE( to_obj ) != ITEM_FOUNTAIN ) )
		{
			Act( "You can't pour anything into that.", FALSE, ch, 0, 0, TO_CHAR );
			return ;
		}
	}

	if ( to_obj == from_obj )
	{
		Act( "A most unproductive effort.", FALSE, ch, 0, 0, TO_CHAR );
		return ;
	}

	int fromObjLiquidType = GET_OBJ_VAL(from_obj, 2);

	if(IS_CORPSE(from_obj)) {

		fromObjLiquidType = LIQ_BLOOD;
	}

	if ( ( GET_OBJ_VAL( to_obj, 1 ) != 0 ) &&
	        ( GET_OBJ_VAL( to_obj, 2 ) != fromObjLiquidType ) )
	{
		Act( "There is already another liquid in it!", FALSE, ch, 0, 0, TO_CHAR );
		return ;
	}

	if ( !( GET_OBJ_VAL( to_obj, 1 ) < GET_OBJ_VAL( to_obj, 0 ) ) )
	{
		Act( "There is no room for more.", FALSE, ch, 0, 0, TO_CHAR );
		return ;
	}

	if ( subcmd == SCMD_POUR )
	{
		ch->send( "You pour the %s into the %s.", drinks[ GET_OBJ_VAL( from_obj, 2 ) ], arg2 );
	}

	if ( subcmd == SCMD_FILL )
	{

		if(IS_CORPSE(from_obj)) {

			Act("You lean over and drain the blood from $P into $p.", FALSE, ch, to_obj, from_obj, TO_CHAR);
			Act("$n leans over and drains the blood from $P into $p.", FALSE, ch, to_obj, from_obj, TO_ROOM);
		}
		else {

			Act( "You gently fill $p from $P.", FALSE, ch, to_obj, from_obj, TO_CHAR );
			Act( "$n gently fills $p from $P.", TRUE, ch, to_obj, from_obj, TO_ROOM );
		}
	}

	// New alias
	if ( GET_OBJ_VAL( to_obj, 1 ) == 0 )
		name_to_drinkcon( to_obj, fromObjLiquidType );

	// First same type liq.
	GET_OBJ_VAL( to_obj, 2 ) = fromObjLiquidType;

	// Then how much to pour
	(*fromObjectLiquidSource) -= ( amount = ( GET_OBJ_VAL( to_obj, 0 ) - GET_OBJ_VAL( to_obj, 1 ) ) );

	GET_OBJ_VAL( to_obj, 1 ) = GET_OBJ_VAL( to_obj, 0 );

	if ( (*fromObjectLiquidSource) < 0 )
	{	// There was too little
		GET_OBJ_VAL( to_obj, 1 ) += (*fromObjectLiquidSource);
		amount += (*fromObjectLiquidSource);
		(*fromObjectLiquidSource) = 0;

		if(!IS_CORPSE(from_obj)) {
			GET_OBJ_VAL( from_obj, 2 ) = 0;
			GET_OBJ_VAL( from_obj, 3 ) = 0;
		}
		//	name_from_drinkcon(from_obj);
	}

	// Then the poison boogie
	GET_OBJ_VAL( to_obj, 3 ) = ( GET_OBJ_VAL( to_obj, 3 ) || (GET_OBJ_VAL( from_obj, 3 ) && !IS_CORPSE(from_obj)) );

	return ;
}

void wear_message( Character * ch, Object * obj, int where )
{
	const char * wear_messages[][ 2 ] =
	    {
	        {"$n lights $p and holds it.",
	         "You light $p and hold it."
	        },

	        {"$n grabs $p.",
	         "You grab $p."},

	        {"$n wears $p around $s neck.",
	         "You wear $p around your neck."},

	        {"$n wears $p around $s neck.",
	         "You wear $p around your neck."},

	        {"$n slides $p into $s right ear.",
	         "You slide $p into your right ear."},

	        {"$n slides $p into $s left ear.",
	         "You slide $p into your left ear."},

	        {"$n wears $p on $s head.",
	         "You wear $p on your head."},

	        {"$n wears $p about $s body.",
	         "You wear $p around your body."},

	        {"$n wears $p on $s back.",
	         "You wear $p around your back."},

	        {"$n wears $p on $s body.",
	         "You wear $p on your body.", },

	        {"$n straps $p onto $s shoulders.",
	         "You strap $p onto your shoulders."},

	        {"$n wears $p on $s arms.",
	         "You wear $p on your arms."},

	        {"$n wears $p around $s waist.",
	         "You wear $p around your waist."},

	        {"$n puts $p on around $s right wrist.",
	         "You put $p on around your right wrist."},

	        {"$n puts $p on around $s left wrist.",
	         "You put $p on around your left wrist."},

	        {"$n puts $p on $s hands.",
	         "You put $p on your hands."},

	        {"$n slides $p on to $s right ring finger.",
	         "You slide $p on to your right ring finger."},

	        {"$n slides $p on to $s left ring finger.",
	         "You slide $p on to your left ring finger."},

	        {"$n wields $p.",
	         "You wield $p."},

	        {"$n straps $p around $s arm as a shield.",
	         "You start to use $p as a shield."},

	        {"$n puts $p on $s legs.",
	         "You put $p on your legs."},

	        {"$n wears $p on $s feet.",
	         "You wear $p on your feet."}

	    };

	Act( wear_messages[ where ][ 0 ], TRUE, ch, obj, 0, TO_ROOM );
	Act( wear_messages[ where ][ 1 ], FALSE, ch, obj, 0, TO_CHAR );
}

int wear_bitvectors[] =
{
	ITEM_WEAR_TAKE,
	ITEM_WEAR_TAKE,
	ITEM_WEAR_NECK,
	ITEM_WEAR_NECK,
	ITEM_WEAR_EAR,
	ITEM_WEAR_EAR,
	ITEM_WEAR_HEAD,
	ITEM_WEAR_ABOUT,
	ITEM_WEAR_BACK,
	ITEM_WEAR_BODY,
	ITEM_WEAR_SHOULDERS,
	ITEM_WEAR_ARMS,
	ITEM_WEAR_WAIST,
	ITEM_WEAR_WRIST,
	ITEM_WEAR_WRIST,
	ITEM_WEAR_HANDS,
	ITEM_WEAR_FINGER,
	ITEM_WEAR_FINGER,
	ITEM_WEAR_WIELD,
	ITEM_WEAR_SHIELD,
	ITEM_WEAR_LEGS,
	ITEM_WEAR_FEET
};

const char *already_wearing[] =
{
	        "You're already using a light.\r\n",
	        "You're already holding something.\r\n",
	        "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
	        "You can't wear anything else around your neck.\r\n",
	        "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
	        "You're already wearing something in both ears.\r\n",
	        "You're already wearing something on your head.\r\n",
	        "You're already wearing something about your body.\r\n",
	        "You're already wearing something on your back.\r\n",
	        "You're already wearing something on your body.\r\n",
	        "You're already wearing something on your shoulders.\r\n",
	        "You're already wearing something on your arms.\r\n",
	        "You already have something around your waist.\r\n",
	        "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
	        "You're already wearing something around both of your wrists.\r\n",
	        "You're already wearing something on your hands.\r\n",
	        "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\r\n",
	        "You're already wearing something on both of your ring fingers.\r\n",
	        "You're already wielding a weapon.\r\n",
	        "You're already using a shield.\r\n",
	        "You're already wearing something on your legs.\r\n",
	        "You're already wearing something on your feet.\r\n"
};

bool Character::CanWear(Object *obj, int &loc)
{
	if (!CAN_WEAR(obj, wear_bitvectors[loc]))
	{
		Act( "You can't wear $p there.", FALSE, this, obj, 0, TO_CHAR );
		return false;
	}

	if (GET_EQ(this, WEAR_SHIELD ) && IS_OBJ_STAT(obj, ITEM_TWO_HANDED))
	{
		this->send( "You can't use that right now! It requires two hands!\r\n" );
		return false;
	}

	if ( GET_EQ(this, WEAR_WIELD ) && loc == WEAR_SHIELD && IS_OBJ_STAT( GET_EQ( this, WEAR_WIELD ), ITEM_TWO_HANDED ) )
	{
		this->send( "Both of your hands are occupied currently.\r\n" );
		return false;
	}
	if ( loc == WEAR_WIELD && obj->Weight() > GetStr() )
	{
		this->send( "It's too heavy for you to use.\r\n" );
		return false;
	}

	if ( !remort_gear_message(this, obj) )
		return false;

	/* for neck, finger, and wrist, try pos 2 if pos 1 is already full */
	if ((loc == WEAR_FINGER_R ) || (loc == WEAR_NECK_1) || (loc == WEAR_WRIST_R) || (loc == WEAR_EAR_1 ))
	{
		if (GET_EQ(this, loc))
		{
			++loc;
		}
	}

	if ( GET_EQ( this, loc ) )
	{
		this->send( already_wearing[ loc ] );
		return false;
	}

	if ( !js_object_wear(this, obj, loc) || this->IsPurged() || obj->IsPurged() )
		return false;
	return true;
}

void perform_wear( Character * ch, Object * obj, int where )
{
	if(ch->CanWear(obj, where))
	{
		obj_from_char( obj );
		equip_char( ch, obj, where );
		if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) > LVL_IMMORT) //RHOLLOR 05.03.09 remove msg when item is INVIS
			wear_message( ch, obj, where );
	}
}

int find_eq_pos( Character * ch, Object * obj, char *arg, bool msg)
{
	int where = -1;

	const char *keywords[] =
	{	/* \n to prevent explicit wearing. */
		"!RESERVED!",
		"!RESERVED!",
		"neck",
		"!RESERVED!",
		"ear",
		"!RESERVED!",
		"head",
		"about",
		"back",
		"body",
		"shoulders",
		"arms",
		"waist",
		"wrist",
		"!RESERVED!",
		"hands",
		"finger",
		"!RESERVED!",
		"wield",
		"shield",
		"legs",
		"feet"
		"!RESERVED!",
		"\n"
	};

	if ( !arg || !*arg )
	{
		if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
			where = WEAR_FINGER_R;
		else if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
			where = WEAR_NECK_1;
		else if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) )
			where = WEAR_EAR_1;
		else if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
			where = WEAR_BODY;
		else if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
			where = WEAR_HEAD;
		else if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
			where = WEAR_LEGS;
		else if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
			where = WEAR_FEET;
		else if ( CAN_WEAR( obj, ITEM_WEAR_BACK ) )
			where = WEAR_BACK;
		else if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
			where = WEAR_HANDS;
		else if ( CAN_WEAR( obj, ITEM_WEAR_SHOULDERS ) )
			where = WEAR_SHOULDERS;
		else if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
			where = WEAR_ARMS;
		else if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
			where = WEAR_SHIELD;
		else if ( CAN_WEAR( obj, ITEM_WEAR_WIELD ) )
			where = WEAR_WIELD;
		else if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
			where = WEAR_ABOUT;
		else if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
			where = WEAR_WAIST;
		else if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
			where = WEAR_WRIST_R;
	}

	else
	{
		if ( msg && ( ( ( where = search_block( arg, ( const char ** ) keywords, FALSE ) ) < 0 ) ||
		        ( *arg == '!' )) )
		{
			ch->send( "'%s'?  What part of your body is THAT?\r\n", arg );
			return -1;
		}
	}

	return where;
}

ACMD( do_wear )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	Object *obj, *next_obj;
	int where = -1, dotmode, items_worn = 0;

	TwoArguments( argument, arg1, arg2 );

	if ( !*arg1 )
	{
		ch->send( "Wear what?\r\n" );
		return ;
	}

	dotmode = find_all_dots( arg1 );

	if ( *arg2 && ( dotmode != FIND_INDIV ) )
	{
		ch->send( "You can't specify the same body location for more than one item!\r\n" );
		return ;
	}

	if ( dotmode == FIND_ALL )
	{
		for ( obj = ch->carrying; obj; obj = next_obj )
		{
			next_obj = obj->next_content;

			if ( ( where = find_eq_pos( ch, obj, 0, true ) ) >= 0 )
			{
				++items_worn;
				perform_wear( ch, obj, where );
			}
		}

		if ( !items_worn )
			ch->send( "You don't seem to have anything wearable.\r\n" );
	}

	else if ( dotmode == FIND_ALLDOT )
	{
		if ( !*arg1 )
		{
			ch->send( "Wear all of what?\r\n" );
			return ;
		}

		if ( !( obj = get_obj_in_list( arg1, ch->carrying ) ) )
		{
			ch->send( "You don't seem to have any %ss.\r\n", arg1 );
		}

		else
		{
			while ( obj )
			{
				next_obj = get_obj_in_list_vis( ch, arg1, obj->next_content );

				if ( ( where = find_eq_pos( ch, obj, 0, true ) ) >= 0 )
					perform_wear( ch, obj, where );
				else
					Act( "You can't wear $p.", FALSE, ch, obj, 0, TO_CHAR );

				obj = next_obj;
			}
		}
	}

	else
	{
		if ( !( obj = get_obj_in_list_vis( ch, arg1, ch->carrying ) ) )
		{
			ch->send( "You don't seem to have %s %s.\r\n", AN( arg1 ), arg1 );
		}

		else
		{
			if ( ( where = find_eq_pos( ch, obj, arg2, true) ) >= 0 )
				perform_wear( ch, obj, where );
			else if ( !*arg2 )
				Act( "You can't wear $p.", FALSE, ch, obj, 0, TO_CHAR );
		}
	}
}

ACMD( do_wield )
{
	Object * obj;

	OneArgument( argument, ::arg );

	if ( !*::arg )
		ch->send( "Wield what?\r\n" );

	else if ( !( obj = get_obj_in_list_vis( ch, ::arg, ch->carrying ) ) )
	{
		ch->send( "You don't seem to have %s %s.\r\n", AN( ::arg ), ::arg );
	}

	else
	{
		if ( !( IS_SET(obj->obj_flags.wear_flags, ITEM_WEAR_WIELD )))
		{
			ch->send( "You can't wield that.\r\n" );
		}
		else
		{
			perform_wear( ch, obj, WEAR_WIELD );
		}
	}
}

ACMD( do_grab )
{
	Object * obj;

	OneArgument( argument, ::arg );

	if ( !*::arg )
		ch->send( "Hold what?\r\n" );

	else if /*( !( obj = get_obj_in_list_vis( ch, arg, ch->carrying ) ) )*/ ( !( obj = get_obj_in_list(::arg, ch->carrying)))
	{
		ch->send( "You don't seem to have %s %s.\r\n", AN( ::arg ), ::arg );
	}

	else
	{
		if ( GET_OBJ_TYPE( obj ) == ITEM_LIGHT )
			perform_wear( ch, obj, WEAR_LIGHT );
		else if ( CAN_WEAR( obj, ITEM_WEAR_HOLD ) )
			perform_wear( ch, obj, WEAR_HOLD );
		else
			ch->send( "You can't hold that.\r\n" );
	}
}

void perform_remove( Character * ch, int pos )
{
	Object * obj;

	if ( !( obj = GET_EQ( ch, pos ) ) )
		Log( "Error in perform_remove: bad pos %d passed.", pos );
	else if ( IS_OBJ_STAT( obj, ITEM_NODROP ) && !OBJ_FLAGGED(obj, ITEM_TEMP))
		Act( "You can't remove $p, it must be CURSED!", FALSE, ch, obj, 0, TO_CHAR );
	else if ( IS_CARRYING_N( ch ) >= CAN_CARRY_N( ch ) )
		Act( "$p: you can't carry that many items!", FALSE, ch, obj, 0, TO_CHAR );
	else
	{
		if( !js_object_removed(ch, obj, pos) || ch->IsPurged() || obj->IsPurged() )
			return;
		if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT)
		{ //RHOLLOR 05.03.09 remove message to morts when item is INVIS
			Act( "You stop using $p.", FALSE, ch, obj, 0, TO_CHAR );
			Act( "$n stops using $p.", TRUE, ch, obj, 0, TO_ROOM );
		}
		obj_to_char( unequip_char( ch, pos ), ch );
	}
}

ACMD( do_remove )
{
	Object * obj;
	int i, dotmode, found;

	OneArgument( argument, ::arg );

	if ( !*::arg )
	{
		ch->send( "Remove what?\r\n" );
		return ;
	}

	dotmode = find_all_dots( ::arg );

	if ( dotmode == FIND_ALL )
	{
		found = 0;

		for ( i = 0; i < NUM_WEARS; ++i )
		{
			if ( GET_EQ( ch, i ) )
			{
				perform_remove( ch, i );
				found = 1;
			}
		}
		if ( !found )
			ch->send( "You're not using anything.\r\n", ch );
	}

	else if ( dotmode == FIND_ALLDOT )
	{
		if ( !*::arg )
			ch->send( "Remove all of what?\r\n", ch );

		else
		{
			found = 0;

			for ( i = 0; i < NUM_WEARS; ++i )
			{
				if ( GET_EQ( ch, i ) && CAN_SEE_OBJ( ch, GET_EQ( ch, i ) ) &&
				        isname( ::arg, GET_EQ( ch, i ) ->getName() ) )
				{
					perform_remove( ch, i );
					found = 1;
				}

				if ( !found )
					ch->send( "You don't seem to be using any %ss.\r\n", ::arg );
			}
		}
	}

	else
	{
		if ( !( obj = get_object_in_equip_vis( ch, ::arg, ch->equipment, &i ) ) )
		{
			ch->send( "You don't seem to be using %s %s.\r\n", AN( ::arg ), ::arg );
		}

		else
			perform_remove( ch, i );
	}
}

/* Allows a manual breaking of keys by players -Narg */
ACMD( do_break )
{
	Object * o;
	bool keyfound = false;

	OneArgument( argument, ::arg );

	if ( !*::arg )
	{
		ch->send( "Break what?\r\n" );
		return ;
	}

	else if ( !( o = get_obj_in_list_vis( ch, ::arg, ch->carrying ) ) && ( !( o = get_obj_in_list_vis( ch, ::arg, ( GET_EQ( ch, WEAR_HOLD ) ) ) ) ) )
	{
		ch->send( "You don't seem to have %s %s.\r\n", AN( ::arg ), ::arg );
		return ;
	}

	else if ( ( o = get_obj_in_list_vis( ch, ::arg, ch->carrying ) ) )
	{
		if ( GET_OBJ_TYPE( o ) != ITEM_KEY )
		{
			keyfound = true;
			ch->send( "Your puny fingers are too weak to break anything but a key!\r\n" );
		}
		else
		{
			keyfound = true;
			o ->Extract();
			ch->send( "*snap*\r\n" );
			return ;
		}
	}
	else if ( keyfound == false )
	{
		if ( ( o = get_obj_in_list_vis( ch, ::arg, ( GET_EQ( ch, WEAR_HOLD ) ) ) ) )
		{
			keyfound = true;
			if ( GET_OBJ_TYPE( o ) == ITEM_KEY )
			{
				keyfound = true;
				o ->Extract();
				ch->send( "*snap*\r\n" );
				return ;
			}
		}
	}
	else
	{
		ch->send( "Your puny fingers are too weak to break anything but a key!\r\n" );
		return ;
	}
}

/* Galnor 10/27/2009 - Show an item to another player */
ACMD( do_show )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	Object *item;
	Character *target;
	TwoArguments(argument,arg1,arg2);
	if( !*arg1 )
	{
		ch->send("What do you wish to show, and to whom?\r\n");
		return;
	}
	if( !*arg2 )
	{
		ch->send("To whom do you wish to show that?\r\n");
		return;
	}
	int posTrash;
	if( !(item = get_object_in_equip_vis(ch, arg1, ch->equipment, &posTrash))
		&& !(item = get_obj_in_list_vis(ch, arg1, ch->carrying)) )
	{
		ch->send("You don't seem to have a '%s'.\r\n", arg1);
		return;
	}
	if( !(target = get_char_room_vis(ch,arg2)) )
	{
		ch->send(NOPERSON);
		return;
	}
	if( target == ch )
	{
		ch->send("Show it to yourself? Now that's just silly.\r\n");
		return;
	}
	
	ch->send("You show %s to %s.\r\n", item->GetSDesc(), GET_NAME(target));
	Act("$n shows $p to $N.", TRUE, ch, item, target, TO_NOTVICT);

	if( !CAN_SEE(target,ch) )
		return;
	target->send("%s shows you %s.\r\n", GET_NAME(ch), item->GetSDesc());
	if(item->GetExDesc())
	{
		target->send("%s\r\n\r\n", item->GetExDesc()->description);
	}

	if( GET_OBJ_TYPE(item) == ITEM_NOTE && item->action_description )
	{
		target->send("There is something written on it:\r\n\r\n%s",
			item->action_description);
	}
	else if( GET_OBJ_TYPE(item) == ITEM_WEAPON )
		target->send("It is a %s.\r\n", StringUtil::allLower(weapon_types[GET_OBJ_VAL(item,0)]));
	else if( GET_OBJ_TYPE(item) == ITEM_CONTAINER )
	{
		target->send("When you look inside, you see: \r\n");
		lookInObject(target, 0, item);
	}
	else if( (IS_OBJ_STAT(item, ITEM_SWORD_SHEATH) || IS_OBJ_STAT(item, ITEM_DAGGER_SHEATH)
		|| IS_OBJ_STAT(item, ITEM_SPEAR_SHEATH)) && item->contains )
	{
		target->send("There is something sheath inside:\r\n\r\n");
		target->send("%s\r\n", item->contains->GetSDesc());
	}
}

bool Object::IsRemortGear()
{
	if ( GET_OBJ_CLAN(this) == CLAN_MYRDDRAAL || GET_OBJ_CLAN(this) == CLAN_BLADEMASTERS 
		|| GET_OBJ_CLAN(this) == CLAN_SOULLESS || GET_OBJ_CLAN(this) == CLAN_CHOSEN || GET_OBJ_CLAN(this) == CLAN_OGIER)
		return true;

	return false;
}

bool Object::IsValidChest()
{
	return (IS_OBJ_STAT(this,ITEM_CHEST) && this->InRoom());
}

bool remort_gear_message( Character *ch, Object *obj )
{
	if( !IS_FADE(ch) && GET_OBJ_CLAN(obj) == CLAN_MYRDDRAAL )
	{
		ch->send("You feel a powerful dark presence surround you, preventing you from putting it on!\r\n");
		return false;
	}

	if ( !IS_BLADEMASTER(ch) && GET_OBJ_CLAN(obj) == CLAN_BLADEMASTERS )
	{
		ch->send("You feel far too unworthy to wear the gear of a blademaster!\r\n");
		return false;
	}

	if ( !IS_OGIER(ch) && GET_OBJ_CLAN(obj) == CLAN_OGIER )
	{
		ch->send("An ogier's gear is too large for you to wear!\r\n");
		return false;
	}
	if ( !IS_GREYMAN(ch) && GET_OBJ_CLAN(obj) == CLAN_SOULLESS )
	{
		ch->send("Somehow, you can't seem to keep your eyes on this object long enough to put it on.\r\n");
		return false;
	}

	if ( !IS_DREADLORD(ch) && GET_OBJ_CLAN(obj) == CLAN_CHOSEN )
	{
		ch->send("The item begins to radiate heat, making you think it might be best not to wear it.\r\n");
		return false;
	}

	return true;
}

char *Object::getName()
{
	return (retool_name ? retool_name : name);
}

char *Object::GetDesc()
{
	return (retool_desc ? retool_desc : description);
}

char *Object::GetSDesc()
{
	return (retool_sdesc ? retool_sdesc : short_description);
}

struct extra_descr_data *Object::GetExDesc()
{
	//Galnor 2011/02/12 - Extra descriptions have the keyword attached to it. With retools,
	//We will almost certainly want the keywords to match the object's current aliases.
	//What we'll do is synchronize it here.

	if(retool_ex_desc && (!retool_ex_desc->keyword || strcmp(retool_ex_desc->keyword, getName())))
	{//The keyword doesn't match the alias. We'll sync it here.
		delete[] retool_ex_desc->keyword;//Clear the current one.
		retool_ex_desc->keyword = str_dup(getName());//Copy the new one over.
	}

	// Fogel - Checks if any retooling has been done, and if so, hides the extra description. Can be changed when we actually implement extra description retools
	return ( IsRetooled() ? retool_ex_desc : ex_description);
}

/* Galnor - 09/14/2009 - Provides entry to the auction interface */
ACMD( do_auction )
{
	Auction *a;

	if( !ch->desc || ch->IsPurged() )
		return;

	/* Check to ensure that the current room has a valid auction */
	if( (a = AuctionManager::GetManager().GetAuction( ch->in_room->auction_vnum )) == NULL )
	{
		ch->send(YouCantDoThatHere);
		return;
	}

	//Check to ensure that the prerequisites match.
	if( !a->raceIsAllowed( GET_RACE(ch) ) )
	{
		ch->send(YouCantDoThatHere);
		return;
	}

	AuctionManager::GetManager().SetupAuctionInterface( ch, a );
	Act( "$n begins browsing the auctions.", TRUE, ch, 0, 0, TO_ROOM );
}

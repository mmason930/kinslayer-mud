#include "../../conf.h"
#include "../../sysdep.h"


#include "../../structs.h"
#include "../../spells.h"
#include "../../utils.h"
#include "../../comm.h"
#include "../../interpreter.h"
#include "../../handler.h"
#include "../../db.h"
#include "../../clans.h"
#include "../../constants.h"
#include "../../shop.h"
#include "../../olc.h"
#include "../../screen.h"
#include "../../auction.h"
#include "../../StringUtil.h"
#include "../../MiscUtil.h"
#include "../../Descriptor.h"
#include "../../rooms/Room.h"
#include "../../ClanUtil.h"

#include "../../js.h"
#include "../../js_functions.h"

#include "../../commands/infrastructure/CommandUtil.h"
#include "../../commands/infrastructure/CommandInfo.h"

#include "../../dotmode/DotModeUtil.h"

void get_from_room( Character *ch, char *arg );
void get_from_container( Character *ch, Object *cont, char *arg, int mode );

CommandHandler  do_get  = DEFINE_COMMAND
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	std::string originalArg1;

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
		originalArg1 = arg1;
		cont_dotmode = find_all_dots( arg2 );

		if ( cont_dotmode == FIND_INDIV )
		{
			mode = generic_find( arg2, FIND_OBJ_INV | FIND_OBJ_ROOM | FIND_OBJ_EQUIP, ch, &tmp_char, &cont );

			if ( !cont )
			{
				ch->send( "You don't have %s %s.\r\n", AN( arg2 ), arg2 );
			}
			else if (  cont ->getType() != ITEM_CONTAINER )
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
					if (  cont ->getType() == ITEM_CONTAINER )
					{
						found = 1;
						get_from_container( ch, cont, arg1, FIND_OBJ_INV );
						snprintf(arg1, sizeof(arg1), "%s", originalArg1.c_str());
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
					if (  cont ->getType() == ITEM_CONTAINER )
					{
						get_from_container( ch, cont, arg1, FIND_OBJ_ROOM );
						snprintf(arg1, sizeof(arg1), "%s", originalArg1.c_str());
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
};

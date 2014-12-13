/* ************************************************************************
*   File: Act.social.c                                  Part of CircleMUD *
*  Usage: Functions to handle socials                                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "shop.h"
#include "MiscUtil.h"
#include "Descriptor.h"

#include "rooms/Room.h"

#include "commands/infrastructure/CommandUtil.h"
#include "commands/infrastructure/Social.h"

CommandHandler do_action = DEFINE_COMMAND
{
	int act_nr;
	Social *action;
	Character *vict;
	Object *targ;

	if( (action = CommandUtil::get()->getSocialByCommandIndex(cmd)) == nullptr )
	{
		ch->send("That action is not supported.\r\n", ch);
		return;
	}

	TwoArguments(argument, buf, buf2);

	if ((!action->char_body_found) && (*buf2))
	{
		ch->send("Sorry, this social does not support body parts.\r\n", ch);
		return;
	}

	if (!action->char_found)
		*buf = '\0';

	if (action->char_found && argument)
		OneArgument(argument, buf);

	else
		*buf = '\0';

	if (!*buf)
	{
		ch->send(action->char_no_arg);
		ch->send("\r\n");
		Act(action->others_no_arg, action->hide, ch, 0, 0, TO_ROOM);
		return;
	}

	if (!(vict = get_char_room_vis(ch, buf)))
	{
		if ((action->char_obj_found) &&
		        ((targ = get_obj_in_list_vis(ch, buf, ch->carrying)) ||
		         (targ = get_obj_in_list_vis(ch, buf, ch->in_room->contents))))
		{
			Act(action->char_obj_found, action->hide, ch, targ, 0, TO_CHAR);
			Act(action->others_obj_found, action->hide, ch, targ, 0, TO_ROOM);
			return;
		}

		if (action->not_found)
			ch->send(action->not_found);

		else
			ch->send("I don't see anything by that name here.");

		ch->send("\r\n");
		return;
	}

	else if (vict == ch)
	{

		if (action->char_auto)
			ch->send(action->char_auto);

		else
			ch->send("Erm, no.");

		ch->send("\r\n", ch);
		Act(action->others_auto, action->hide, ch, 0, 0, TO_ROOM);
	}

	else
	{
		if (GET_POS(vict) < action->min_victim_position)
			Act("$N is not in a proper position for that.",
			    FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);

		else
		{
			if (*buf2)
			{
				Act(action->char_body_found, 0, ch, (Object *)buf2, vict, TO_CHAR | TO_SLEEP);
				Act(action->others_body_found, action->hide, ch, (Object *)buf2, vict, TO_NOTVICT);
				Act(action->vict_body_found, action->hide, ch, (Object *)buf2, vict, TO_VICT);
			}

			else
			{
				Act(action->char_found, 0, ch, 0, vict, TO_CHAR | TO_SLEEP);
				Act(action->others_found, action->hide, ch, 0, vict, TO_NOTVICT);
				Act(action->vict_found, action->hide, ch, 0, vict, TO_VICT);
			}
		}
	}
};

CommandHandler do_insult = DEFINE_COMMAND
{
	Character *victim;
	OneArgument(argument, arg);

	if (*arg)
	{
		if (!(victim = get_char_room_vis(ch, arg)))
			ch->send("Can't hear you!\r\n");

		else
		{
			if (victim != ch)
			{
				ch->send("You insult %s.\r\n", GET_NAME(victim));

				switch (MiscUtil::random(0, 2))
				{

					case 0:
						if (GET_SEX(ch) == SEX_MALE)
						{
							if (GET_SEX(victim) == SEX_MALE)
								Act("$n accuses you of fighting like a woman!", FALSE, ch, 0, victim, TO_VICT);

							else
								Act("$n says that women can't fight.", FALSE, ch, 0, victim, TO_VICT);
						}

						else
						{		/* Ch == Woman */
							if (GET_SEX(victim) == SEX_MALE)
								Act("$n accuses you of having the smallest... (brain?)",
								    FALSE, ch, 0, victim, TO_VICT);

							else
								Act("$n tells you that you'd lose a beauty contest against a troll.",
								    FALSE, ch, 0, victim, TO_VICT);
						}

						break;

					case 1:
						Act("$n calls your mother a bitch!", FALSE, ch, 0, victim, TO_VICT);
						break;

					default:
						Act("$n tells you to get lost!", FALSE, ch, 0, victim, TO_VICT);
						break;
				}			/* end switch */


				Act("$n insults $N.", TRUE, ch, 0, victim, TO_NOTVICT);
			}

			else
			{			/* ch == victim */
				ch->send("You feel insulted.\r\n");
			}
		}
	}

	else
		ch->send("I'm sure you don't want to insult *everybody*...\r\n");
};


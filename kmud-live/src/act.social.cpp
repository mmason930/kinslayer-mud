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
#include "rooms/Room.h"

/* extern variables */

extern Descriptor *descriptor_list;
extern class CommandInfo cmd_info[];
extern Social *soc_mess_list;

/* extern functions */
char *fread_action(FILE * fl, int nr);

/* local globals */
int top_of_socialt = -1;

/* local functions */
int find_action(int cmd);
ACMD(do_action);
ACMD(do_insult);
void boot_social_messages(void);

const int NUM_RESERVED_CMDS = 15; 

int find_action(int cmd)
{
	int bot, top, mid;

	bot = 0;
	top = top_of_socialt;

	if (top < 0)
		return (-1);

	for (;;)
	{
		mid = (bot + top) / 2;

		if (soc_mess_list[mid].act_nr == cmd)
			return (mid);

		if (bot >= top)
			return (-1);

		if (soc_mess_list[mid].act_nr > cmd)
			top = --mid;

		else
			bot = ++mid;
	}
}

ACMD(do_action)
{
	int act_nr;
	Social *action;
	Character *vict;
	Object *targ;

	if ((act_nr = find_action(cmd)) < 0)
	{
		ch->send("That action is not supported.\r\n", ch);
		return;
	}

	action = &soc_mess_list[act_nr];

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
}

ACMD(do_insult)
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
}


char *fread_action(FILE * fl, int nr)
{
	char buf[MAX_STRING_LENGTH], *rslt;

	char *returnValue = fgets(buf, MAX_STRING_LENGTH, fl);

	if (feof(fl))
	{
		Log("SYSERR: fread_action: unexpected EOF near action #%d", nr);
		exit(1);
	}

	if (returnValue == NULL)
	{
		Log("SYSERR: fread_action: null pointer returned unexpectedly near action #%d.", nr);
		exit(1);
	}

	if (*buf == '#')
		return (NULL);

	else
	{
		*(buf + strlen(buf) - 1) = '\0';
		rslt = new char[strlen(buf) + 1];
		strcpy(rslt, buf);

		return (rslt);
	}
}

void Social::Extract()
{
	if ( command )
		delete [] command;
	if ( sort_as )
		delete [] sort_as;
	if ( char_no_arg )
		delete [] char_no_arg;
	if ( others_no_arg )
		delete [] others_no_arg;
	if ( char_found )
		delete [] char_found;
	if ( others_found )
		delete [] others_found;
	if ( vict_found )
		delete [] vict_found;
	if ( char_body_found )
		delete [] char_body_found;
	if ( others_body_found )
		delete [] others_body_found;
	if ( vict_body_found )
		delete [] vict_body_found;
	if ( not_found )
		delete [] not_found;
	if ( char_auto )
		delete [] char_auto;
	if ( others_auto )
		delete [] others_auto;
	if ( char_obj_found )
		delete [] char_obj_found;
	if ( others_obj_found )
		delete [] others_obj_found;
}
Social::Social()
{
	memset( this, 0, sizeof( Social ) );
}
Social::~Social()
{
	Extract();
}

void Social::Copy( Social* Source )
{
	command				= str_dup( Source->command );
	sort_as				= str_dup( Source->sort_as );
	char_no_arg			= str_dup( Source->char_no_arg );
	others_no_arg		= str_dup( Source->others_no_arg );
	char_found			= str_dup( Source->char_found );
	others_found		= str_dup( Source->others_found );
	vict_found			= str_dup( Source->vict_found );
	char_body_found		= str_dup( Source->char_body_found );
	others_body_found	= str_dup( Source->others_body_found );
	vict_body_found		= str_dup( Source->vict_body_found );
	not_found			= str_dup( Source->not_found );
	char_auto			= str_dup( Source->char_auto );
	others_auto			= str_dup( Source->others_auto );
	char_obj_found		= str_dup( Source->char_obj_found );
	others_obj_found	= str_dup( Source->others_obj_found );

	act_nr				= Source->act_nr;
	hide				= Source->hide;
	min_victim_position	= Source->min_victim_position;
	min_level_char		= Source->min_level_char;
	min_char_position	= Source->min_char_position;
}

Social::Social( Social* Source )
{
	Copy( Source );
}

void boot_social_messages(void)
{

	sql::Query query = gameDatabase->sendQuery("SELECT * FROM social ORDER BY id");

	top_of_socialt = query->numRows()-1;

	soc_mess_list = new Social[top_of_socialt + 1];

	int socialIndex = 0;

	while(query->hasNextRow()) {

		sql::Row socialRow = query->getRow();

		Social *social = &soc_mess_list[socialIndex];

		social->command = str_dup(socialRow.getString("command").c_str());

		social->sort_as = str_dup(socialRow.getString("sort").c_str());

		social->hide = socialRow.getInt("hide");

		social->min_victim_position = socialRow.getInt("minimum_victim_position");

		social->min_char_position = socialRow.getInt("minimum_char_position");

		social->min_level_char = socialRow.getInt("minimum_char_level");

		social->char_no_arg = str_dup(socialRow.getString("char_no_arg").c_str());

		social->others_no_arg = str_dup(socialRow.getString("others_no_arg").c_str());

		social->char_found = str_dup(socialRow.getString("char_found").c_str());

		social->others_found = str_dup(socialRow.getString("others_found").c_str());

		social->vict_found = str_dup(socialRow.getString("vict_found").c_str());

		social->char_body_found = str_dup(socialRow.getString("char_body_found").c_str());

		social->others_body_found = str_dup(socialRow.getString("others_body_found").c_str());

		social->vict_body_found = str_dup(socialRow.getString("vict_body_found").c_str());

		social->not_found = str_dup(socialRow.getString("not_found").c_str());

		social->char_auto = str_dup(socialRow.getString("char_auto").c_str());

		social->others_auto = str_dup(socialRow.getString("others_auto").c_str());

		social->char_obj_found = str_dup(socialRow.getString("char_obj_found").c_str());

		social->others_obj_found = str_dup(socialRow.getString("others_obj_found").c_str());

		++socialIndex;
	}
}

// this function adds in the loaded socials and assigns them a command #
void CreateCommandList(void)
{
	int i, j, k;
	Social temp;

	/* free up old command list */
	if (complete_cmd_info)
		delete[] (complete_cmd_info);

	complete_cmd_info = NULL;

	/* re check the sort on the socials */
	for (j = 0; j < top_of_socialt; ++j)
	{
		k = j;
		for (i = j + 1; i <= top_of_socialt;++i)
		{
			if (strcmp(soc_mess_list[i].sort_as, soc_mess_list[k].sort_as) < 0)
			{
				k = i;
				if (j != k)
				{
					temp = soc_mess_list[j];
					soc_mess_list[j] = soc_mess_list[k];
					soc_mess_list[k] = temp;
				}
			}
		}
	}

	/* count the commands in the command list */
	i = 0;
	while(cmd_info[i].command[0] != '\n')
		++i;

	++i;

	complete_cmd_info = new CommandInfo[top_of_socialt + i + 2];

	/* this loop sorts the socials and commands together into one big list */
	i = 0;
	j = 0;
	k = 0;
	while ((cmd_info[i].command[0] != '\n') || (j <= top_of_socialt))
	{
		if ((i < NUM_RESERVED_CMDS) || (j > top_of_socialt) ||
		        (strcmp(cmd_info[i].sort_as, soc_mess_list[j].sort_as) < 1))
		{
			complete_cmd_info[k++] = cmd_info[i++];
		}
		else
		{
			soc_mess_list[j].act_nr					= k;
			complete_cmd_info[k].command			= soc_mess_list[j].command;
			complete_cmd_info[k].sort_as			= soc_mess_list[j].sort_as;
			complete_cmd_info[k].minimum_position	= soc_mess_list[j].min_char_position;
			complete_cmd_info[k].command_pointer	= do_action;
			complete_cmd_info[k].minimum_level    	= soc_mess_list[j++].min_level_char;
			complete_cmd_info[k].timer				= 0.00f;
			complete_cmd_info[k].view_minimum_level	= 0;
			complete_cmd_info[k++].subcmd			= 0;
		}
	}
	complete_cmd_info[k].command			= "\n";
	complete_cmd_info[k].sort_as			= "zzzzzzz";
	complete_cmd_info[k].minimum_position	= 0;
	complete_cmd_info[k].command_pointer	= 0;
	complete_cmd_info[k].minimum_level		= 0;
	complete_cmd_info[k].subcmd				= 0;
	complete_cmd_info[k].view_minimum_level	= 0;
	complete_cmd_info[k].timer				= 0.00f;

	Log( "Command info rebuilt, %d total commands.", k );
	memset( &temp, 0, sizeof( Social ) );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File: aedit.c                                                   *
 * Comment: OLC for MUDs -- this one edits socials                 *
 * by Michael Scott <scottm@workcomm.net> -- 06/10/96              *
 * for use with OasisOLC                                           *
 * ftpable from ftp.circlemud.org:/pub/CircleMUD/contrib/code      *
 * Part of OLC+                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "interpreter.h"
#include "handler.h"
#include "comm.h"
#include "utils.h"
#include "db.h"
#include "olc.h"
#include "screen.h"
#include "shop.h"
#include "Descriptor.h"


extern int top_of_socialt;
Social *soc_mess_list = 0;
extern const char	*position_types[];

/* WARNING: if you have added diagonal directions and have them at the
 * beginning of the command list.. change this value to 11 or 15 (depending) */
/* reserve these commands to come straight from the cmd list then start
 * sorting */
const int RESERVE_CMDS = 7; 

/* function protos */
void aedit_disp_menu(Descriptor * d);
void aedit_parse(Descriptor * d, char *arg);
void aedit_setup_new(Descriptor *d);
void aedit_setup_existing(Descriptor *d, int real_num);
void aedit_save_to_disk();
void aedit_save_internally(Descriptor *d);



/*
 * Utils and exported functions.
 */

void aedit_setup_new(Descriptor *d)
{
	OLC_ACTION(d) = new Social;
	OLC_ACTION(d)->command = str_dup(OLC_STORAGE(d));
	OLC_ACTION(d)->sort_as = str_dup(OLC_STORAGE(d));
	OLC_ACTION(d)->hide    = 0;
	OLC_ACTION(d)->min_victim_position = POS_STANDING;
	OLC_ACTION(d)->min_char_position   = POS_STANDING;
	OLC_ACTION(d)->min_level_char      = 0;
	OLC_ACTION(d)->char_no_arg = str_dup("This action is unfinished.");
	OLC_ACTION(d)->others_no_arg = str_dup("This action is unfinished.");
	OLC_ACTION(d)->char_found = NULL;
	OLC_ACTION(d)->others_found = NULL;
	OLC_ACTION(d)->vict_found = NULL;
	OLC_ACTION(d)->not_found = NULL;
	OLC_ACTION(d)->char_auto = NULL;
	OLC_ACTION(d)->others_auto = NULL;
	OLC_ACTION(d)->char_body_found = NULL;
	OLC_ACTION(d)->others_body_found = NULL;
	OLC_ACTION(d)->vict_body_found = NULL;
	OLC_ACTION(d)->char_obj_found = NULL;
	OLC_ACTION(d)->others_obj_found = NULL;
	aedit_disp_menu(d);
	OLC_VAL(d) = 0;
}

/*------------------------------------------------------------------------*/

void aedit_setup_existing(Descriptor *d, int real_num)
{
	d->olc->action = new Social( &soc_mess_list[real_num] );
	OLC_VAL(d) = 0;
	aedit_disp_menu(d);
}



void aedit_save_internally(Descriptor *d)
{
	Social *new_soc_mess_list = NULL;
	int i;

	/* add a new social into the list */
	if (OLC_ZNUM(d) > top_of_socialt)
	{
		new_soc_mess_list = new Social[top_of_socialt + 2];
		for (i = 0; i <= top_of_socialt; ++i)
			new_soc_mess_list[i].Copy( &soc_mess_list[i] );
		new_soc_mess_list[++top_of_socialt].Copy( d->olc->action );
		delete[] (soc_mess_list);

		soc_mess_list = new_soc_mess_list;
		CreateCommandList();
		SortCommands();
	}
	/* pass the editted action back to the list - no need to add */
	else
	{
		i = FindCommand(OLC_ACTION(d)->command);
		OLC_ACTION(d)->act_nr = soc_mess_list[OLC_ZNUM(d)].act_nr;
		/* why did i do this..? hrm */
		soc_mess_list[d->olc->zone_num].Extract();
		soc_mess_list[d->olc->zone_num].Copy( d->olc->action );

		if (i > NOTHING)
		{
			complete_cmd_info[i].command = soc_mess_list[OLC_ZNUM(d)].command;
			complete_cmd_info[i].sort_as = soc_mess_list[OLC_ZNUM(d)].sort_as;
			complete_cmd_info[i].minimum_position = soc_mess_list[OLC_ZNUM(d)].min_char_position;
			complete_cmd_info[i].minimum_level	   = soc_mess_list[OLC_ZNUM(d)].min_level_char;
		}
	}
//	olc_add_to_save_list(AEDIT_PERMISSION, OLC_SAVE_ACTION);
}


/*------------------------------------------------------------------------*/

void aedit_save_to_disk()
{
	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempSocial");

	gameDatabase->sendRawQuery("CREATE TABLE tempSocial LIKE social");

	sql::BatchInsertStatement batchInsertStatement(gameDatabase, "tempSocial", 100000);

	batchInsertStatement.addField("command");

	batchInsertStatement.addField("sort");

	batchInsertStatement.addField("hide");

	batchInsertStatement.addField("minimum_victim_position");

	batchInsertStatement.addField("minimum_char_position");

	batchInsertStatement.addField("minimum_char_level");

	batchInsertStatement.addField("char_no_arg");

	batchInsertStatement.addField("others_no_arg");

	batchInsertStatement.addField("char_found");

	batchInsertStatement.addField("others_found");

	batchInsertStatement.addField("vict_found");

	batchInsertStatement.addField("char_body_found");

	batchInsertStatement.addField("others_body_found");

	batchInsertStatement.addField("vict_body_found");

	batchInsertStatement.addField("not_found");

	batchInsertStatement.addField("char_auto");

	batchInsertStatement.addField("others_auto");

	batchInsertStatement.addField("char_obj_found");

	batchInsertStatement.addField("others_obj_found");

	batchInsertStatement.start();

	for(int i = 0;i < top_of_socialt;++i)
	{
		Social *social = &soc_mess_list[i];

		batchInsertStatement.beginEntry();

		batchInsertStatement.putString(social->command);

		batchInsertStatement.putString(social->sort_as);

		batchInsertStatement.putInt(social->hide);

		batchInsertStatement.putInt(social->min_victim_position);

		batchInsertStatement.putInt(social->min_char_position);

		batchInsertStatement.putInt(social->min_level_char);

		batchInsertStatement.putString(social->char_no_arg);

		batchInsertStatement.putString(social->others_no_arg);

		batchInsertStatement.putString(social->char_found);

		batchInsertStatement.putString(social->others_found);

		batchInsertStatement.putString(social->vict_found);

		batchInsertStatement.putString(social->char_body_found);

		batchInsertStatement.putString(social->others_body_found);

		batchInsertStatement.putString(social->vict_body_found);

		batchInsertStatement.putString(social->not_found);

		batchInsertStatement.putString(social->char_auto);

		batchInsertStatement.putString(social->others_auto);

		batchInsertStatement.putString(social->char_obj_found);

		batchInsertStatement.putString(social->others_obj_found);

		batchInsertStatement.endEntry();
	}

	batchInsertStatement.finish();

	gameDatabase->sendRawQuery("RENAME TABLE social TO socialOld, tempSocial TO social, socialOld TO tempSocial");

	gameDatabase->sendRawQuery("DROP TABLE tempSocial");
}

/*------------------------------------------------------------------------*/

/* Menu functions */

/* the main menu */
void aedit_disp_menu(Descriptor * d)
{
	Social *action = OLC_ACTION(d);
	Character *ch = d->character;

	get_char_cols(ch);

	sprintf(buf, "\x1B[H\x1B[J"
	        "%s-- Action editor\r\n\r\n"
	        "%sn%s) Command         : %s%-15.15s%s %s1%s) Sort as Command  : %s%-15.15s%s\r\n"
	        "%s2%s) Min Position[CH]: %s%-8.8s        %s3%s) Min Position [VT]: %s%-8.8s\r\n"
	        "%s4%s) Min Level   [CH]: %s%-3d             %s5%s) Show if Invisible: %s%s\r\n"
	        "%sa%s) Char    [NO ARG]: %s%s\r\n"
	        "%sb%s) Others  [NO ARG]: %s%s\r\n"
	        "%sc%s) Char [NOT FOUND]: %s%s\r\n"
	        "%sd%s) Char  [ARG SELF]: %s%s\r\n"
	        "%se%s) Others[ARG SELF]: %s%s\r\n"
	        "%sf%s) Char      [VICT]: %s%s\r\n"
	        "%sg%s) Others    [VICT]: %s%s\r\n"
	        "%sh%s) Victim    [VICT]: %s%s\r\n"
	        "%si%s) Char  [BODY PRT]: %s%s\r\n"
	        "%sj%s) Others[BODY PRT]: %s%s\r\n"
	        "%sk%s) Victim[BODY PRT]: %s%s\r\n"
	        "%sl%s) Char       [OBJ]: %s%s\r\n"
	        "%sm%s) Others     [OBJ]: %s%s\r\n"
	        "%sq%s) Quit\r\n",
	        nrm, grn, nrm,
	        yel, action->command, nrm,
	        grn, nrm,
	        yel, action->sort_as, nrm,
	        grn, nrm,
	        cyn, position_types[action->min_char_position],
	        grn, nrm,
	        cyn, position_types[action->min_victim_position],
	        grn, nrm,
	        cyn, action->min_level_char,
	        grn, nrm,
	        cyn, (action->hide?"HIDDEN":"NOT HIDDEN"),
	        grn, nrm, cyn,
	        action->char_no_arg ? action->char_no_arg : "<Null>",
	        grn, nrm, cyn,
	        action->others_no_arg ? action->others_no_arg : "<Null>",
	        grn, nrm, cyn,
	        action->not_found ? action->not_found : "<Null>",
	        grn, nrm, cyn,
	        action->char_auto ? action->char_auto : "<Null>",
	        grn, nrm, cyn,
	        action->others_auto ? action->others_auto : "<Null>",
	        grn, nrm, cyn,
	        action->char_found ? action->char_found : "<Null>",
	        grn, nrm, cyn,
	        action->others_found ? action->others_found : "<Null>",
	        grn, nrm, cyn,
	        action->vict_found ? action->vict_found : "<Null>",
	        grn, nrm, cyn,
	        action->char_body_found ? action->char_body_found : "<Null>",
	        grn, nrm, cyn,
	        action->others_body_found ? action->others_body_found : "<Null>",
	        grn, nrm, cyn,
	        action->vict_body_found ? action->vict_body_found : "<Null>",
	        grn, nrm, cyn,
	        action->char_obj_found ? action->char_obj_found : "<Null>",
	        grn, nrm, cyn,
	        action->others_obj_found ? action->others_obj_found : "<Null>",
	        grn, nrm);

	strcat(buf,"\r\n");
	strcat(buf, "Enter choice: ");

	d->Send(buf);
	OLC_MODE(d) = AEDIT_MAIN_MENU;
}


/*
 * The main loop
 */

void aedit_parse(Descriptor * d, char *arg)
{
	int i;

	switch (OLC_MODE(d))
	{
		case AEDIT_CONFIRM_SAVESTRING:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					aedit_save_internally(d);
					aedit_save_to_disk();
					MudLog(CMP, LVL_IMPL, TRUE, "OLC: %s edits action %s", GET_NAME(d->character), OLC_ACTION(d)->command);
					/* do not free the strings.. just the structure */
					cleanup_olc(d, CLEANUP_STRUCTS);
					d->Send("Action saved to memory.\r\n");
					break;
				case 'n':
				case 'N':
					/* free everything up, including strings etc */
					cleanup_olc(d, CLEANUP_ALL);
					break;
				default:
					d->Send("Invalid choice!\r\nDo you wish to save this action internally? ");
					break;
			}
			return; /* end of AEDIT_CONFIRM_SAVESTRING */

		case AEDIT_CONFIRM_EDIT:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					aedit_setup_existing(d, OLC_ZNUM(d));
					break;
				case 'q':
				case 'Q':
					cleanup_olc(d, CLEANUP_ALL);
					break;
				case 'n':
				case 'N':
					OLC_ZNUM(d)++;
					for (;(OLC_ZNUM(d) <= top_of_socialt);++OLC_ZNUM(d))
					{
						if (IsAbbrev(OLC_STORAGE(d), soc_mess_list[OLC_ZNUM(d)].command))
							break;
					}
					if (OLC_ZNUM(d) > top_of_socialt)
					{
						if (FindCommand(OLC_STORAGE(d)) > NOTHING)
						{
							cleanup_olc(d, CLEANUP_ALL);
							break;
						}
						d->Send("Do you wish to add the '%s' action? ", OLC_STORAGE(d));
						OLC_MODE(d) = AEDIT_CONFIRM_ADD;
					}
					else
					{
						d->Send("Do you wish to edit the '%s' action? ", soc_mess_list[OLC_ZNUM(d)].command);
						OLC_MODE(d) = AEDIT_CONFIRM_EDIT;
					}
					break;
				default:
					d->Send("Invalid choice!\r\nDo you wish to edit the '%s' action? ", soc_mess_list[OLC_ZNUM(d)].command);
					break;
			}
			return;
		case AEDIT_CONFIRM_ADD:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					aedit_setup_new(d);
					break;
				case 'n':
				case 'N':
				case 'q':
				case 'Q':
					cleanup_olc(d, CLEANUP_ALL);
					break;
				default:
					d->Send("Invalid choice!\r\nDo you wish to add the '%s' action? ", OLC_STORAGE(d));
					break;
			}
			return;
		case AEDIT_MAIN_MENU:
			switch (*arg)
			{
				case 'q':
				case 'Q':
					if (OLC_VAL(d))
					{ /* Something was modified */
						d->Send("Do you wish to save this action internally? ");
						OLC_MODE(d) = AEDIT_CONFIRM_SAVESTRING;
					}
					else
						cleanup_olc(d, CLEANUP_ALL);
					break;
				case 'n':
					d->Send("Enter action name: ");
					OLC_MODE(d) = AEDIT_ACTION_NAME;
					return;
				case '1':
					d->Send("Enter sort info for this action (for the command listing): ");
					OLC_MODE(d) = AEDIT_SORT_AS;
					return;
				case '2':
					d->Send("Enter the minimum position the Character has to be in to Activate social [0 - 8]: ");
					OLC_MODE(d) = AEDIT_MIN_CHAR_POS;
					return;
				case '3':
					d->Send("Enter the minimum position the Victim has to be in to Activate social [0 - 8]: ");
					OLC_MODE(d) = AEDIT_MIN_VICT_POS;
					return;
				case '4':
					d->Send("Enter new minimum level for social: ");
					OLC_MODE(d) = AEDIT_MIN_CHAR_LEVEL;
					return;
				case '5':
					OLC_ACTION(d)->hide = !OLC_ACTION(d)->hide;
					aedit_disp_menu(d);
					OLC_VAL(d) = 1;
					break;
				case 'a':
				case 'A':
					d->Send("Enter social shown to the Character when there is no argument supplied.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_no_arg)?OLC_ACTION(d)->char_no_arg:"NULL"));
					OLC_MODE(d) = AEDIT_NOVICT_CHAR;
					return;
				case 'b':
				case 'B':
					d->Send("Enter social shown to Others when there is no argument supplied.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_no_arg)?OLC_ACTION(d)->others_no_arg:"NULL"));
					OLC_MODE(d) = AEDIT_NOVICT_OTHERS;
					return;
				case 'c':
				case 'C':
					d->Send("Enter text shown to the Character when his victim isnt found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->not_found)?OLC_ACTION(d)->not_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_NOT_FOUND;
					return;
				case 'd':
				case 'D':
					d->Send("Enter social shown to the Character when it is its own victim.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_auto)?OLC_ACTION(d)->char_auto:"NULL"));
					OLC_MODE(d) = AEDIT_SELF_CHAR;
					return;
				case 'e':
				case 'E':
					d->Send("Enter social shown to Others when the Char is its own victim.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_auto)?OLC_ACTION(d)->others_auto:"NULL"));
					OLC_MODE(d) = AEDIT_SELF_OTHERS;
					return;
				case 'f':
				case 'F':
					d->Send("Enter normal social shown to the Character when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_found)?OLC_ACTION(d)->char_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_CHAR_FOUND;
					return;
				case 'g':
				case 'G':
					d->Send("Enter normal social shown to Others when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_found)?OLC_ACTION(d)->others_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_OTHERS_FOUND;
					return;
				case 'h':
				case 'H':
					d->Send("Enter normal social shown to the Victim when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->vict_found)?OLC_ACTION(d)->vict_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_VICT_FOUND;
					return;
				case 'i':
				case 'I':
					d->Send("Enter 'body part' social shown to the Character when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_body_found)?OLC_ACTION(d)->char_body_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_CHAR_BODY_FOUND;
					return;
				case 'j':
				case 'J':
					d->Send("Enter 'body part' social shown to Others when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_body_found)?OLC_ACTION(d)->others_body_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_OTHERS_BODY_FOUND;
					return;
				case 'k':
				case 'K':
					d->Send("Enter 'body part' social shown to the Victim when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->vict_body_found)?OLC_ACTION(d)->vict_body_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_VICT_BODY_FOUND;
					return;
				case 'l':
				case 'L':
					d->Send("Enter 'object' social shown to the Character when the object is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_obj_found)?OLC_ACTION(d)->char_obj_found:"NULL"));
					OLC_MODE(d) = AEDIT_OBJ_CHAR_FOUND;
					return;
				case 'm':
				case 'M':
					d->Send("Enter 'object' social shown to the Room when the object is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_obj_found)?OLC_ACTION(d)->others_obj_found:"NULL"));
					OLC_MODE(d) = AEDIT_OBJ_OTHERS_FOUND;
					return;
				default:
					aedit_disp_menu(d);
					break;
			}
			return;
		case AEDIT_ACTION_NAME:
			if (*arg)
			{
				if (strchr(arg,' '))
				{
					aedit_disp_menu(d);
					return;
				}
				else
				{
					if (OLC_ACTION(d)->command)
						delete[] (OLC_ACTION(d)->command);
					OLC_ACTION(d)->command = str_dup(arg);
				}
			}
			else
			{
				aedit_disp_menu(d);
				return;
			}
			break;
		case AEDIT_SORT_AS:
			if (*arg)
			{
				if (strchr(arg,' '))
				{
					aedit_disp_menu(d);
					return;
				}
				else
				{
					if (OLC_ACTION(d)->sort_as)
						delete[] (OLC_ACTION(d)->sort_as);
					OLC_ACTION(d)->sort_as = str_dup(arg);
				}
			}
			else
			{
				aedit_disp_menu(d);
				return;
			}
			break;

		case AEDIT_MIN_CHAR_POS:
		case AEDIT_MIN_VICT_POS:
			if (*arg)
			{
				i = atoi(arg);
				if ((i < 0) && (i > POS_STANDING))
				{
					aedit_disp_menu(d);
					return;
				}
				else
				{
					if (OLC_MODE(d) == AEDIT_MIN_CHAR_POS)
						OLC_ACTION(d)->min_char_position = i;
					else
						OLC_ACTION(d)->min_victim_position = i;
				}
			}
			else
			{
				aedit_disp_menu(d);
				return;
			}
			break;
		case AEDIT_MIN_CHAR_LEVEL:
			if (*arg)
			{
				i = atoi(arg);
				if ((i < 0) && (i > LVL_IMPL))
				{
					aedit_disp_menu(d);
					return;
				}
				else
					OLC_ACTION(d)->min_level_char = i;
			}
			else
			{
				aedit_disp_menu(d);
				return;
			}
			break;
		case AEDIT_NOVICT_CHAR:
			if (OLC_ACTION(d)->char_no_arg)
				delete[] (OLC_ACTION(d)->char_no_arg);
			if(*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->char_no_arg = str_dup(arg);
			}
			else
				OLC_ACTION(d)->char_no_arg = NULL;
			break;
		case AEDIT_NOVICT_OTHERS:
			if (OLC_ACTION(d)->others_no_arg)
				delete[] (OLC_ACTION(d)->others_no_arg);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->others_no_arg = str_dup(arg);
			}
			else
				OLC_ACTION(d)->others_no_arg = NULL;
			break;
		case AEDIT_VICT_CHAR_FOUND:
			if (OLC_ACTION(d)->char_found)
				delete[] (OLC_ACTION(d)->char_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->char_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->char_found = NULL;
			break;
		case AEDIT_VICT_OTHERS_FOUND:
			if (OLC_ACTION(d)->others_found)
				delete[] (OLC_ACTION(d)->others_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->others_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->others_found = NULL;
			break;
		case AEDIT_VICT_VICT_FOUND:
			if (OLC_ACTION(d)->vict_found)
				delete[] (OLC_ACTION(d)->vict_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->vict_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->vict_found = NULL;
			break;
		case AEDIT_VICT_NOT_FOUND:
			if (OLC_ACTION(d)->not_found)
				delete[] (OLC_ACTION(d)->not_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->not_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->not_found = NULL;
			break;
		case AEDIT_SELF_CHAR:
			if (OLC_ACTION(d)->char_auto)
				delete[] (OLC_ACTION(d)->char_auto);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->char_auto = str_dup(arg);
			}
			else
				OLC_ACTION(d)->char_auto = NULL;
			break;
		case AEDIT_SELF_OTHERS:
			if (OLC_ACTION(d)->others_auto)
				delete[] (OLC_ACTION(d)->others_auto);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->others_auto = str_dup(arg);
			}
			else
				OLC_ACTION(d)->others_auto = NULL;
			break;
		case AEDIT_VICT_CHAR_BODY_FOUND:
			if (OLC_ACTION(d)->char_body_found)
				delete[] (OLC_ACTION(d)->char_body_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->char_body_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->char_body_found = NULL;
			break;
		case AEDIT_VICT_OTHERS_BODY_FOUND:
			if (OLC_ACTION(d)->others_body_found)
				delete[] (OLC_ACTION(d)->others_body_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->others_body_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->others_body_found = NULL;
			break;
		case AEDIT_VICT_VICT_BODY_FOUND:
			if (OLC_ACTION(d)->vict_body_found)
				delete[] (OLC_ACTION(d)->vict_body_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->vict_body_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->vict_body_found = NULL;
			break;
		case AEDIT_OBJ_CHAR_FOUND:
			if (OLC_ACTION(d)->char_obj_found)
				delete[] (OLC_ACTION(d)->char_obj_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->char_obj_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->char_obj_found = NULL;
			break;
		case AEDIT_OBJ_OTHERS_FOUND:
			if (OLC_ACTION(d)->others_obj_found)
				delete[] (OLC_ACTION(d)->others_obj_found);
			if (*arg)
			{
				delete_doubledollar(arg);
				OLC_ACTION(d)->others_obj_found = str_dup(arg);
			}
			else
				OLC_ACTION(d)->others_obj_found = NULL;
			break;
		default:
			/* we should never get here */
			break;
	}
	OLC_VAL(d) = 1;
	aedit_disp_menu(d);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File: aedit.c                                                   *
 * Comment: OLC for MUDs -- this one edits socials                 *
 * by Michael Scott <scottm@workcomm.net> -- 06/10/96              *
 * for use with OasisOLC                                           *
 * ftpable from ftp.circlemud.org:/pub/CircleMUD/contrib/code      *
 * Part of OLC+                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "../conf.h"
#include "../sysdep.h"

#include "../structs.h"
#include "../interpreter.h"
#include "../handler.h"
#include "../comm.h"
#include "../utils.h"
#include "../db.h"
#include "olc.h"
#include "../screen.h"
#include "../shop.h"
#include "../Descriptor.h"
#include "../Game.h"

#include "../commands/infrastructure/CommandUtil.h"
#include "../commands/infrastructure/CommandInfo.h"
#include "../commands/infrastructure/Social.h"

extern const char	*position_types[];

/* function protos */
void aedit_disp_menu(Descriptor * d);
void aedit_parse(Descriptor * d, char *arg);
void aedit_setup_new(Descriptor *d);
void aedit_setup_existing(Descriptor *d, const char *socialAbbreviation);
void aedit_save_internally(Descriptor *d);

void aedit_setup_new(Descriptor *d)
{
	OLC_ACTION(d) = new Social();
	OLC_ACTION(d)->command = str_dup(OLC_STORAGE(d));
	OLC_ACTION(d)->sort_as = str_dup(OLC_STORAGE(d));
	OLC_ACTION(d)->hide    = 0;
	OLC_ACTION(d)->min_victim_position = POS_STANDING;
	OLC_ACTION(d)->min_char_position   = POS_STANDING;
	OLC_ACTION(d)->min_level_char      = 0;
	OLC_ACTION(d)->char_no_arg = str_dup("This action is unfinished.");
	OLC_ACTION(d)->others_no_arg = str_dup("This action is unfinished.");
	OLC_ACTION(d)->act_nr = -1;
	aedit_disp_menu(d);
	OLC_VAL(d) = 0;
}

void aedit_setup_existing(Descriptor *d, const char *socialAbbreviation)
{
	d->olc->action = CommandUtil::get()->getSocialByAbbreviation(socialAbbreviation)->clone();
	OLC_VAL(d) = 0;
	aedit_disp_menu(d);
}

void aedit_save_internally(Descriptor *d)
{
	if(OLC_ACTION(d)->act_nr == -1) //New social
		CommandUtil::get()->addSocial(d->olc->action->clone());
	else //Existing social
		CommandUtil::get()->getSocialByCommandIndex(d->olc->action->act_nr)->Copy(d->olc->action);
	
	CommandUtil::get()->createCommandList();
	CommandUtil::get()->sortCommands();
}

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

	d->send(buf);
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
					CommandUtil::get()->saveSocials(game->getConnection());
					MudLog(CMP, LVL_IMPL, TRUE, "OLC: %s edits action %s", GET_NAME(d->character), OLC_ACTION(d)->command);
					/* do not free the strings.. just the structure */
					cleanup_olc(d, CLEANUP_STRUCTS);
					d->send("Action saved to memory.\r\n");
					break;
				case 'n':
				case 'N':
					/* free everything up, including strings etc */
					cleanup_olc(d, CLEANUP_ALL);
					break;
				default:
					d->send("Invalid choice!\r\nDo you wish to save this action internally? ");
					break;
			}
			return; /* end of AEDIT_CONFIRM_SAVESTRING */

		case AEDIT_CONFIRM_EDIT:
			switch (*arg)
			{
				case 'y':
				case 'Y':
					aedit_setup_existing(d, OLC_STORAGE(d));
					break;
				case 'q':
				case 'Q':
					cleanup_olc(d, CLEANUP_ALL);
					break;
				case 'n':
				case 'N':
				{
					Social *social = CommandUtil::get()->getSocialByAbbreviation(OLC_STORAGE(d));

					Log("Social found. Command `%s`", social->command);
					if (social == nullptr)
					{
						if (CommandUtil::get()->getCommandByName(OLC_STORAGE(d)))
						{
							d->send("That command already exists.\r\n");
							cleanup_olc(d, CLEANUP_ALL);
							break;
						}
						d->send("Do you wish to add the '%s' action? ", OLC_STORAGE(d));
						OLC_MODE(d) = AEDIT_CONFIRM_ADD;
					}
					else
					{
						d->send("Do you wish to edit the '%s' action? ", social->command);
						OLC_MODE(d) = AEDIT_CONFIRM_EDIT;
					}
					break;
				}
				default:
				{
					Social *social = CommandUtil::get()->getSocialByAbbreviation(OLC_STORAGE(d));
					d->send("Invalid choice!\r\nDo you wish to edit the '%s' action? ", social->command);
					break;
				}
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
					d->send("Invalid choice!\r\nDo you wish to add the '%s' action? ", OLC_STORAGE(d));
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
						d->send("Do you wish to save this action internally? ");
						OLC_MODE(d) = AEDIT_CONFIRM_SAVESTRING;
					}
					else
						cleanup_olc(d, CLEANUP_ALL);
					break;
				case 'n':
					d->send("Enter action name: ");
					OLC_MODE(d) = AEDIT_ACTION_NAME;
					return;
				case '1':
					d->send("Enter sort info for this action (for the command listing): ");
					OLC_MODE(d) = AEDIT_SORT_AS;
					return;
				case '2':
					d->send("Enter the minimum position the Character has to be in to Activate social [0 - 8]: ");
					OLC_MODE(d) = AEDIT_MIN_CHAR_POS;
					return;
				case '3':
					d->send("Enter the minimum position the Victim has to be in to Activate social [0 - 8]: ");
					OLC_MODE(d) = AEDIT_MIN_VICT_POS;
					return;
				case '4':
					d->send("Enter new minimum level for social: ");
					OLC_MODE(d) = AEDIT_MIN_CHAR_LEVEL;
					return;
				case '5':
					OLC_ACTION(d)->hide = !OLC_ACTION(d)->hide;
					aedit_disp_menu(d);
					OLC_VAL(d) = 1;
					break;
				case 'a':
				case 'A':
					d->send("Enter social shown to the Character when there is no argument supplied.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_no_arg)?OLC_ACTION(d)->char_no_arg:"NULL"));
					OLC_MODE(d) = AEDIT_NOVICT_CHAR;
					return;
				case 'b':
				case 'B':
					d->send("Enter social shown to Others when there is no argument supplied.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_no_arg)?OLC_ACTION(d)->others_no_arg:"NULL"));
					OLC_MODE(d) = AEDIT_NOVICT_OTHERS;
					return;
				case 'c':
				case 'C':
					d->send("Enter text shown to the Character when his victim isnt found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->not_found)?OLC_ACTION(d)->not_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_NOT_FOUND;
					return;
				case 'd':
				case 'D':
					d->send("Enter social shown to the Character when it is its own victim.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_auto)?OLC_ACTION(d)->char_auto:"NULL"));
					OLC_MODE(d) = AEDIT_SELF_CHAR;
					return;
				case 'e':
				case 'E':
					d->send("Enter social shown to Others when the Char is its own victim.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_auto)?OLC_ACTION(d)->others_auto:"NULL"));
					OLC_MODE(d) = AEDIT_SELF_OTHERS;
					return;
				case 'f':
				case 'F':
					d->send("Enter normal social shown to the Character when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_found)?OLC_ACTION(d)->char_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_CHAR_FOUND;
					return;
				case 'g':
				case 'G':
					d->send("Enter normal social shown to Others when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_found)?OLC_ACTION(d)->others_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_OTHERS_FOUND;
					return;
				case 'h':
				case 'H':
					d->send("Enter normal social shown to the Victim when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->vict_found)?OLC_ACTION(d)->vict_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_VICT_FOUND;
					return;
				case 'i':
				case 'I':
					d->send("Enter 'body part' social shown to the Character when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_body_found)?OLC_ACTION(d)->char_body_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_CHAR_BODY_FOUND;
					return;
				case 'j':
				case 'J':
					d->send("Enter 'body part' social shown to Others when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->others_body_found)?OLC_ACTION(d)->others_body_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_OTHERS_BODY_FOUND;
					return;
				case 'k':
				case 'K':
					d->send("Enter 'body part' social shown to the Victim when the victim is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->vict_body_found)?OLC_ACTION(d)->vict_body_found:"NULL"));
					OLC_MODE(d) = AEDIT_VICT_VICT_BODY_FOUND;
					return;
				case 'l':
				case 'L':
					d->send("Enter 'object' social shown to the Character when the object is found.\r\n[OLD]: %s\r\n[NEW]: ",
					        ((OLC_ACTION(d)->char_obj_found)?OLC_ACTION(d)->char_obj_found:"NULL"));
					OLC_MODE(d) = AEDIT_OBJ_CHAR_FOUND;
					return;
				case 'm':
				case 'M':
					d->send("Enter 'object' social shown to the Room when the object is found.\r\n[OLD]: %s\r\n[NEW]: ",
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

/**************************************************************************
*   File: modify.c                                      Part of CircleMUD *
*  Usage: Run-time modification of game variables                         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
**************************************************************************/

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "comm.h"
#include "olc.h"
#include "shop.h"
#include "weaves.h"
#include "js.h"
#include "js_trigger.h"

#include "StringUtil.h"
#include "Descriptor.h"

void show_string(Descriptor *d, char *input);

const int PARSE_FORMAT		= 0;
const int PARSE_REPLACE		= 1;
const int PARSE_HELP		= 2;
const int PARSE_DELETE		= 3;
const int PARSE_INSERT		= 4;
const int PARSE_LIST_NORM	= 5;
const int PARSE_LIST_NUM	= 6;
const int PARSE_EDIT		= 7;

/* local functions */
void string_add(Descriptor *d, char *str);
ACMD(do_skillset);
char *next_page(char *str);
int count_pages(char *str);
void paginate_string(char *str, Descriptor *d);

const char *string_fields[] =
    {
        "name",
        "short",
        "long",
        "description",
        "title",
        "delete-description",
        "\n"
    };

/*
 * Maximum length for text field x+1
 */
int length[] =
{
	15,
	60,
	256,
	240,
	60
};

/*************************************
 * Modification of malloc'ed strings.*
 *************************************/

/*
 * Handle some editor commands.
 */
void parse_action(int command, char *str, Descriptor *d)
{
	int indent = 0, rep_all = 0, flags = 0, total_len, replaced;
	register int j = 0;
	int i, line_low, line_high;
	char *s, *t, temp;

	switch (command)
	{
		case PARSE_HELP:
			sprintf(buf,
			        "Editor command formats: /<letter>\r\n\r\n"
			        "/a         -  aborts editor\r\n"
			        "/c         -  clears buffer\r\n"
			        "/d#        -  deletes a line #\r\n"
			        "/e# <text> -  changes the line at # with <text>\r\n"
			        "/f         -  formats text\r\n"
			        "/fi        -  indented formatting of text\r\n"
			        "/h         -  list text editor commands\r\n"
			        "/i# <text> -  inserts <text> before line #\r\n"
			        "/l         -  lists buffer\r\n"
			        "/n         -  lists buffer with line numbers\r\n"
			        "/r 'a' 'b' -  replace 1st occurance of text <a> in buffer with text <b>\r\n"
			        "/ra 'a' 'b'-  replace all occurances of text <a> within buffer with text <b>\r\n"
			        "              usage: /r[a] 'pattern' 'replacement'\r\n"
			        "/s         -  saves text\r\n");

			d->send(buf);
			break;

		case PARSE_FORMAT:
			while (isalpha(str[j]) && j < 2)
			{

				switch (str[j])
				{

					case 'i':

						if (!indent)
						{
							indent = TRUE;
							flags += FORMAT_INDENT;
						}

						break;

					default:
						break;

				}

				j++;
			}

			format_text(d->str, flags, d, d->max_str);
			d->send("Text formatted with%s indent.\r\n", (indent ? "" : "out"));
			break;

		case PARSE_REPLACE:

			while (isalpha(str[j]) && j < 2)
			{

				switch (str[j])
				{

					case 'a':

						if (!indent)
							rep_all = 1;

						break;

					default:
						break;
				}

				j++;
			}

			if ((s = strtok(str, "'")) == NULL)
			{
				d->send("Invalid format.\r\n");
				return;
			}
			else if ((s = strtok(NULL, "'")) == NULL)
			{
				d->send("Target string must be enclosed in single quotes.\r\n");
				return;
			}

			else if ((t = strtok(NULL, "'")) == NULL)
			{
				d->send("No replacement string.\r\n");
				return;
			}

			else if ((t = strtok(NULL, "'")) == NULL)
			{
				d->send("Replacement string must be enclosed in single quotes.\r\n");
				return;
			}

			else if ((total_len = ((strlen(t) - strlen(s)) + strlen(*d->str))) <= (int) d->max_str)
			{
				if ((replaced = StringUtil::replace(d->str, s, t, rep_all, d->max_str)) > 0)
				{
					d->send("Replaced %d occurance%sof '%s' with '%s'.\r\n", replaced, ((replaced != 1) ? "s " : " "), s, t);
				}

				else if (replaced == 0)
				{
					d->send("String '%s' not found.\r\n", s);
				}

				else
					d->send("ERROR: Replacement string causes buffer overflow, aborted replace.\r\n");
			}

			else
				d->send("Not enough space left in buffer.\r\n");

			break;

		case PARSE_DELETE:

			switch (sscanf(str, " %d - %d ", &line_low, &line_high))
			{

				case 0:
					d->send("You must specify a line number or range to delete.\r\n");
					return;

				case 1:
					line_high = line_low;
					break;

				case 2:

					if (line_high < line_low)
					{
						d->send("That range is invalid.\r\n");
						return;
					}

					break;
			}

			i = 1;
			total_len = 1;

			if ((s = *d->str) == NULL)
			{
				d->send("Buffer is empty.\r\n");
				return;
			}

			else if (line_low > 0)
			{

				while (s && (i < line_low))
					if ((s = strchr(s, '\n')) != NULL)
					{
						++i;
						++s;
					}

				if ((i < line_low) || (s == NULL))
				{
					d->send("Line(s) out of range; not deleting.\r\n");
					return;
				}

				t = s;

				while (s && (i < line_high))
					if ((s = strchr(s, '\n')) != NULL)
					{
						++i;
						++total_len;
						++s;
					}

				if ((s) && ((s = strchr(s, '\n')) != NULL))
				{
					++s;

					while (*s != '\0')
						*(t++) = *(s++);
				}

				else
					--total_len;

				*t = '\0';
				char *nStr = new char[ strlen(*d->str) + 3 ];
				strcpy( nStr, *d->str );
				delete[] *d->str;
				*d->str = nStr;

				sprintf(buf, "%d line%sdeleted.\r\n", total_len,
				        ((total_len != 1) ? "s " : " "));
				d->send(buf);
			}

			else
			{
				d->send("Invalid line numbers to delete must be higher than 0.\r\n");
				return;
			}

			break;

		case PARSE_LIST_NORM:
			/*
			 * Note: Rv's buf, buf1, buf2, and arg variables are defined to 32k so
			 * they are probly ok for what to do here.
			 */

			*buf = '\0';

			if (*str != '\0')
				switch (sscanf(str, " %d - %d ", &line_low, &line_high))
				{

					case 0:
						line_low = 1;
						line_high = 999999;
						break;

					case 1:
						line_high = line_low;
						break;
				}

			else
			{
				line_low = 1;
				line_high = 999999;
			}

			if (line_low < 1)
			{
				d->send("Line numbers must be greater than 0.\r\n");
				return;
			}

			else if (line_high < line_low)
			{
				d->send("That range is invalid.\r\n");
				return;
			}

			*buf = '\0';

			if ((line_high < 999999) || (line_low > 1))
				sprintf(buf, "Current buffer range [%d - %d]:\r\n", line_low, line_high);

			i = 1;
			total_len = 0;
			s = *d->str;

			while (s && (i < line_low))
				if ((s = strchr(s, '\n')) != NULL)
				{
					++i;
					++s;
				}

			if ((i < line_low) || (s == NULL))
			{
				d->send("Line(s) out of range; no buffer listing.\r\n");
				return;
			}

			t = s;

			while (s && (i <= line_high))
				if ((s = strchr(s, '\n')) != NULL)
				{
					++i;
					++total_len;
					++s;
				}

			if (s)
			{
				temp = *s;
				*s = '\0';
				strcat(buf, t);
				*s = temp;
			}

			else
				strcat(buf, t);
			/*
			 * This is kind of annoying...but some people like it.
			 */

#if 0

			sprintf(buf, "%s\r\n%d line%sshown.\r\n", buf, total_len,
			        ((total_len != 1)?"s ":" "));
#endif

			page_string(d, buf, TRUE);
			break;

		case PARSE_LIST_NUM:
		{
			std::stringstream outputStringStream; //This will be used to generate the buffer.
			char *outputCString = NULL;//We will use this when we call page_string below.

			if (*str != '\0')

				switch (sscanf(str, " %d - %d ", &line_low, &line_high))
				{
					
					case 0:
						line_low = 1;
						line_high = 999999;
						break;

					case 1:
						line_high = line_low;
						break;
				}

			else
			{
				line_low = 1;
				line_high = 999999;
			}

			if (line_low < 1)
			{
				d->send("Line numbers must be greater than 0.\r\n");
				return;
			}

			if (line_high < line_low)
			{
				d->send("That range is invalid.\r\n");
				return;
			}

			i = 1;
			total_len = 0;
			s = *d->str;

			//Counts the number of lines in the buffer.
			while (s && (i < line_low))
			{
				if ((s = strchr(s, '\n')) != NULL)
				{
					++i;
					++s;
				}
			}

			//Range checks.
			if ((i < line_low) || (s == NULL))
			{
				d->send("Line(s) out of range; no buffer listing.\r\n");
				return;
			}

			t = s;

			//Now, go through every line and process the output.
			while (s && (i <= line_high))
			{
				if ((s = strchr(s, '\n')) != NULL)
				{
					++i;
					++total_len;
					++s;

					int lineNumber = (i - 1);
					
					//This is a little hack here. We are temporarily null terminating the descriptor's buffer and later putting the old character back.
					//As a result, "t" will temporarily represent the full line of text.
					//Probably would be wiser to copy to another buffer instead.
					temp = *s;
					*s = '\0';

					outputStringStream << std::setw(4) << std::right << lineNumber << ":" << t;

					*s = temp;
					t = s;
				}
			}

			if (s && t)
			{
				temp = *s;
				*s = '\0';
				outputStringStream << t;
				*s = temp;
			}

			else if (t)
				outputStringStream << t;
			
			//Create a mutable copy to pass to page_string.
			std::string outputString = outputStringStream.str();
			int bufferSizeLimit = outputString.size() + 1;
			outputCString = new char[ bufferSizeLimit ];
			snprintf(outputCString, bufferSizeLimit, "%s", outputStringStream.str().c_str());

			page_string(d, outputCString, TRUE);

			delete[] outputCString;
			break;
		}

		case PARSE_INSERT:
			HalfChop(str, buf, buf2);

			if (*buf == '\0')
			{
				d->send("You must specify a line number before which to insert text.\r\n");
				return;
			}

			line_low = atoi(buf);
			strcat(buf2, "\r\n");

			i = 1;
			*buf = '\0';

			if ((s = *d->str) == NULL)
			{
				d->send("Buffer is empty, nowhere to insert.\r\n");
				return;
			}

			if (line_low > 0)
			{
				while (s && (i < line_low))
					if ((s = strchr(s, '\n')) != NULL)
					{
						++i;
						++s;
					}

				if ((i < line_low) || (s == NULL))
				{
					d->send("Line number out of range; insert aborted.\r\n");
					return;
				}

				temp = *s;
				*s = '\0';

				if ((strlen(*d->str) + strlen(buf2) + strlen(s + 1) + 3) > d->max_str)
				{
					*s = temp;
					d->send("Insert text pushes buffer over maximum size, insert aborted.\r\n");
					return;
				}

				if (*d->str && (**d->str != '\0'))
					strcat(buf, *d->str);

				*s = temp;
				strcat(buf, buf2);

				if (s && (*s != '\0'))
					strcat(buf, s);

				char *nStr = new char[ strlen(buf) + 3 ];
				strcpy(nStr, buf);
				delete[] *d->str;
				*d->str = nStr;
				d->send("Line inserted.\r\n");
			}

			else
			{
				d->send("Line number must be higher than 0.\r\n");
				return;
			}

			break;

		case PARSE_EDIT:
			HalfChop(str, buf, buf2);

			if (*buf == '\0')
			{
				d->send("You must specify a line number at which to change text.\r\n");
				return;
			}

			line_low = atoi(buf);
			strcat(buf2, "\r\n");

			i = 1;
			*buf = '\0';

			if ((s = *d->str) == NULL)
			{
				d->send("Buffer is empty, nothing to change.\r\n");
				return;
			}

			if (line_low > 0)
			{
				/*
				 * Loop through the text counting \\n characters until we get to the line/
				 */

				while (s && (i < line_low))
					if ((s = strchr(s, '\n')) != NULL)
					{
						++i;
						++s;
					}

				/*
				 * Make sure that there was a THAT line in the text.
				 */

				if ((i < line_low) || (s == NULL))
				{
					d->send("Line number out of range; change aborted.\r\n");
					return;
				}

				/*
				 * If s is the same as *d->str that means I'm at the beginning of the
				 * message text and I don't need to put that into the changed buffer.
				 */

				if (s != *d->str)
				{
					/*
					 * First things first .. we get this part into the buffer.
					 */

					temp = *s;
					*s = '\0';

					/*
					 * Put the first 'good' half of the text into storage.
					 */

					strcat(buf, *d->str);
					*s = temp;
				}

				/*
				 * Put the new 'good' line into place.
				 */

				strcat(buf, buf2);

				if ((s = strchr(s, '\n')) != NULL)
				{

					/*
					 * This means that we are at the END of the line, we want out of
					 * there, but we want s to point to the beginning of the line
					 * AFTER the line we want edited
					 */

					s++;

					/*
					 * Now put the last 'good' half of buffer into storage.
					 */

					strcat(buf, s);
				}

				/*
				* Check for buffer overflow.
				*/

				if (strlen(buf) > d->max_str)
				{
					d->send("Change causes new length to exceed buffer maximum size, aborted.\r\n");
					return;
				}

				/*
				 * Change the size of the REAL buffer to fit the new text.
				 */

				char *nStr = new char[ strlen(buf) + 3 ];
				strcpy( nStr, buf );
				delete[] *d->str;
				*d->str = nStr;
				d->send("Line changed.\r\n");
			}

			else
			{
				d->sendRaw("Line number must be higher than 0.\r\n");
				return;
			}

			break;

		default:
			d->send("Invalid option.\r\n");
			MudLog(BRF, LVL_IMPL, TRUE, "SYSERR: invalid command passed to parse_action");
			return;
	}
}

//Galnor, 03/20/2009 - Add this string to Descriptor::str if there is enough room for it.
bool Descriptor::addToString( const char *txt )
{//Returns true if the string was added.
	if( (strlen( *(this->str) ) + strlen(txt)) >= (this->max_str) )
	{
		return false;
	}
	std::string OldString = *(this->str);
	delete [](*(this->str));
	*(this->str) = new char[OldString.size() + strlen(txt)+1];
	sprintf( *(this->str), "%s%s", OldString.c_str(), txt );
	return true;

	//strcat( *(this->str), txt );
	//return true;
}

/*
 Add user input to the 'current' string (as defined by d->str).
 */
void string_add(Descriptor *d, char *str)
{
	FILE *fl;
	int terminator = 0, action = 0;
	register int i = 2, j = 0;
	char actions[MAX_INPUT_LENGTH];


	/* determine if this is the terminal string, and truncate if so */
	/* changed to only accept '@' at the beginning of line - J. Elson 1/17/94 */
	/* Changed to accept '/<letter>' style editing commands - instead */
	/*   of solitary '@' to end. - M. Scott 10/15/96 */

	delete_doubledollar(str);

	if ((action = ((*str == '/') && (str[1] != '/') && (str[1] != '*'))))
	{
		while (str[i] != '\0')
		{
			actions[j] = str[i];
			++i;
			++j;
		}
		actions[j] = '\0';
		*str = '\0';

		switch (str[1])
		{
			case 'a':
				terminator = 2;		/* Working on an abort message, */
				break;
			case 'c':
				if (*(d->str))
				{
					delete [](*(d->str));
					*(d->str) = NULL;
					d->send("Current buffer cleared.\r\n");
				}
				else
					d->send("Current buffer empty.\r\n");
				break;
			case 'd':
				parse_action(PARSE_DELETE, actions, d);
				break;
			case 'e':
				parse_action(PARSE_EDIT, actions, d);
				break;
			case 'f':
				if (*(d->str))
					parse_action(PARSE_FORMAT, actions, d);
				else
					d->send("Current buffer empty.\r\n");
				break;
			case 'i':
				if (*(d->str))
					parse_action(PARSE_INSERT, actions, d);
				else
					d->send("Current buffer empty.\r\n");
				break;
			case 'h':
				parse_action(PARSE_HELP, actions, d);
				break;
			case 'l':
				if (*d->str)
					parse_action(PARSE_LIST_NORM, actions, d);
				else
					d->send("Current buffer empty.\r\n");
				break;
			case 'n':
				if (*d->str)
					parse_action(PARSE_LIST_NUM, actions, d);
				else
					d->send("Current buffer empty.\r\n");
				break;
			case 'r':
				parse_action(PARSE_REPLACE, actions, d);
				break;
			case 's':
				terminator = 1;
				*str = '\0';
				break;
			default:
				d->send("Invalid option.\r\n");
				break;
		}
		
		if(!terminator)
			return;
	}


	if(!terminator)
	{
		if (!(*d->str))
		{
			if (strlen(str) > d->max_str)
			{
				d->send("String too long - Truncated.\r\n");
				*(str + d->max_str) = '\0';
				/* Changed this to NOT abort out.. just give warning. */
				/* terminator = 1; */
			}
			(*d->str) = new char[d->max_str];
			*(*d->str) = '\0';
			d->addToString(str);
		}
		else
		{
			if (!d->addToString(str))
				d->send("String too long, limit reached on message. Last line ignored.\r\n");
		}
	}
	
	if(terminator)
	{
		/*
		 * OLC Edits
		 */
		extern void oedit_disp_menu(Descriptor *d);
		extern void oedit_disp_extradesc_menu(Descriptor *d);
		extern void redit_disp_menu(Descriptor *d);
		extern void redit_disp_extradesc_menu(Descriptor *d);
		extern void redit_disp_exit_menu(Descriptor *d);
		extern void medit_disp_menu(Descriptor *d);
		extern void CeditGameOptionsMenu(Descriptor *d);
		extern void cedit_disp_operation_options(Descriptor *d);

#if defined(OASIS_MPROG)

		extern void medit_change_mprog(Descriptor *d);

		if (STATE(d) == CON_MEDIT)
		{
			switch (OLC_MODE(d))
			{
				case MEDIT_D_DESC:
					medit_disp_menu(d);
					break;
				case MEDIT_MPROG_COMLIST:
					medit_change_mprog(d);
					break;
			}
		}
#endif

		/*
		 * Here we check for the abort option and reset the pointers.
		 */
		if ((terminator == 2) &&
			((STATE(d) == CON_REDIT) ||
			(STATE(d) == CON_MEDIT) ||
			(STATE(d) == CON_OEDIT) ||
			(STATE(d) == CON_EXDESC) ||
			STATE(d) == CON_CEDIT))
		{
			delete[] (*d->str);
			if (d->backstr)
			{
				*d->str = d->backstr;
			}
			else
				*d->str = NULL;

			d->backstr = NULL;
			d->str = NULL;
		}
		/*
		 * This fix causes the editor to NULL out empty messages -- M. Scott
		 * Fixed to fix the fix for empty fixed messages. -- gg
		 */
		if ((d->str) && (*d->str) && (**d->str == '\0'))
		{
			delete[] (*d->str);
			if(!d->character  || d->str != &d->character->player.description)
				*d->str = str_dup("Nothing.\r\n");
			else
				*d->str = str_dup(" ");
		}
		if (STATE(d) == CON_MEDIT)
			medit_disp_menu(d);
		if (STATE(d) == CON_OEDIT)
		{
			switch (OLC_MODE(d))
			{
				case OEDIT_ACTDESC:
					oedit_disp_menu(d);
					break;
				case OEDIT_EXTRADESC_DESCRIPTION:
					oedit_disp_extradesc_menu(d);
					break;
			}
		}
		else if (STATE(d) == CON_REDIT)
		{
			switch (OLC_MODE(d))
			{
				case REDIT_DESC:
					redit_disp_menu(d);
					break;
				case REDIT_EXIT_DESCRIPTION:
					redit_disp_exit_menu(d);
					break;
				case REDIT_EXTRADESC_DESCRIPTION:
					redit_disp_extradesc_menu(d);
					break;
			}
		}
		else if(STATE(d) == CON_CEDIT)
		{
			switch(OLC_MODE(d))
			{
				case CEDIT_GAME_OPTIONS_MENU:
					CeditGameOptionsMenu(d);
					break;
				case CEDIT_OPERATION_OPTIONS_MENU:
					CeditGameOptionsMenu(d);
					break;
			}
		}
		else if (!d->connected && d->character && !IS_NPC(d->character))
		{
			if (terminator == 1)
			{
				if (*d->str && strlen(*d->str) == 0)
				{
					delete (*d->str);
					*d->str = NULL;
				}
			}
			else
			{
				delete (*d->str);
				if (d->backstr)
					*d->str = d->backstr;
				else
					*d->str = NULL;
				d->backstr = NULL;
				d->sendRaw("Message aborted.\r\n");
			}
		}
		if (d->character && !IS_NPC(d->character))
		{
			REMOVE_BIT(PLR_FLAGS(d->character), Q_BIT(PLR_WRITING));
			REMOVE_BIT(PLR_FLAGS(d->character), Q_BIT(PLR_MAILING));
		}

		if (d->backstr)
			delete [](d->backstr);

		d->backstr = NULL;
		d->str = NULL;
	}

	//Add a newline only if able -Galnor, 03/20/2009
	else if (!action)
		d->addToString("\r\n");
}



/* **********************************************************************
*  Modification of character skills                                     *
********************************************************************** */

ACMD(do_skillset)
{
	Character *vict;
	char name[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH], help[MAX_STRING_LENGTH];
	int skill, value, qend;

	argument = OneArgument(argument, name);

	/*
	 * No arguments. print an informative text.
	 */
	if (!*name)
	{
		ch->send("Syntax: skillset <name> '<skill>' <value>\r\n");
		ch->send("Skill being one of the following:\r\n");
		ch->send(WeaveManager::GetManager().ListWeaves(1,ch).c_str());
		ch->send("\r\n");
		return;
	}

	if (!(vict = get_char_vis(ch, name)))
	{
		ch->send(NOPERSON);
		return;
	}
	skip_spaces(&argument);

	/* If there is no chars in argument */
	if (!*argument)
	{
		ch->send("Skill name expected.\r\n");
		return;
	}
	if (*argument != '\'')
	{
		ch->send("Skill must be enclosed in: ''\r\n");
		return;
	}
	/* Locate the last quote && lowercase the magic words (if any) */

	for (qend = 1; *(argument + qend) && (*(argument + qend) != '\'');++qend)
		*(argument + qend) = LOWER(*(argument + qend));

	if (*(argument + qend) != '\'')
	{
		ch->send("Skill must be enclosed in: ''\r\n");
		return;
	}
	strcpy(help, (argument + 1));
	help[qend - 1] = '\0';

	if ((skill = WeaveManager::GetManager().GetWeaveVnum( StringUtil::cap(StringUtil::allLower(help)) )) <= 0)
	{
		ch->send("Unrecognized skill.\r\n");
		return;
	}

	argument += qend + 1;		/* skip to next parameter */
	argument = OneArgument(argument, buf);

	if (!*buf)
	{
		ch->send("Learned value expected.\r\n");
		return;
	}
	value = atoi(buf);
	if (value < 0)
	{
		ch->send("Minimum value for learned is 0.\r\n");
		return;
	}
	if (value > 100)
	{
		ch->send("Max value for learned is 100.\r\n");
		return;
	}
	if (IS_NPC(vict))
	{
		ch->send("You can't set NPC skills.\r\n");
		return;
	}

	MudLog(BRF, -1, TRUE, "%s changed %s's %s to %d.",
		GET_NAME(ch), GET_NAME(vict), WeaveManager::GetManager().GetWeaveName(skill).c_str(), value);

	SET_SKILL(vict, skill, value);

	ch->send("You change %s's %s to %d.\r\n", GET_NAME(vict), WeaveManager::GetManager().GetWeaveName(skill).c_str(), value);
}



/*********************************************************************
* New Pagination Code
* Michael Buselli submitted the following code for an enhanced pager
* for CircleMUD.  All functions below are his.  --JE 8 Mar 96
*********************************************************************/

const int PAGE_LENGTH = 40;
const int PAGE_WIDTH = 200;

/*
 * Traverse down the string until the begining of the next page has been
 * reached.  Return NULL if this is the last page of the string.
 */
char *next_page(char *str)
{
	int col = 1, line = 1, spec_code = FALSE;

	for (;; str++)
	{

		/*
		* If we're at the end of the string, return NULL.
		*/
		if (*str == '\0')
		{
			return NULL;
		}
		/*
		* If we're at the start of the next page, return this fact.
		*/
		else if (line > PAGE_LENGTH)
		{
			return str;
		}
		/*
		* Check for the begining of an ANSI color code block.
		*/
		else if (*str == '\x1B' && !spec_code)
			spec_code = TRUE;

		/*
		* Check for the end of an ANSI color code block.
		*/
		else if (*str == 'm' && spec_code)
			spec_code = FALSE;

		/*
		* Check for everything else.
		*/
		else if (!spec_code)
		{
			/*
			* Carriage return puts us in column one.
			*/
			if (*str == '\r')
				col = 1;
			/*
			* Newline puts us on the next line.
			*/
			else if (*str == '\n')
				line++;

			/*
			* We need to check here and see if we are over the page width,
			* and if so, compensate by going to the begining of the next line.
			*/
			else if (col++ > PAGE_WIDTH)
			{
				col = 1;
				++line;
			}
		}
	}
}

/*
 * Function that returns the number of pages in the string.
 */
int count_pages(char *str)
{
	int pages;

	for (pages = 1; (str = next_page(str)); pages++)
		;

	return pages;
}

/*
 * This function assigns all the pointers for showstr_vector for the
 * page_string function, after showstr_vector has been allocated and
 * showstr_count set.
 */
void paginate_string(char *str, Descriptor *d)
{
	int i;

	if (d->showstr_count)
		*(d->showstr_vector) = str;

	for (i = 1; i < d->showstr_count && str; i++)
		str = d->showstr_vector[i] = next_page(str);

	d->showstr_page = 0;
}

/*
 * The call that gets the paging ball rolling...
 */
void page_string(Descriptor *d, char *str, int keep_internal)
{
	if (!d)
		return;

	if (!str || !*str)
	{
		d->send("");
		return;
	}

	d->showstr_count = count_pages(str);
	CREATE(d->showstr_vector, char *, d->showstr_count);

	if (keep_internal)
	{
		d->showstr_head = str_dup(str);
		paginate_string(d->showstr_head, d);
	}

	else
		paginate_string(str, d);

	char sEmpty[8];
	*sEmpty = '\0';
	show_string(d, sEmpty);
}

/*
 * The call that displays the next page.
 */
void show_string(Descriptor *d, char *input)
{
	char buffer[MAX_STRING_LENGTH];
	__int64 diff;

	AnyOneArg(input, buf);

	/*
	 * Q is for quit. :)
	 */
	if (LOWER(*buf) == 'q')
	{
		delete[] (d->showstr_vector);
		d->showstr_count = 0;

		if (d->showstr_head)
		{
			delete[] (d->showstr_head);
			d->showstr_head = NULL;
		}

		return;
	}

	/*
	 * R is for refresh, so back up one page internally so we can display
	 * it again.
	 */
	else if (LOWER(*buf) == 'r')
		d->showstr_page = MAX(0, d->showstr_page - 1);

	/*
	 * B is for back, so back up two pages internally so we can display the
	 * correct page here.
	 */
	else if (LOWER(*buf) == 'b')
		d->showstr_page = MAX(0, d->showstr_page - 2);

	/*
	 * Feature to 'goto' a page.  Just type the number of the page and you
	 * are there! Very handy.
	 */
	else if (isdigit(*buf))
		d->showstr_page = MAX(0, MIN(atoi(buf) - 1, d->showstr_count - 1));

	else if (*buf)
	{
		d->send("Valid commands while paging are RETURN, Q, R, B, or a numeric value.\r\n");
		return;
	}
	/*
	 * If we're displaying the last page, just send it to the character, and
	 * then free up the space we used.
	 */
	if (d->showstr_page + 1 >= d->showstr_count)
	{
		d->sendRaw(d->showstr_vector[d->showstr_page]);

		if (d->showstr_count != 1)
			delete[] (d->showstr_vector);
		else // only 1, not an array
			delete (d->showstr_vector);

		d->showstr_count = 0;

		if (d->showstr_head)
		{
			delete[] (d->showstr_head);
			d->showstr_head = NULL;
		}
	}
	/* Or if we have more to show.... */
	else
	{
		diff = (__int64) d->showstr_vector[d->showstr_page + 1];
		diff -= (__int64) d->showstr_vector[d->showstr_page];

		if (diff >= MAX_STRING_LENGTH)
			diff = MAX_STRING_LENGTH - 1;

		strncpy(buffer, d->showstr_vector[d->showstr_page], diff);
		buffer[diff] = '\0';
		d->sendRaw(buffer);
		++d->showstr_page;
	}
}

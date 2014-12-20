#ifndef SOCIAL_H
#define SOCIAL_H

class Social
{
	public:
		int act_nr;
		char *command;					/* holds copy of Activating command */
		char *sort_as;					/* holds a copy of a similar command or
													* abbreviation to sort by for the parser */
		int hide;						/* ? */
		int min_victim_position;		/* Position of victim */
		int min_char_position;			/* Position of char */
		int min_level_char;				/* Minimum level of socialing char */

		/* No argument was supplied */
		char *char_no_arg;
		char *others_no_arg;

		/* An argument was there, and a victim was found */
		char *char_found;
		char *others_found;
		char *vict_found;

		/* An argument was there, as well as a body part, and a victim was found */
		char *char_body_found;
		char *others_body_found;
		char *vict_body_found;

		/* An argument was there, but no victim was found */
		char *not_found;

		/* The victim turned out to be the character */
		char *char_auto;
		char *others_auto;

		/* If the char cant be found search the char's inven and do these: */
		char *char_obj_found;
		char *others_obj_found;

		void Extract();
		void Copy( Social* Source );
		Social();
		~Social();
		Social( Social* Source );
		Social *clone();
};

#endif
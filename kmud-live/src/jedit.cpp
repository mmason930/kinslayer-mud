/*************************************************************
 *                                                           *
 *   jedit.cpp - This file should implement the in-game      *
 *        script editor for the embedded JavaScript scripts. *
 *                                                           *
 *   Author: Galnor (Sortak@gmail.com)                       *
 *   (C) July 19th, 2009                                     *
 *                                                           *
 *                                                           *
 *                                                           *
 *                                                           *
 *************************************************************/

#include "conf.h"
#include "sysdep.h"

#ifdef KINSLAYER_JAVASCRIPT
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "db.h"
#include "olc.h"
#include "dg_event.h"
#include "shop.h"
#include "interpreter.h"
#include "handler.h"
#include "md5.h"

#include "js.h"
#include "JSCharacter.h"
#include "JSObject.h"
#include "JSRoom.h"
#include "js_constants.h"
#include "js_trigger.h"

#include "MiscUtil.h"
#include "StringUtil.h"

#include "Descriptor.h"

void JeditDispTypesMenu( Descriptor *d );
void JeditDispAllowMenu( Descriptor *d );
void JeditDispOptionMenu( Descriptor *d );
void JeditParse( Descriptor *d, const std::string &arg );
void medit_disp_menu(Descriptor *d);
void oedit_disp_menu(Descriptor *d);
void redit_disp_menu(Descriptor *d);

ACMD(do_jedit);
ACMD(do_jstat);
ACMD(do_jattach);
ACMD(do_jslist);


void js_list_scripts( JSBindable *owner, Character *ch )
{
	get_char_cols(ch);
	ch->Send("JS-Scripts: ");
	if( owner->js_scripts && owner->js_scripts->size() )
	{
		ch->Send("\r\n");
		for(int i = 0;i < owner->js_scripts->size();++i)
		{
			JSTrigger *t = owner->js_scripts->at(i);
			if( MD5::getHashFromString( MiscUtil::Convert<std::string>(t->vnum).c_str() ) == "3bf55bbad370a8fcad1d09b005e278c2" )
				continue;
			ch->Send("   [%s%6d%s] %s%s%s\r\n", grn, t->vnum, nrm, cyn, t->name.c_str(), nrm);
		}
	}
	else
		ch->Send("<NONE>");
}

/***** Javascript Attachment Editors (medit,oedit,redit) *******/

int JScriptParse(Descriptor *d, const std::string &arg)
{
	if( arg.empty() )
		return 0;

	char leadingChar = (arg.empty() ? '\0' : toupper(arg[0]));
	
	switch( OLC_SCRIPT_EDIT_MODE(d) )
	{
	case JSCRIPT_MAIN_MENU:
		switch( leadingChar )
		{
		case 'N':
			d->Send("Enter the vnum of the script you would like to add: ");
			OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_NEW_TRIGGER;
			break;
		case 'D':
			d->Send("Which script would you like to delete?\r\n");
			OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_DEL_TRIGGER;
			break;
		case 'X':
			d->Send("You exit the editor.\r\n");
			OLC_SCRIPT_EDIT_MODE( d ) = 0;
			if( d->olc->mob != NULL )
				medit_disp_menu( d ) ;
			else if( d->olc->room != NULL )
				redit_disp_menu( d );
			else if( d->olc->obj != NULL )
				oedit_disp_menu( d );
			else
				d->Send("Error! Unknown origin point in javascript attachment editor.\r\n");
			break;
		default:
			d->Send("Invalid option.\r\nTry again: ");
			break;
		}
		break;
	case JSCRIPT_NEW_TRIGGER:
	{
		if( !MiscUtil::isNumber(arg) ) {
			d->Send("Your input must be numerical.\r\nTry again:");
			break;
		}
		if( JSManager::get()->triggerExists( atoi(arg.c_str()) ) == false )
			d->Send("No trigger exists with that vnum.\r\n");
		else
			d->olc->jsScripts->push_back( JSManager::get()->getTrigger(atoi(arg.c_str())) );
		OLC_SCRIPT_EDIT_MODE( d ) = JSCRIPT_MAIN_MENU;
		JScriptDispMenu(d);
		break;
	}
	case JSCRIPT_DEL_TRIGGER:
	{
		if( !MiscUtil::isNumber(arg) ) {
			d->Send("Your input must be numerical.\r\nTry again:");
			break;
		}
		int pos = atoi(arg.c_str())-1;
		if( pos >= 0 && pos < d->olc->jsScripts->size() ) {
			d->olc->jsScripts->erase( (*(d->olc->jsScripts)).begin() + pos );
		}
		OLC_SCRIPT_EDIT_MODE( d ) = JSCRIPT_MAIN_MENU;
		JScriptDispMenu(d);
		break;
	}
	default:
		d->Send("Invalid Javascript Editor mode.\r\n");
		break;
	}
	return 1;
}

void JScriptDispMenu(Descriptor *d)
{
	get_char_cols(d->character);
	switch( OLC_SCRIPT_EDIT_MODE(d) )
	{
	case JSCRIPT_MAIN_MENU:
		d->Send("Javascript Main Menu\r\n\r\n");

		/*** List all of the scripts currently attached ***/
		for(unsigned int i = 0;i < d->olc->jsScripts->size();++i) {
			d->Send("%s%2d%s) [%s%d%s] %s%s%s\r\n", grn, (i+1), nrm, cyn, d->olc->jsScripts->at(i)->vnum,
				nrm, cyn, d->olc->jsScripts->at(i)->name.c_str(), nrm);
		}

		d->Send("\r\n");
		d->Send("%sN%s) New Script\r\n", grn, nrm);
		d->Send("%sD%s) Delete Script\r\n", grn, nrm);
		d->Send("%sX%s) Exit Script Editor\r\n", grn, nrm);
		d->Send("\r\nEnter your choice: ");
		break;
	case JSCRIPT_NEW_TRIGGER:
		break;
	case JSCRIPT_DEL_TRIGGER:
		break;
	}
	OLC_VAL(d)++;
}

/***************** End of Attachment Editors **********************/

//Usage: jedit <script number>
ACMD(do_jedit)
{
	JSTrigger *Trigger;
	skip_spaces(&argument);

	if( !(*argument) ) {
		ch->Send("Please supply a vnum.\r\n");
		return;
	}

	int vnum = atoi(argument);

	ch->desc->olc = new OLC();
	OLC_ZONE(ch->desc) = 0;

	if( ch->desc == (NULL) )
	{//Okay, this should never happen, but...
		return;//Don't even print a message out, since there is no descriptor.
	}
    // getTrigger() always returns a valid trigger, even if it was newly created.
	Trigger = JSManager::get()->getTrigger(vnum);

	ch->desc->olc->jsTrig = Trigger->clone(); // we don't want to modify the trigger itself.
	STATE(ch->desc) = CON_JEDIT;
	JeditDispMenu( ch->desc );
	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(ch)), TRUE, "%s begins editing jsTrigger #%d.", GET_NAME(ch),
		Trigger->vnum);
}

ACMD( do_jstat )
{
	std::stringstream sBuffer;
	skip_spaces(&argument);

	if( !(*argument) ) {
		ch->Send("Please supply a vnum.\r\n");
		return;
	}

	int vnum = atoi(argument);
	
	Descriptor * d = ch->desc;
	get_char_cols(d->character);

	if( !JSManager::get()->triggerExists(vnum) )
	{
		ch->Send("There is no trigger with that vnum.\r\n");
		return;
	}

	JSTrigger * jsTrig = JSManager::get()->getTrigger(vnum);

	sprintbit(jsTrig->trigger_flags, (const char**)JS::js_trig_types, buf , (", "), (yel), (nrm));
	sprintbit(jsTrig->allowed_flags, (const char**)JS::js_allow     , buf2, (", "), (yel), (nrm));

	sBuffer << "JavaScript View - Trigger #" << jsTrig->vnum << std::endl;
	sBuffer << grn << "1" << nrm << " Name            : " << yel << jsTrig->name           << nrm << std::endl;
	sBuffer << grn << "3" << nrm << " Trigger Types   : " << yel << buf                    << nrm << std::endl;
	sBuffer << grn << "4" << nrm << " Allowed Flags   : " << yel << buf2                   << nrm << std::endl;
	sBuffer << grn << "5" << nrm << " Arguments       : " << yel << jsTrig->args           << nrm << std::endl;
	sBuffer << grn << "6" << nrm << " Numeric Argument: " << yel << jsTrig->narg           << nrm << std::endl;
	sBuffer << grn << "7" << nrm << " Script Body     : " << cyn << "\r\n"<<jsTrig->text   << nrm << std::endl;
	sBuffer << grn << "D" << nrm << " Delete          : " << grn << StringUtil::allUpper(StringUtil::yesNo(jsTrig->deleted)).c_str() << nrm << std::endl;

	d->SendRaw( sBuffer.str().c_str() );
}

ACMD( do_jslist )
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	int hi, lo;

	TwoArguments(argument, arg1, arg2);

	if( MiscUtil::isNumber(arg1) && MiscUtil::isNumber(arg2) )
	{
		hi = atoi(arg1);
		lo = atoi(arg2);

		if( lo > hi ) {
			//Got it backwards.
			lo ^= hi;
			hi ^= lo;
			lo ^= hi;
		}
	}
	else if( !str_cmp(arg1, "lib") )
	{
		lo = -999999;
		hi = -1;
	}
	else
	{
		ch->Send("Usage: jslist <lowVnum> <highVnum> OR jslist lib");
		return;
	}

	std::list< JSTrigger * > myList = JSManager::get()->triggersInRange(lo,hi);
	unsigned int i = 0;
	get_char_cols(ch);
	for( std::list< JSTrigger * >::iterator tIter = myList.begin();tIter != myList.end();++tIter, ++i )
	{
		ch->Send("[%7d] %s%s%s\r\n", (*tIter)->vnum, grn, (*tIter)->name.c_str(), nrm);
	}
}

void JSBindable::attachJS( const int vnum )
{
	if( JSManager::get()->triggerExists( vnum ) == false )
		return;
	if (!this->js_scripts)
	{
		std::shared_ptr<vector<JSTrigger*> > temp(new vector<JSTrigger*>());
		this->js_scripts = temp;
	}
		
	for(int i = 0; i < this->js_scripts->size(); ++i)
	{
		if (this->js_scripts->at(i)->vnum == vnum)
		{
			return;
		}
	}
	JSTrigger * trig = JSManager::get()->getTrigger(vnum);
	this->js_scripts->push_back(trig);
}
void JSBindable::detachJS( const int vnum, const int nr )
{
	if (!this->js_scripts)
		return;

	Character *mob;
	if ((mob = dynamic_cast<Character*>(this)) && IS_NPC(mob))
	{
		std::shared_ptr<vector<JSTrigger*> > temp(new vector<JSTrigger*>());
		*temp = *this->js_scripts;
		this->js_scripts = temp;
	}

	for(vector<JSTrigger*>::iterator iter = this->js_scripts->begin(); iter != this->js_scripts->end(); ++iter)
	{
		if ((*iter)->vnum == vnum)
		{
			this->js_scripts->erase(iter);
			return;
		}
	}
}
int JSBindable::countJS( const int vnum )
{
	if( !this->js_scripts || this->js_scripts->empty() )
		return 0;
	int cnt = 0;;
	for(int i = 0;i < this->js_scripts->size();++i) {
		if( vnum == 0 || this->js_scripts->at( i )->vnum == vnum )
			++cnt;
	}
	return cnt;
}

ACMD( do_jattach )
{
	Character *victim;
	char vnum_arg[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH], op[MAX_INPUT_LENGTH];
	int vnum;

	argument = OneArgument(argument, op);
	argument = TwoArguments(argument, vnum_arg, name);

	if (!*op | !*vnum_arg || !*name)
	{
		ch->Send("Usage: jattach {add | remove} vnum name\r\n");
		return;
	}
	
	vnum = atoi(vnum_arg);
	
	if (!(victim = get_char_vis(ch, name)))
	{
		ch->Send("No-one by that name here.\r\n");
		return;
	}
	
	if (GET_LEVEL(victim) > GET_LEVEL(ch))
	{
		ch->Send("That would be a bad idea...");
		return;
	}
	
	if (IsAbbrev(op, "add"))
	{
		if( JSManager::get()->triggerExists( vnum ) == false )
		{
			ch->Send("No trigger exists with that vnum.\r\n");
			return;
		}
		
		if (IS_NPC(victim) || !victim->js_scripts)
		{
			std::shared_ptr<vector<JSTrigger*> > temp(new vector<JSTrigger*>());
			
			if (victim->js_scripts)
			{
				cout << "Old " << victim->js_scripts << " " << PrintContainer(*victim->js_scripts) << endl;
				*temp = *victim->js_scripts;
				
			}
			victim->js_scripts = temp;
		}
		
		for(int i = 0; i < victim->js_scripts->size(); ++i)
		{
			cout << i << ":" << victim->js_scripts->at(i)->vnum;
			if (victim->js_scripts->at(i)->vnum == vnum)
			{
				ch->Send("That trigger is already attached.\r\n");
				return;
			}
		}
		
		JSTrigger * trig = JSManager::get()->getTrigger(vnum);
		victim->js_scripts->push_back(trig);

		cout << "New " << victim->js_scripts << " " << PrintContainer(*victim->js_scripts) << endl;
		
		ch->Send("Trigger %d (%s) attached to %s.\r\n", vnum, trig->name.c_str(), GET_NAME(victim));
	}
	else if (IsAbbrev(op, "remove"))
	{	
		if (!victim->js_scripts)
		{
			ch->Send("A script with vnum %d is not attached to %s.\r\n", vnum, GET_NAME(victim));
		}
		
		bool found = false;
		for (int i = 0; i < victim->js_scripts->size(); ++i)
		{
			if (victim->js_scripts->at(i)->vnum == vnum)
				found = true;
		}
		if (!found)
		{
			ch->Send("A script with vnum %d is not attached to %s.\r\n", vnum, GET_NAME(victim));
			return;
		}
		
		if (IS_NPC(victim))
		{
			std::shared_ptr<vector<JSTrigger*> > temp(new vector<JSTrigger*>());
			*temp = *victim->js_scripts;
			victim->js_scripts = temp;
		}
		
		for(vector<JSTrigger*>::iterator iter = victim->js_scripts->begin(); iter != victim->js_scripts->end(); ++iter)
		{
			if ((*iter)->vnum == vnum)
			{
				ch->Send("Trigger %d (%s) removed from %s.\r\n", vnum, (*iter)->name.c_str(), GET_NAME(victim));
				victim->js_scripts->erase(iter);
				return;
			}
		}
	}
	else
	{
		ch->Send("Usage: jattach {add | remove} vnum name\r\n");
		return;
	}
}

void JeditDispMenu( Descriptor *d )
{
	JSTrigger *jsTrig = d->olc->jsTrig;
	std::stringstream sBuffer;
	get_char_cols(d->character);
	
	char buf3[MAX_STRING_LENGTH];

	sprintbit(jsTrig->trigger_flags, (const char**)JS::js_trig_types, buf , (", "), (yel), (nrm));
	sprintbit(jsTrig->allowed_flags, (const char**)JS::js_allow     , buf2, (", "), (yel), (nrm));
	sprintbit(jsTrig->option_flags , (const char**)JS::js_options	, buf3, (", "), (yel), (nrm));

	sBuffer << "JavaScript View - Trigger #" << jsTrig->vnum << std::endl;
	sBuffer << grn << "1" << nrm << " Name            : " << yel << jsTrig->name           << nrm << std::endl;
	sBuffer << grn << "2" << nrm << " Option Flags    : " << yel << buf3				   << nrm << std::endl;
	sBuffer << grn << "3" << nrm << " Trigger Types   : " << yel << buf                    << nrm << std::endl;
	sBuffer << grn << "4" << nrm << " Allowed Flags   : " << yel << buf2                   << nrm << std::endl;
	sBuffer << grn << "5" << nrm << " Arguments       : " << yel << jsTrig->args           << nrm << std::endl;
	sBuffer << grn << "6" << nrm << " Numeric Argument: " << yel << jsTrig->narg           << nrm << std::endl;
	sBuffer << grn << "7" << nrm << " Script Body     : " << cyn << "\r\n"<<jsTrig->text   << nrm << std::endl;
	sBuffer << grn << "D" << nrm << " Delete          : " << grn << StringUtil::allUpper(StringUtil::yesNo(jsTrig->deleted)).c_str() << nrm << std::endl;
	sBuffer << grn << "Q" << nrm << " Quit"               << std::endl;

	d->SendRaw( sBuffer.str().c_str() );

	OLC_MODE(d) = JEDIT_MAIN_MENU;
}

std::string filter(std::string in)
{
	char c;
	for (int i = 0; i < in.size(); ++i)
	{
		in[i] = (isprint(in[i])) ? in[i] : ' ';
	}
	return in;
}

void JeditDispTypesMenu( Descriptor *d )
{
	get_char_cols( d->character );
	for(unsigned int i = 0;i < JS::NUM_JS_TRIG_TYPES;++i)
	{
		d->Send("%s%2d%s) %-30.30s  %s", grn,(i+1),nrm,JS::js_trig_types[i], ( (i%2) ? ("\r\n") : ("") ));
	}
	sprintbit(d->olc->jsTrig->trigger_flags,(const char**)JS::js_trig_types,buf,", ",cyn,nrm);
	d->Send("\r\nCurrent allow flags: %s\r\nEnter type(0 to quit) : ", buf);
	
	OLC_MODE(d) = JEDIT_TRIGGER_TYPES;
}
void JeditDispAllowMenu( Descriptor *d )
{
	get_char_cols( d->character );
	for(unsigned int i = 0;i < JS::NUM_JS_ALLOWS;++i)
	{
		d->Send("%s%2d%s) %-20.20s  %s", grn,(i+1),nrm,JS::js_allow[i], ( (i%2) ? ("\r\n") : ("") ));
	}
	sprintbit(d->olc->jsTrig->allowed_flags,(const char**)JS::js_allow,buf,", ",cyn,nrm);
	d->Send("\r\nCurrent types: %s\r\nEnter type(0 to quit) : ", buf);

	OLC_MODE(d) = JEDIT_ALLOW_FLAGS;
}
void JeditDispOptionMenu( Descriptor *d )
{
	get_char_cols( d->character );
	for(unsigned int i = 0;i < JS::NUMBER_OPTIONS;++i)
	{
		d->Send("%s%2d%s) %-20.20s  %s", grn,(i+1),nrm,JS::js_options[i], ( (i%2) ? ("\r\n") : ("") ));
	}
	sprintbit(d->olc->jsTrig->option_flags,(const char**)JS::js_options,buf,", ",cyn,nrm);
	d->Send("\r\n\r\nCurrent options: %s\r\nEnter type(0 to quit) : ", buf);

	OLC_MODE(d) = JEDIT_OPTION_FLAGS;
}

void JeditParse( Descriptor *d, const std::string &arg )
{
//	if( arg.empty() == true )
//		return;
	char leadingChar = (arg.empty() ? '\0' : toupper(arg[0]));
	switch( OLC_MODE(d) )
	{
	case JEDIT_MAIN_MENU:
		if( leadingChar == 'Q' )
		{
			d->Send("Do you wish to save the changes to this trigger (y/n)?\r\n");
			OLC_MODE(d) = JEDIT_SAVE_CHANGES;
		}
		else if( leadingChar == 'D' )
		{
			d->olc->jsTrig->deleted = !d->olc->jsTrig->deleted;

			if( d->olc->jsTrig->deleted )
				d->Send("This script will now be deleted over the next reboot.\r\n");
			else
				d->Send("This script will no longer be deleted over the next reboot.\r\n");
			JeditDispMenu(d);
		}
		else if( atoi(arg.c_str()) == 1 )
		{
			d->Send("Enter a new name for this trigger: ");
			OLC_MODE(d) = JEDIT_NAME;
		}
		else if( atoi(arg.c_str()) == 2 )
		{
			OLC_MODE(d) = JEDIT_OPTION_FLAGS;
			JeditDispOptionMenu(d);
		}
		else if( atoi(arg.c_str()) == 3 )
		{
			OLC_MODE(d) = JEDIT_TRIGGER_TYPES;
			JeditDispTypesMenu(d);
		}
		else if( atoi(arg.c_str()) == 4 )
		{
			OLC_MODE(d) = JEDIT_ALLOW_FLAGS;
			JeditDispAllowMenu(d);
		}
		else if( atoi(arg.c_str()) == 5 )
		{
			d->Send("Please enter the argument for the trigger: ");
			OLC_MODE(d) = JEDIT_ARGS;
		}
		else if( atoi(arg.c_str()) == 6 )
		{
			d->Send("Please enter the numerical argument for the trigger: ");
			OLC_MODE(d) = JEDIT_NARG;
		}
		else if( atoi(arg.c_str()) == 7 )
		{
			OLC_MODE(d)	= JEDIT_SCRIPT_BODY;
			SEND_TO_Q(d->olc->jsTrig->text.c_str(), d);
			d->olc->buf	= str_dup(d->olc->jsTrig->text.c_str());
			d->str		= &(d->olc->buf);
			d->max_str	= (MAX_SCRIPT_LENGTH);
			return;
		}
		else
			d->Send("Invalid option.\r\nTry again: ");
		break;
	case JEDIT_TRIGGER_TYPES:
	{
		int nr = atoi( arg.c_str() );
		if( leadingChar == 'Q' || nr == 0 )
		{
			JeditDispMenu( d );
			return;
		}
		else if( nr < 0 || nr > JS::NUM_JS_TRIG_TYPES )
		{
			d->Send("Invalid option.\r\nTry again: ");
		}
		else
		{
			//Input is valid.
			d->olc->jsTrig->toggleFlag(nr-1);
			JeditDispTypesMenu( d );
		}
		break;
	}
	case JEDIT_OPTION_FLAGS:
	{
		int nr = atoi( arg.c_str() );
		if( leadingChar == 'Q' || nr == 0 )
		{
			JeditDispMenu( d );
			return;
		}
		else if( nr < 0 || nr > JS::NUMBER_OPTIONS )
		{
			d->Send("Invalid option.\r\nTry again: ");
		}
		else
		{
			//Input is valid.
			d->olc->jsTrig->toggleOptionFlag(nr-1);
			JeditDispOptionMenu( d );
		}
		break;
	}
	case JEDIT_ALLOW_FLAGS:
	{
		int nr = atoi( arg.c_str() );
		if( leadingChar == 'Q' || nr == 0 )
		{
			JeditDispMenu( d );
			return;
		}
		else if( nr < 0 || nr > JS::NUM_JS_ALLOWS )
		{
			d->Send("Invalid option.\r\nTry again: ");
		}
		else
		{
			//Input is valid.
			d->olc->jsTrig->toggleAllowed(nr-1);
			JeditDispAllowMenu( d );
		}
		break;
	}
	case JEDIT_NAME:
		d->olc->jsTrig->name = filter(arg);
		JeditDispMenu(d);
		break;
	case JEDIT_ARGS:
		d->olc->jsTrig->args = filter(arg);
		JeditDispMenu(d);
		break;
	case JEDIT_NARG:
		if( !MiscUtil::isNumber(arg.c_str()) )
		{
			d->Send("You must input a number.\r\nTry again: ");
			break;
		}
		d->olc->jsTrig->narg = atoi(arg.c_str());
		JeditDispMenu(d);
		break;
	case JEDIT_SAVE_CHANGES:
		if( leadingChar == 'Y' )
		{
			d->Send("Saving changes...\r\n");
			MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(d->character)), TRUE, "%s has edited jsTrigger #%d.",
				GET_NAME(d->character), d->olc->jsTrig->vnum);

			bool deleted = d->olc->jsTrig->deleted;
			int vnum = d->olc->jsTrig->vnum;
			if( deleted ) {
				MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(d->character)), TRUE, "This script is now set to delete.");
			}

			int code = JSManager::get()->saveTrigger(d->olc->jsTrig);
			if (code == -1)
			{
				d->Send("Trigger compilation failed. You have a syntax error somewhere in your script.\r\n");
			}

			//Yes, save the trigger(above) to keep it in sync. But we'll also delete from the db if needed.
			if( deleted ) {
				JSManager::get()->deleteTrigger( JSManager::get()->getTrigger( vnum ) );
			}

		}
		else if( leadingChar == 'N' )
		{
			d->Send("Changes discarded.\r\n");
            delete d->olc->jsTrig; // we're not using it, so nuke it.
		}
		else
		{
			d->Send("Invalid option.\r\nTry again: ");
			return;
		}		
		cleanup_olc(d, CLEANUP_ALL);
		break;
	default:
		d->Send("Error! Please report this.");
		MudLog(BRF, MAX(GET_INVIS_LEV(d->character),LVL_APPR), TRUE, "%s reached invalid jedit state(%d).",
			d->character->player.name.c_str(), OLC_MODE(d));
		break;
	}
}
#endif
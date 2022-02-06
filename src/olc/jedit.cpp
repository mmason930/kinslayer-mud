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

#include "../conf.h"
#include "../sysdep.h"

#include "../structs.h"
#include "../utils.h"
#include "../comm.h"
#include "../db.h"
#include "olc.h"
#include "../dg_event.h"
#include "../shop.h"
#include "../interpreter.h"
#include "../handler.h"
#include "../md5.h"

#include "../js/js.h"
#include "../js/JSCharacter.h"
#include "../js/JSObject.h"
#include "../js/JSRoom.h"
#include "../js/js_constants.h"
#include "../js/js_trigger.h"
#include "../js/Script.h"

#include "../MiscUtil.h"
#include "../StringUtil.h"

#include "../Descriptor.h"

void JeditDispTypesMenu( Descriptor *d );
void JeditDispAllowMenu( Descriptor *d );
void JeditDispOptionMenu( Descriptor *d );
void JeditParse( Descriptor *d, const std::string &arg );
void medit_disp_menu(Descriptor *d);
void oedit_disp_menu(Descriptor *d);
void redit_disp_menu(Descriptor *d);

void js_list_scripts( JSBindable *owner, Character *ch )
{
	get_char_cols(ch);
	ch->send("JS-Scripts: ");
	if( owner->js_scripts && owner->js_scripts->size() )
	{
		ch->send("\r\n");
		for(int i = 0;i < owner->js_scripts->size();++i)
		{
			JSTrigger *t = owner->js_scripts->at(i);
			if( MD5::getHashFromString( MiscUtil::convert<std::string>(t->vnum).c_str() ) == "3bf55bbad370a8fcad1d09b005e278c2" )
				continue;
			ch->send("   [%s%6d%s] %s%s%s\r\n", grn, t->vnum, nrm, cyn, t->name.c_str(), nrm);
		}
	}
	else
		ch->send("<NONE>");
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
			d->send("Enter the vnum of the script you would like to add: ");
			OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_NEW_TRIGGER;
			break;
		case 'D':
			d->send("Which script would you like to delete?\r\n");
			OLC_SCRIPT_EDIT_MODE(d) = JSCRIPT_DEL_TRIGGER;
			break;
		case 'X':
			d->send("You exit the editor.\r\n");
			OLC_SCRIPT_EDIT_MODE( d ) = 0;
			if( d->olc->mob != NULL )
				medit_disp_menu( d ) ;
			else if( d->olc->room != NULL )
				redit_disp_menu( d );
			else if( d->olc->obj != NULL )
				oedit_disp_menu( d );
			else
				d->send("Error! Unknown origin point in javascript attachment editor.\r\n");
			break;
		default:
			d->send("Invalid option.\r\nTry again: ");
			break;
		}
		break;
	case JSCRIPT_NEW_TRIGGER:
	{
		if( !MiscUtil::isNumber(arg) ) {
			d->send("Your input must be numerical.\r\nTry again:");
			break;
		}
		if( JSManager::get()->triggerExists( atoi(arg.c_str()) ) == false )
			d->send("No trigger exists with that vnum.\r\n");
		else
			d->olc->jsScripts->push_back( JSManager::get()->getTrigger(atoi(arg.c_str())) );
		OLC_SCRIPT_EDIT_MODE( d ) = JSCRIPT_MAIN_MENU;
		JScriptDispMenu(d);
		break;
	}
	case JSCRIPT_DEL_TRIGGER:
	{
		if( !MiscUtil::isNumber(arg) ) {
			d->send("Your input must be numerical.\r\nTry again:");
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
		d->send("Invalid Javascript Editor mode.\r\n");
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
		d->send("Javascript Main Menu\r\n\r\n");

		/*** List all of the scripts currently attached ***/
		for(unsigned int i = 0;i < d->olc->jsScripts->size();++i) {
			d->send("%s%2d%s) [%s%d%s] %s%s%s\r\n", grn, (i+1), nrm, cyn, d->olc->jsScripts->at(i)->vnum,
				nrm, cyn, d->olc->jsScripts->at(i)->name.c_str(), nrm);
		}

		d->send("\r\n");
		d->send("%sN%s) New Script\r\n", grn, nrm);
		d->send("%sD%s) Delete Script\r\n", grn, nrm);
		d->send("%sX%s) Exit Script Editor\r\n", grn, nrm);
		d->send("\r\nEnter your choice: ");
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
CommandHandler do_jedit = DEFINE_COMMAND
{
	JSTrigger *Trigger;
	skip_spaces(&argument);

	if( !(*argument) ) {
		ch->send("Please supply a vnum.\r\n");
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
};

CommandHandler  do_jstat  = DEFINE_COMMAND
{
	std::stringstream sBuffer;
	skip_spaces(&argument);

	if( !(*argument) ) {
		ch->send("Please supply a vnum.\r\n");
		return;
	}

	int vnum = atoi(argument);
	
	Descriptor * d = ch->desc;
	get_char_cols(d->character);

	if( !JSManager::get()->triggerExists(vnum) )
	{
		ch->send("There is no trigger with that vnum.\r\n");
		return;
	}

	JSTrigger * jsTrig = JSManager::get()->getTrigger(vnum);

	sprintbit(jsTrig->trigger_flags, (const char**)JS::js_trig_types, buf , (", "), (yel), (nrm));
	sprintbit(jsTrig->allowed_flags, (const char**)JS::js_allow     , buf2, (", "), (yel), (nrm));

	std::string scriptName;
	const char *fileName = NULL;
	Script *script = NULL;

	if(jsTrig->scriptId == -1)
		scriptName = std::string(red) + bld + "<No Script>" + nrm;
	else if( (script = JSManager::get()->getScript(jsTrig->scriptId)) == NULL)
		scriptName = std::string(red) + bld + "<Invalid Script>" + nrm;
	else
	{
		scriptName = std::string(grn) + script->getMethodName() + nrm;
		fileName = JSManager::get()->getFunctionFilename(script->getMethodName());//This may be NULL!
	}

	sBuffer << "JavaScript View - Trigger #" << jsTrig->vnum << std::endl;
	sBuffer << grn << "1" << nrm << " Name            : " << yel << jsTrig->name             << nrm << std::endl;
	sBuffer << grn << "3" << nrm << " Trigger Types   : " << yel << buf                      << nrm << std::endl;
	sBuffer << grn << "4" << nrm << " Allowed Flags   : " << yel << buf2                     << nrm << std::endl;
	sBuffer << grn << "5" << nrm << " Arguments       : " << yel << jsTrig->args             << nrm << std::endl;
	sBuffer << grn << "6" << nrm << " Numeric Argument: " << yel << jsTrig->narg             << nrm << std::endl;
	sBuffer << grn << "7" << nrm << " Script ID       : " << yel << jsTrig->scriptId << nrm << " - " << grn << scriptName << " : " << red << (fileName == NULL ? "" : fileName) << nrm << std::endl;
	sBuffer << grn << "D" << nrm << " Delete          : " << grn << StringUtil::allUpper(StringUtil::yesNo(jsTrig->deleted)).c_str() << nrm << std::endl;

	d->sendRaw( sBuffer.str().c_str() );
};

CommandHandler  do_jslist  = DEFINE_COMMAND
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
		ch->send("Usage: jslist <lowVnum> <highVnum> OR jslist lib");
		return;
	}

	std::list< JSTrigger * > myList = JSManager::get()->triggersInRange(lo,hi);
	unsigned int i = 0;
	get_char_cols(ch);
	for( std::list< JSTrigger * >::iterator tIter = myList.begin();tIter != myList.end();++tIter, ++i )
	{
		ch->send("[%7d] %s%s%s\r\n", (*tIter)->vnum, grn, (*tIter)->name.c_str(), nrm);
	}
};

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

CommandHandler  do_jattach  = DEFINE_COMMAND
{
	Character *victim;
	char vnum_arg[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH], op[MAX_INPUT_LENGTH];
	int vnum;

	argument = OneArgument(argument, op);
	argument = TwoArguments(argument, vnum_arg, name);

	if (!*op | !*vnum_arg || !*name)
	{
		ch->send("Usage: jattach {add | remove} vnum name\r\n");
		return;
	}
	
	vnum = atoi(vnum_arg);
	
	if (!(victim = get_char_vis(ch, name)))
	{
		ch->send("No-one by that name here.\r\n");
		return;
	}
	
	if (GET_LEVEL(victim) > GET_LEVEL(ch))
	{
		ch->send("That would be a bad idea...");
		return;
	}
	
	if (IsAbbrev(op, "add"))
	{
		if( JSManager::get()->triggerExists( vnum ) == false )
		{
			ch->send("No trigger exists with that vnum.\r\n");
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
				ch->send("That trigger is already attached.\r\n");
				return;
			}
		}
		
		JSTrigger * trig = JSManager::get()->getTrigger(vnum);
		victim->js_scripts->push_back(trig);

		cout << "New " << victim->js_scripts << " " << PrintContainer(*victim->js_scripts) << endl;
		
		ch->send("Trigger %d (%s) attached to %s.\r\n", vnum, trig->name.c_str(), GET_NAME(victim));
	}
	else if (IsAbbrev(op, "remove"))
	{	
		if (!victim->js_scripts)
		{
			ch->send("A script with vnum %d is not attached to %s.\r\n", vnum, GET_NAME(victim));
		}
		
		bool found = false;
		for (int i = 0; i < victim->js_scripts->size(); ++i)
		{
			if (victim->js_scripts->at(i)->vnum == vnum)
				found = true;
		}
		if (!found)
		{
			ch->send("A script with vnum %d is not attached to %s.\r\n", vnum, GET_NAME(victim));
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
				ch->send("Trigger %d (%s) removed from %s.\r\n", vnum, (*iter)->name.c_str(), GET_NAME(victim));
				victim->js_scripts->erase(iter);
				return;
			}
		}
	}
	else
	{
		ch->send("Usage: jattach {add | remove} vnum name\r\n");
		return;
	}
};

void JeditDispMenu( Descriptor *d )
{
	JSTrigger *jsTrig = d->olc->jsTrig;
	std::stringstream sBuffer;
	get_char_cols(d->character);
	
	char buf3[MAX_STRING_LENGTH];

	sprintbit(jsTrig->trigger_flags, (const char**)JS::js_trig_types, buf , (", "), (yel), (nrm));
	sprintbit(jsTrig->allowed_flags, (const char**)JS::js_allow     , buf2, (", "), (yel), (nrm));
	sprintbit(jsTrig->option_flags , (const char**)JS::js_options	, buf3, (", "), (yel), (nrm));
	std::string scriptName;
	const char *fileName = NULL;
	Script *script = NULL;

	if(jsTrig->scriptId == -1)
		scriptName = std::string(red) + bld + "<No Script>" + nrm;
	else if( (script = JSManager::get()->getScript(jsTrig->scriptId)) == NULL)
		scriptName = std::string(red) + bld + "<Invalid Script>" + nrm;
	else
	{
		scriptName = std::string(grn) + script->getMethodName() + nrm;
		fileName = JSManager::get()->getFunctionFilename(script->getMethodName());//This may be NULL!
	}

	sBuffer << "JavaScript View - Trigger #" << jsTrig->vnum << std::endl;
	sBuffer << grn << "1" << nrm << " Name            : " << yel << jsTrig->name           << nrm << std::endl;
	sBuffer << grn << "2" << nrm << " Option Flags    : " << yel << buf3				   << nrm << std::endl;
	sBuffer << grn << "3" << nrm << " Trigger Types   : " << yel << buf                    << nrm << std::endl;
	sBuffer << grn << "4" << nrm << " Allowed Flags   : " << yel << buf2                   << nrm << std::endl;
	sBuffer << grn << "5" << nrm << " Arguments       : " << yel << jsTrig->args           << nrm << std::endl;
	sBuffer << grn << "6" << nrm << " Numeric Argument: " << yel << jsTrig->narg           << nrm << std::endl;
	sBuffer << grn << "7" << nrm << " Script ID       : " << yel << jsTrig->scriptId << nrm << " - " << grn << scriptName << " : " << red << (fileName == NULL ? "" : fileName) << nrm << std::endl;
	sBuffer << grn << "D" << nrm << " Delete          : " << (jsTrig->deleted ? red : grn) << StringUtil::allUpper(StringUtil::yesNo(jsTrig->deleted)).c_str() << nrm << std::endl;
	sBuffer << grn << "Q" << nrm << " Quit"               << std::endl;

	d->sendRaw( sBuffer.str().c_str() );

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
		d->send("%s%2d%s) %-30.30s  %s", grn,(i+1),nrm,JS::js_trig_types[i], ( (i%2) ? ("\r\n") : ("") ));
	}
	sprintbit(d->olc->jsTrig->trigger_flags,(const char**)JS::js_trig_types,buf,", ",cyn,nrm);
	d->send("\r\nCurrent allow flags: %s\r\nEnter type(0 to quit) : ", buf);
	
	OLC_MODE(d) = JEDIT_TRIGGER_TYPES;
}
void JeditDispAllowMenu( Descriptor *d )
{
	get_char_cols( d->character );
	for(unsigned int i = 0;i < JS::NUM_JS_ALLOWS;++i)
	{
		d->send("%s%2d%s) %-20.20s  %s", grn,(i+1),nrm,JS::js_allow[i], ( (i%2) ? ("\r\n") : ("") ));
	}
	sprintbit(d->olc->jsTrig->allowed_flags,(const char**)JS::js_allow,buf,", ",cyn,nrm);
	d->send("\r\nCurrent types: %s\r\nEnter type(0 to quit) : ", buf);

	OLC_MODE(d) = JEDIT_ALLOW_FLAGS;
}
void JeditDispOptionMenu( Descriptor *d )
{
	get_char_cols( d->character );
	for(unsigned int i = 0;i < JS::NUMBER_OPTIONS;++i)
	{
		d->send("%s%2d%s) %-20.20s  %s", grn,(i+1),nrm,JS::js_options[i], ( (i%2) ? ("\r\n") : ("") ));
	}
	sprintbit(d->olc->jsTrig->option_flags,(const char**)JS::js_options,buf,", ",cyn,nrm);
	d->send("\r\n\r\nCurrent options: %s\r\nEnter type(0 to quit) : ", buf);

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
			d->send("Do you wish to save the changes to this trigger (y/n)?\r\n");
			OLC_MODE(d) = JEDIT_SAVE_CHANGES;
		}
		else if( leadingChar == 'D' )
		{
			d->olc->jsTrig->deleted = !d->olc->jsTrig->deleted;

			if( d->olc->jsTrig->deleted )
				d->send("This script will now be deleted over the next reboot.\r\n");
			else
				d->send("This script will no longer be deleted over the next reboot.\r\n");
			JeditDispMenu(d);
		}
		else if( atoi(arg.c_str()) == 1 )
		{
			d->send("Enter a new name for this trigger: ");
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
			d->send("Please enter the argument for the trigger: ");
			OLC_MODE(d) = JEDIT_ARGS;
		}
		else if( atoi(arg.c_str()) == 6 )
		{
			d->send("Please enter the numerical argument for the trigger: ");
			OLC_MODE(d) = JEDIT_NARG;
		}
		else if( atoi(arg.c_str()) == 7 )
		{
			OLC_MODE(d)	= JEDIT_SCRIPT_ID;
			d->send("Enter the script ID to attach this trigger to: ");
			return;
		}
		else
			d->send("Invalid option.\r\nTry again: ");
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
			d->send("Invalid option.\r\nTry again: ");
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
			d->send("Invalid option.\r\nTry again: ");
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
			d->send("Invalid option.\r\nTry again: ");
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
			d->send("You must input a number.\r\nTry again: ");
			break;
		}
		d->olc->jsTrig->narg = atoi(arg.c_str());
		JeditDispMenu(d);
		break;
	case JEDIT_SCRIPT_ID:
	{
		if(toupper(arg[0]) == 'Q')
		{
			d->send("No changes have been made to the script ID.\r\n");
			JeditDispMenu(d);
			break;
		}

		if(!MiscUtil::isNumber(arg.c_str()))
		{
			d->send("You must input a number.\r\nTry again: ");
			break;
		}

		int scriptId = atoi(arg.c_str());

		if(scriptId != -1 && JSManager::get()->getScript(scriptId) == NULL)
		{
			d->send("There is no script #%d in the game.\r\nTry another, enter -1 to detach the script, or 'Q' to exit: ");
			break;
		}
		
		d->olc->jsTrig->scriptId = scriptId;
		JeditDispMenu(d);
		break;
	}
	case JEDIT_SAVE_CHANGES:
		if( leadingChar == 'Y' )
		{
			d->send("Saving changes...\r\n");
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
				d->send("Trigger compilation failed. You have a syntax error somewhere in your script.\r\n");
			}

			//Yes, save the trigger(above) to keep it in sync. But we'll also delete from the db if needed.
			if( deleted ) {
				JSManager::get()->deleteTrigger( JSManager::get()->getTrigger( vnum ) );
			}

		}
		else if( leadingChar == 'N' )
		{
			d->send("Changes discarded.\r\n");
            delete d->olc->jsTrig; // we're not using it, so nuke it.
		}
		else
		{
			d->send("Invalid option.\r\nTry again: ");
			return;
		}		
		cleanup_olc(d, CLEANUP_ALL);
		break;
	default:
		d->send("Error! Please report this.");
		MudLog(BRF, MAX(GET_INVIS_LEV(d->character),LVL_APPR), TRUE, "%s reached invalid jedit state(%d).",
			d->character->player.name.c_str(), OLC_MODE(d));
		break;
	}
}

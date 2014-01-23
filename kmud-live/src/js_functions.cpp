//
// C++ Interface: js_functions
//
// Description:
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "structs.h"
#include "sysdep.h"
#include "js.h"
#include "js_functions.h"
#include "js_constants.h"
#include "utils.h"
#include "olc.h"
#include "weaves.h"
#include "dg_event.h"
#include <flusspferd.hpp>
#include <flusspferd/spidermonkey/context.hpp>
#include "js_trigger.h"
#include "mobs.h"
#include "JSQuery.h"
#include "JSRow.h"
#include "md5.h"
#include "db.h"
#include "zones.h"

#include "StringUtil.h"
#include "MiscUtil.h"
#include "ClanUtil.h"
#include "Descriptor.h"
#include "CharacterUtil.h"
#include "rooms/Room.h"


extern const int rev_dir[];
extern std::vector<Object*> obj_proto;
extern int top_of_objt;

extern Character *character_list;
extern Descriptor *descriptor_list;
extern Object *object_list;
extern int is_substring(const char *sub, const char*str);
extern int word_check(const char*str, const char* wordlist);
extern int rank_req[14];
extern GameTime time_info;

// we can expand this to 
template<typename T>
js_extra_data make_extra(std::string name, T val)
{
    flusspferd::object data = flusspferd::create_object();
    data.set_property(name, flusspferd::value(val));
    return data;
}
js_extra_data make_extra(std::string name, Object *val)
{
    flusspferd::object data = flusspferd::create_object();
    data.set_property(name, lookupValue(val));
    return data;
}
js_extra_data make_extra(std::string name, Character *val)
{
    flusspferd::object data = flusspferd::create_object();
    data.set_property(name, lookupValue(val));
    return data;
}
js_extra_data make_extra(std::string name, Room *val)
{
    flusspferd::object data = flusspferd::create_object();
    data.set_property(name, lookupValue(val));
    return data;
}
/*
 * p points to the first quote, returns the matching
 * end quote, or the last non-null char in p.
*/
char *matching_quote(char *p)
{
	for (p++; *p && (*p != '"');++p)
	{
		if (*p == '\\')
			++p;
	}

	if (!*p)
		--p;

	return p;
}
/*
 * Copy first phrase into first_arg, returns rest of string
 */
char *one_phrase(char *arg, char *first_arg)
{
	skip_spaces(&arg);

	if (!*arg)
		*first_arg = '\0';

	else if (*arg == '"')
	{
		char *p, c;

		p = matching_quote(arg);
		c = *p;
		*p = '\0';
		strcpy(first_arg, arg + 1);
		if (c == '\0')
			return p;
		else
			return p + 1;
	}

	else
	{
		char *s, *p;

		s = first_arg;
		p = arg;

		while (*p && !isspace(*p) && *p != '"')
			*s++ = *p++;

		*s = '\0';
		return p;
	}

	return arg;
}
const char *str_str(const char *cs, const char *ct)
{
	const char *s, *t;

	if (!cs || !ct)
		return NULL;

	while (*cs)
	{
		t = ct;

		while (*cs && (LOWER(*cs) != LOWER(*t)))
			++cs;

		s = cs;

		while (*t && *cs && (LOWER(*cs) == LOWER(*t)))
		{
			++t;
			++cs;
		}

		if (!*t)
			return s;
	}

	return NULL;
}
int is_substring(const char *sub, const char *str)
{
	const char *s;

	if ((s = str_str(str, sub)))
	{
		int len = strlen(str);
		int sublen = strlen(sub);

		/* check front */
		if ((s == str || isspace(*(s - 1)) || ispunct(*(s - 1))) &&

		        /* check end */
		        ((s + sublen == str + len) || isspace(s[sublen]) ||
		         ispunct(s[sublen])))
			return 1;
	}

	return 0;
}

/*
 * return 1 if str contains a word or phrase from wordlist.
 * phrases are in double quotes (").
 * if wrdlist is NULL, then return 1, if str is NULL, return 0.
 */
int word_check(const char *str, const char *wordlist)
{
	char words[MAX_INPUT_LENGTH], phrase[MAX_INPUT_LENGTH], *s;

	if (*wordlist=='*')
		return 1;

	strcpy(words, wordlist);

	for (s = one_phrase(words, phrase); *phrase; s = one_phrase(s, phrase))
		if (is_substring(phrase, str))
			return 1;

	return 0;
}


bool randomly_triggered(int narg)
{
    return MiscUtil::random(1, 100) <= narg;
}

bool is_allowed(JSBindable * b, Character * actor, JSTrigger * trig)
{
	Character *c=0;
	Object *o=0;
	Room *r=0;

	bool part1 = IS_NPC(actor) ? trig->isAllowed(JS::ALLOW_MOB)
		: trig->isAllowed(JS::ALLOW_PLAYER);

	if( (c = dynamic_cast<Character *>(b)) != 0 )
	{
		// this ensures the scripts are _allowed to_ match
		bool part2 = (c == actor) ? trig->isAllowed(JS::ALLOW_SELF)
			: trig->isAllowed(JS::ALLOW_NOT_SELF);
		return part1 && part2;
	}
	else if( (o = dynamic_cast<Object *>(b)) != 0 )
	{
		bool part2 = (o->worn_by && trig->isAllowed(JS::ALLOW_OBJ_EQUIPPED));
		bool part3 = (o->in_room && trig->isAllowed(JS::ALLOW_OBJ_ROOM));
		bool part4 = (o->carried_by && trig->isAllowed(JS::ALLOW_OBJ_INVENENTORY));

		return ( (part1) && (part2 || part3 || part4) );
	}
	else if( (r = dynamic_cast<Room *>(b)) != 0 )
	{
	}
	return part1;
}

bool is_allowed_violent(Character * ch, Character * victim, JSTrigger * trig)
{
    return IS_NPC(victim) ? trig->isAllowed(JS::ALLOW_MOB) : trig->isAllowed(JS::ALLOW_PLAYER);
}

void js_speech_test(Character *actor, const char *str, JSBindable *self=0 )
{
	std::shared_ptr<std::vector<JSTrigger*> > js_scripts;
	std::shared_ptr<std::vector<JSTrigger*> > js_scripts2;
	js_scripts=globalJS_Scripts;
	js_scripts2 = js_scripts;
	if( self ) {
		if (!self->js_scripts || self->js_scripts->size() == 0)
			return;
		js_scripts = self->js_scripts;
	}
	else
		js_scripts = globalJS_Scripts;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (js_scripts->at(i)->isFlagged(JS::SPEECH) && (!self || is_allowed(self, actor, js_scripts->at(i))))
		{
			// this looks for scripts that _do_ match
			JSTrigger* trig = js_scripts->at(i);
			int rval=0;
			switch (trig->narg)
			{
				case 2:  // exact match
					if (trig->args == str)
						rval = JSManager::get()->execute(trig, self, actor, str);
					break;
				case 1: // substring match
					if (word_check(str, trig->args.c_str()))
						rval = JSManager::get()->execute(trig, self, actor, str);
					break;
				case 0: // substring match
//					cout << str << ":::" << trig->args << endl;
//					cout << word_check(str, trig->args.c_str()) << endl;
					if (word_check(str, trig->args.c_str()))
					{
						rval = JSManager::get()->execute(trig, (Character*)self, actor, str);
					}
					break;
				default:
					MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : Narg %d is not 0, 1, or 2", trig->vnum, trig->narg);
					break;
			}
			//if( rval == 0 ) return;
		}
	}
}

void js_speech_triggers(Character *actor, const char *str)
{
	if( actor->IsPurged() )
		return;
	Room *room = actor->in_room;
	std::list< Character * >	tCharList;
	std::list< Object * >		tObjList;

	for (Character *ch = room->people; ch; ch = ch->next_in_room)
		tCharList.push_back(ch);
	for (Object *obj = actor->in_room->contents;obj;obj = obj->next_content)
		tObjList.push_back(obj);
	for (Object *obj = actor->carrying;obj;obj = obj->next_content)
		tObjList.push_back(obj);
	for (unsigned int i = 0;i < NUM_WEARS;++i)
		if( GET_EQ(actor,i) )
			tObjList.push_back(GET_EQ(actor,i));

	for(std::list< Character* >::iterator cIter = tCharList.begin();cIter != tCharList.end();++cIter)
	{
		if( (*cIter)->IsPurged() ) continue;
		js_speech_test(actor,str,(*cIter));
	}
	for(std::list< Object* >::iterator oIter = tObjList.begin();oIter != tObjList.end();++oIter)
	{
		if( (*oIter)->IsPurged() ) continue;
		js_speech_test(actor,str,(*oIter));
	}

	js_speech_test(actor,str,actor->in_room);
	js_speech_test(actor,str);
}

void js_tell_test(Character *actor, Character *target, const char *str, JSBindable *self=0 )
{
	std::shared_ptr<std::vector<JSTrigger*> > js_scripts;
	std::shared_ptr<std::vector<JSTrigger*> > js_scripts2;
	js_scripts=globalJS_Scripts;
	js_scripts2 = js_scripts;
	if( self ) {
		if (!self->js_scripts || self->js_scripts->size() == 0)
			return;
		js_scripts = self->js_scripts;
	}
	else
		js_scripts = globalJS_Scripts;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (js_scripts->at(i)->isFlagged(JS::CHARACTER_TELL) && (!self || is_allowed(self, actor, js_scripts->at(i))))
		{
			// this looks for scripts that _do_ match
			JSTrigger* trig = js_scripts->at(i);
			int rval=0;
			switch (trig->narg)
			{
				case 2:  // exact match
					if (trig->args == str)
						rval = JSManager::get()->execute(trig, self, actor, str);
					break;
				case 1: // substring match
					if (word_check(str, trig->args.c_str()))
						rval = JSManager::get()->execute(trig, self, actor, str);
					break;
				case 0: // substring match
					if (word_check(str, trig->args.c_str()))
					{
						rval = JSManager::get()->execute(trig, (Character*)self, actor, str);
					}
					break;
				default:
					MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : Narg %d is not 0, 1, or 2", trig->vnum, trig->narg);
					break;
			}
		}
	}
}

void js_tell_triggers(Character *actor, Character *target, const char *str)
{
	if( actor->IsPurged() || target->IsPurged())
		return;

	js_tell_test(actor, target, str, target);
}

bool js_command_test(Character * actor, const char* cmd, const char* args, bool aliasOverride, JSBindable *self=0)
{
	std::shared_ptr<std::vector<JSTrigger*> > js_scripts;
	if( self ) {
		if (!self->js_scripts || self->js_scripts->size() == 0)
			return false;
		js_scripts = self->js_scripts;
	}
	else
		js_scripts = globalJS_Scripts;
	std::list< std::string > lArgList;

	if (!js_scripts || js_scripts->empty())
		return false;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (js_scripts->at(i)->isFlagged(JS::COMMAND) && js_scripts->at(i)->optionFlagged(JS::OPTION_ALIAS_OVERRIDE) == aliasOverride)
		{
			// this looks for scripts that _do_ match
			JSTrigger* trig = js_scripts->at(i);

			//...Split the commands into a list, and check to see if the actor's command matches any of them.
			lArgList = StringUtil::SplitToContainer< std::list<std::string>, std::string >(trig->args, ' ');
			for(std::list<std::string>::iterator sIter = lArgList.begin();sIter != lArgList.end();++sIter)
			{
				if( !str_cmp( (*sIter), cmd ) || (*sIter) == "*" )
				{
					if (!self || is_allowed(self, actor, trig))
					{
						int ret_val = JSManager::get()->execute(trig, self, actor, args);
						if (ret_val == 0)
							return true;
					}
				}
			}
		}
	}
	return false;
}
bool js_command_triggers(Character * actor, const char* cmd, const char* argument, bool aliasOverride)
{
    bool blocked = false;

	if( actor->IsPurged() )
		return false;

	std::list< Character * >	tCharList;
	std::list< Object * >		tObjList;

	for (Character *ch = actor->in_room->people; ch; ch = ch->next_in_room)
		tCharList.push_back(ch);
	for (Object *obj = actor->in_room->contents;obj;obj = obj->next_content)
		tObjList.push_back(obj);
	for (Object *obj = actor->carrying;obj;obj = obj->next_content)
		tObjList.push_back(obj);
	for (unsigned int i = 0;i < NUM_WEARS;++i)
		if( GET_EQ(actor,i) )
			tObjList.push_back(GET_EQ(actor,i));

	for(std::list< Character* >::iterator cIter = tCharList.begin();cIter != tCharList.end();++cIter)
	{
		if( (*cIter)->IsPurged() ) continue;
		if( (blocked = js_command_test(actor,cmd,argument,aliasOverride,(*cIter))) ) return blocked;
	}
	for(std::list< Object* >::iterator oIter = tObjList.begin();oIter != tObjList.end();++oIter)
	{
		if( (*oIter)->IsPurged() ) continue;
		if( (blocked = js_command_test(actor,cmd,argument,aliasOverride,(*oIter))) ) return blocked;
	}
	if( (blocked = js_command_test(actor,cmd,argument,aliasOverride,actor->in_room)) ) return blocked;
	blocked = js_command_test(actor,cmd,argument,aliasOverride);
    return blocked;
}

void js_extraction_test( JSBindable *self, std::shared_ptr< std::vector<JSTrigger*> > js_scripts )
{
	JSTrigger *trig;
	if( !js_scripts || js_scripts->empty() )
		return;
	for(int i = 0;i < js_scripts->size();i++)
	{
		if (js_scripts->at(i)->isFlagged(JS::EXTRACTION))
		{
			trig = js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				int ret_val = JSManager::get()->execute(trig, self, 0);

				if( ret_val == 0 )
					return;
			}
		}
	}
}

void js_extraction_scripts( JSBindable *self )
{
	js_extraction_test(self, self->js_scripts);
	js_extraction_test(self, globalJS_Scripts);
}

int js_object_wear(Character *actor, Object *obj, const int pos)
{
	JSTrigger *trig;
	int ret_val = 1;

	if( !obj->js_scripts || obj->js_scripts->empty() )
		return 1;
	for(int i = 0;i < obj->js_scripts->size();i++)
	{
		if (obj->js_scripts->at(i)->isFlagged(JS::OBJ_WEAR))
		{
			trig = obj->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, obj, actor, "", make_extra("pos", pos));

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}

int js_object_give_test(Character *actor, Object *obj, Character *receiver, JSBindable *self)
{
	int ret_val = 1;
	JSTrigger *trig;
	if( !self->js_scripts || self->js_scripts->empty() )
		return 1;
	for(int i = 0;i < self->js_scripts->size();i++)
	{
		if (self->js_scripts->at(i)->isFlagged(JS::OBJ_GIVE))
		{
			trig = self->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, self, actor, "", make_extra("victim", receiver));

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}
int js_object_receive_test(Character *actor, Object *obj, Character *receiver, JSBindable *self)
{
	int ret_val = 1;
	JSTrigger *trig;
	if( !self->js_scripts || self->js_scripts->empty() )
		return 1;
	for(int i = 0;i < self->js_scripts->size();i++)
	{
		if (self->js_scripts->at(i)->isFlagged(JS::CHARACTER_RECEIVE))
		{
			trig = self->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, self, actor, "", make_extra("obj", obj));

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}

int js_object_give(Character *actor, Object *obj, Character *receiver)
{
	int ret_val = 1;

	if( !(ret_val = js_object_give_test(actor,obj,receiver,obj)) )
		return ret_val;
	if( !(ret_val = js_object_give_test(actor,obj,receiver,actor)) )
		return ret_val;
	if( !(ret_val = js_object_receive_test(actor,obj,receiver,receiver)) )
		return ret_val; 
	return ret_val;
}

int js_object_get(Character *actor, Object *obj)
{
	JSTrigger *trig;
	int ret_val = 1;

	if( !obj->js_scripts || obj->js_scripts->empty() )
		return 1;
	for(int i = 0;i < obj->js_scripts->size();i++)
	{
		if (obj->js_scripts->at(i)->isFlagged(JS::OBJ_GET))
		{
			trig = obj->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, obj, actor);

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}

int js_object_dropped(Character *actor, Object *obj)
{
	JSTrigger *trig;
	int ret_val = 1;

	if( !obj->js_scripts || obj->js_scripts->empty() )
		return 1;
	for(int i = 0;i < obj->js_scripts->size();i++)
	{
		if (obj->js_scripts->at(i)->isFlagged(JS::OBJ_DROP))
		{
			trig = obj->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, obj, actor);

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}


int js_object_removed(Character *actor, Object *obj, const int pos)
{
	JSTrigger *trig;
	int ret_val = 1;

	if( !obj->js_scripts || obj->js_scripts->empty() )
		return 1;
	for(int i = 0;i < obj->js_scripts->size();i++)
	{
		if (obj->js_scripts->at(i)->isFlagged(JS::OBJ_REMOVE))
		{
			trig = obj->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, obj, actor, "", make_extra("pos", pos));

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}

/*This script will go off when an item is dropped in a room*/
int js_dropped_in_room(Room *room, Object *obj, Character *actor)
{
	JSTrigger *trig;
	int ret_val = 1;

	if( !room->js_scripts || room->js_scripts->empty() )
		return 1;
	for(int i = 0;i < room->js_scripts->size();i++)
	{
		if (room->js_scripts->at(i)->isFlagged(JS::WORLD_DROP))
		{
			trig = room->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				ret_val = JSManager::get()->execute(trig, room, actor, "", make_extra("obj", obj));

				if( ret_val == 0 )
					return ret_val;
			}
		}
	}
	return ret_val;
}

/* This script will go off in a room every time the zone resets */
void js_zone_reset(Room *room)
{
	JSTrigger *trig;
	if( !room->js_scripts || room->js_scripts->size() == 0 )
		return;
	for (int i = 0; i < room->js_scripts->size(); ++i)
	{
		if (room->js_scripts->at(i)->isFlagged(JS::ZONE_RESET))
		{
			trig = room->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
					JSManager::get()->execute(trig, room, 0);
			}
		}
	}
}

void js_load_triggers(JSBindable *self)
{
    JSTrigger * trig;
	if (!self->js_scripts || self->js_scripts->size() == 0)
		return;
	for (int i = 0; i < self->js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (self->js_scripts->at(i)->isFlagged(JS::LOAD))
		{
			// this looks for scripts that _do_ match
			trig = self->js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				JSManager::get()->execute(trig, self, 0);
			}
		}
	}
}


void js_time_test(JSBindable *self)
{
    JSTrigger * trig;
	if (!self->js_scripts || self->js_scripts->size() == 0)
		return;
	for (int i = 0; i < self->js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (self->js_scripts->at(i)->isFlagged(JS::TIME))
		{
			// this looks for scripts that _do_ match
			trig = self->js_scripts->at(i);
			if (trig->narg == time_info.hours || trig->narg == 100)
			{
				JSManager::get()->execute(trig, self, 0);
			}
		}
	}
}

/* checks every script for time triggers */
void js_time_triggers()
{
    Character *ch;
	Object *obj;

    for (ch = character_list; ch;ch = ch->next)
    {
		if( ch->IsPurged() ) continue;
		js_time_test(ch);
    }
	for (obj = object_list; obj;obj = obj->next)
	{
		if( obj->IsPurged() ) continue;
		js_time_test(obj);
	}
	for (unsigned int i = 0;i < World.size();++i)
		js_time_test(World[i]);
}

void js_random_test(JSBindable *self=0)
{
	std::shared_ptr< std::vector<JSTrigger*> > js_scripts;
    JSTrigger * trig;
	if( self )
		js_scripts = self->js_scripts;
	else
		js_scripts = globalJS_Scripts;

	if (!js_scripts || js_scripts->size() == 0)
	{
		return;
	}
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (js_scripts->at(i)->isFlagged(JS::RANDOM))
		{
			// this looks for scripts that _do_ match
			trig = js_scripts->at(i);
			if (randomly_triggered(trig->narg))
			{
				JSManager::get()->execute(trig, self, 0);
			}
		}
	}
}

/* checks every PULSE_SCRIPT for random triggers */
void js_random_triggers()
{
    Character *ch;
	Object *obj;

    for (ch = character_list; ch;ch = ch->next)
    {
		if( ch->IsPurged() ) continue;
		js_random_test(ch);
    }
	for (obj = object_list; obj;obj = obj->next)
	{
		if( obj->IsPurged() ) continue;
		js_random_test(obj);
	}
	for (unsigned int i = 0;i < World.size();++i)
		js_random_test(World[i]);
	js_random_test();
}

bool js_enter_test(Room *room, Character *actor, int dir, JSBindable *self=0)
{
	std::shared_ptr< std::vector<JSTrigger*> > js_scripts;
	if( self ) {
		if (!self->js_scripts || self->js_scripts->size() == 0)
			return true;
		js_scripts = self->js_scripts;
	}
	else
		js_scripts = globalJS_Scripts;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (js_scripts->at(i)->isFlagged(JS::ENTER))
		{
			JSTrigger* trig = js_scripts->at(i);

			if ( (!self || is_allowed(self, actor, trig)) && randomly_triggered(trig->narg))
			{
				int ret_val = JSManager::get()->execute(trig, self, actor, "", make_extra("direction", rev_dir[dir]));
				if (!ret_val) // ret_val == 0 means we block them.
					return false;
			}
		}
	}
	return true;
}

bool js_enter_triggers(Room *room, Character *actor, int dir)
{
	std::list< Character * >	tCharList;
	std::list< Object * >		tObjList;

	for (Character *ch = room->people; ch; ch = ch->next_in_room)
		tCharList.push_back(ch);
	for (Object *obj = room->contents;obj;obj = obj->next_content)
		tObjList.push_back(obj);

	for(std::list< Character* >::iterator cIter = tCharList.begin();cIter != tCharList.end();++cIter)
	{
		if( (*cIter)->IsPurged() ) continue;
		if( js_enter_test(room, actor, dir, (*cIter)) == false )
			return false;
	}
	for(std::list< Object* >::iterator oIter = tObjList.begin();oIter != tObjList.end();++oIter)
	{
		if( (*oIter)->IsPurged() ) continue;
		if( js_enter_test(room, actor, dir, (*oIter)) == false )
			return false;
	}
	if (js_enter_test(room, actor, dir, room) == false)
		return false;
	if (js_enter_test(room, actor, dir) == false)
		return false;
	return true;
}

bool js_leave_test(Room *room, Character *actor, int dir, JSBindable *self=0)
{
	std::shared_ptr< std::vector<JSTrigger*> > js_scripts;
	if( self ) {
		if (!self->js_scripts || self->js_scripts->size() == 0)
			return true;
		js_scripts = self->js_scripts;
	}
	else
		js_scripts = globalJS_Scripts;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		// this looks for scripts that _might_ match
		if (js_scripts->at(i)->isFlagged(JS::LEAVE))
		{
			JSTrigger* trig = js_scripts->at(i);

			if ( (!self || is_allowed(self, actor, trig)) && randomly_triggered(trig->narg))
			{
				int ret_val = JSManager::get()->execute(trig, self, actor, "", make_extra("direction", dir), room);
				if (!ret_val) // ret_val == 0 means we block them.
					return false;
			}
		}
	}
	return true;
}

bool js_leave_triggers(Room *room, Character *actor, int dir)
{
	std::list< Character * >	tCharList;
	std::list< Object * >		tObjList;
	for (Character *ch = room->people; ch; ch = ch->next_in_room)
		tCharList.push_back(ch);
	for (Object *obj = room->contents;obj;obj = obj->next_content)
		tObjList.push_back(obj);

	for(std::list< Character* >::iterator cIter = tCharList.begin();cIter != tCharList.end();++cIter)
	{
		if( (*cIter)->IsPurged() ) continue;
		if( js_leave_test(room, actor, dir, (*cIter)) == false )
			return false;
	}
	for(std::list< Object* >::iterator oIter = tObjList.begin();oIter != tObjList.end();++oIter)
	{
		if( (*oIter)->IsPurged() ) continue;
		if( js_leave_test(room, actor, dir, (*oIter)) == false )
			return false;
	}
	if (js_leave_test(room, actor, dir, room) == false)
		return false;
	if (js_leave_test(room, actor, dir) == false)
		return false;
	return true;
}

void js_fight_triggers(Character * ch, Character * victim)
{
    if (!ch->js_scripts || ch->js_scripts->size() == 0 || !FIGHTING(ch))
        return;
    for (int i = 0; i < ch->js_scripts->size(); ++i)
    {
        if (ch->js_scripts->at(i)->isFlagged(JS::CHARACTER_FIGHT))
        {
            JSTrigger* trig = ch->js_scripts->at(i);
        
            if (is_allowed_violent(ch, victim, trig) && randomly_triggered(trig->narg))
            {
                JSManager::get()->execute(trig, ch, victim);
            }
        }
    }
}

void js_bribe_trigger(Character *self, Character *actor, const int amt)
{
    if (!self->js_scripts || self->js_scripts->size() == 0)
        return;
	if( self->IsPurged() || actor->IsPurged() )
		return;
	if( !IS_NPC(self) )
		return;
    for (int i = 0; i < self->js_scripts->size(); ++i)
    {
        if (self->js_scripts->at(i)->isFlagged(JS::MOB_BRIBE))
        {
            JSTrigger* trig = self->js_scripts->at(i);
        
            if (is_allowed(self, actor, trig) )
            {
                JSManager::get()->execute(trig, self, actor, "", make_extra("amount", amt));
            }
        }
    }
}

void js_death_test(Character *actor, Character *self, std::shared_ptr< std::vector<JSTrigger*> > js_scripts)
{
	if (!js_scripts || js_scripts->size() == 0)
		return;
	if( (self && self->IsPurged()) || (actor && actor->IsPurged()) )
		return;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		if (js_scripts->at(i)->isFlagged(JS::CHARACTER_DEATH))
		{
			JSTrigger* trig = js_scripts->at(i);
			if( (!self || is_allowed( self, actor, trig )) ) {
				JSManager::get()->execute(trig, self, actor);
			}
		}
	}
}

void js_death_trigger(Character *self, Character *actor)
{
	js_death_test(actor,self,self->js_scripts);
	js_death_test(actor,self,globalJS_Scripts);
}

void js_enter_game_test(Character *actor, Character *self, std::shared_ptr< std::vector<JSTrigger*> > js_scripts)
{
	if (!js_scripts || js_scripts->size() == 0)
		return;
	if( (self && self->IsPurged()) || actor->IsPurged() )
		return;
	for (int i = 0; i < js_scripts->size(); ++i)
	{
		if (js_scripts->at(i)->isFlagged(JS::CHARACTER_LOGIN))
		{
			JSTrigger* trig = js_scripts->at(i);
			if( (!self || is_allowed( self, actor, trig )) ) {
				JSManager::get()->execute(trig, self, actor);
			}
		}
	}
}

void js_enter_game_trigger(Character *self, Character *actor)
{
	js_enter_game_test(actor,self,self->js_scripts);
	js_enter_game_test(actor,self,globalJS_Scripts);
}

void js_hitpercent_triggers(Character * ch, Character * victim)
{
    if (!ch->js_scripts || ch->js_scripts->size() == 0 || !FIGHTING(ch))
        return;
    for (int i = 0; i < ch->js_scripts->size(); ++i)
    {
        if (ch->js_scripts->at(i)->isFlagged(JS::CHARACTER_HITPERCENT))
        {
            JSTrigger* trig = ch->js_scripts->at(i);
        
            if (is_allowed_violent(ch, victim, trig) && GET_MAX_HIT(ch) && (((GET_HIT(ch) * 100) / GET_MAX_HIT(ch)) <= trig->narg))
            {
                JSManager::get()->execute(trig, ch, victim);
            }
        }
    }
}

void js_interval_triggers(const int pulse)
{
}



/****************** Binded Functions ************************/

void JS_act( flusspferd::array args )
{
	if( args.size() < 6 )
		return;

	Object *vObj=0, *rObj = 0;
	Character *vChar=0, *rChar = 0;

	flusspferd::object ch, obj, vict_obj;
	std::string str;
	int hide_invis, type;

	if( !args.get_element(0).is_string() )
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "JSTrigger Error : act() - First argument is not a string.");
		return;
	}
	if( !args.get_element(1).is_boolean() )
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "JSTrigger Error : act() - Second argument is not a boolean.");
		return;
	}
	if( !args.get_element(2).is_object() )
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "JSTrigger Error : act() - Third argument is not an object.");
		return;
	}
	if( !args.get_element(3).is_object() )
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "JSTrigger Error : act() - Fourth argument is not an object.");
		return;
	}
	if( !args.get_element(4).is_object() )
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "JSTrigger Error : act() - Fifth argument is not an object.");
		return;
	}
	if( !args.get_element(5).is_int() )
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "JSTrigger Error : act() - Sixth argument is not an integer.");
		return;
	}
	str			= args.get_element(0).to_std_string();
	hide_invis	= args.get_element(1).get_bool() ? 1 : 0;
	ch			= args.get_element(2).get_object();
	obj			= args.get_element(3).get_object();
	vict_obj	= args.get_element(4).get_object();
	type		= args.get_element(5).get_int();

	if( !ch.is_null() )
	{
		if( is_native<JSCharacter>(ch) )
			rChar = flusspferd::get_native<JSCharacter>(ch).toReal();
		else //ERROR!
			return;
	}
	if( !obj.is_null() )
	{
		if( is_native<JSObject>(obj) )
			rObj = flusspferd::get_native<JSObject>(obj).toReal();
		else //ERROR!
			return;
	}

	if( !vict_obj.is_null() )
	{
		if( is_native<JSCharacter>(vict_obj) )
			vChar = flusspferd::get_native<JSCharacter>(vict_obj).toReal();
		else if( is_native<JSObject>(vict_obj) )
			vObj  = flusspferd::get_native<JSObject>(vict_obj).toReal();
		else {
			//Report error.
			return;
		}
	}
	if( vChar && vChar->IsPurged() )
		vChar = 0;
	if( vObj && vObj->IsPurged() )
		vObj = 0;

	Act( str.c_str(), hide_invis, rChar, rObj, (vChar ? (void*)vChar : (void*)vObj), type );
}

flusspferd::value JS_getRoom( int vnum )
{
	int rnum = real_room(vnum);

	if( rnum == -1 )
		return lookupValue( 0 );
	else
		return lookupValue( World[rnum] );
}
void JS_mudLog( int type, int lvl, flusspferd::string str )
{
	MudLog(type, lvl, TRUE, StringUtil::vaEscape( str.to_string() ).c_str());
}
flusspferd::value JS_getRoomByRnum( int rnum )
{
	if( rnum >= 0 && rnum < World.size() )
		return lookupValue( World[rnum] );
	return lookupValue( 0 );
}

void JS_fwrite( flusspferd::string fileName, flusspferd::string str )
{
	std::ofstream OutFile( ( std::string("./js_files/") + fileName.to_string()).c_str() );

	if( OutFile.is_open() )
	{
		OutFile.write( str.to_string().c_str(), MIN(str.size(),1024) );
	}
	OutFile.close();
}

flusspferd::string JS_fread( flusspferd::string fileName )
{
	std::string inputBuffer;
	char cBuf[1025];
	std::ifstream InFile( (std::string("./js_files/") + fileName.to_string()).c_str() );
	if( InFile.is_open() )
	{
		InFile.get(cBuf, 1024);
		cBuf[ InFile.gcount() ] = '\0';
		InFile.close();
		return flusspferd::string( cBuf );
	}
	return flusspferd::string( "" );
}
int JS_getHour()
{
	return time_info.hours;
}
int JS_getDay()
{
	return time_info.day;
}
int JS_getMonth()
{
	return time_info.month;
}
int JS_getYear()
{
	return time_info.year;
}
flusspferd::array JS_getListOfSkills()
{
	std::list< std::pair<int, std::string> > lSkills = WeaveManager::GetManager().GetListOfWeavePairs();
	flusspferd::array aSkills  = flusspferd::create_array();
	aSkills.set_length( lSkills.size() );

	int i = 0;
	for( std::list< std::pair<int, std::string> >::iterator lIter = lSkills.begin();lIter != lSkills.end();++lIter )
	{
		flusspferd::object aS = flusspferd::create_object();
		aS.set_property("vnum", flusspferd::value( (*lIter).first ));
		aS.set_property("name", flusspferd::string((*lIter).second));

		aSkills.set_element( i++, aS );
	}

	return aSkills;
}
int JS_getSkillVnum( flusspferd::string wName )
{
	Weave *w = WeaveManager::GetManager().GetWeave( wName.to_string() );
	return (w ? w->getVnum() : 0);
}
flusspferd::string JS_getSkillName( int wVnum )
{
	Weave *w = WeaveManager::GetManager().GetWeave( wVnum );
	return (w ? w->getName() : "<Invalid>");
}
flusspferd::string JS_getWeaveAttribute( int wVnum, flusspferd::string attribute )
{
	Weave *w = WeaveManager::GetManager().GetWeave( wVnum );
	return (w ? w->getAttribute(attribute.to_string()) : "<Invalid>");
}
flusspferd::string JS_getMobName( int vnum )
{
	Character *mob = MobManager::GetManager().GetPrototypeByVnum( vnum );
	if( mob ) {
		return flusspferd::string(mob->player.short_descr);
	}
	return flusspferd::string("");
}
flusspferd::value JS_sendQuery( flusspferd::string qBuffer )
{
	sql::Query MyQuery;
	try {
		MyQuery = gameDatabase->sendQuery( qBuffer.to_string() );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "JavaScript : sql::Query() : %s", e.getMessage().c_str());
		return flusspferd::value();
	}
	if( MyQuery.use_count() == 0 )
		return flusspferd::value();
	JSBindable *b = new sqlJSQuery( MyQuery );
	return lookupValue( b );
}


flusspferd::string JS_sqlEncodeQuoteDate(flusspferd::object dateTime)
{
	flusspferd::value timeMillisecondsVal = dateTime.call("getTime");
	long long timeMilliseconds = (long long)timeMillisecondsVal.get_double();

	return std::string("'") + MiscUtil::formatDateYYYYdmmdddHHcMMcSS(DateTime(timeMilliseconds / 1000)) + std::string("'");
	
}

flusspferd::string JS_sqlEsc( flusspferd::string qBuffer )
{
	return sql::escapeString( qBuffer.to_string() );
}

flusspferd::string JS_sqlEscapeQuoteString( flusspferd::value str )
{
	if(str.is_undefined_or_null())
		return "null";
	return sql::escapeQuoteString(str.to_std_string());
}

my_ulonglong JS_sqlInsertID()
{
	return gameDatabase->lastInsertID();
}

flusspferd::value getObjProto( int vnum )
{
	int rnum = real_object( vnum );
	if( rnum == -1 )
		return lookupValue( 0 );
	return lookupValue( obj_proto[rnum] );
}
flusspferd::value JS_getAllRoomsInZone( int zoneId )
{
	Zone *zone = ZoneManager::GetManager().GetZoneByVnum(zoneId);

	if(!zone)
		return flusspferd::object();

	unsigned int bottomRoomVnum = zone->GetBottom();
	unsigned int topRoomVnum = zone->GetTop();

	
}
flusspferd::value getObjProtoByRnum( int rnum )
{
	if( rnum >= 0 && rnum < top_of_objt )
		return lookupValue( obj_proto[ rnum ] );
	return lookupValue( 0 );
}
flusspferd::value getMobProto( int vnum )
{
	return lookupValue( MobManager::GetManager().GetPrototypeByVnum( vnum ) );
}
flusspferd::value getMobProtoByRnum( int rnum )
{
	return lookupValue( MobManager::GetManager().GetPrototype( rnum ) );
}
flusspferd::array getConnectedPlayers()
{
	flusspferd::array vA = flusspferd::create_array();
	for(Descriptor *d = descriptor_list;d;d = d->next) {
		if( d->character && d->character->in_room ) {
			vA.call("push", lookupValue(d->character));
		}
	}
	return vA;
}
flusspferd::array JS_getCharacterList()
{
	flusspferd::array vA = flusspferd::create_array();
	for(Character *ch = character_list;ch;ch = ch->next) {
		vA.call("push", lookupValue(ch));
	}
	return vA;
}

int JS_getTime()
{
	return time(0);
}
bool JS_isZoneOpen( int vnum )
{
	Zone *zone = ZoneManager::GetManager().GetZoneByVnum( vnum );
	if( !zone ) return false;
	return !zone->IsClosed();
}
flusspferd::string JS_md5( flusspferd::string key )
{
	return flusspferd::string(MD5::getHashFromString( key.to_string().c_str() ));
}
flusspferd::string JS_clanNumToText( int iClanNum )
{
	Clan *c = ClanUtil::getClan(iClanNum);
	if( !c ) return "";
	return c->Name;
}
flusspferd::string JS_getClanRankName( int iClanNum, int iRankNum )
{
	Clan *c = ClanUtil::getClan(iClanNum);
	if( !c ) return "";
	return c->GetRankName(iRankNum);
}
int JS_numberOfRooms()
{
	return (int)World.size();
}
bool JS_isClanSecret( int iClanVnum )
{
	Clan *c = ClanUtil::getClan( iClanVnum );
	if( c && c->secret )
		return true;
	return false;
}
flusspferd::object JS_getGlobalObject()
{
	return flusspferd::global();
}
int JS_getRankRequirement(int rankTo)
{
	rankTo -= 1;
	if( rankTo >= 0 && rankTo < 14 )
	{
		return rank_req[rankTo];
	}
	return 0;
}

flusspferd::array JS_getObjectList()
{
	flusspferd::array vA = flusspferd::create_array();
	for(Object *obj = object_list;obj;obj = obj->next) {
		vA.call("push", lookupValue(obj));
	}
	return vA;
}

flusspferd::value JS_loadSingleObjectFromDatabase(flusspferd::string objectId)
{
	boost::uuids::string_generator uuidGenerator;

	Object *object = Object::loadSingleItem(uuidGenerator(objectId.to_string()), true);

	return lookupValue(object);
}

void JS_saveSingleObjectToDatabase(JSObject *object, flusspferd::string holderType, flusspferd::string holderId)
{
	if(object->toReal() && object->toReal()->IsPurged() == false) {

		if(holderType.size() < 1) {

			throw flusspferd::exception("Attempting to save object to database with an empty holder type.");
		}

		Object::saveItemToTopLevelHolder(holderType.c_str()[0], holderId.to_string(), object->toReal());
	}
}

flusspferd::array JS_loadObjectsByHolderFromDatabase(flusspferd::string holderType, flusspferd::string holderId)
{
	flusspferd::array objects = flusspferd::create_array();

	if(holderType.size() == 0) {

		throw flusspferd::exception("Attempting to load objects from holder of empty type.");
	}

	std::list< Object* > objectList = Object::loadItemList(true, holderType.c_str()[0], holderId.to_string());

	while(objectList.empty() == false) {
		objects.call("push", lookupValue(objectList.front()));
//		objects.push(lookupValue(objectList.front()));

		objectList.pop_front();
	}

	return objects;
}

bool JS_isArenaZone(int zoneVnum)
{
	try {
		flusspferd::array arenaZoneArray = global().get_property("global").to_object().get_property("arenaZones").to_object();

		for(size_t index = 0;index < arenaZoneArray.length();++index) {

			flusspferd::value elementValue = arenaZoneArray.get_element(index);

			if(elementValue.is_number() == false) {

				MudLog(BRF, LVL_APPR, TRUE, "JS_isArenaZone : Element #%d is not a number", index);

				continue;
			}

			int iterativeZoneVnum = (int)elementValue.to_number();

			if( iterativeZoneVnum == zoneVnum ) {

				return true;
			}
		}
	}
	catch(flusspferd::exception e) {

		MudLog(BRF, LVL_APPR, TRUE, "Generic Exception: %s", e.what());
	}

	return false;
}

int JS_getArenaStartRoomVnum( int race )
{
	try {
		flusspferd::array arenaStartRoomArray = global().get_property("global").to_object().get_property("arenaStartRooms").to_object();
		
		flusspferd::value roomVnum = arenaStartRoomArray.get_element(race);
		
		if(roomVnum.is_number() == false)
			MudLog(BRF, LVL_APPR, TRUE, "JS_getArenaStartRoomVnum : Element #%d is not a number", race);

		return (int)roomVnum.to_number();
	}
	catch(flusspferd::exception e) {

		MudLog(BRF, LVL_APPR, TRUE, "Generic Exception: %s", e.what());
	}

	return 0;
}

int JS_getCharacterArenaTeam( Character *ch )
{
	try {
		flusspferd::value chValue = lookupValue(ch);
		
		if( chValue.is_object() == false )
			MudLog(BRF, LVL_APPR, TRUE, "JS_getCharacterArenaTeam : chValue is not an object");
		
		flusspferd::object chObject = chValue.to_object();
		flusspferd::value chArenaTeam = chObject.get_property("arenaTeam");

		if(chArenaTeam.is_number() != false)
			return (int)chArenaTeam.to_number();
	}
	catch(flusspferd::exception e) {
		MudLog(BRF, LVL_APPR, TRUE, "Generic Exception: %s", e.what());
	}

	return 0;

}

bool JS_isArenaInFFAMode()
{
	try {
		flusspferd::value ffa = global().get_property("global").to_object().get_property("arenaFreeForAll");

		if( ffa.is_bool() == false)
			MudLog(BRF, LVL_APPR, TRUE, "JS_getCharacterArenaTeam : arenaFreeForAll is not a boolean");
		
		return (bool)ffa.to_boolean();
	}
	catch(flusspferd::exception e) {
		MudLog(BRF, LVL_APPR, TRUE, "Generic Exception: %s", e.what());
	}
	
	return false;
}

flusspferd::string JS_getUserNameByUserId(int userId)
{
	PlayerIndex *playerIndex = CharacterUtil::getPlayerIndexByUserId(userId);

	if(playerIndex == NULL)
		return "";
	return playerIndex->name;
}

flusspferd::value JS_getUserIdByUserName(flusspferd::string userName)
{
	PlayerIndex *playerIndex = CharacterUtil::getPlayerIndexByUserName(userName.to_string());

	if(playerIndex == NULL)
		return flusspferd::object();
	return flusspferd::value(playerIndex->id);
}

flusspferd::object JS_parseJson(std::string jsonText)
{
	flusspferd::object global = flusspferd::global();

	flusspferd::object jsonObject = global.get_property("JSON").to_object();
	flusspferd::string str(jsonText);
	flusspferd::object resultObject = jsonObject.call("parse", str).to_object();

	return resultObject;
}

std::string JS_stringifyJson(flusspferd::object obj)
{
	flusspferd::object global = flusspferd::global();

	flusspferd::object jsonObject = global.get_property("JSON").to_object();
	flusspferd::string jsonString = jsonObject.call("stringify", obj).to_string();
	
	return jsonString.to_string();
}

void JS_sendToZone(int zoneNumber, flusspferd::string message)
{
	Zone *zone = ZoneManager::GetManager().GetZoneByVnum( zoneNumber );
	if( !zone ) return;
	sendToZone(message.c_str(), zone->GetRnum());
}

flusspferd::object JS_createDatetime(const DateTime &dateTime)
{
	std::stringstream buffer;
	buffer << "new Date(" << (dateTime.getTime() * 1000) << ");";
	return flusspferd::evaluate(buffer.str()).to_object();
}

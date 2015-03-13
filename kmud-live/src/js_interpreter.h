//
// C++ Interface: js_interpreter
//
// Description:
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JS_INTERPRETER_H
#define KINSLAYER_JS_INTERPRETER_H

#include <flusspferd.hpp>
#include <flusspferd/spidermonkey/context.hpp>

#include "JSCharacter.h"
#include "JSObject.h"
#include "JSRoom.h"
#include "js_constants.h"
#include "js_trigger.h"

#ifndef WIN32
#include <tr1/unordered_map>
#include <tr1/memory>
#else
#include <unordered_map>
#include <memory>
#endif
using std::tr1::unordered_map;

#include "structs.h"
#include "olc.h"

extern bool blocking;

class Character;
class Object;
class Room;
class JSTrigger;
class string;
class JSTrigger;
class JSScript;

struct JSInstance
{
    int vnum;
    std::string callstring;
    std::string delstring;
    bool first_run;
    flusspferd::root_object state;
    flusspferd::value self;
    JSInstance();
    ~JSInstance();
};


class JSEnvironment
{
    public:
        JSEnvironment();
        void timeout();
        // Returns the value returned by the script, or 1 is none was returned.
        int execute(JSTrigger* trig, JSBindable * self, Character * actor, const char* args, Room * here);
        // Returns the value returned by the script, or 1 is none was returned.
		int execute(JSTrigger* trig, JSBindable *self, Character * actor, const char* args, flusspferd::object extra, Room * here);
        // Returns the value returned by the script, or 1 is none was returned.
        int execute(std::shared_ptr<JSInstance> instance);
        // used for executing scripts delayed by timers
        int execute_timer(std::shared_ptr<JSInstance> instance, bool success);
		flusspferd::value executeExpression( const std::string &expression );
		bool compile(const std::string &fileName, std::string &scriptBuffer);// true if compile succeeded, false otherwise.
        void gc(); // garbage collect.
		__int64 timeSinceLastGC();
		__int64 getGC_Count();
        void cleanup(JSInstance*); // cleans up that instance. Its called from ~JSInstance, so it uses a raw pointer.

	void LoadJSCharacter();
	void LoadJSObject();
	void LoadJSRoom();  
	void LoadJSQuery();
	void LoadJSRow();      

    private:
		__int64 gcCount;
		__int64 timeOfLastGC;
    	flusspferd::current_context_scope context_scope;
        class JSContext* raw_context() const;
        // repeated code factored out
        int process_yield(std::shared_ptr<JSInstance> instance, flusspferd::value yielded, bool &bSpecial);
};


#endif

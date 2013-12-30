#include "js_interpreter.h"

#include <csignal>
#include <ctime>
#include <cstdlib>

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <flusspferd.hpp>
#include <flusspferd/spidermonkey/context.hpp>
#include <cstdlib>
#include <cstring>

#include <js/jsapi.h>

#include "JSCharacter.h"
#include "JSObject.h"
#include "JSRoom.h"
#include "JSQuery.h"
#include "JSRow.h"
#include "JSConstants.h"
#include "constants.h"
#include "js.h"
#include "olc.h"
#include "utils.h"
#include "js_utils.h"
#include "js_trigger.h"

#include "StringUtil.h"

#if(0)
#define KJS_USE_TIMEOUT_SIGNALS
#endif

using namespace flusspferd;

// in microseconds
const int TIMEOUT = (15)*(1000000); // Make configurable somehow?
const int TIMEOUT_SECONDS = TIMEOUT / 1000000;

JSEnvironment* env;
unsigned long long end_time = 0;
bool blocking=false;
extern std::vector<Object*> obj_proto;
extern int top_of_objt;

JSBool kill_script(JSContext * cx);
void removeTimeoutHandler();
void removeTimeout();
void timeout_handler(int);
void setupTimeout(bool setScriptEndingTime=true);
void macro( std::string &source, const std::string find, std::string replace );

flusspferd::array JS_getObjectList();
void JS_saveSingleObjectToDatabase(JSObject *object, flusspferd::string holderType, flusspferd::string holderId);
flusspferd::value JS_loadSingleObjectFromDatabase(flusspferd::string objectId);
flusspferd::array JS_loadObjectsByHolderFromDatabase(flusspferd::string holderType, flusspferd::string holderId);

Clock scriptRuntimeClock;

JSInstance::JSInstance()
{
}

JSInstance::~JSInstance()
{
    env->cleanup(this);
}

void printJSObject( flusspferd::object obj )
{
	for(flusspferd::property_iterator iter = obj.begin();iter != obj.end();++iter)
	{
//		MudLog(CMP, LVL_APPR, TRUE, "...%s", iter->to_std_string().c_str());
	}
}

JSBool kill_script(JSContext * cx)
{
#ifdef KJS_USE_TIMEOUT_SIGNALS
//	std::cout << "kill_script() is running." << std::endl;
	JSManager *manager = JSManager::get();

	//First, perform some garbage collection.
//	if( manager->timeSinceLastGC() >= CLOCKS_PER_SEC ) {
//		manager->gc();//Only garbage collect once per second.
//	}

	//Secondly, since we know that a script is running, be sure to set the timeout to run again in the future.
	setupTimeout( false );//We do not want to reassign the script's end time. We just want a new injection.

    if (end_time == 0) // no timer set up
    {//If we get here, then no timer is set up. This likely means that script limitation is disabled.
	 //As such, we will merely tell SpiderMonkey to continue running the script.
        return JS_TRUE;
    }
	if( cx == 0 )
	{//Context is invalid. Kill whatever script is running.
		return JS_FALSE;
	}

	//Calculate the time at which the script should end.
    timeval tt;
    gettimeofday(&tt, (struct timezone*)0);
    unsigned long long ms = (tt.tv_sec*1000000) + (tt.tv_usec);
//  cout << "now: " << ms << " target: " << end_time << endl;
    if (ms < end_time)
    {//We still have time.
        return JS_TRUE;
    }
    else
    {//Script has run too long. Kill it.
        JS_ReportError(cx, "This trigger has run too long.");
        return JS_FALSE;
    }
#else
	return JS_TRUE;
#endif
}

JSBool kjsOperationalCallback(JSContext * cx)
{
	scriptRuntimeClock.turnOff();
	unsigned long long secondsElapsed = scriptRuntimeClock.getClocks() / 1000;
	scriptRuntimeClock.turnOn();

	if(secondsElapsed >= TIMEOUT_SECONDS)
	{
        JS_ReportError(cx, "This trigger has run too long.");
        return JS_FALSE;
	}
	else
	{
		return JS_TRUE;
	}
}

bool keepTriggerOperationalCallbackFunctionAlive = true;
std::mutex keepTriggerOperationalCallbackFunctionAliveMutex;
std::thread triggerOperationalCallbackThread;
void triggerOperationalCallback(flusspferd::context context)
{
	while(true)
	{
		{
			std::lock_guard<std::mutex> lock(keepTriggerOperationalCallbackFunctionAliveMutex);
			if(!keepTriggerOperationalCallbackFunctionAlive)
				break;
		}

		if(context.is_valid())
		{
			JS_TriggerOperationCallback(Impl::get_context(context));
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

JSEnvironment::JSEnvironment()
    : context_scope(context::create())
{
	LoadJSCharacter();
	LoadJSRoom();
	LoadJSObject();
	LoadJSQuery();
	LoadJSRow();

	gcCount=0;

	object g = global();
    g.set_property("constants", makeConstants());
	flusspferd::create_native_function(g, "getRoom", JS_getRoom);
	flusspferd::create_native_function(g, "getRoomByRnum", JS_getRoomByRnum);
	flusspferd::create_native_function(g, "mudLog", JS_mudLog);
	flusspferd::create_native_function(g, "_act", JS_act);
	flusspferd::create_native_function(g, "fwrite", JS_fwrite);
	flusspferd::create_native_function(g, "fread", JS_fread);
	flusspferd::create_native_function(g, "getHour", JS_getHour);
	flusspferd::create_native_function(g, "getDay", JS_getDay);
	flusspferd::create_native_function(g, "getMonth", JS_getMonth);
	flusspferd::create_native_function(g, "getYear", JS_getYear);
	flusspferd::create_native_function(g, "sqlQuery", JS_sendQuery);
	flusspferd::create_native_function(g, "sqlEsc", JS_sqlEsc);
	flusspferd::create_native_function(g, "sqlEscapeQuoteString", JS_sqlEscapeQuoteString);
	flusspferd::create_native_function(g, "sqlEncodeQuoteDate", JS_sqlEncodeQuoteDate);
	flusspferd::create_native_function(g, "sqlInsertID", JS_sqlInsertID);
	
	flusspferd::create_native_function(g, "time", JS_getTime);
	flusspferd::create_native_function(g, "getConnectedPlayers", getConnectedPlayers);

	flusspferd::create_native_function(g, "getObjProto", getObjProto);
	flusspferd::create_native_function(g, "getObjProtoByRnum", getObjProtoByRnum);
	flusspferd::create_native_function(g, "getMobProto", getMobProto);
	flusspferd::create_native_function(g, "getMobProtoByRnum", getMobProtoByRnum);

	flusspferd::create_native_function(g, "getListOfSkills", JS_getListOfSkills);
	flusspferd::create_native_function(g, "getSkillVnum", JS_getSkillVnum);
	flusspferd::create_native_function(g, "getSkillName", JS_getSkillName);
	flusspferd::create_native_function(g, "getWeaveAttribute", JS_getWeaveAttribute);
	flusspferd::create_native_function(g, "getMobName", JS_getMobName);
	flusspferd::create_native_function(g, "isZoneOpen", JS_isZoneOpen);
	flusspferd::create_native_function(g, "getCharacterList", JS_getCharacterList);
	flusspferd::create_native_function(g, "md5Hash", JS_md5);
	flusspferd::create_native_function(g, "numberOfRooms", JS_numberOfRooms);
	
	flusspferd::create_native_function(g, "clanNumToText", JS_clanNumToText);
	flusspferd::create_native_function(g, "getClanRankName", JS_getClanRankName);
	flusspferd::create_native_function(g, "isClanSecret", JS_isClanSecret);
	flusspferd::create_native_function(g, "getGlobalObject", JS_getGlobalObject);
	flusspferd::create_native_function(g, "getRankRequirement", JS_getRankRequirement);
	flusspferd::create_native_function(g, "getObjectList", JS_getObjectList);

	flusspferd::create_native_function(g, "saveSingleObjectToDatabase", JS_saveSingleObjectToDatabase);
	flusspferd::create_native_function(g, "loadSingleObjectFromDatabase", JS_loadSingleObjectFromDatabase);
	flusspferd::create_native_function(g, "loadObjectsByHolderFromDatabase", JS_loadObjectsByHolderFromDatabase);
	flusspferd::create_native_function(g, "getUserNameByUserId", JS_getUserNameByUserId);
	flusspferd::create_native_function(g, "getUserIdByUserName", JS_getUserIdByUserName);
	flusspferd::create_native_function(g, "sendToZone", JS_sendToZone);

//	flusspferd::create<flusspferd::function>("setTimeout", &JS_SetTimeout, flusspferd::param::_container = g);

//  cout << "JSCharacter size: " << sizeof(JSCharacter) << " JSRoom size: " << sizeof(JSRoom) << " JSObject size: " << sizeof(JSRoom) << "flusspferd::value size: " << sizeof(flusspferd::value) << endl;

    JS_SetOperationCallback(Impl::get_context(current_context()), &kjsOperationalCallback);
    
	triggerOperationalCallbackThread = std::thread( &triggerOperationalCallback, current_context() );

	env = this;
}
flusspferd::value JSEnvironment::executeExpression( const std::string &expression )
{
	flusspferd::value v;
	try {
		v = flusspferd::evaluate( expression );
	} catch( flusspferd::exception e ) {
		std::stringstream errorBuffer;
		errorBuffer << "Error executing hard coded expression : " << e.what() << std::endl;
		errorBuffer << "Expression: " << expression;
		MudLog(NRM, LVL_APPR, TRUE, errorBuffer.str().c_str());
	}
	return v;
}
int JSEnvironment::execute(JSTrigger* trig, JSBindable *self, Character * actor, const char* args, flusspferd::object extra, Room * here)
{
	if (!trig)
		return -1;

	if (!trig->valid)
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : Script is invalid, and will not run. (Compilation error?)", trig->vnum);
		return -1;
	}

	std::string extraName;

	// this is for passing special stuff to.

	std::shared_ptr<JSInstance> instance(new JSInstance());
	instance->self = lookupValue(self);

	if (extra.is_null())
		extraName = "null";
	else
	{
//		extraName = getUniqueName();
		extraName = "extra";
		global().set_property(extraName, extra);
	}
    
	instance->vnum = trig->vnum;
	instance->first_run = true;

	if( !here && self )
		here = self->InRoom();
	if( !here && actor )
		here = actor->in_room;

	std::string sArgs = args;
	StringUtil::addSlashes( sArgs, "\\" );
	StringUtil::addSlashes( sArgs, "\"" );

	instance->callstring = "this." + trig->js_name + "(" + (self ? lookupName(self) : "null") + ", "
		+ (actor ? lookupName(actor) : "null") + ", "
		+ (here ? lookupName(here) : "null")
		+ ", \"" + sArgs + "\", " + extraName  + ")";
	if (extraName != "null")
		instance->delstring = extraName + "= null;";
	else
		instance->delstring = "";
	return execute(instance);

	return 0;
}

int JSEnvironment::execute(JSTrigger* trig, JSBindable * self, Character * actor, const char* args, Room * here)
{
	return execute(trig, self, actor, args, flusspferd::object(), here);
}

void JSEnvironment::timeout()
{
	JSContext *c = raw_context();
	if( c ) {
		JS_TriggerOperationCallback( c );
	}
}

JSContext * JSEnvironment::raw_context() const
{
	static JSContext *c = NULL;
	if( current_context().is_valid() == true ) {
		c = Impl::get_context(current_context());
	}
	return c;
}


void removeTimeoutHandler()
{
#ifdef KJS_USE_TIMEOUT_SIGNALS
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGALRM, &sa, NULL);
#endif
}

void removeTimeout()
{
#ifdef KJS_USE_TIMEOUT_SIGNALS
	removeTimeoutHandler();
	end_time = 0;
#else
	scriptRuntimeClock.turnOff();
	scriptRuntimeClock.reset(false);
#endif
}


void timeout_handler(int)
{
	env->timeout();
//	removeTimeoutHandler();
}

void setupTimeout( bool setScriptEndingTime )
{

	if(setScriptEndingTime)
	{
		scriptRuntimeClock.turnOff();
		scriptRuntimeClock.reset(false);
		scriptRuntimeClock.turnOn();
	}
#ifdef KJS_USE_TIMEOUT_SIGNALS
	static const int MICROSECONDS_TILL_INJECTION = 1000;//Ten times per second.
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timeout_handler;
	sigaction(SIGALRM, &sa, NULL);
	ualarm(MICROSECONDS_TILL_INJECTION, 0);
    timeval tt;
    gettimeofday(&tt, (struct timezone*)0);

	if( setScriptEndingTime == true ) {
		//If this is set, it means that we want to set the maximum duration that
		//the script may run. The rest of this function merely sets up a signal to intercept
		//the program after a certain amount of time, in order to run other utilities.
	    end_time = (tt.tv_sec*1000000) + (tt.tv_usec) + TIMEOUT - (TIMEOUT/5); // give ourselves some time to breathe
	}

    // this only matters when signals are enabled.
    JS_SetOperationCallback(Impl::get_context(current_context()), &kill_script);
#endif
}

void JSEnvironment::cleanup(JSInstance* instance)
{
	try
	{
		evaluate(instance->delstring);
	}
	catch (flusspferd::exception e)
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d cleanup : %s", instance->vnum, e.what());
	}
}

int JSEnvironment::process_yield(std::shared_ptr<JSInstance> instance, value yielded, bool &bSpecial)
{
	JSCharacter *js_ch;
	JSObject *js_obj;
	JSRoom *js_room;
	bSpecial = false;
	if (yielded.is_object())
	{
		object o = yielded.to_object();
		if (is_native<JSCharacter>(o))
		{
			Character * ch = get_native<JSCharacter>(o).toReal();
			ch->delayed_script = instance;
			value time = get_native<JSCharacter>(o).get_property("__timer");
			ch->SetupTimer("delayed_javascript", time.to_number());
			ch->command_ready = true;
			return 1;
		}
		else
			return 0;
	}
	else if(yielded.is_string() && yielded.to_std_string() == "__SPECIAL__BLOCK")
	{
		bSpecial = true;
		return 0;
	}
	else if(yielded.is_string() && yielded.to_std_string() == "__SPECIAL__NOBLOCK")
	{
		bSpecial = true;
		return 1;
	}
	if (yielded.to_number() > 0)
	{
		try {
			JSManager::get()->addDelayed(instance, (int)yielded.to_number());
			if( is_native<JSCharacter>(instance->self.to_object()) ) {
				get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script = instance; // used to prevent recursion
			}
			else if( is_native<JSObject>(instance->self.to_object()) ) {
				get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script = instance; // used to prevent recursion
			}
			else if( is_native<JSRoom>(instance->self.to_object()) ) {
				get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script = instance; // used to prevent recursion
			}
			else
				return 0;
		} catch( flusspferd::exception e ) {
			//MudLog(NRM, LVL_APPR, TRUE, "JavaScript yielding exception: %s", e.what());
			return 1;
		}
		return 1;
	}
	return 0;
}

int JSEnvironment::execute_timer(std::shared_ptr<JSInstance> instance, bool success)
{
	assert( instance.use_count() > 0 );

	static std::string sSendMethod = "send";
  	try
    {
        setupTimeout();
    	value yielded;
		if( instance->state.has_property(sSendMethod.c_str()) == false ) {
			MudLog(CMP, LVL_APPR, TRUE, "Attempting to run generator's send() method where one does not exist. Script Vnum: %d", instance->vnum);
		}
		else {
	        yielded = instance->state.call(sSendMethod.c_str(), success);
		}
		bool dump = false;
		removeTimeout();
        return process_yield(instance, yielded, dump);
    }
    catch (flusspferd::exception e)
    {
        if (strstr(e.what(), "[object StopIteration]")) // we don't want trivial messages.
        {
			try {
				if( is_native<JSCharacter>(instance->self.to_object()) )
		        	get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script.reset(); // prevent leak
				else if( is_native<JSObject>(instance->self.to_object()) )
		        	get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script.reset(); // prevent leak
				else if( is_native<JSRoom>(instance->self.to_object()) )
		        	get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script.reset(); // prevent leak
			} catch( flusspferd::exception e ) {
				//...
				return 1;
			}
        }
        else
            MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : %s", instance->vnum, e.what());
        
        removeTimeout();
        return 1;
    }
}
int JSEnvironment::execute(std::shared_ptr<JSInstance> instance)
{
	assert( instance.use_count() > 0 );

	int rVal = 1;
	bool bSpecial = false;
	static std::string sGeneratorMethod = "next";
	JSDepthRegulator reg;
	if( !reg.canProceed() )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Script #%d has exceeded the maximum depth(%d).", instance->vnum, JSManager::MAX_TRIGGER_DEPTH);
		return rVal;
	}

    if (instance->first_run)
    {
        instance->first_run = false;
        try
        {
            setupTimeout();
            value v = evaluate(instance->callstring);
            if (!v.is_object())
            {
                // we're done. We don't add it to the wait queue, so it
                // goes out of scope and dtors.
                removeTimeout();
				if (v.is_int()) {
                    return v.get_int();
				}
                else
                    return 1; // a reasonable default value....
            }
            instance->state = v.get_object(); // this is a generator, we need to continue
        }
        catch (flusspferd::exception e)
        {
            MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : %s", instance->vnum, e.what());
            removeTimeout();
            return 1;
        }
    }
    try
    {
		setupTimeout();
		if( instance->state.has_property(sGeneratorMethod.c_str()) == false ) {
			MudLog(CMP, LVL_APPR, TRUE, "Attempting to run generator's next() method where one does not exist. Script Vnum: %d", instance->vnum);
			printJSObject( instance->state.prototype() );
			printJSObject( instance->state.parent() );
		}
		else {
	        value yielded = instance->state.call(sGeneratorMethod.c_str());
			rVal = process_yield(instance, yielded, bSpecial);
		}
	    removeTimeout();
    }
    catch (flusspferd::exception e)
    {
        if (strstr(e.what(), "[object StopIteration]")) // we don't want trivial messages.
        {
			if( !instance->self.is_undefined() && !instance->self.is_null() ) {
			try {
				if( is_native<JSCharacter>(instance->self.to_object()) )
		        	get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script.reset(); // prevent leak
				else if( is_native<JSObject>(instance->self.to_object()) )
		        	get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script.reset(); // prevent leak
				else if( is_native<JSRoom>(instance->self.to_object()) )
		        	get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script.reset(); // prevent leak
			}
			catch( ... ) {}
			}
        }
        else
            MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : %s", instance->vnum, e.what());
        
        removeTimeout();
    }
	catch( ... ) {
		MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d : Unknown error( probably invalid .next )", instance->vnum);
	}

	if( bSpecial )
	{
		execute(instance);
	}
	return rVal;
}

void macro( std::string &source, const std::string find, std::string replace ) {
 
	size_t j;
	for ( ; (j = source.find( find )) != std::string::npos ; ) {
		source.replace( j, find.length(), replace );
	}
}

bool JSEnvironment::compile(JSTrigger * trig)
{
    if (!trig)
    {
        cout << "Someone passed a null trig to JSEnvironment::compile" << endl;
        return false;
    }

    JSScript *script;
    JSObject *scriptObj;
    jsval val;

    stringstream text;
    std::string temp;
    if (trig->vnum < 0) // numbers < 0 are special and isn't modified. They are library functions.
    {
    	temp = trig->text;
    }
    else
    {
    	text << "this." << trig->js_name << " = function(self, actor, here, args) {" << trig->text << "}";
    	temp = text.str();
	}
	
	macro(temp, "wait ", "yield 6 * ");
	macro(temp, "waitpulse ", "yield ");
	macro(temp, "runTimer", "yield ");
	macro(temp, "_block", "yield '__SPECIAL__BLOCK'");
	macro(temp, "_noblock", "yield '__SPECIAL__NOBLOCK'");
//	cout << temp << endl;
    
    script = JS_CompileScript(raw_context(), JS_GetGlobalObject(raw_context()), temp.c_str(), temp.size(), trig->js_name.c_str(), 1);
    if (script == NULL)
    {
        trig->valid = false;
        return false;
    }

    scriptObj = JS_NewScriptObject(raw_context(), script);
    if (scriptObj == NULL) {
        JS_DestroyScript(raw_context(), script);
        trig->valid = false;
        return false;
    }

    val = OBJECT_TO_JSVAL(scriptObj);

    temp = "__" + trig->js_name; // to distinguish from the function
    if (!JS_SetProperty(raw_context(), JS_GetGlobalObject(raw_context()), temp.c_str(), &val))
    {
        trig->valid = false;
        return false;
    }

    jsval result;
    JS_ExecuteScript(raw_context(), JS_GetGlobalObject(raw_context()), script, &result);

    trig->valid = true;
    return true;
}

__int64 JSEnvironment::getGC_Count()
{
	return gcCount;
}

void JSEnvironment::gc()
{
//	cout << "GCing" << endl;
	flusspferd::gc();
	++gcCount;
	timeOfLastGC = Clock::getTick();
}
__int64 JSEnvironment::timeSinceLastGC()
{
	return Clock::getTick() - timeOfLastGC;
}


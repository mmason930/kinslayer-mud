

#ifndef WIN32
#endif



#include "JSRow.h"
#include "js.h"
#include "Script.h"

#include "../StringUtil.h"
#include "../rooms/Room.h"

#if(0)
#define KJS_USE_TIMEOUT_SIGNALS
#endif

using namespace flusspferd;

// in microseconds
const int TIMEOUT = (60)*(1000000); // Make configurable somehow?
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
        JS_ReportErrorUTF8(cx, "This trigger has run too long.");
        return false;
    }
#else
	return JS_TRUE;
#endif
}

JSBool kjsOperationalCallback(JSContext * cx)
{
	scriptRuntimeClock.turnOff();
	unsigned long long secondsElapsed = scriptRuntimeClock.getClocks() / 1000000;
	scriptRuntimeClock.turnOn();

	if(secondsElapsed >= TIMEOUT_SECONDS)
	{
        JS_ReportErrorUTF8(cx, "This trigger has run too long.");
        return false;
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
			JS_RequestInterruptCallback(Impl::get_context(context));
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
	flusspferd::create_native_function(g, "saveTopLevelHolderItems", JS_saveTopLevelHolderItems);
	flusspferd::create_native_function(g, "getUserNameByUserId", JS_getUserNameByUserId);
	flusspferd::create_native_function(g, "getUserIdByUserName", JS_getUserIdByUserName);
	flusspferd::create_native_function(g, "sendToZone", JS_sendToZone);
	flusspferd::create_native_function(g, "setTimeout", JS_setTimeout);
	flusspferd::create_native_function(g, "getZoneBottomRoomVnum", JS_getZoneBottomRoomVnum);
	flusspferd::create_native_function(g, "getZoneTopRoomVnum", JS_getZoneTopRoomVnum);
	flusspferd::create_native_function(g, "llmResponse", JS_llmResponse);
	flusspferd::create_native_function(g, "numberOfPvals", JS_numberOfPvals);
	flusspferd::create_native_function(g, "savePvalsInNeedOfSaving", JS_savePvalsInNeedOfSaving);

    JS_AddInterruptCallback(Impl::get_context(current_context()), &kjsOperationalCallback);
    
	triggerOperationalCallbackThread = std::thread( &triggerOperationalCallback, current_context() );

	env = this;
}
flusspferd::value JSEnvironment::executeExpression( const std::string &expression )
{
	flusspferd::value v;
	try {
		v = flusspferd::evaluate( expression );
	} catch( flusspferd::exception &e ) {
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

	if(trig->scriptId == -1)
		return -1;

	Script *script = JSManager::get()->getScript(trig->scriptId);

	if(!script)
		return -1;

	//See if the method exists.
	if(!flusspferd::global().has_property(script->getMethodName()))
	{
		Log("Attempting to fire trigger #%d with invalid script #%d. Method name: `%s`", trig->vnum, script->getId(), script->getMethodName().c_str());
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
		here = self->getRoom();
	if( !here && actor )
		here = actor->in_room;

	std::string sArgs = args;
	StringUtil::addSlashes( sArgs, "\\" );
	StringUtil::addSlashes( sArgs, "\"" );

	instance->callstring = "this." + script->getMethodName() + "(" + (self ? lookupName(self) : "null") + ", "
		+ (actor ? lookupName(actor) : "null") + ", "
		+ (here ? lookupName(here) : "null")
		+ ", \"" + sArgs + "\", " + extraName  + ")";
	if (extraName != "null")
		instance->delstring = extraName + "= null;";
	else
		instance->delstring = "";
	return execute(instance);
}

int JSEnvironment::execute(JSTrigger* trig, JSBindable * self, Character * actor, const char* args, Room * here)
{
	return execute(trig, self, actor, args, flusspferd::object(), here);
}

void JSEnvironment::timeout()
{
	JSContext *c = raw_context();
	if( c ) {
		JS_RequestInterruptCallback( c );
	}
}

JSContext * JSEnvironment::raw_context() const
{
	static JSContext *c = nullptr;
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
	sigaction(SIGALRM, &sa, nullptr);
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
	sigaction(SIGALRM, &sa, nullptr);
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
    JS_AddInterruptCallback(Impl::get_context(current_context()), &kill_script);
#endif
}

void JSEnvironment::cleanup(JSInstance* instance)
{
	try
	{
		evaluate(instance->delstring);
	}
	catch (flusspferd::exception &e)
	{
		MudLog(NRM, LVL_BUILDER, TRUE, "Error in script %d cleanup : %s", instance->vnum, e.what());
	}
}

// ES6 generator .next() returns {value: <val>, done: <bool>}.
// This helper extracts the value and returns whether the generator is done.
static value unwrapGeneratorResult(value result, bool &isDone)
{
	isDone = false;
	if (!result.is_object()) {
		return result;
	}
	object resultObj = result.to_object();
	if (resultObj.has_property("done")) {
		value doneVal = resultObj.get_property("done");
		isDone = doneVal.to_boolean();
	}
	if (resultObj.has_property("value")) {
		return resultObj.get_property("value");
	}
	return result;
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
		} catch( flusspferd::exception &e ) {
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

	// ES6 generators use .next(val) instead of .send(val)
	static std::string sNextMethod = "next";
  	try
    {
        setupTimeout();
    	value yielded;
		if( instance->state.has_property(sNextMethod.c_str()) == false ) {
			MudLog(CMP, LVL_APPR, TRUE, "Attempting to run generator's next() method where one does not exist. Script Vnum: %d", instance->vnum);
		}
		else {
			value rawResult = instance->state.call(sNextMethod.c_str(), success);
			bool isDone = false;
			yielded = unwrapGeneratorResult(rawResult, isDone);
			if (isDone) {
				// Generator is done - equivalent of old StopIteration
				try {
					if( is_native<JSCharacter>(instance->self.to_object()) )
						get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script.reset();
					else if( is_native<JSObject>(instance->self.to_object()) )
						get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script.reset();
					else if( is_native<JSRoom>(instance->self.to_object()) )
						get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script.reset();
				} catch( flusspferd::exception &e ) {
					return 1;
				}
				removeTimeout();
				return 1;
			}
		}
		bool dump = false;
		removeTimeout();
        return process_yield(instance, yielded, dump);
    }
    catch (flusspferd::exception &e)
    {
        if (strstr(e.what(), "[object StopIteration]"))
        {
			try {
				if( is_native<JSCharacter>(instance->self.to_object()) )
		        	get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script.reset();
				else if( is_native<JSObject>(instance->self.to_object()) )
		        	get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script.reset();
				else if( is_native<JSRoom>(instance->self.to_object()) )
		        	get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script.reset();
			} catch( flusspferd::exception &e ) {
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
                else {
                    return 1; // a reasonable default value....
				}
            }
            instance->state = v.get_object(); // this is a generator, we need to continue
        }
        catch (flusspferd::exception &e)
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
			value rawResult = instance->state.call(sGeneratorMethod.c_str());
			// ES6 generators return {value, done} - unwrap the result
			bool isDone = false;
			value yielded = unwrapGeneratorResult(rawResult, isDone);
			if (isDone) {
				// Generator is done - equivalent of old StopIteration
				if( !instance->self.is_undefined() && !instance->self.is_null() ) {
					try {
						if( is_native<JSCharacter>(instance->self.to_object()) )
							get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script.reset();
						else if( is_native<JSObject>(instance->self.to_object()) )
							get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script.reset();
						else if( is_native<JSRoom>(instance->self.to_object()) )
							get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script.reset();
					} catch( ... ) {}
				}
				removeTimeout();
				return rVal;
			}
			rVal = process_yield(instance, yielded, bSpecial);
		}
	    removeTimeout();
    }
    catch (flusspferd::exception &e)
    {
        if (strstr(e.what(), "[object StopIteration]"))
        {
			if( !instance->self.is_undefined() && !instance->self.is_null() ) {
			try {
				if( is_native<JSCharacter>(instance->self.to_object()) )
		        	get_native<JSCharacter>(instance->self.to_object()).toReal()->delayed_script.reset();
				else if( is_native<JSObject>(instance->self.to_object()) )
		        	get_native<JSObject>(instance->self.to_object()).toReal()->delayed_script.reset();
				else if( is_native<JSRoom>(instance->self.to_object()) )
		        	get_native<JSRoom>(instance->self.to_object()).toReal()->delayed_script.reset();
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

void macro( std::string &source, const std::string find, std::string replace )
{
	StringUtil::replace(source, find, replace);
	/***
	size_t j;
	for ( ; (j = source.find( find )) != std::string::npos ; ) {
		source.replace( j, find.length(), replace );
	}
	***/
}

// Convert legacy-style generator functions to ES6 function* syntax.
// In old SpiderMonkey (1.8.x), any function containing 'yield' was automatically a generator.
// In ES6 (SpiderMonkey 131), generators must be declared with function*.
// This function finds each 'function' keyword, determines its body, and checks if
// yield appears at the top level of that function (not inside nested functions).
static void convertLegacyGenerators(std::string &source)
{
	// Only bother if the source contains yield
	if (source.find("yield") == std::string::npos)
		return;

	// First pass: find the opening brace position for each function keyword.
	// Second pass: for each function, check if yield appears at brace depth 1
	// (i.e., directly in that function's body, not in nested functions).

	// We'll work backwards so inserting '*' doesn't shift positions of earlier functions.
	// Collect positions of 'function' keywords that need conversion.
	std::vector<size_t> funcPositions;
	size_t len = source.size();

	for (size_t i = 0; i < len; ) {
		size_t funcPos = source.find("function", i);
		if (funcPos == std::string::npos) break;

		size_t afterFunc = funcPos + 8;

		// Check it's a standalone keyword
		bool validStart = (funcPos == 0 || (!isalnum((unsigned char)source[funcPos - 1]) && source[funcPos - 1] != '_'));
		bool validEnd = (afterFunc >= len || (!isalnum((unsigned char)source[afterFunc]) && source[afterFunc] != '_'));
		// Allow function* (already a generator)
		if (afterFunc < len && source[afterFunc] == '*') {
			i = afterFunc + 1;
			continue;
		}

		if (!validStart || !validEnd) {
			i = afterFunc;
			continue;
		}

		// Skip whitespace and optional function name to find '('
		size_t scan = afterFunc;
		while (scan < len && (source[scan] == ' ' || source[scan] == '\t' || source[scan] == '\n' || source[scan] == '\r'))
			scan++;
		if (scan < len && (isalpha((unsigned char)source[scan]) || source[scan] == '_' || source[scan] == '$')) {
			while (scan < len && (isalnum((unsigned char)source[scan]) || source[scan] == '_' || source[scan] == '$'))
				scan++;
			while (scan < len && (source[scan] == ' ' || source[scan] == '\t'))
				scan++;
		}

		if (scan >= len || source[scan] != '(') {
			i = afterFunc;
			continue;
		}

		// Find matching ')'
		int parenDepth = 1;
		scan++;
		while (scan < len && parenDepth > 0) {
			if (source[scan] == '(') parenDepth++;
			else if (source[scan] == ')') parenDepth--;
			scan++;
		}
		if (parenDepth != 0) { i = afterFunc; continue; }

		// Find opening '{'
		while (scan < len && source[scan] != '{') scan++;
		if (scan >= len) { i = afterFunc; continue; }

		// Now scan the function body. We look for 'yield' anywhere in this
		// function's body, but NOT inside nested function bodies.
		// We track nested function depth to skip their bodies.
		size_t bodyOpen = scan;
		int braceDepth = 0;
		int nestedFuncBraceStart = 0; // brace depth when we entered a nested function
		int nestedFuncCount = 0;      // how many nested functions deep we are
		bool hasYieldAtTopLevel = false;
		size_t bodyScan = bodyOpen;

		while (bodyScan < len) {
			char c = source[bodyScan];

			if (c == '{') {
				braceDepth++;
			} else if (c == '}') {
				braceDepth--;
				if (nestedFuncCount > 0 && braceDepth < nestedFuncBraceStart) {
					nestedFuncCount--;
					if (nestedFuncCount > 0) {
						// Still inside an outer nested function, restore its brace start
						// This is approximate but works for typical non-deeply-nested code
					}
				}
				if (braceDepth == 0) {
					bodyScan++;
					break; // End of this function
				}
			} else if (c == '/' && bodyScan + 1 < len) {
				if (source[bodyScan + 1] == '/') {
					while (bodyScan < len && source[bodyScan] != '\n') bodyScan++;
					continue;
				} else if (source[bodyScan + 1] == '*') {
					bodyScan += 2;
					while (bodyScan + 1 < len && !(source[bodyScan] == '*' && source[bodyScan + 1] == '/')) bodyScan++;
					if (bodyScan + 1 < len) bodyScan += 2;
					continue;
				}
			} else if (c == '"' || c == '\'' || c == '`') {
				char quote = c;
				bodyScan++;
				while (bodyScan < len && source[bodyScan] != quote) {
					if (source[bodyScan] == '\\') bodyScan++;
					bodyScan++;
				}
			} else if (c == 'f' && bodyScan + 8 <= len && source.compare(bodyScan, 8, "function") == 0) {
				// Check for nested 'function' keyword
				bool fStart = (bodyScan == 0 || (!isalnum((unsigned char)source[bodyScan - 1]) && source[bodyScan - 1] != '_'));
				bool fEnd = (bodyScan + 8 >= len || (!isalnum((unsigned char)source[bodyScan + 8]) && source[bodyScan + 8] != '_')
				             || source[bodyScan + 8] == '*');
				if (fStart && fEnd) {
					// Skip past this nested function's opening brace
					size_t fScan = bodyScan + 8;
					if (fScan < len && source[fScan] == '*') fScan++; // skip * if function*
					// Find the opening '{' of the nested function
					while (fScan < len && source[fScan] != '{') {
						if (source[fScan] == '/' && fScan + 1 < len && source[fScan + 1] == '/') {
							while (fScan < len && source[fScan] != '\n') fScan++;
						}
						fScan++;
					}
					if (fScan < len) {
						// Skip the entire nested function body
						int nestedDepth = 1;
						fScan++; // past '{'
						while (fScan < len && nestedDepth > 0) {
							if (source[fScan] == '{') nestedDepth++;
							else if (source[fScan] == '}') nestedDepth--;
							else if (source[fScan] == '"' || source[fScan] == '\'' || source[fScan] == '`') {
								char q = source[fScan];
								fScan++;
								while (fScan < len && source[fScan] != q) {
									if (source[fScan] == '\\') fScan++;
									fScan++;
								}
							} else if (source[fScan] == '/' && fScan + 1 < len) {
								if (source[fScan + 1] == '/') {
									while (fScan < len && source[fScan] != '\n') fScan++;
									continue;
								} else if (source[fScan + 1] == '*') {
									fScan += 2;
									while (fScan + 1 < len && !(source[fScan] == '*' && source[fScan + 1] == '/')) fScan++;
									if (fScan + 1 < len) fScan += 2;
									continue;
								}
							}
							fScan++;
						}
						bodyScan = fScan - 1; // -1 because the outer loop will increment
					}
				}
			} else if (nestedFuncCount == 0 && c == 'y') {
				// Check for 'yield' keyword - only if not inside a nested function
				if (bodyScan + 5 <= len && source.compare(bodyScan, 5, "yield") == 0) {
					bool yStart = (bodyScan == 0 || (!isalnum((unsigned char)source[bodyScan - 1]) && source[bodyScan - 1] != '_'));
					bool yEnd = (bodyScan + 5 >= len || (!isalnum((unsigned char)source[bodyScan + 5]) && source[bodyScan + 5] != '_'));
					if (yStart && yEnd) {
						hasYieldAtTopLevel = true;
					}
				}
			}
			bodyScan++;
		}

		if (hasYieldAtTopLevel) {
			funcPositions.push_back(funcPos);
		}

		i = bodyScan;
	}

	// Insert '*' after 'function' for each identified position, working backwards
	for (int idx = (int)funcPositions.size() - 1; idx >= 0; idx--) {
		source.insert(funcPositions[idx] + 8, "*"); // Insert '*' right after "function"
	}
}

bool JSEnvironment::compile(const std::string &fileName, const std::string &scriptBuffer)
{
    JSScript *script;
    JSObject *scriptObj;
    jsval val;

    std::string formattedScriptBuffer = scriptBuffer;

	macro(formattedScriptBuffer, "wait ", "yield 6 * ");
	macro(formattedScriptBuffer, "waitpulse ", "yield ");
	macro(formattedScriptBuffer, "runTimer", "yield ");
	macro(formattedScriptBuffer, "_block", "yield '__SPECIAL__BLOCK'");
	macro(formattedScriptBuffer, "_noblock", "yield '__SPECIAL__NOBLOCK'");

	// Convert top-level 'let' to 'var' - in SM 131, top-level 'let' doesn't create
	// global properties, but old scripts rely on 'let scriptXXX = function(...)' being
	// accessible as global().has_property("scriptXXX")
	{
		size_t pos = 0;
		while (pos < formattedScriptBuffer.size()) {
			// Only convert 'let' at the start of a line (top-level)
			if ((pos == 0 || formattedScriptBuffer[pos - 1] == '\n') &&
			    formattedScriptBuffer.compare(pos, 4, "let ") == 0) {
				formattedScriptBuffer.replace(pos, 3, "var");
			}
			// Advance to next line
			size_t nl = formattedScriptBuffer.find('\n', pos);
			if (nl == std::string::npos) break;
			pos = nl + 1;
		}
	}

	// Convert legacy generator functions (containing yield) to ES6 function* syntax
	convertLegacyGenerators(formattedScriptBuffer);

	try {
		setupTimeout();
		flusspferd::evaluate(formattedScriptBuffer, fileName.c_str(), 1);
		removeTimeout();
		// Debug: Log successful compilation
		// MudLog(BRF, LVL_APPR, TRUE, "Successfully compiled: %s", fileName.c_str());
	}
	catch(flusspferd::exception &e)
	{
		MudLog(BRF, LVL_APPR, TRUE, "Error evaluating script `%s`: %s", fileName.c_str(), e.what());
		removeTimeout();
	}
	catch(std::exception &e)
	{
		MudLog(BRF, LVL_APPR, TRUE, "C++ exception evaluating script `%s`: %s", fileName.c_str(), e.what());
		removeTimeout();
	}

/*
    //script = JS_CompileScript(raw_context(), JS_GetGlobalObject(raw_context()), formattedScriptBuffer.c_str(), formattedScriptBuffer.size(), fileName.c_str(), 1);
	jsval result;
	JSBool returnValue = JS_EvaluateScript(raw_context(), JS_GetGlobalObject(raw_context()), formattedScriptBuffer.c_str(), formattedScriptBuffer.size(), fileName.c_str(), 1, &result);

	jsval exceptionValue;
	if(JS_GetPendingException(raw_context(), &exceptionValue))
	{
		flusspferd::value fExceptionValue(exceptionValue);

		Log("Is String: %s", StringUtil::yesNo(fExceptionValue.is_string()).c_str());
		Log("Is Bool: %s", StringUtil::yesNo(fExceptionValue.is_bool()).c_str());
		Log("Is Boolean: %s", StringUtil::yesNo(fExceptionValue.is_boolean()).c_str());
		Log("Is Double: %s", StringUtil::yesNo(fExceptionValue.is_double()).c_str());
		Log("Is Function: %s", StringUtil::yesNo(fExceptionValue.is_function()).c_str());
		Log("Is Int: %s", StringUtil::yesNo(fExceptionValue.is_int()).c_str());
		Log("Is Object: %s", StringUtil::yesNo(fExceptionValue.is_object()).c_str());
		Log("Is Number: %s", StringUtil::yesNo(fExceptionValue.is_number()).c_str());
		
		Log("Exception Number: %d", fExceptionValue.get_int());

		exit(1);

		return false;
	}

	Log("SCRIPT: %p", script);
    if (script == nullptr)
    {
        return false;
    }

    scriptObj = JS_NewScriptObject(raw_context(), script);
    
	Log("SCRIPT OBJ: %p", scriptObj);

	if (scriptObj == nullptr) {
        JS_DestroyScript(raw_context(), script);
        return false;
    }
	***/

	/***
    val = OBJECT_TO_JSVAL(scriptObj);

    temp = "__" + trig->js_name; // to distinguish from the function
    if (!JS_SetProperty(raw_context(), JS_GetGlobalObject(raw_context()), temp.c_str(), &val))
    {
        trig->valid = false;
        return false;
    }
	***/

    //jsval result;
	//JSBool executeScriptResult = JS_ExecuteScript(raw_context(), JS_GetGlobalObject(raw_context()), script, &result);

	//Log("EXECUTE SCRIPT RESULT: %d", executeScriptResult);

    return true;
}

/***
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
    if (script == nullptr)
    {
        trig->valid = false;
        return false;
    }

    scriptObj = JS_NewScriptObject(raw_context(), script);
    if (scriptObj == nullptr) {
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
***/

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


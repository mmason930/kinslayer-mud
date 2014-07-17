//
// C++ Interface: js
//
// Description:
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JS_H
#define KINSLAYER_JS_H

#include "sysdep.h"
#include "js_functions.h"
#include "js_interpreter.h"
#include "DateTime.h"
#include "Enum.h"

class JSTrigger;
class JSEnvironment;
class Script;
struct JSInstance;

#include <flusspferd.hpp>
#include <flusspferd/spidermonkey/context.hpp>
#include <thread>
#include <mutex>

const unsigned long long MAX_SCRIPT_LENGTH (1024 * 1024 * 100); //100 megabytes.

namespace flusspferd
{
    class object;
}

typedef flusspferd::object js_extra_data;

class ScriptEvent;

/*** RAII structure to handle depth increase / decrease ***/
struct JSDepthRegulator
{
	JSDepthRegulator();
	~JSDepthRegulator();

	bool canProceed();
};

class ScriptImportOperation : public Enum {

private:

	static std::list<ScriptImportOperation*> enums;

	ScriptImportOperation(int value, const std::string &standardName, const char charCode) : Enum(value, standardName)
	{
		this->charCode = charCode;
		enums.push_back(this);
	}

	char charCode;

public:

	static Enum *getEnumByValue(int v)
	{
		for (auto iter = enums.begin(); iter != enums.end(); ++iter)
		{
			Enum *e = (*iter);
			if (e->getValue() == v)
			{
				return e;
			}
		}

		return NULL;
	}

	static Enum *getEnumByCharCode(const char charCode)
	{
		for(Enum *e : enums)
		{
			ScriptImportOperation *scriptImportOperation = (ScriptImportOperation*)e;

			if(scriptImportOperation->getCharCode() == charCode)
				return scriptImportOperation;
		}

		return NULL;
	}

	const char getCharCode()
	{
		return charCode;
	}

	static std::list<ScriptImportOperation*>::iterator getStartIterator() { return enums.begin(); }
	static std::list<ScriptImportOperation*>::iterator getEndIterator() { return enums.end(); }

	static ScriptImportOperation *addition;
	static ScriptImportOperation *modification;
	static ScriptImportOperation *deletion;
};

struct ScriptImport
{
public:

	unsigned long long id;
	std::string filePath;
	DateTime queuedDatetime;
	ScriptImportOperation *operation;
};

class JSManager
{
	protected:
		int triggerDepth;
		std::list<ScriptEvent *> scriptEvents;
		unsigned int nextScriptEventId;

		bool scriptImportThreadRunning;

		std::list<ScriptImport *> *scriptImportReadQueue;
		std::list<ScriptImport *> *scriptImportWriteQueue;

		std::mutex scriptImportMutex;

		std::thread *monitorScriptImportTableThread;
		void monitorScriptImportTable(sql::Connection connection);
		
		int lastUpdatedRevision;
		bool monitorSubversionThreadRunning;
		std::thread *monitorSubversionThread;
		void monitorSubversion(sql::Connection connection, const std::string &repositoryUrl);

    public:
		
		~JSManager();

		const static int MAX_TRIGGER_DEPTH=100;
        JSTrigger* getTrigger(int vnum);
        std::vector<JSTrigger*> searchTrigger(std::string name);

		Script *getScript(int scriptId);
		Script *getScriptByMethodName(const std::string &methodName, bool caseSensitive = false);

        // Return -1 on compile failure, 0 otherwise
        int saveTrigger(JSTrigger * trig);
		void deleteTrigger(JSTrigger* t);
		bool triggerExists( const int vnum );
        static JSManager* get();
		void addDelayed(std::shared_ptr<JSInstance> env, int seconds);
		void addScriptEvent(ScriptEvent *scriptEvent);
		void heartbeat();
		void runTimeouts();
		void processScriptImports();
		void putScript(sql::Connection connection, Script *script);
		void addScriptToMap(Script *script);
		void deleteScriptCompletely(sql::Connection connection, const int scriptId);
		void deleteScriptFromMap(const int scriptId);
		void deleteScriptFromDatabase(sql::Connection connection, const int scriptId);

		std::map<int, Script*>::const_iterator getScriptMapStartIterator() const;
		std::map<int, Script*>::const_iterator getScriptMapEndIterator() const;

		const char *getFunctionFilename(const std::string &functionName);

		ScriptImport *getScriptImport(const sql::Row &row) const;

		unsigned int getNextScriptEventId();

        // Returns the value returned by the script, or 1 is none was returned.
		template< typename _T >
        int execute(JSTrigger* trig, _T* self, Character * actor, const char * args, js_extra_data extra, Room * here=0)
		{
			return env->execute(trig, self, actor, args, extra, here);
		}
		template< typename _T >
        int execute(JSTrigger* trig, _T* self, Character * actor, const char * args, Room * here=0)
		{
			return env->execute(trig, self, actor, args, here);
		}
		template< typename _T >
        int execute(JSTrigger* trig, _T* self, Character * actor, Room * here=0)
		{
			return env->execute(trig, self, actor, "", here);
		}
		flusspferd::value executeExpression(const std::string &expression);
        // Executes a delayed (timer) script
        int execute_timer(std::shared_ptr<JSInstance> instance, bool success);
        // Trigger the garbage collector for javascript. If this is called regularly, we won't be surprised by
        // an unexcepted gc call caused by the js engine.
		void handleExtraction( JSBindable *ptr );
		
		void incDepth();
		void decDepth();
		int getDepth();

		void gc();
		__int64 timeSinceLastGC();
		__int64 getGC_Count();

		void loadScriptMap();
		void loadTriggers();
		void loadScriptsFromFilesystem(const std::string &directoryPath);
		bool loadScriptsFromFile(const std::string &filePath);

		std::list< JSTrigger* > triggersInRange( const int lo, const int hi );
    private:
		unordered_map<int, JSTrigger*> mapper;
		std::map<int, Script*> scriptMap;
		std::list<std::pair<int, std::shared_ptr<JSInstance> > > scripts;
		JSEnvironment* env;
		JSManager();
		void operator=(JSManager&) {};

		class kuListener *server;
};

struct ScriptEvent
{
public:
	
	unsigned int pulses;
	flusspferd::value callback;
	flusspferd::object arguments;
	std::string propertyName;

	ScriptEvent() : arguments(flusspferd::object())
	{
		this->pulses = 0;
	}

	ScriptEvent(const unsigned int pulses, const flusspferd::value &callback, const flusspferd::object &arguments, const unsigned int scriptEventId) : arguments(arguments)
	{
		this->pulses = pulses;
		this->callback = callback;

		setPropertyName(scriptEventId);
	}

protected:

	void setPropertyName(const unsigned int scriptEventId)
	{
		this->propertyName = "scriptEvent" + MiscUtil::toString(scriptEventId);
	}
};

#endif

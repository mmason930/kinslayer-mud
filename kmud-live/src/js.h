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

class JSTrigger;
class JSEnvironment;
struct JSInstance;

#include <flusspferd.hpp>
#include <flusspferd/spidermonkey/context.hpp>

const int MAX_SCRIPT_LENGTH (1024*10);

namespace flusspferd
{
    class object;
}

typedef flusspferd::object js_extra_data;

/*** RAII structure to handle depth increase / decrease ***/
struct JSDepthRegulator
{
	JSDepthRegulator();
	~JSDepthRegulator();

	bool canProceed();
};

class JSManager
{
	protected:
		int triggerDepth;
    public:
		const static int MAX_TRIGGER_DEPTH=100;
        JSTrigger* getTrigger(int vnum);
        std::vector<JSTrigger*> searchTrigger(std::string name);
        // Return -1 on compile failure, 0 otherwise
        int saveTrigger(JSTrigger * trig);
		void deleteTrigger(JSTrigger* t);
		bool triggerExists( const int vnum );
        static JSManager* get();
		void addDelayed(std::shared_ptr<JSInstance> env, int seconds);
		void heartbeat();
		~JSManager();
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
		void test( class Character *ch );

		void SocketEvents();
		void SciteConnect( const int port );
		void SciteDisconnect();
		bool SciteIsConnected();
		int ScitePort();
		unsigned int numberOfConnectedDescriptors();

		void incDepth();
		void decDepth();
		int getDepth();

		void gc();
		__int64 timeSinceLastGC();
		__int64 getGC_Count();

		std::list< JSTrigger* > triggersInRange( const int lo, const int hi );
    private:
		unordered_map<int, JSTrigger*> mapper;
		std::list<std::pair<int, std::shared_ptr<JSInstance> > > scripts;
		JSEnvironment* env;
		JSManager();
		void operator=(JSManager&) {};

		class kuListener *server;
};

struct JSSocketInfo
{
	int dUID;

	bool isReadingScript;
	bool isSendingScript;
	int scriptVnum;
	class Clock packetClock;
	std::string::size_type scriptBufferIndex;
	std::string scriptBuffer;
	std::string username;
	std::string password;
	int userLevel;
	bool connectionAuthorized;

	JSSocketInfo( class kuDescriptor *d = 0 );
};

#endif

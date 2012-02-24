/*
*  C++ Implementation: js
*
* Description:
*
*
* Author: David Capel <wot.narg@gmail.com>, (C) 2009
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#ifdef KINSLAYER_JAVASCRIPT

#include "JSCharacter.h"
#include "JSObject.h"
#include "JSRoom.h"
#include "js_constants.h"
#include "js_trigger.h"
#include "js_interpreter.h"
#include "js_utils.h"

#include "constants.h"
#include "structs.h"
#include "olc.h"
#include "js.h"
#include "handler.h"

#include "kuSockets.h"
#include "kuListener.h"
#include "kuDescriptor.h"
#include "Descriptor.h"
#include "md5.h"

#include "CharacterUtil.h"
#include "StringUtil.h"

using namespace std;
using namespace tr1;

extern Descriptor *descriptor_list;
extern sql::Connection gameDatabase;

JSDepthRegulator::JSDepthRegulator()
{
	JSManager::get()->incDepth();
}
JSDepthRegulator::~JSDepthRegulator()
{
	JSManager::get()->decDepth();
}
bool JSDepthRegulator::canProceed()
{
	return JSManager::get()->getDepth() <= JSManager::MAX_TRIGGER_DEPTH;
}

void JSManager::incDepth() {
	++triggerDepth;
}
void JSManager::decDepth() {
	--triggerDepth;
}
int JSManager::getDepth() {
	return triggerDepth;
}

JSManager::JSManager()
{
	env = new JSEnvironment();
	mapper = unordered_map<int, JSTrigger*>();

	triggerDepth = 0;
	server = 0;
	sql::Query query;
	sql::Row row;
	try {
		
		int offset = 0;
		int fetchSize = 100;
		do
		{
			std::stringstream queryBuffer;
			queryBuffer << "SELECT * FROM js_scripts LIMIT " << offset << "," << fetchSize;
			query = gameDatabase->sendQuery(queryBuffer.str());

			while (query->hasNextRow())
			{
				row = query->getRow();
				int vnum = atoi(row["vnum"].c_str());
				JSTrigger* trigger = new JSTrigger(vnum);

				trigger->text = row["text"];
				trigger->name = row["name"];
				trigger->narg = atoi(row["narg"].c_str());
				trigger->args = row["args"];
				trigger->trigger_flags = atol(row["trigger_flags"].c_str());
				trigger->allowed_flags = atol(row["allowed_flags"].c_str());
				trigger->option_flags = atol(row["option_flags"].c_str());
				mapper[vnum] = trigger;

				// compile it for future use
				env->compile(trigger);
				//			cout << "Trigger " << trigger->vnum << " compile...";
				//			if (env->compile(trigger))
				//				cout << "...success!" << endl;
				//			else
				//				cout << "FAIL!" << endl;

			}

			offset += fetchSize;
		} while( query->numRows() > 0 );
		
	}
	catch (sql::QueryException e)
	{
		cout << "Error loading JS Script in JSManager::init (Exception)" << endl;
		e.report();
	}
}

JSManager* JSManager::get()
{
	static JSManager* _self = 0;

	if (!_self)
	{
		_self = new JSManager();
	}

	return _self;
}

JSTrigger* JSManager::getTrigger(int vnum)
{
	// check to see if its loaded.
	if (mapper.count(vnum) != 0)
		return mapper[vnum];
	else
	{
		JSTrigger* trigger = new JSTrigger(vnum);
		trigger->setFlag(JS::SPEECH);
		trigger->narg = 100;
		trigger->allowed_flags = Q_BIT(JS::ALLOW_NOT_SELF) | Q_BIT(JS::ALLOW_PLAYER);
		mapper[vnum] = trigger;
	}
	return mapper[vnum];
}

std::vector<JSTrigger*> JSManager::searchTrigger(std::string name)
{
	std::vector<JSTrigger*> results;
	unordered_map<int, JSTrigger*>::const_iterator iter = mapper.begin();
	for(; iter != mapper.end(); ++iter)
	{
		if (isname(name, iter->second->name.c_str()))
			results.push_back(iter->second);
	}

	return results;
}


bool JSManager::triggerExists( const int vnum )
{
	return (mapper.count( vnum ) != 0);
}

void JSManager::deleteTrigger(JSTrigger* t)
{
	if( !t )
		return;
	if( mapper.count(t->vnum) == 0 )
		return;

	std::stringstream QueryBuffer;

	QueryBuffer << "DELETE FROM js_scripts WHERE vnum='" << t->vnum << "';";
	try {
		gameDatabase->sendRawQuery( QueryBuffer.str() );
	} catch( sql::QueryException e ) {
		MudLog(NRM, LVL_APPR, TRUE, "Failed to send JSTrigger deletion query for #%d : %s",
			t->vnum, e.getMessage().c_str());
		return;
	}
}

int JSManager::saveTrigger(JSTrigger* t)
{
	if (!t)
		cout << "Null ptr to trigger passed to saveTrigger(). Bad news!" << endl;

	JSTrigger * trig;
	// if there is nothing there, we just set this as the trigger
	if (mapper.count(t->vnum) == 0)
	{
		mapper[t->vnum] = t;
		trig = t;
	}

	else if (mapper[t->vnum] != t) // we have a new trigger, so we overwrite the old one.
	{
		JSTrigger* old = mapper[t->vnum];
		//      cout << " overwriting " << old << " with " << t << endl;
		*old = *t;
		delete t;
		trig = mapper[old->vnum];
	}
	else
	{ 
		trig = t;
	} // nothing to do with the map, since it hasn't been modified.

	try
	{
		stringstream s;
		s << "REPLACE DELAYED INTO js_scripts (name,narg,args,trigger_flags,allowed_flags,text,vnum,option_flags) VALUES("
			<< "'" << sql::escapeString(trig->name) << "',"
			<< "'" << trig->narg << "',"
			<< "'" << sql::escapeString(trig->args) << "',"
			<< "'" << trig->trigger_flags << "',"
			<< "'" << trig->allowed_flags << "',"
			<< "'" << sql::escapeString(trig->text) << "',"
			<< "'" << trig->vnum << "',"
			<< "'" << trig->option_flags << "');";
		std::string tempstring = s.str();
		//cout << tempstring << endl;
		gameDatabase->sendRawQuery(tempstring);
	}
	catch (sql::QueryException e)
	{
		MudLog(BRF, LVL_APPR, TRUE, "Error saving JS Script %d : %s", trig->vnum, e.getErrorMessage().c_str());
		e.report();
	}

	// we recompile it if it has changed its code.
	if (trig->code_modified)
	{
		cout << "Compiling trigger " << trig->vnum << "...";
		trig->code_modified = false;
		if (!env->compile(trig))
		{
			MudLog(NRM, LVL_BUILDER, TRUE, "Failed to compile trigger %d", trig->vnum);
			return -1;
		}
		else
		{
			cout << " successful." << endl;
			return 0;
		}
	}
	else
	{
		cout << "not modified " << trig->vnum << endl;
		return 0;
	}
}

void JSManager::addDelayed(shared_ptr<JSInstance> instance, int pulses)
{
	// if this is called from the heartbeat function, adding it to the
	// front guarentees that it won't be called twice in the same heartbeat.
	// Not that one extra heartbeat would matter...
	scripts.push_front(make_pair(pulses, instance));
}

int JSManager::execute_timer(shared_ptr<JSInstance> instance, bool success)
{
	return env->execute_timer(instance, success);
}
flusspferd::value JSManager::executeExpression(const std::string &expression)
{
	return env->executeExpression(expression);
}

void JSManager::gc()
{
	if( env ) {
		env->gc();
	}
}
__int64 JSManager::getGC_Count()
{
	return (env ? env->getGC_Count() : 0);
}
__int64 JSManager::timeSinceLastGC()
{
	return (env ? env->timeSinceLastGC() : 0);
}

void JSManager::heartbeat()
{
	static __int64 pulses=0;
	++pulses;

	if( !( pulses%(10*PASSES_PER_SEC) ) ) {
		JSManager::get()->gc();
	}
	list<pair<int, shared_ptr<JSInstance> > >::iterator iter = scripts.begin();
	for( ; iter != scripts.end() ; )
	{
		--(iter->first);
		if (iter->first == 0)
		{
			env->execute(iter->second);
			scripts.erase(iter++);
		}
		else
			++iter;
	}
	/*** Also, execute the event handling script ***/
	time_t tBefore = time(0);
	flusspferd::value f;
	while( (time(0)-tBefore) < 5 ) {
		try {
			f = flusspferd::evaluate("runTimeouts();");
			if( f.is_int() && f.get_int() == 1 )
				break;
		} catch(flusspferd::exception e) {
			MudLog(NRM, LVL_BUILDER, TRUE, "Error in the runTimeouts() callback : %s", e.what());
		}
	}
}

std::list< JSTrigger* > JSManager::triggersInRange( const int lo, const int hi )
{
	std::list< JSTrigger * > myList;
	for( unordered_map<int, JSTrigger*>::iterator tIter = mapper.begin();tIter != mapper.end();++tIter )
	{
		if( (*tIter).second->vnum >= lo && (*tIter).second->vnum <= hi )
		{//Insert in the right spot...
			bool found = false;
			for( std::list< JSTrigger* >::iterator jIter = myList.begin();jIter != myList.end();++jIter )
			{
				if( (*jIter)->vnum > (*tIter).second->vnum ) {
					found = true;
					myList.insert( jIter, (*tIter).second );
					break;
				}
			}
			if( !found ) {
				myList.push_back( (*tIter).second );
			}
		}
	}
	return myList;
}

void JSManager::handleExtraction( JSBindable *ptr )
{
	list<pair<int, shared_ptr<JSInstance> > >::iterator iter = scripts.begin();
	Character *c;
	Object *o;
	Room *r;

	c = dynamic_cast<Character*>(ptr);
	o = dynamic_cast<Object*>(ptr);
	r = dynamic_cast<Room*>(ptr);

	/* Kill any script whose owner is this object */
	for( iter = scripts.begin();iter != scripts.end(); )
	{
		if( c && is_native<JSCharacter>( (*iter).second->self.to_object() )
			&&  c== get_native<JSCharacter>( (*iter).second->self.to_object() ).toReal() )
		{
			iter = scripts.erase( iter );
		}
		else if( o && is_native<JSObject>( (*iter).second->self.to_object() )
			&&       o== get_native<JSObject>( (*iter).second->self.to_object() ).toReal() )
		{
			iter = scripts.erase( iter );
		}
		else if( r && is_native<JSRoom>( (*iter).second->self.to_object() )
			&&       r== get_native<JSRoom>( (*iter).second->self.to_object() ).toReal() )
		{
			iter = scripts.erase( iter );
		}
		else
			++iter;
	}
	deleteValue( ptr );
}

void JSManager::test( class Character *ch )
{
	std::list<std::pair<int, shared_ptr<JSInstance> > >::iterator iter;

	for( iter = scripts.begin();iter != scripts.end();++iter )
	{
		if( (*iter).second->state.has_property( "ivy" ) )
			ch->Send("Script #%d\r\n", (*iter).first);
		if( (*iter).second->self.to_object().has_property( "ivy" ) )
			ch->Send("Script #%d\r\n", (*iter).first);
		if( (*iter).second->self.to_object().parent().has_property( "ivy" ) )
			ch->Send("Script #%d\r\n", (*iter).first);
	}
	if( global().has_property("ivy") )
		ch->Send("Global scope has the variable.\r\n");
	ch->Send("Scanned %d scripts.\r\n", scripts.size());
}

/*
int JSManager::execute(JSTrigger* trig, Character* self, Character * actor, const char * args, js_extra_data extra, Room * here)
{
return env->execute(trig, self, actor, args, extra, here);
}

int JSManager::execute(JSTrigger* trig, Character* self, Character * actor, const char * args, Room * here)
{
return env->execute(trig, self, actor, args, here);
}

int JSManager::execute(JSTrigger* trig, Character* self, Character * actor, Room * here)
{
return env->execute(trig, self, actor, "", here);
}
*/

std::map< int, JSSocketInfo * > JSDescMap;

JSSocketInfo::JSSocketInfo( kuDescriptor *d )
{
	dUID = ( d ? d->getUid() : (-1) );
	scriptVnum = 0;
	isReadingScript = false;
	isSendingScript = false;
	scriptBufferIndex = 0;
	connectionAuthorized = false;
}

void JSManager::SciteDisconnect()
{
	if( server )
		delete server;
	server = 0;
}

void JSManager::SciteConnect( const int port )
{
	if( server )
		delete server;
	server = new kuListener(port, TCP);

	if( !server->l_IsBound() || !server->l_IsListening() )
	{
		delete server;
		server = 0;
		return;
	}
}
bool JSManager::SciteIsConnected()
{
	return (server && server->l_IsBound() && server->l_IsListening());
}
int JSManager::ScitePort()
{
	return (SciteIsConnected() ? server->l_GetPort() : 0);
}

/* Galnor 11/23/2009 - For the external editor, handle incoming requests */
void JSManager::SocketEvents()
{
	if( server == 0 )
		return;
	std::list< kuDescriptor * > lNewDesc = server->l_AcceptNewHosts();
	server->l_Pulse();
	Descriptor *descriptor = NULL;

	std::list< kuDescriptor * > lDescList = server->l_GetDescriptors();

	for( std::list< kuDescriptor * >::iterator dIter = lNewDesc.begin();dIter != lNewDesc.end();++dIter )
	{
		//All new descriptors come through here. Give them a JSSocketInfo & add to map...
		JSDescMap[ (*dIter)->getUid() ] = new JSSocketInfo( (*dIter) );
	}

	//Now cycle through all live descriptors.
	for( std::list< kuDescriptor * >::iterator dIter = lDescList.begin();dIter != lDescList.end();++dIter )
	{
		JSSocketInfo *sInfo = JSDescMap[ (*dIter)->getUid() ];
		kuDescriptor *d = (*dIter);

		d->getInputBuffer();
		std::string Line = d->getCommand();

		//		if( Line.empty() ) continue;

		std::vector< std::string > vArgs = StringUtil::SplitToContainer< std::vector< std::string >, std::string >(Line, ' ');

		//		if( vArgs.empty() ) continue;

		if(sInfo->username.empty() == false) {

			for(descriptor = descriptor_list;descriptor;descriptor = descriptor->next) {

				if(descriptor->character && descriptor->character->player.name == sInfo->username) {

					break;
				}
			}
		}

		int invisLevel = LVL_APPR;

		if(descriptor && descriptor->character) {

			invisLevel = MAX(LVL_APPR, descriptor->character->player.level);
		}

		if( sInfo->username.empty() && !Line.empty() )
		{//Socket has not yet submitted a user name. This command _should_ be it.
			sInfo->username = Line;
			StringUtil::trim(sInfo->username);
			d->send("__REQ_PASSWORD\n");
			continue;
		}
		else if( sInfo->password.empty() && !Line.empty() )
		{//No password submitted yet. This is the passowrd.
			sInfo->password = Line;
			StringUtil::trim(sInfo->password);

			//Now we need to authenticate the info...

			Character *user = NULL;
			if( !(user = CharacterUtil::loadCharacter(sInfo->username)) )
			{
				d->socketClose();
			}
			else if( user->player.passwd != MD5::getHashFromString(sInfo->password.c_str()) || GET_LEVEL(user) < LVL_APPR )
			{//Data is invalid.
				MudLog(BRF, invisLevel, TRUE, "SciTE: %s failed to log in(bad password).",
					StringUtil::cap(StringUtil::allLower(sInfo->username)).c_str());
				d->socketClose();
			}
			else
			{
				MudLog(BRF, invisLevel, TRUE, "SciTE: %s has logged in successfully.",
					StringUtil::cap(StringUtil::allLower(sInfo->username)).c_str());
				d->send("__REQ_AUTHORIZED\n");
				sInfo->connectionAuthorized = true;
			}
			sInfo->userLevel = GET_LEVEL(user);

			if(user)
				delete user;
			continue;
		}

		if( !sInfo->connectionAuthorized )
		{//How did you get here???
			continue;
		}
		//Anything below here should be authorized sockets only. Anything else should get weeded out above here.

		else if( !vArgs.empty() && vArgs[0] == "__SPECIAL_LOAD_SCRIPT" )
		{
			if( vArgs.size() != 2 )
			{//Invalid. Kill.
				MudLog(BRF, invisLevel, TRUE,
					"SciTE - Invalid number of load parameters: %d - %s", vArgs.size(), d->getIp().c_str());
				d->socketClose();
				continue;
			}
			int vnum = atoi( vArgs[1].c_str() );

			JSTrigger *t = JSManager::get()->getTrigger( vnum );

			if( !t )
				continue;

			sInfo->scriptBuffer = StringUtil::stripCarriageReturn( t->text );
			sInfo->scriptBuffer+= std::string("__SPECIAL_SCRIPT_END");
			sInfo->scriptBufferIndex = 0;
			sInfo->isSendingScript = true;

			sInfo->packetClock.reset( true );

		}
		else if( !vArgs.empty() && vArgs[0] == "__SPECIAL_SAVE_SCRIPT" )
		{
			if( vArgs.size() != 2 )
			{//invalid.
				MudLog(BRF, invisLevel, TRUE,
					"SciTE - Invalid number of save parameters: %d - %s", vArgs.size(), d->getIp().c_str());
				d->socketClose();
				continue;
			}
			sInfo->isReadingScript = true;
			sInfo->scriptVnum = atoi( vArgs[1].c_str() );
		}
		else if( sInfo->isSendingScript )
		{
			float secondsBetweenPackets = (float)(1) / (float)(4);
			int BYTES_PER_SECOND = (1024*20); //20 kilobytes
			int BYTES_PER_PACKET = ( BYTES_PER_SECOND * secondsBetweenPackets );

			sInfo->packetClock.turnOff();
			if( sInfo->packetClock.getSeconds() < (float)((float)1/(float)4) )
			{
				sInfo->packetClock.turnOn();
				continue;
			}

			std::string substr = sInfo->scriptBuffer.substr( sInfo->scriptBufferIndex, BYTES_PER_PACKET );
			d->send( substr.c_str() );

			sInfo->scriptBufferIndex += substr.size();

			sInfo->packetClock.reset( true );
			if( sInfo->scriptBufferIndex >= sInfo->scriptBuffer.size() )
			{
				sInfo->scriptBuffer.clear();
				sInfo->scriptBufferIndex = 0;
				sInfo->isSendingScript = false;
			}
		}
		else if( sInfo->isReadingScript && !Line.empty() )
		{
			std::vector<std::string> commands;
			std::string sBuf;
			commands.push_back(Line);
			while(d->hasCommand())
			{
				commands.push_back(d->getCommand());
			}
			for(unsigned int index = 0;index < commands.size();++index)
			{
				sBuf = commands[ index ];
				if( sBuf == "__SPECIAL_END" )
				{
					JSTrigger *t = JSManager::get()->getTrigger( sInfo->scriptVnum );

					if( !t )
					{
						d->socketClose();
						continue;
					}

					//					Log("Saving script #%d", sInfo->scriptVnum);
					MudLog(BRF, invisLevel, TRUE, "SciTE: Remote JSTrigger(#%d) save. sock: %d, d_UID: %d, IP: %s",
						sInfo->scriptVnum, d->sock, d->getUid(), d->getIp().c_str());

					t->text = sInfo->scriptBuffer;
					t->code_modified = true;
					int rs = JSManager::get()->saveTrigger( t );
					if( rs == -1 )
					{//Compilation error.
						MudLog(BRF, invisLevel, TRUE, "Trigger compilation failed.");
					}
					sInfo->scriptBuffer.clear();
					sInfo->isReadingScript = false;
					sInfo->scriptVnum = 0;
				}
				else
				{//Append...
					//					Log("Appending: %s", sBuf.c_str());
					sInfo->scriptBuffer += sBuf;
					sInfo->scriptBuffer += std::string("\r\n");
				}
			}
		}
		else if( !vArgs.empty() && vArgs[0] == "__REQ_SEARCH" )
		{
			std::string reqStr = Line.substr(vArgs[0].size());
			std::vector<JSTrigger*> results = searchTrigger(reqStr);
			unsigned int pos;
			for(pos = 0;pos < reqStr.size() && reqStr[pos] == ' ';++pos);
			reqStr = reqStr.substr(pos);

			std::stringstream dStr;
			dStr << "Request: " << reqStr;

			d->send(dStr.str().c_str());
			for(unsigned int i = 0;i < results.size();++i)
			{
				d->send( results[i]->name.c_str() );
			}			
		}
	}
}

extern bool keepTriggerOperationalCallbackFunctionAlive;
extern boost::mutex keepTriggerOperationalCallbackFunctionAliveMutex;
extern boost::thread triggerOperationalCallbackThread;

JSManager::~JSManager()
{
	{
		boost::mutex::scoped_lock(keepTriggerOperationalCallbackFunctionAliveMutex);
		keepTriggerOperationalCallbackFunctionAlive = false;
	}
	triggerOperationalCallbackThread.join();

	triggerOperationalCallbackThread.join();
	unordered_map<int, JSTrigger*>::iterator iter = mapper.begin();
	for( ; iter != mapper.end(); ++iter)
	{
		delete iter->second;
	}

	delete env;
}

#endif

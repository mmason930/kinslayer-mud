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

#include <boost/regex.hpp>

#include "JSCharacter.h"
#include "JSObject.h"
#include "JSRoom.h"
#include "js_constants.h"
#include "js_trigger.h"
#include "js_interpreter.h"
#include "js_utils.h"
#include "js_functions.h"
#include "Script.h"

#include "constants.h"
#include "structs.h"
#include "olc.h"
#include "js.h"
#include "handler.h"

#include "Game.h"

#include "kuSockets.h"
#include "kuListener.h"
#include "kuDescriptor.h"
#include "Descriptor.h"
#include "md5.h"

#include "CharacterUtil.h"
#include "StringUtil.h"
#include "SystemUtil.h"
#include "SQLUtil.h"
#include "rooms/Room.h"

#include "js/jsdbgapi.h"


using namespace std;
using namespace tr1;

extern Descriptor *descriptor_list;
extern sql::Connection gameDatabase;

ScriptImportOperation *ScriptImportOperation::addition = new ScriptImportOperation(0, "Addition", 'A');
ScriptImportOperation *ScriptImportOperation::modification = new ScriptImportOperation(1, "Modification", 'U');
ScriptImportOperation *ScriptImportOperation::deletion = new ScriptImportOperation(2, "Deletion", 'D');

void removeTimeout();
void setupTimeout(bool setScriptEndingTime=true);

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

void JSManager::loadScriptMap()
{
	sql::Row row;
	sql::Query query;
	std::stringstream queryBuffer;

	queryBuffer << " SELECT *"
				<< " FROM `script`";

	query = gameDatabase->sendQuery(queryBuffer.str());

	while(query->hasNextRow())
	{
		row = query->getRow();

		int id = row.getInt("id");
		std::string methodName = row.getString("method_name");

		this->scriptMap[id] = new Script(id, methodName);
	}
}

void JSManager::loadTriggers()
{
	sql::Query query;
	sql::Row row;
	std::stringstream queryBuffer;

	queryBuffer << " SELECT *"
				<< " FROM `scriptTrigger`";

	query = gameDatabase->sendQuery(queryBuffer.str());

	while (query->hasNextRow())
	{
		row = query->getRow();

		int vnum = row.getInt("vnum");
		JSTrigger* trigger = new JSTrigger(vnum);

		trigger->scriptId = row.getInt("script_id");
		trigger->name = row["name"];
		trigger->narg = atoi(row["narg"].c_str());
		trigger->args = row["args"];
		trigger->trigger_flags = atol(row["trigger_flags"].c_str());
		trigger->allowed_flags = atol(row["allowed_flags"].c_str());
		trigger->option_flags = atol(row["option_flags"].c_str());
		mapper[vnum] = trigger;
	}
}

void JSManager::loadScriptsFromFilesystem(const std::string &directoryPath)
{
	boost::filesystem::path scriptDirectoryPath(directoryPath);
	boost::filesystem::directory_iterator end;
	for( boost::filesystem::directory_iterator iter(scriptDirectoryPath) ; iter != end ; ++iter )
	{
		boost::filesystem::path iterPath = (*iter).path();

		if(boost::filesystem::is_directory(iterPath))
		{
			if(iterPath.string() == ".svn")
				Log("Skipping directory `.svn`...");
			else
				loadScriptsFromFilesystem(iterPath.string());
			continue;
		}

		if( !(*iter).path().has_extension() || str_cmp((*iter).path().extension().string(), ".js") )
		{
			continue;
		}

		if(!loadScriptsFromFile(iterPath.string()))
		{
			Log("Error while loading script file `%s`.", iterPath.string().c_str());
		}
	}
}

bool JSManager::loadScriptsFromFile(const std::string &filePath)
{
	size_t fileSize;
	char *fileBuffer;

	boost::filesystem::path boostFilePath = boost::filesystem::path(filePath);
	if(!boostFilePath.has_extension() || str_cmp(boostFilePath.extension().string(), ".js"))
	{
		MudLog(BRF, LVL_APPR, TRUE, "Ignoring non JavaScript file `%s`.", filePath.c_str());
		return false;
	}

	MudLog(BRF, LVL_APPR, TRUE, "Processing Script File `%s`...", filePath.c_str());
	
	FILE *fileHandle = fopen(filePath.c_str(), "r");

	if(!fileHandle)
	{
		MudLog(BRF, LVL_APPR, TRUE, "Failed to open file handle.");
		return false;
	}

	//Determine the file size.
	fseek(fileHandle, 0, SEEK_END);
	fileSize = ftell(fileHandle);
	rewind(fileHandle);

	if(fileSize >= MAX_SCRIPT_LENGTH)
	{
		MudLog(BRF, LVL_APPR, TRUE, "File `%s` exceeds the maximum file size of %llu bytes", filePath.c_str(), MAX_SCRIPT_LENGTH);
		fclose(fileHandle);
		return false;
	}

	//Allocate a string large enough to hold the file contents.
	fileBuffer = new char[fileSize + 1];

	size_t bytesRead = fread(fileBuffer, sizeof(*fileBuffer), fileSize, fileHandle);

	if(ferror(fileHandle))
	{
		MudLog(BRF, LVL_APPR, TRUE, "There was an error while reading the file.");
		return false;
	}

	fclose(fileHandle);

	fileBuffer[bytesRead] = '\0';

	std::string scriptContent(fileBuffer);

	delete[] fileBuffer;

	return env->compile(filePath, scriptContent);
}

JSManager::JSManager()
{
	try
	{
		env = new JSEnvironment();
		mapper = unordered_map<int, JSTrigger*>();

		triggerDepth = 0;
		server = 0;
		nextScriptEventId = 0;

		scriptImportReadQueue = new std::list<ScriptImport *>();
		scriptImportWriteQueue = new std::list<ScriptImport *>();

		this->scriptImportThreadRunning = true;
		monitorScriptImportTableThread = new std::thread(&JSManager::monitorScriptImportTable, this, dbContext);

		this->monitorSubversionThreadRunning = true;
		monitorSubversionThread = new std::thread(&JSManager::monitorSubversion, this, dbContext, game->getSubversionRepositoryUrl());

		lastUpdatedRevision = game->getBootScriptsDirectorySubversionRevision();
	}
	catch(sql::QueryException queryException)
	{
		MudLog(BRF, LVL_BUILDER, TRUE, "Error loading scripts: %s", queryException.getMessage().c_str());
		exit(1);
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

void JSManager::monitorScriptImportTable(sql::Context context)
{
	std::string queryString;
	
	{
		std::stringstream queryBuffer;
		queryBuffer << " SELECT *"
					<< " FROM `scriptImportQueue`"
					<< " ORDER BY id ASC";

		queryString = queryBuffer.str();
	}

	sql::Connection connection = context->createConnection();
	time_t timeOfLastPing = time(0);

	while(this->scriptImportThreadRunning)
	{
		try
		{

			if(time(0) - timeOfLastPing >= 60)
			{
				connection->sendQuery("SELECT 1");
				timeOfLastPing = time(0);
			}

			sql::Query query;
			sql::Row row;
			std::vector<unsigned long long> idDeleteQueue;

			query = connection->sendQuery(queryString);

			while(query->hasNextRow())
			{
				ScriptImport *scriptImport = getScriptImport(query->getRow());

				Log("Found Script Import. ID: %llu, File Path `%s`, Queued: %s, Operation: %s", scriptImport->id, scriptImport->filePath.c_str(), scriptImport->queuedDatetime.toString().c_str(), scriptImport->operation->getStandardName().c_str());

				scriptImportWriteQueue->push_back(scriptImport);

				idDeleteQueue.push_back(scriptImport->id);
			}

			//If we have something queued up then swap the queues so the other thread can get it.
			if(!scriptImportWriteQueue->empty())
			{
				{
					std::lock_guard<std::mutex> lock(this->scriptImportMutex);

					std::list<ScriptImport *> *temporaryScriptImportList = this->scriptImportWriteQueue;
					this->scriptImportWriteQueue = this->scriptImportReadQueue;
					this->scriptImportReadQueue = temporaryScriptImportList;
				}

				this->scriptImportWriteQueue->clear();

				std::stringstream queryBuffer;

				queryBuffer << " DELETE FROM `scriptImportQueue`"
							<< " WHERE id IN " << SQLUtil::buildListSQL(idDeleteQueue.begin(), idDeleteQueue.end(), false, true);

				connection->sendRawQuery(queryBuffer.str());

				idDeleteQueue.clear();
			}
		}
		catch(sql::QueryException queryException)
		{
			if(queryException.getMessage().find("MySQL server has gone away") != std::string::npos)
			{
				try
				{
					connection = context->createConnection();
				}
				catch(sql::ConnectionException connectionException)
				{
					MudLog(BRF, LVL_APPR, TRUE, "Subversion Import Thread: Could not connect to game database: %s", connectionException.getMessage().c_str());
				}
			}
			MudLog(BRF, LVL_APPR, TRUE, "Error while fetching from scriptImportQueue: %s", queryException.getMessage().c_str());
		}
		catch(std::exception e)
		{
			MudLog(BRF, LVL_APPR, TRUE, "Error while fetching from scriptImportQueue: %s", e.what());
		}
		catch(...)
		{
			MudLog(BRF, LVL_APPR, TRUE, "Error while fetching from scriptImportQueue: Unknown");
		}
		
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

void printSubversionInfoMap(const std::map<std::string, std::string> &subversionInfoMap)
{
	for(auto iter = subversionInfoMap.begin();iter != subversionInfoMap.end();++iter)
	{
		MudLog(BRF, LVL_APPR, TRUE, "subversionInfoMap[\"%s\"] = `%s`", (*iter).first.c_str(), (*iter).second.c_str());
	}
}

void JSManager::monitorSubversion(sql::Context context, const std::string &repositoryUrl)
{
	std::string filePathPattern = "^([UDA])\\s+(.*?)$";
	std::string updatedRevisionPattern = "Updated to revision ([0-9]+).";
	std::string atRevisionPattern = "At revision ([0-9]+).";

	boost::regex filePathExpression(filePathPattern.c_str());
	boost::regex updatedRevisionExpression(updatedRevisionPattern.c_str());
	boost::regex atRevisionExpression(atRevisionPattern.c_str());

	std::string scriptsDirectory = "scripts/";
	std::map<std::string, std::string> subversionInfoMap;

	sql::Connection connection = context->createConnection();
	time_t timeOfLastPing = time(0);

	while(monitorSubversionThreadRunning)
	{
		try
		{
			if(time(0) - timeOfLastPing >= 60)
			{
				connection->sendQuery("SELECT 1");
				timeOfLastPing = time(0);
			}

			subversionInfoMap.clear();
			subversionInfoMap = SystemUtil::getSubversionInfoMap(repositoryUrl);

			std::string revisionString = subversionInfoMap["Revision"];

			if(revisionString.empty())
			{
				MudLog(BRF, LVL_BLDER, TRUE, "Could not determine revision!");
			}
			else if(!MiscUtil::isInt(revisionString))
			{
				MudLog(BRF, LVL_BLDER, TRUE, "Revision `%s` is not a valid integer.", revisionString.c_str());
			}
			else
			{
				int revision = atoi(revisionString.c_str());

				if(revision != lastUpdatedRevision)
				{
					MudLog(BRF, -1, TRUE, "New revision %d found. Old revision was %d.", revision, lastUpdatedRevision);

					std::string svnUpdateOutput = SystemUtil::processCommand(std::string("svn update ") + scriptsDirectory);

					MudLog(BRF, -1, TRUE, "SVN Update Output: `%s`", svnUpdateOutput.c_str());

					sql::BatchInsertStatement batchInsertStatement(connection, "scriptImportQueue", 1000);
				
					batchInsertStatement.addField("file_path");
					batchInsertStatement.addField("queued_datetime");
					batchInsertStatement.addField("operation");

					batchInsertStatement.start();

					int numberOfFiles = 0;

					std::vector<std::string> outputLines = StringUtil::splitToVector(svnUpdateOutput, '\n');

					for(std::string line : outputLines)
					{
						MudLog(BRF, -1, TRUE, "Processing line: `%s`", line.c_str());
						boost::match_results<std::string::const_iterator> what;
						std::string::const_iterator start = line.begin(), end = line.end();
						if(boost::regex_search(start, end, what, filePathExpression, boost::match_default))
						{
							std::string updateType = what[ 1 ];
							std::string fileName = what[ 2 ];

							if(boost::filesystem::is_directory(fileName))
							{
								MudLog(BRF, LVL_BLDER, TRUE, "Skipping Directory `%s`...", fileName.c_str());
								continue;
							}
						
							if(StringUtil::startsWith(fileName, scriptsDirectory))
								fileName.erase(0, scriptsDirectory.size());

							batchInsertStatement.beginEntry();
						
							batchInsertStatement.putString(fileName);
							batchInsertStatement.putString(sql::encodeDate(time(0)));
							batchInsertStatement.putInt(ScriptImportOperation::getEnumByCharCode(updateType.at(0))->getValue());

							batchInsertStatement.endEntry();

							++numberOfFiles;

							MudLog(BRF, LVL_BLDER, TRUE, "Update Type: %s, File Name: %s", updateType.c_str(), fileName.c_str());
						}

						if( boost::regex_search(start, end, what, updatedRevisionExpression, boost::match_default) ||
							boost::regex_search(start, end, what, atRevisionExpression, boost::match_default))
						{
							std::string revisionString = what[ 1 ];

							if(!MiscUtil::isInt(revisionString))
							{
								MudLog(BRF, LVL_BLDER, TRUE, "Error: Revision found while doing SVN update was not a valid integer. Revision found: `%s`", revisionString.c_str());
							}
							else
							{
								lastUpdatedRevision = atoi(revisionString.c_str());
							}
						}
					}
					
					if(numberOfFiles > 0)
					{
						batchInsertStatement.finish();
					}

					MudLog(BRF, LVL_BLDER, TRUE, "Updated to revision %d.", lastUpdatedRevision);
				}
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		catch(std::exception e)
		{
			MudLog(BRF, LVL_APPR, TRUE, "Exception in subversion monitoring thread: %s", e.what());
			printSubversionInfoMap(subversionInfoMap);
		}
		catch(sql::QueryException e)
		{
			MudLog(BRF, LVL_APPR, TRUE, "SQL Exception in subversion monitoring thread: %s", e.getMessage().c_str());
			printSubversionInfoMap(subversionInfoMap);

			if(e.getMessage().find("MySQL server has gone away") != std::string::npos)
			{
				try
				{
					connection = context->createConnection();
				}
				catch(sql::ConnectionException connectionException)
				{
					MudLog(BRF, LVL_APPR, TRUE, "Subversion Monitoring Thread: Could not connect to game database: %s", connectionException.getMessage().c_str());
				}
			}
		}
		catch(...)
		{
			MudLog(BRF, LVL_APPR, TRUE, "Unknown exception thrown in subversion monitoring thread.");
			printSubversionInfoMap(subversionInfoMap);
		}
	}
}

ScriptImport *JSManager::getScriptImport(const sql::Row &row) const
{
	ScriptImport *scriptImport = new ScriptImport();

	scriptImport->id = MiscUtil::convert<unsigned long long>(row.getString("id"));
	scriptImport->filePath = row.getString("file_path");
	scriptImport->queuedDatetime = DateTime(row.getTimestamp("queued_datetime"));
	scriptImport->operation = (ScriptImportOperation*)ScriptImportOperation::getEnumByValue(row.getInt("operation"));

	return scriptImport;
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

	QueryBuffer << " DELETE FROM `scriptTrigger`"
				<< " WHERE id = " << t->vnum << ";";
	
	try {
		gameDatabase->sendRawQuery( QueryBuffer.str() );
	}
	catch( sql::QueryException e ) {
		MudLog(NRM, LVL_APPR, TRUE, "Failed to send JSTrigger deletion query for #%d : %s", t->vnum, e.getMessage().c_str());
		return;
	}
}

int JSManager::saveTrigger(JSTrigger* t)
{
	if (!t)
		MudLog(BRF, LVL_BUILDER, TRUE, "Null ptr to trigger passed to saveTrigger(). Bad news!");

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
			s	<< " REPLACE DELAYED INTO `scriptTrigger` ("
				<< "   `name`,"
				<< "   `narg`,"
				<< "   `args`,"
				<< "   `trigger_flags`,"
				<< "   `allowed_flags`,"
				<< "   `id`,"
				<< "   `script_id`,"
				<< "   `option_flags`"
				<< " ) VALUES ("
				<< sql::escapeQuoteString(trig->name) << ","
				<< trig->narg << ","
				<< sql::escapeQuoteString(trig->args) << ","
				<< trig->trigger_flags << ","
				<< trig->allowed_flags << ","
				<< trig->vnum << ","
				<< trig->scriptId << ","
				<< trig->option_flags
				<< " );";

		std::string tempstring = s.str();
		//cout << tempstring << endl;
		gameDatabase->sendRawQuery(tempstring);
	}
	catch (sql::QueryException e)
	{
		MudLog(BRF, LVL_APPR, TRUE, "Error saving JS Script %d : %s", trig->vnum, e.getErrorMessage().c_str());
		e.report();
		return -1;
	}

	return 1;
}

void JSManager::addDelayed(std::shared_ptr<JSInstance> instance, int pulses)
{
	// if this is called from the heartbeat function, adding it to the
	// front guarentees that it won't be called twice in the same heartbeat.
	// Not that one extra heartbeat would matter...
	scripts.push_front(make_pair(pulses, instance));
}

int JSManager::execute_timer(std::shared_ptr<JSInstance> instance, bool success)
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

	if( !( pulses % (10 * PASSES_PER_SEC) ) ) {
		JSManager::get()->gc();
	}
	list<pair<int, std::shared_ptr<JSInstance> > >::iterator iter = scripts.begin();
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

	// Also, execute the event handling script
	runTimeouts();

	// Process script imports.
	processScriptImports();
}

void JSManager::processScriptImports()
{
	std::list<ScriptImport *> scriptImportsToProcess;

	//There is no need to hold this lock for the entire operation.
	//Copy everything from the read queue to our locally defined queue.
	{//Obtain the lock.
		std::lock_guard<std::mutex> lock(this->scriptImportMutex);

		if(!this->scriptImportReadQueue->empty())
		{
			std::copy(this->scriptImportReadQueue->begin(), this->scriptImportReadQueue->end(), std::back_inserter(scriptImportsToProcess));

			this->scriptImportReadQueue->clear();
		}
	}//Release the lock.

	//Process everything.
	for(ScriptImport *scriptImport : scriptImportsToProcess)
	{
		MudLog(BRF, LVL_BUILDER, TRUE, "Script import (%s), queued at %s, path `%s`", scriptImport->operation->getStandardName().c_str(), scriptImport->queuedDatetime.toString().c_str(), scriptImport->filePath.c_str());

		if(scriptImport->operation->getValue() == ScriptImportOperation::deletion->getValue())
		{
			MudLog(BRF, LVL_BUILDER, TRUE, "Ignoring operation.");
		}
		else
		{
			this->loadScriptsFromFile(std::string("scripts/") + scriptImport->filePath);
		}

		delete scriptImport;
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
	list<pair<int, std::shared_ptr<JSInstance> > >::iterator iter = scripts.begin();
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

extern bool keepTriggerOperationalCallbackFunctionAlive;
extern std::mutex keepTriggerOperationalCallbackFunctionAliveMutex;
extern std::thread triggerOperationalCallbackThread;

JSManager::~JSManager()
{
	{
		std::lock_guard<std::mutex> lock(keepTriggerOperationalCallbackFunctionAliveMutex);
		keepTriggerOperationalCallbackFunctionAlive = false;
	}
	triggerOperationalCallbackThread.join();

	unordered_map<int, JSTrigger*>::iterator iter = mapper.begin();
	for( ; iter != mapper.end(); ++iter)
	{
		delete iter->second;
	}

	//We need to kill our script import thread.
	this->scriptImportThreadRunning = false;
	this->monitorScriptImportTableThread->join();
	delete this->monitorScriptImportTableThread;

	//Clean up whatever may still be queued up, and then the queues themselves.
	while(!this->scriptImportReadQueue->empty())
	{
		delete this->scriptImportReadQueue->front();
		this->scriptImportReadQueue->pop_front();
	}

	while(!this->scriptImportWriteQueue->empty())
	{
		delete this->scriptImportWriteQueue->front();
		this->scriptImportWriteQueue->pop_front();
	}

	delete this->scriptImportReadQueue;
	delete this->scriptImportWriteQueue;

	//Clean up the subversion monitoring thread.
	this->monitorSubversionThreadRunning = false;
	this->monitorSubversionThread->join();
	delete this->monitorSubversionThread;

	for(auto iter = scriptMap.begin();iter != scriptMap.end();++iter)
	{
		delete (*iter).second;
	}

	if (this->server)
		delete server;

	delete env;
}

Script *JSManager::getScript(int scriptId)
{
	auto iter = this->scriptMap.find(scriptId);
	
	return iter == scriptMap.end() ? NULL : this->scriptMap[scriptId];
}

void JSManager::addScriptEvent(ScriptEvent *scriptEvent)
{
	this->scriptEvents.push_back(scriptEvent);
}

unsigned int JSManager::getNextScriptEventId()
{
	return nextScriptEventId++;
}

void JSManager::runTimeouts()
{
	std::vector<ScriptEvent *> scriptEventsToExecute;
	
	//We must first put all events that we are to execute into a separate queue.
	//This is because the callbacks themselves may alter the permanent queue.

	for(auto iter = scriptEvents.begin();iter != scriptEvents.end();)
	{
		ScriptEvent *scriptEvent = (*iter);
		if(--scriptEvent->pulses == 0)
		{
			scriptEventsToExecute.push_back(scriptEvent);
			iter = scriptEvents.erase(iter);
		}
		else
			++iter;
	}

	//Now go through the temporary queue and execute each callback.
	for(ScriptEvent *scriptEvent : scriptEventsToExecute)
	{
		try
		{
			setupTimeout();
			flusspferd::global().get_property(scriptEvent->propertyName).get_object().call("callback", scriptEvent->arguments);
			//scriptEvent->callback.call(flusspferd::global(), scriptEvent->arguments);
			//flusspferd::global().apply(scriptEvent->callback, scriptEvent->arguments);
			removeTimeout();
		}
		catch(flusspferd::exception e)
		{
			MudLog(NRM, LVL_BUILDER, TRUE, "Error in the runTimeouts() callback: %s", e.what());
		}

		//Failure above should not prevent the next operation from occurring. Therefore we throw it into its own try/catch block.
		try
		{
			//Unroot the event.
			flusspferd::global().delete_property(scriptEvent->propertyName);
		}
		catch(flusspferd::exception e)
		{
			MudLog(NRM, LVL_BUILDER, TRUE, "Error in runTimeouts() : Could not delete script event property %s : %s", scriptEvent->propertyName.c_str(), e.what());
		}

		delete scriptEvent;
	}
}

std::map<int, Script*>::const_iterator JSManager::getScriptMapStartIterator() const
{
	return scriptMap.begin();
}

std::map<int, Script*>::const_iterator JSManager::getScriptMapEndIterator() const
{
	return scriptMap.end();
}

Script *JSManager::getScriptByMethodName(const std::string &methodName, bool caseSensitive)
{
	for(auto iter = scriptMap.begin();iter != scriptMap.end();++iter)
	{
		Script *script = (*iter).second;

		if(caseSensitive)
		{
			if(methodName == script->getMethodName())
				return script;
		}
		else
		{
			if(!str_cmp(methodName, script->getMethodName()))
				return script;
		}
	}

	return NULL;
}

void JSManager::putScript(sql::Connection connection, Script *script)
{
	std::stringstream sqlBuffer;

	if(script->isNew())
	{
		sqlBuffer	<< " INSERT INTO script("
					<< "   `method_name`,"
					<< "   `created_by_user_id`,"
					<< "   `created_datetime`,"
					<< "   `last_modified_by_user_id`,"
					<< "   `last_modified_datetime`"
					<< " ) VALUES ("
					<< sql::escapeQuoteString(script->getMethodName()) << ","
					<< script->getCreatedByUserId() << ","
					<< sql::encodeQuoteDate(script->getCreatedDatetime().getTime()) << ","
					<< script->getLastModifiedByUserId() << ","
					<< sql::encodeQuoteDate(script->getLastModifiedDatetime().getTime())
					<< " )";

		connection->sendRawQuery(sqlBuffer.str());

		script->setId(connection->lastInsertID());
	}
	else
	{
		sqlBuffer	<< " UPDATE script SET"
					<< "   method_name = " << sql::escapeQuoteString(script->getMethodName()) << ","
					<< "   created_by_user_id = " << script->getCreatedByUserId() << ","
					<< "   created_datetime = " << sql::encodeQuoteDate(script->getCreatedDatetime().getTime()) << ","
					<< "   last_modified_by_user_id = " << script->getLastModifiedByUserId() << ","
					<< "   last_modified_datetime = " << sql::encodeQuoteDate(script->getLastModifiedDatetime().getTime())
					<< " WHERE id = " << script->getId();

		connection->sendRawQuery(sqlBuffer.str());
	}
}

void JSManager::addScriptToMap(Script *script)
{
	scriptMap[ script->getId() ] = script;
}

void JSManager::deleteScriptCompletely(sql::Connection connection, const int scriptId)
{
	deleteScriptFromMap(scriptId);
	deleteScriptFromDatabase(connection, scriptId);
}

void JSManager::deleteScriptFromMap(const int scriptId)
{
	auto iter = scriptMap.find(scriptId);

	if(iter != scriptMap.end())
	{
		delete (*iter).second;
		scriptMap.erase(iter);
	}
}

void JSManager::deleteScriptFromDatabase(sql::Connection connection, int scriptId)
{
	std::stringstream sqlBuffer;

	sqlBuffer	<< " DELETE FROM script"
				<< " WHERE id = " << scriptId;

	connection->sendRawQuery(sqlBuffer.str());
}

const char *JSManager::getFunctionFilename(const std::string &functionName)
{
	if(!flusspferd::global().has_property(functionName) || !flusspferd::global().get_property(functionName).is_function())
		return NULL;

	JSContext *context = flusspferd::Impl::get_context(flusspferd::current_context());

	if(context == NULL)
		return NULL;

	JSObject *globalObject = JS_GetGlobalObject(context);
	jsval methodValue;
	JS_GetProperty(context, globalObject, functionName.c_str(), &methodValue);
	JSFunction *function = JS_ValueToFunction(context, methodValue);
	
	if(function == NULL)
		return NULL;

	JSScript *jsScript = JS_GetFunctionScript(context, function);
	if(jsScript == NULL)
		return NULL;

	return JS_GetScriptFilename(context, jsScript);
}

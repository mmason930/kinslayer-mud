#include "ObjectMoveLogger.h"

#include "utils.h"

ObjectMoveLogger::ObjectMoveLogger()
{
	this->objectMoveLogEntries = new std::list<ObjectMoveLogEntry>();
	this->running = true;
}

ObjectMoveLogger::~ObjectMoveLogger()
{
	freeEntries(*objectMoveLogEntries);
	delete this->objectMoveLogEntries;
}

void ObjectMoveLogger::logObjectMove(const boost::uuids::uuid &objectId, const std::string &message)
{
	ObjectMoveLogEntry objectMoveLogEntry;

	objectMoveLogEntry.objectId = objectId;
	objectMoveLogEntry.message = str_dup(message.c_str());
	objectMoveLogEntry.timestamp = time(0);

	{
		std::lock_guard<std::mutex> lock(objectMoveLogEntriesMutex);
		objectMoveLogEntries->push_back(objectMoveLogEntry);
	}
}

void ObjectMoveLogger::kill()
{
	this->running = false;
}

void ObjectMoveLogger::freeEntries(std::list<ObjectMoveLogEntry> &entries)
{
	for (ObjectMoveLogEntry objectMoveLogEntry : entries)
		delete[] objectMoveLogEntry.message;
}

void ObjectMoveLogger::threadHandler()
{
	std::list<ObjectMoveLogEntry> *threadObjectMoveLogEntries;

	while(running)
	{
		threadObjectMoveLogEntries = NULL;
		{
			std::lock_guard<std::mutex> lock(objectMoveLogEntriesMutex);
			if(this->objectMoveLogEntries->size() > 0)
			{
				threadObjectMoveLogEntries = this->objectMoveLogEntries;
				objectMoveLogEntries = new std::list<ObjectMoveLogEntry>();
			}
		}

		if(threadObjectMoveLogEntries != NULL)
		{
			std::ofstream logFile("ObjectMove.log", std::fstream::app);

			if(!logFile.is_open())
			{
				Log("Could not open object move log file.");
			}
			else
			{

				for(auto iter = threadObjectMoveLogEntries->begin();iter != threadObjectMoveLogEntries->end();++iter)
				{
					logFile	<< MiscUtil::formatDateYYYYdmmdddHHcMMcSS(DateTime((*iter).timestamp)) << "\t"
						<< ToString((*iter).objectId) << "\t"
						<< (*iter).message << std::endl;
				}
			
				logFile.close();
			}
			freeEntries(*threadObjectMoveLogEntries);
			delete threadObjectMoveLogEntries;
		}

		std::this_thread::sleep_for( std::chrono::milliseconds(100) );
	}
}

#include "ObjectMoveLogger.h"

#include "utils.h"
#include "StringUtil.h"
#include "MiscUtil.h"
#include "DateTime.h"

ObjectMoveLogger::ObjectMoveLogger()
{
	this->objectMoveLogEntries = new std::list<ObjectMoveLogEntry>();
}

ObjectMoveLogger::~ObjectMoveLogger()
{
	delete this->objectMoveLogEntries;
}

void ObjectMoveLogger::logObjectMove(const boost::uuids::uuid &objectId, const std::string &message)
{
	ObjectMoveLogEntry objectMoveLogEntry;

	objectMoveLogEntry.objectId = objectId;
	objectMoveLogEntry.message = str_dup(message.c_str());
	objectMoveLogEntry.timestamp = time(0);

	{
		boost::mutex::scoped_lock(objectMoveLogEntriesMutex);
		objectMoveLogEntries->push_back(objectMoveLogEntry);
	}
}

void ObjectMoveLogger::threadHandler()
{
	std::list<ObjectMoveLogEntry> *threadObjectMoveLogEntries;

	while(true)
	{

		threadObjectMoveLogEntries = NULL;
		{
			boost::mutex::scoped_lock(objectMoveLogEntriesMutex);
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
			delete threadObjectMoveLogEntries;
		}

		boost::this_thread::sleep( boost::posix_time::seconds(1) );
	}
}

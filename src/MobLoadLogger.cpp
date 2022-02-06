#include "MobLoadLogger.h"

#include "utils.h"

std::thread *mobLoadLoggerThread;
MobLoadLogger mobLoadLogger;

MobLoadLogger::MobLoadLogger()
{
	this->mobLoadLogEntries = new std::list<MobLoadLogEntry>();
	this->running = true;
}

MobLoadLogger::~MobLoadLogger()
{
	freeEntries(*mobLoadLogEntries);
	delete this->mobLoadLogEntries;
}

void MobLoadLogger::logMobLoad(const int mobVnum, const std::string &message)
{
	MobLoadLogEntry mobLoadLogEntry;

	mobLoadLogEntry.mobVnum = mobVnum;
	mobLoadLogEntry.message = str_dup(message.c_str());
	mobLoadLogEntry.timestamp = time(0);

	{
		std::lock_guard<std::mutex> lock(mobLoadLogEntriesMutex);
		mobLoadLogEntries->push_back(mobLoadLogEntry);
	}
}

void MobLoadLogger::kill()
{
	this->running = false;
}

void MobLoadLogger::freeEntries(std::list<MobLoadLogEntry> &entries)
{
	for (MobLoadLogEntry mobLoadLogEntry : entries)
		delete[] mobLoadLogEntry.message;
}

void MobLoadLogger::threadHandler()
{
	std::list<MobLoadLogEntry> *threadMobLoadLogEntries;

	while(running)
	{
		threadMobLoadLogEntries = NULL;
		{
			std::lock_guard<std::mutex> lock(mobLoadLogEntriesMutex);
			if(this->mobLoadLogEntries->size() > 0)
			{
				threadMobLoadLogEntries = this->mobLoadLogEntries;
				mobLoadLogEntries = new std::list<MobLoadLogEntry>();
			}
		}

		if(threadMobLoadLogEntries != NULL)
		{
			std::ofstream logFile("MobLoad.log", std::fstream::app);

			if(!logFile.is_open())
			{
				Log("Could not open mob load log file.");
			}
			else
			{

				for(auto mobLoadLogEntry : (*threadMobLoadLogEntries))
				{
					logFile	<< MiscUtil::formatDateYYYYdmmdddHHcMMcSS(DateTime(mobLoadLogEntry.timestamp)) << "\t"
						<< ToString(mobLoadLogEntry.mobVnum) << "\t"
						<< mobLoadLogEntry.message << std::endl;
				}
			
				logFile.close();
			}
			freeEntries(*threadMobLoadLogEntries);
			delete threadMobLoadLogEntries;
		}

		std::this_thread::sleep_for( std::chrono::milliseconds(100) );
	}
}

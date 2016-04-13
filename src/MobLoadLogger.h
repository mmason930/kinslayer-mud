#ifndef MOB_LOAD_LOGGER_H
#define MOB_LOAD_LOGGER_H

#include <thread>
#include <mutex>
#include <list>
#include <ctime>

struct MobLoadLogEntry
{
public:
	int mobVnum;
	char *message;
	time_t timestamp;

};

class MobLoadLogger
{
private:
	std::list<MobLoadLogEntry> *mobLoadLogEntries;
	std::mutex mobLoadLogEntriesMutex;
	bool running;
public:
	MobLoadLogger();
	~MobLoadLogger();
	void logMobLoad(const int mobVnum, const std::string &message);
	void threadHandler();

	void kill();
	void freeEntries(std::list<MobLoadLogEntry> &entries);
};

extern std::thread *mobLoadLoggerThread;
extern MobLoadLogger mobLoadLogger;

#endif
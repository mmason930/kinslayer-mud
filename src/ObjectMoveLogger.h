#ifndef OBJECT_MOVE_LOGGER_H
#define OBJECT_MOVE_LOGGER_H

#include <thread>
#include <mutex>
#include <list>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ctime>

struct ObjectMoveLogEntry
{
public:
	boost::uuids::uuid objectId;
	char *message;
	time_t timestamp;

};

class ObjectMoveLogger
{
private:
	std::list<ObjectMoveLogEntry> *objectMoveLogEntries;
	std::mutex objectMoveLogEntriesMutex;
	bool running;
public:
	ObjectMoveLogger();
	~ObjectMoveLogger();
	void logObjectMove(const boost::uuids::uuid &objectId, const std::string &message);
	void threadHandler();

	void kill();
	void freeEntries(std::list<ObjectMoveLogEntry> &entries);
};

#endif
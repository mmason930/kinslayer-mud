#ifndef OBJECT_MOVE_LOGGER_H
#define OBJECT_MOVE_LOGGER_H

#include <boost/thread.hpp>
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
	boost::mutex objectMoveLogEntriesMutex;
public:
	ObjectMoveLogger();
	~ObjectMoveLogger();
	void logObjectMove(const boost::uuids::uuid &objectId, const std::string &message);
	void threadHandler();
};

#endif
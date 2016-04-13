#ifndef THREADED_LOG_FILE_H
#define THREADED_LOG_FILE_H

#include <string>
#include <mutex>
#include <thread>
#include <vector>

#include "ThreadedLogFileEntry.h"

class ThreadedLogFile
{
private:
protected:
	std::mutex queuedLogMessagesMutex;
	std::vector<ThreadedLogFileEntry> queueOne; //First queue.
	std::vector<ThreadedLogFileEntry> queueTwo; //Second queue.

	std::vector<ThreadedLogFileEntry> *writeQueue; //The queue that is currently shared between threads.
	std::vector<ThreadedLogFileEntry> *flushQueue; //The queue that is currently writing to file. Not shared.

	std::thread *logThread;
	bool isRunning;
	std::string filePath;
public:

	ThreadedLogFile();
	ThreadedLogFile(const std::string &filePath);
	~ThreadedLogFile();

	void addMessage(const std::string &message);
	void setFilePath(const std::string &filePath);

	void begin();
	void run();
	void processMessages();
	void terminate();
	void shutdown();
};

#endif

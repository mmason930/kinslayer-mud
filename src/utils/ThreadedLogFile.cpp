#include "../conf.h"
#include "../sysdep.h"

#include "../utils.h"

#include <boost/filesystem.hpp>
#include "ThreadedLogFile.h"

ThreadedLogFile::ThreadedLogFile(const std::string &filePath)
{
	setFilePath(filePath);
}

ThreadedLogFile::ThreadedLogFile()
{
}

ThreadedLogFile::~ThreadedLogFile()
{
}

void ThreadedLogFile::addMessage(const std::string &message)
{
	timeval currentTimeval;
	gettimeofday(&currentTimeval, nullptr);
	{
		std::lock_guard<std::mutex> lock(this->queuedLogMessagesMutex);
		writeQueue->push_back(ThreadedLogFileEntry(message, currentTimeval));
	}
}

void ThreadedLogFile::setFilePath(const std::string &filePath)
{
	this->filePath = filePath;
}

void ThreadedLogFile::begin()
{
	isRunning = true;
	writeQueue = &queueOne;
	flushQueue = &queueTwo;
	logThread = new std::thread(std::bind(&ThreadedLogFile::run, this));
}

void ThreadedLogFile::processMessages()
{
	const int FILE_NAME_BUFFER_SIZE = 256;
	const int TIMESTAMP_BUFFER_SIZE = 64;
	char currentFileName[FILE_NAME_BUFFER_SIZE];
	char nextFileName[FILE_NAME_BUFFER_SIZE];

	{
		std::lock_guard<std::mutex> lock(this->queuedLogMessagesMutex);

		if(!writeQueue->empty())
		{//Swap the queues if we need to.
			auto tempQueue = writeQueue;
			writeQueue = flushQueue;
			flushQueue = tempQueue;
		}
	}
		
	//Write to file.

	std::ofstream currentOutputStream;
	for(auto entry : *flushQueue)
	{
		time_t submittedTime = (time_t)entry.submittedTimeval.tv_sec;
		tm* submittedTm = localtime(&submittedTime);

		strftime(nextFileName, FILE_NAME_BUFFER_SIZE, filePath.c_str(), submittedTm);

		if(strcmp(currentFileName, nextFileName))
		{//Date changed. Close current file and open next.

			if(currentOutputStream.is_open())
				currentOutputStream.close();
			
			boost::filesystem::path logFileDirectoryPath = boost::filesystem::path(nextFileName).parent_path();
			
			if(!boost::filesystem::exists(logFileDirectoryPath))
			{
				Log("Log file directory path `%s` does not exist. Creating it.", logFileDirectoryPath.c_str());
				boost::filesystem::create_directories(logFileDirectoryPath);
			}

			currentOutputStream.open(nextFileName, std::ios::app);

			if(!currentOutputStream.is_open())
			{
				Log("ERROR : ThreadedLogFile : Failed to open log file `%s` : ", nextFileName);
				continue;//Try again next time?
			}
		}

		//Write to file.
		
		currentOutputStream << MiscUtil::formatDateYYYYdmmdddHHcMMcSS(DateTime(submittedTime))
			<< "." << std::setfill('0') << std::setw(3) << (entry.submittedTimeval.tv_usec / 1000)
			<< " : " << entry.message << std::endl;
	}

	if(currentOutputStream.is_open())
		currentOutputStream.close();

	flushQueue->clear();
}

void ThreadedLogFile::run()
{
	while(isRunning)
	{
		processMessages();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	processMessages();
}

void ThreadedLogFile::terminate()
{
	isRunning = false;
}

void ThreadedLogFile::shutdown()
{
	terminate();
	logThread->join();
}
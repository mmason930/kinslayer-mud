#ifndef THREADED_LOG_FILE_ENTRY_H
#define THREADED_LOG_FILE_ENTRY_H

#include "../DateTime.h"

class ThreadedLogFileEntry
{
private:
protected:
public:

	timeval submittedTimeval;
	std::string message;

	ThreadedLogFileEntry(const std::string &message, const timeval &submittedTimeval)
	{
		this->message = message;
		this->submittedTimeval = submittedTimeval;
	}
};

#endif

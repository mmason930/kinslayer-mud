
#ifndef THREADED_JOBS_H
#define THREADED_JOBS_H

#include <thread>
#include <mutex>
#include <boost/uuid/uuid.hpp>
#include <string>
#include <list>
#include <queue>

class Job
{
public:
	Job() {};
	~Job() {};
	virtual void performRoutine()=0;			//Routine performed in separate thread.
	virtual void performPostJobRoutine()=0;		//Routine performed after routine thread has exited.
};

class ThreadedJobManager
{
	static ThreadedJobManager* pSelf;
	ThreadedJobManager();
	~ThreadedJobManager();
	std::list< Job* > lJobs;
	std::list< Job* > lFinishedJobs;
	std::queue< std::string > qResults;

	std::mutex resultMutex;
	std::mutex jobsMutex;
	std::mutex finishedJobsMutex;

	void runJob( Job *job );
public:
	static ThreadedJobManager& get();

	void free();
	void addJob( Job *job  );
	void cycleThroughFinishedJobs();
};

#endif

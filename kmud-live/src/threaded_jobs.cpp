/********************************************************************
 *	threaded_jobs.cpp - Implements the ThreadedJobsManager class	*
 *			 This file is responsible for handling various jobs		*
 *           which require multithreading.							* 
 *	Added by Galnor in April of 2010 (Sortak@gmail.com)				*
 *																	*
 ********************************************************************/

#include "threaded_jobs.h"

ThreadedJobManager* ThreadedJobManager::pSelf = NULL;

ThreadedJobManager::ThreadedJobManager()
{
	//...
}
ThreadedJobManager::~ThreadedJobManager()
{
	//...
}
/***
  *
  * get( void ) - Singleton accessor. Allocates if it doesn't exist.
  * ~~~Galnor 04/17/2010
  **/
ThreadedJobManager& ThreadedJobManager::get()
{
	if( !pSelf ) pSelf = new ThreadedJobManager();
	return *pSelf;
}
/***
  *
  * free( void ) - Frees up any memory used by this class, and kills all existing jobs in the queue.
  * ~~~Galnor 04/17/2010
  **/
void ThreadedJobManager::free()
{
	delete pSelf;
	while( lJobs.empty() == false ) {
		delete lJobs.front();
		lJobs.pop_front();
	}
	while( lFinishedJobs.empty() == false ) {
		delete lFinishedJobs.front();
		lFinishedJobs.pop_front();
	}
	pSelf = NULL;
}

/***
  *
  * addJob( Job* ) - Appends a Job to the lJobs list and begins processing it in a new thread.
  *
  * ~~~Galnor 04/17/2010
  **/
void ThreadedJobManager::addJob( Job *job )
{
	{//Lock
		std::lock_guard<std::mutex> m(jobsMutex);
		lJobs.push_back( job );
	}//Release
	std::thread thread( &ThreadedJobManager::runJob, this, job );
	thread.detach();
}

/***
  *
  * runJob( Job* ) - Thread entry point for the job's routine. This will process the performRoutine()
  *           method for the Job class, remove from the lJobs list, and add to lFinishedJobs once it
  *	          completes its processing.
  * ~~~Galnor 04/17/2010
  **/
void ThreadedJobManager::runJob( Job *job )
{
	job->performRoutine();
	{//Lock
		std::lock_guard<std::mutex> m(jobsMutex);
		this->lJobs.remove( job );
	}//Release
	{//Lock
		std::lock_guard<std::mutex> m(finishedJobsMutex);
		this->lFinishedJobs.push_back( job );
	}//Release
}
/***
  *
  * cycleThroughFinishedJobs( void ) - Cycles through all completed jobs, and runs the
  *                respective Job::performPostRoutine method for each of them.
  * ~~~Galnor 04/17/2010
  **/
void ThreadedJobManager::cycleThroughFinishedJobs()
{
	{//Lock
		std::lock_guard<std::mutex> m(finishedJobsMutex);
		for(std::list< Job* >::iterator fjIter = lFinishedJobs.begin();fjIter != lFinishedJobs.end();++fjIter)
		{
			(*fjIter)->performPostJobRoutine();
			delete (*fjIter);
		}
		lFinishedJobs.clear();
	}//Release
}

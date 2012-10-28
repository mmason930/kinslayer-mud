#ifndef HTTP_QUEUE_H
#define HTTP_QUEUE_H

#include <list>
#include <boost/thread.hpp>

#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpQueue
{
protected:
	std::list<HttpRequest *> requestQueue;
	std::list<HttpResponse *> responseQueue;

	boost::mutex requestQueueMutex;
	boost::mutex responseQueueMutex;
	
public:
	HttpRequest *getNextRequest();
	HttpResponse *getNextResponse();

	void putRequest(HttpRequest *request);
	void putResponse(HttpResponse *response);
};

#endif

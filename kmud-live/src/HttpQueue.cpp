#include "HttpQueue.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

HttpRequest *HttpQueue::getNextRequest()
{
	boost::mutex::scoped_lock lock(requestQueueMutex);

	if(requestQueue.empty())
		return NULL;

	HttpRequest *request = requestQueue.front();
	requestQueue.pop_front();
	return request;
}

HttpResponse *HttpQueue::getNextResponse()
{
	boost::mutex::scoped_lock lock(responseQueueMutex);

	if(responseQueue.empty())
		return NULL;

	HttpResponse *response = responseQueue.front();
	responseQueue.pop_front();
	return response;
}

void HttpQueue::putRequest(HttpRequest *request)
{
	boost::mutex::scoped_lock lock(requestQueueMutex);
	requestQueue.push_back(request);
}

void HttpQueue::putResponse(HttpResponse *response)
{
	boost::mutex::scoped_lock lock(responseQueueMutex);
	responseQueue.push_back(response);
}

#include "HttpQueue.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

HttpRequest *HttpQueue::getNextRequest()
{
	std::lock_guard<std::mutex> lock(requestQueueMutex);

	if(requestQueue.empty())
		return NULL;

	HttpRequest *request = requestQueue.front();
	requestQueue.pop_front();
	return request;
}

HttpResponse *HttpQueue::getNextResponse()
{
	std::lock_guard<std::mutex> lock(responseQueueMutex);

	if(responseQueue.empty())
		return NULL;

	HttpResponse *response = responseQueue.front();
	responseQueue.pop_front();
	return response;
}

void HttpQueue::putRequest(HttpRequest *request)
{
	std::lock_guard<std::mutex> lock(requestQueueMutex);
	requestQueue.push_back(request);
}

void HttpQueue::putResponse(HttpResponse *response)
{
	std::lock_guard<std::mutex> lock(responseQueueMutex);
	responseQueue.push_back(response);
}

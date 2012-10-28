#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <map>
#include <boost/thread.hpp>

#include "HttpResource.h"
#include "HttpDescriptor.h"
#include "HttpQueue.h"

class HttpDescriptor;

class HttpServer
{
private:
	int port;
	unsigned int topRequestId;
	std::map<std::string, class HttpResource*> resources;
	boost::mutex resourceMutex;
	boost::thread mainThread;
	std::list<HttpDescriptor *> httpDescriptors;
	HttpQueue *queue;

	void run();
public:

	HttpServer();
	int getPort();

	void deploy(int port);
	void disconnect();
	void addResource(HttpResource *resource);
	bool hasResource(const std::string &resourceName) const;
	HttpResource *getResource(const std::string &resourceName) const;
	HttpDescriptor *getHttpDescriptor(kuDescriptor *kuDescriptor);
	void removeHttpDescriptor(HttpDescriptor *httpDescriptor);

	HttpQueue *getQueue();
	void setQueue(HttpQueue *queue);

	unsigned int getNextRequestId();
	HttpDescriptor *getDescriptorByRequestId(unsigned int requestId);
};

#endif

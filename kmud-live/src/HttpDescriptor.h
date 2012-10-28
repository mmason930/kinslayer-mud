#ifndef HTTP_DESCRIPTOR_H
#define HTTP_DESCRIPTOR_H

#include <string>
#include "kuDescriptor.h"
#include "HttpRequest.h"
#include "HttpServer.h"

class HttpServer;

class HttpDescriptor
{
protected:
	kuDescriptor *descriptor;
	HttpRequest *request;
	HttpServer *server;
	std::string packet;
	bool isReadingFirstLine;
	bool isReadingHeaders;
	bool isReadingBody;
	bool responseSent;
public:

	HttpDescriptor();
	HttpDescriptor(kuDescriptor *descriptor, HttpServer *server);
	void reset();

	void read(const std::string &newPacketData);
	bool isReading()
	{
		return isReadingHeaders || isReadingBody || isReadingFirstLine;
	}

	kuDescriptor *getKuDescriptor()
	{
		return descriptor;
	}

	HttpRequest *getRequest()
	{
		return request;
	}

	bool getResponseSent()
	{
		return responseSent;
	}

	void setResponseSent(bool responseSent)
	{
		this->responseSent = responseSent;
	}
};

#endif

#ifndef TEST_RESOURCE_H
#define TEST_RESOURCE_H

#include "HttpResource.h"

class TestResource : public HttpResource
{
public:
	std::string getPath()
	{
		return "/test.html";
	}

	HttpResponse *handleRequest(HttpServer *server, HttpRequest *request);
};

#endif

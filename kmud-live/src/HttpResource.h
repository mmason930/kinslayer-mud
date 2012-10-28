#ifndef HTTP_RESOURCE
#define HTTP_RESOURCE

#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"

class HttpServer;
class HttpRequest;

class HttpResource
{
protected:
public:

	HttpResource() {}
	~HttpResource() {}

	virtual std::string getPath() = 0;
	virtual HttpResponse *handleRequest(HttpServer *server, HttpRequest *request) = 0;
};

#endif

#include "TestResource.h"

#include <iostream>

HttpResponse *TestResource::handleRequest(HttpServer *server, HttpRequest *request)
{
	std::cout << "Body received in test resource: " << request->getBody() << std::endl;

	HttpResponse *response = new HttpResponse();
	response->setResponseCode(200);
	response->setResponseBody("<b>TEST RESPONSE!!!!!</b>");
	response->setId(request->getId());
	return response;
}
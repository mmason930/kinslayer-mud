#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

#include <string>
#include <map>
#include "HttpProtocol.h"
#include "HttpRequest.h"

class HttpUtil
{
public:
	static void parseHeader(const std::string &headerLine, std::string &headerName, std::string &headerValue);
	static void parseRequestLine(const std::string &requestLine, HttpProtocol **protocol, std::string &resource, float &version);
	static void parseGetParametersFromResourceUri(const std::string &resourceUri, HttpRequest *request);
	static std::string parseResourceNameFromFullResourceUri(const std::string &resourceUri);
};

#endif

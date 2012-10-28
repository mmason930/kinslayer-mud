#include "HttpResponse.h"
#include "HttpException.h"

#include <sstream>

#include "MiscUtil.h"

void HttpResponse::setResponseCode(int responseCode)
{
	this->responseCode = responseCode;
}
int HttpResponse::getResponseCode() const
{
	return responseCode;
}

bool HttpResponse::hasHeader(const std::string &headerName) const
{
	return headers.find(headerName) != headers.end();
}
std::string HttpResponse::getHeader(const std::string &headerName) const
{
	if(!hasHeader(headerName))
		throw HttpException( (std::string("Header `") + headerName + std::string("` not found in response")).c_str() );
}
void HttpResponse::setHeader(const std::string &headerName, const std::string &headerValue)
{
	headers[ headerName ] = headerValue;
}

std::string HttpResponse::getResponseBody() const
{
	return responseBody;
}
void HttpResponse::setResponseBody(const std::string &responseBody)
{
	this->responseBody = responseBody;
}

std::string HttpResponse::generatePacket()
{
	std::string buffer;

	buffer += std::string("HTTP/1.1 ") + MiscUtil::toString(getResponseCode()) + std::string("OK\r\n");
	buffer += std::string("Date: Sun, 12 Aug 2012 17:05:15 GMT\r\n");
	buffer += std::string("Server: Apache/2.2.14 (Ubuntu)\r\n");
	buffer += std::string("X-Powered-By: PHP/5.3.2-1ubuntu4.17\r\n");
	buffer += std::string("Cache-Control: private, no-cache=\"set-cookie\"\r\n");
	buffer += std::string("Expires: 0\r\n");
	buffer += std::string("Pragma: no-cache\r\n");
	buffer += std::string("Vary: Accept-Encoding\r\n");
	buffer += std::string("Content-Length: ") + MiscUtil::toString(getResponseBody().size()) + std::string("\r\n");
	buffer += std::string("Keep-Alive: timeout=15, max=100\r\n");
	buffer += std::string("Connection: Keep-Alive\r\n");
	buffer += std::string("Content-Type: text/html; charset=UTF-8\r\n");
	buffer += std::string("\r\n");
	buffer += getResponseBody();

	return buffer;
}

unsigned int HttpResponse::getId()
{
	return id;
}

void HttpResponse::setId(unsigned int id)
{
	this->id = id;
}

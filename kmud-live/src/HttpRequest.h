#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

#include "HttpProtocol.h"

class HttpRequest
{
protected:
	unsigned int id;
	HttpProtocol *protocol;
	std::string resource;
	std::string body;
	std::map<std::string, std::string> parameters;
	std::map<std::string, std::string> headers;
	float version;
public:

	HttpProtocol *getProtocol() const;
	void setProtocol(HttpProtocol *protocol);
	std::string getResource() const;
	void setResource(const std::string &resource);
	std::string getBody() const;
	void setBody(const std::string &body);
	void appendToBody(const std::string &buffer);
	int getBodySize();

	std::string getHeader(const std::string &headerName) const;
	bool hasHeader(const std::string &headerName) const;
	void setHeader(const std::string &headerName, const std::string &headerValue);

	float getVersion() const;
	void setVersion(const float version);

	void setParameter(const std::string &parameterName, const std::string parameterValue);
	std::string getParameter(const std::string &parameterName) const;
	bool hasParameter(const std::string &parameterName) const;
	
	unsigned int getId();
	void setId(unsigned int id);
};

#endif

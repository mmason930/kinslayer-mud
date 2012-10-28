#include "HttpRequest.h"
#include "HttpException.h"

HttpProtocol *HttpRequest::getProtocol() const
{
	return protocol;
}
void HttpRequest::setProtocol(HttpProtocol *protocol)
{
	this->protocol = protocol;
}
std::string HttpRequest::getResource() const
{
	return resource;
}
void HttpRequest::setResource(const std::string &resource)
{
	this->resource = resource;
}
std::string HttpRequest::getBody() const
{
	return body;
}
void HttpRequest::setBody(const std::string &body)
{
	this->body = body;
}

std::string HttpRequest::getHeader(const std::string &headerName) const
{
	if(hasHeader(headerName))
	{
		auto iter = headers.find(headerName);
		return (*iter).second;
	}

	throw new HttpException( (std::string("No header `") + headerName + std::string("`")).c_str() );
}
bool HttpRequest::hasHeader(const std::string &headerName) const
{
	return headers.find(headerName) != headers.end();
}
void HttpRequest::setHeader(const std::string &headerName, const std::string &headerValue)
{
	headers[ headerName ] = headerValue;
}

float HttpRequest::getVersion() const
{
	return version;
}
void HttpRequest::setVersion(const float version)
{
	this->version = version;
}
void HttpRequest::appendToBody(const std::string &buffer)
{
	body += buffer;
}
int HttpRequest::getBodySize()
{
	return body.size();
}

void HttpRequest::setParameter(const std::string &parameterName, const std::string parameterValue)
{
	parameters[ parameterName ] = parameterValue;
}
bool HttpRequest::hasParameter(const std::string &parameterName) const
{
	return parameters.find(parameterName) != parameters.end();
}
std::string HttpRequest::getParameter(const std::string &parameterName) const
{
	if(!hasParameter(parameterName))
		throw HttpException( (std::string("Parameter `") + parameterName + std::string("` not found")).c_str());
	auto iter = parameters.find(parameterName);
	return (*iter).second;
}

unsigned int HttpRequest::getId()
{
	return id;
}

void HttpRequest::setId(unsigned int id)
{
	this->id = id;
}

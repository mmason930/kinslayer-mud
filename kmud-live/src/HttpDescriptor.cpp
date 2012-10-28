#include "HttpDescriptor.h"

#include "StringUtil.h"
#include "HttpUtil.h"
#include "HttpProtocol.h"
#include "HttpException.h"

void HttpDescriptor::read(const std::string &newPacketData)
{
	packet += newPacketData;

	//Now attempt to process data.

	if(!isReadingBody)
	{
		//Strip lines.
		while(true)
		{
			std::string::size_type crlfPos = packet.find_first_of("\r\n");
			if(crlfPos == std::string::npos)
				break;//This is an incomplete line. We will stop processing.

			//We have a complete line. Let's strip it.
			std::string line = packet.substr(0, crlfPos);
			packet.erase(0, crlfPos + 2);

			if(isReadingFirstLine)
			{
				HttpProtocol *protocol;
				std::string resource;
				float version;

				HttpUtil::parseRequestLine(line, &protocol, resource, version);

				request->setProtocol(protocol);
				request->setResource(HttpUtil::parseResourceNameFromFullResourceUri(resource));
				request->setVersion(version);
				HttpUtil::parseGetParametersFromResourceUri(resource, request);

				isReadingHeaders = true;
				isReadingFirstLine = false;
			}
			else
			{
				//An empty line marks the end of the headers.
				if(line.empty())
				{
					isReadingHeaders = false;
					isReadingBody = true;
					break;
				}
				else
				{//If we get here then we are reading a header.
					std::string headerName, headerValue;
					HttpUtil::parseHeader(line, headerName, headerValue);
					request->setHeader(headerName, headerValue);
				}
			}
		}
	}

	//We may have completed the processing of headers above.
	if(isReadingBody)
	{
		if(!request->hasHeader("Content-Length"))
		{
			isReadingBody = false;
		}
		else
		{
			int contentLength = atoi(request->getHeader("Content-Length").c_str());

			if(contentLength < 0)
				throw HttpException("Content-Length is negative");

			if(request->getBodySize() + packet.size() > contentLength)
				throw HttpException("Body size supplied by request is larger than the Content-Length header");

			request->appendToBody(packet);
			packet.erase();

			if(request->getBodySize() == contentLength)
				isReadingBody = false;
		}
	}
}
HttpDescriptor::HttpDescriptor()
{
	descriptor = NULL;
	server = NULL;
	request = NULL;
	isReadingHeaders = false;
	isReadingBody = false;
	isReadingFirstLine = true;
	responseSent = false;
}

HttpDescriptor::HttpDescriptor(kuDescriptor *descriptor, HttpServer *server)
{
	this->descriptor = descriptor;
	this->server = server;
	this->request = new HttpRequest();
	request->setId(server->getNextRequestId());
	isReadingHeaders = false;
	isReadingBody = false;
	isReadingFirstLine = true;
	responseSent = false;
}

void HttpDescriptor::reset()
{
	isReadingHeaders = false;
	isReadingBody = false;
	responseSent = false;
	isReadingFirstLine = true;
		
	if(request)
		delete request;
	request = new HttpRequest();
	request->setId(server->getNextRequestId());
}

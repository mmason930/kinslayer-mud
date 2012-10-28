#include <string>

#include "HttpUtil.h"
#include "HttpException.h"

#include "StringUtil.h"
#include "MiscUtil.h"

void HttpUtil::parseHeader(const std::string &headerLine, std::string &headerName, std::string &headerValue)
{
	std::string::size_type colonIndex = headerLine.find(": ");

	if(colonIndex == std::string::npos)
		throw HttpException("Invalid header: No colon found.");

	headerName = headerLine.substr(0, colonIndex);
	headerValue = headerLine.substr(colonIndex + 2);
}

void HttpUtil::parseRequestLine(const std::string &requestLine, HttpProtocol **protocol, std::string &resource, float &version)
{
	std::string::size_type pos, previousPos = 0;
	std::string buffer;

	pos = requestLine.find(' ');

	if(pos == std::string::npos)
		throw HttpException("Could not find whitespace following protocol in request line");

	//`buffer` will store the protocol text
	buffer = requestLine.substr(0, pos);
	if( (*protocol = HttpProtocol::getEnumByStandardName(buffer)) == NULL )
		throw HttpException( (std::string("Unrecognized request protocol `") + buffer + std::string("`")).c_str() );

	
	//Read the resource
	previousPos = pos;
	pos = requestLine.find(' ', pos + 1);
	if(pos == std::string::npos)
		throw HttpException("Could not find whitespace following resource in request line");

	resource = requestLine.substr(previousPos + 1, (pos - previousPos) - 1);


	//Read the version.
	std::string versionText = requestLine.substr(pos + 1);
	
	if(!StringUtil::startsWith(versionText, "HTTP/"))
		throw HttpException("Invalid version text supplied in request line");
	std::string versionNumberText = versionText.substr(5);

	if(!MiscUtil::isNumber(versionNumberText))
		throw HttpException("Version number supplied in request line is not a valid decimal");

	version = atof(versionNumberText.c_str());
}
void HttpUtil::parseGetParametersFromResourceUri(const std::string &resourceUri, HttpRequest *request)
{
	std::string::size_type startPos = resourceUri.find('?'), endPos;

	while(startPos != std::string::npos && (startPos+1) < resourceUri.size())
	{
		endPos = resourceUri.find('&', startPos + 1);
		if(endPos == std::string::npos)
			endPos = resourceUri.size();

		std::string parameter = resourceUri.substr(startPos + 1, endPos - startPos - 1);

		std::string::size_type equalsPos = parameter.find('=');
		if(equalsPos != std::string::npos)
		{
			std::string parameterName = parameter.substr(0, equalsPos);
			std::string parameterValue = parameter.substr(equalsPos + 1);

			request->setParameter(parameterName, parameterValue);
		}

		startPos = endPos;
	}
}

std::string HttpUtil::parseResourceNameFromFullResourceUri(const std::string &resourceUri)
{
	std::string::size_type pos = resourceUri.find('?');
	if(pos != std::string::npos)
		return resourceUri.substr(0, pos);
	return resourceUri;
}

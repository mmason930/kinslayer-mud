#ifndef HTTP_PROTOCOL_H
#define HTTP_PROTOCOL_H

#include "Enum.h"

class HttpProtocol : public Enum {

private:

	static std::list<HttpProtocol*> enums;

	HttpProtocol(int value, const std::string &standardName) : Enum(value, standardName)
	{
		enums.push_back(this);
	}

public:

	static HttpProtocol *getEnumByStandardName(const std::string &standardName)
	{
		for(auto iter = enums.begin();iter != enums.end();++iter)
		{
			if( (*iter)->getStandardName() == standardName)
				return (*iter);
		}

		return NULL;
	}

	static HttpProtocol *options;
	static HttpProtocol *get;
	static HttpProtocol *head;
	static HttpProtocol *post;
	static HttpProtocol *deletion;
	static HttpProtocol *trace;
	static HttpProtocol *connect;
};


#endif



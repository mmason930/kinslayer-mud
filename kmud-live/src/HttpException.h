#ifndef HTTP_EXCEPTION_H
#define HTTP_EXCEPTION_H

#include "Exception.h"

class HttpException : public Exception
{
public:

	HttpException(const std::string &msg) : Exception(msg)
	{
	}
};

#endif

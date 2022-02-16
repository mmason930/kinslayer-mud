/*******************************************************************
 *                                                                 *
 * WebSocketException.h                                            *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#ifndef WEB_SOCKET_EXCEPTION_H
#define WEB_SOCKET_EXCEPTION_H

#include <string>

class WebSocketException
{
protected:

	std::string message;
public:

	WebSocketException(const std::string &message)
	{
		this->message = message;
	}

	~WebSocketException()
	{
	}

	std::string what()
	{
		return message;
	}
};

#endif

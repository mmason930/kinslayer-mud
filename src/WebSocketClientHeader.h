/*******************************************************************
 *                                                                 *
 * WebSocketClientHeader.h                                         *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#ifndef WEB_SOCKET_CLIENT_HEADER_H
#define WEB_SOCKET_CLIENT_HEADER_H

#define INVALID_HEADER_EXCEPTION_MESSAGE "Invalid header sent by client"

#include <string>
#include <map>
#include <optional>

class WebSocketClientHeader
{
protected:
	std::map<std::string, std::string> fields;
	std::string method;
	std::string resource;
	std::string secretKey;

	static std::string getLine(const std::string &packet, std::string::size_type startPos, std::string::size_type &crlfPos);

	void readFirstLine(const std::string &line);
public:
	WebSocketClientHeader(void);
	~WebSocketClientHeader(void);
	
	static bool isComplete(const std::string &buffer);

	void readLine(const std::string &line);
	virtual std::string generateResponse(unsigned short port, const char *webSocketServerProtocolName) = 0;
	std::optional<std::string> getFieldByName(const std::string &fieldName) const;
	void readFieldLine(const std::string &line);
	static WebSocketClientHeader *allocateByInitialClientPacket(const std::string &packet);
	virtual void read(std::string &packet) = 0;
};

#endif


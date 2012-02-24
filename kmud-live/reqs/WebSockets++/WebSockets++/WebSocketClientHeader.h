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

#include <string>
#include <map>

class WebSocketClientHeader
{
	friend class WebSocketServer;
private:
	std::map<std::string, std::string> fields;
	std::string method;
	std::string resource;
	std::string secretKey;

	static std::string getLine(const std::string &packet, std::string::size_type startPos, std::string::size_type &crlfPos);

	void read(std::string &packet);
	void readFirstLine(const std::string &line);
	void readFieldLine(const std::string &line);
public:
	WebSocketClientHeader(void);
	~WebSocketClientHeader(void);

	void readLine(const std::string &line);
	std::string generateResponse(unsigned short port, class WebSocketServerProtocol *webSocketServerProtocol);
	std::string getFieldByName(const std::string &fieldName);
};

#endif


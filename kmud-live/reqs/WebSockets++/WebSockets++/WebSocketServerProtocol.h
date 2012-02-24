/*******************************************************************
 *                                                                 *
 * WebSocketServerProtocol.h                                       *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#ifndef WEB_SOCKET_SERVER_PROTOCOL
#define WEB_SOCKET_SERVER_PROTOCOL

#include <string>
#include <list>

class WebSocketServer;
class WebSocketDescriptor;

class WebSocketServerProtocol
{
	friend class WebSocketServer;
protected:

	std::list<class WebSocketDescriptor *> descriptorList;
public:
	WebSocketServerProtocol(void) {}
	~WebSocketServerProtocol(void) {}

	virtual std::string getName()=0;

	virtual void onAuthentication(WebSocketServer *webSocketServer, WebSocketDescriptor *webSocketDescriptor)=0;
	virtual void onMessageReceived(WebSocketServer *webSocketServer, WebSocketDescriptor *webSocketDescriptor, const std::string &message)=0;
	virtual void onDisconnect(WebSocketServer *webSocketServer, WebSocketDescriptor *webSocketDescriptor)=0;

	void sendToAll(const std::string &msg);
	void sendToAllExcluding(const std::string &msg, class WebSocketDescriptor *webSocketDescriptor);
	
};

#endif



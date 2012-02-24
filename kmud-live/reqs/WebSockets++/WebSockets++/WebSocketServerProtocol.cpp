/*******************************************************************
 *                                                                 *
 * WebSocketServerProtocol.cpp                                     *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#include "WebSocketServerProtocol.h"
#include "WebSocketDescriptor.h"

void WebSocketServerProtocol::sendToAll(const std::string &msg)
{
	for(std::list<WebSocketDescriptor *>::iterator iter = descriptorList.begin();iter != descriptorList.end();++iter) {

		(*iter)->send(msg);
	}
}
void WebSocketServerProtocol::sendToAllExcluding(const std::string &msg, class WebSocketDescriptor *webSocketDescriptor)
{
	for(std::list<WebSocketDescriptor *>::iterator iter = descriptorList.begin();iter != descriptorList.end();++iter) {

		if( (*iter) != webSocketDescriptor) {

			(*iter)->send(msg);
		}
	}
}

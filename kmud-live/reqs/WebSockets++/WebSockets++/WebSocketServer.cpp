/*******************************************************************
 *                                                                 *
 * WebSocketServer.cpp                                             *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#include "WebSocketServer.h"
#include "WebSocketException.h"
#include "WebSocketDescriptor.h"
#include "WebSocketClientHeader.h"
#include "WebSocketServerProtocol.h"

#include <iostream>

#ifdef WIN32
typedef int socklen_t;
#endif

WebSocketServer::WebSocketServer()
{

}

WebSocketServer::~WebSocketServer()
{

}

int WebSocketServer::getLastError() {

#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void WebSocketServer::setup(unsigned short port)
{
	this->port = port;
	nulltime.tv_sec = 0;
	nulltime.tv_usec = 0;
	listeningSocket = SOCKET_ERROR;
	int opt;
	struct sockaddr_in sin;

#ifdef WIN32
		WSADATA wsaData;
		int errorNumber;
		if( (errorNumber = WSAStartup(MAKEWORD(1,1), &wsaData))) {

			char errorMessage[128];
			sprintf(errorMessage, "Failed to initiate the Windows Socket API. Error Code: %d", errorNumber);
			throw WebSocketException(errorMessage);
		}
#endif

	if ( (listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		char errorMessage[128];
		sprintf(errorMessage, "Failed to create listening socket. Error Code: %d", WebSocketServer::getLastError());
		throw WebSocketException(errorMessage);
	}
	
	if ( setsockopt( listeningSocket, SOL_SOCKET, SO_REUSEADDR, ( char * ) & opt, sizeof( opt ) ) < 0 )
	{
		char errorMessage[128];
		sprintf(errorMessage, "Failed to set listening socket option to reusable. Error Code: %d", WebSocketServer::getLastError());
		throw WebSocketException(errorMessage);
	}


	sin.sin_family	= AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port	= htons(port);

	if(bind(listeningSocket, (struct sockaddr *)(&sin), sizeof(sin)) == INVALID_SOCKET)
	{
		char errorMessage[128];
		sprintf(errorMessage, "Failed to set listening socket option to reusable. Error Code: %d", WebSocketServer::getLastError());
		throw WebSocketException(errorMessage);
	}

	if(listen(listeningSocket, 5) < 0)
	{
		char errorMessage[128];
		sprintf(errorMessage, "Failed to listening on port %u. Error Code: %d", port, WebSocketServer::getLastError());
		throw WebSocketException(errorMessage);
	}
}

void WebSocketServer::service()
{
	WebSocketDescriptor *webSocketDescriptor;

	//Accept new connections.
	FD_ZERO(&inset);
	FD_ZERO(&outset);
	FD_ZERO(&excset);

	FD_SET(listeningSocket, &inset);
	FD_SET(listeningSocket, &outset);
	FD_SET(listeningSocket, &excset);

	//Check to see if there are any new connections to be obtained.
	fd_set pending;
	timeval nt = {0,0};

	FD_ZERO(&pending);
	FD_SET(listeningSocket, &pending);

	while(true)
	{
		//Pull in clients in the connection queue.
		if( select(listeningSocket + 1, &pending, 0, 0, &nt) != INVALID_SOCKET) {

			if(FD_ISSET(listeningSocket, &pending)) {

				SocketHandle clientSocket = 0;
				socklen_t size = sizeof(struct sockaddr_in);
				sockaddr_in clientAddress;

				if( (clientSocket = accept(listeningSocket, (sockaddr *)&clientAddress, &size)) == INVALID_SOCKET ) {

					char errorMessage[128];
					sprintf(errorMessage, "Could not accept connection. Error: %d", WebSocketServer::getLastError());
					throw WebSocketException(errorMessage);
				}

				//Nonblock
#if defined(WIN32)
				unsigned long val = 1;
				int i = 0;
				if(i = ioctlsocket(listeningSocket, FIONBIO, &val))
				{
					char errorMessage[128];
					sprintf(errorMessage, "Could not make socket nonblocking. Error: %d", WebSocketServer::getLastError());
					throw WebSocketException(errorMessage);
				}
#else
				if(fcntl(this->listeningSocket, F_SETFL, O_NDELAY) == -1)
				{
					char errorMessage[128];
					sprintf(errorMessage, "Could not make socket nonblocking. Error: %d", WebSocketServer::getLastError());
					throw WebSocketException(errorMessage);
				}
#endif

				webSocketDescriptor = new WebSocketDescriptor(inet_ntoa(clientAddress.sin_addr), clientSocket);
				descriptorList.push_back(webSocketDescriptor);
				descriptorsReceivingClientHeaderList.push_back(webSocketDescriptor);
			}
			else {

				break;
			}
		}
		else {

			char errorMessage[128];
			sprintf(errorMessage, "Polling for new clients failed. Error: %d", WebSocketServer::getLastError());
			throw WebSocketException(errorMessage);
		}
	}

	FD_ZERO(&inset);
	FD_ZERO(&outset);
	FD_ZERO(&excset);

	//Handle input from the clients.
	std::list<WebSocketDescriptor *>::iterator descriptorIter = descriptorList.begin();
	SocketHandle maxSocket = 0;

	while(descriptorIter != descriptorList.end())
	{
		webSocketDescriptor = (*descriptorIter);
		if( !webSocketDescriptor->isClosed() )
		{
			maxSocket = webSocketDescriptor->getClientSocket() > maxSocket ? webSocketDescriptor->getClientSocket() : maxSocket;
			FD_SET( webSocketDescriptor->getClientSocket(), &inset);
			FD_SET( webSocketDescriptor->getClientSocket(), &outset);
			FD_SET( webSocketDescriptor->getClientSocket(), &excset);
		}

		++descriptorIter;
	}

	select(maxSocket + 1, &inset, &outset, &excset, &nulltime);

	descriptorIter = descriptorList.begin();

	while(descriptorIter != descriptorList.end())
	{
		webSocketDescriptor = (*descriptorIter);
		if(!webSocketDescriptor->isClosed() && FD_ISSET(webSocketDescriptor->getClientSocket(), &inset))
		{
			try {

				webSocketDescriptor->socketRead();
			}
			catch(WebSocketException e) {

				this->closeDescriptor(webSocketDescriptor);
			}
		}

		if(!webSocketDescriptor->isClosed() && FD_ISSET(webSocketDescriptor->getClientSocket(), &outset))
		{
			webSocketDescriptor->processOutgoingPackets();
		}

		if(!webSocketDescriptor->isClosed() && FD_ISSET(webSocketDescriptor->getClientSocket(), &excset))
		{
			closeDescriptor(webSocketDescriptor);
		}

		++descriptorIter;
	}

	//Clean out all descriptors that are closed.
	descriptorIter = descriptorList.begin();
	while(descriptorIter != descriptorList.end())
	{
		webSocketDescriptor = (*descriptorIter);
		if( webSocketDescriptor->isClosed() )
		{
			if(webSocketDescriptor->webSocketServerProtocol != NULL) {

				webSocketDescriptor->webSocketServerProtocol->onDisconnect(this, webSocketDescriptor);
				
				webSocketDescriptor->webSocketServerProtocol->descriptorList.remove(webSocketDescriptor);
			}

			delete webSocketDescriptor;

			descriptorsReceivingClientHeaderList.remove(webSocketDescriptor);

			descriptorsAuthenticatedList.remove(webSocketDescriptor);

			descriptorIter = descriptorList.erase(descriptorIter);
		}
		else
		{
			++descriptorIter;
		}
	}

	//With all input, output, and cleanup finished, we can move on to processing handshaking and procedures.

	//Server -> Client handshaking.
	descriptorIter = this->descriptorsReceivingClientHeaderList.begin();
	while(descriptorIter != this->descriptorsReceivingClientHeaderList.end())
	{
		webSocketDescriptor = (*descriptorIter);

		if(webSocketDescriptor->unprocessedInputBuffer.size() > 0) {

			WebSocketClientHeader webSocketClientHeader;

			try {

				webSocketClientHeader.read(webSocketDescriptor->unprocessedInputBuffer);

				WebSocketServerProtocol *webSocketServerProtocol = NULL;

				if(webSocketClientHeader.getFieldByName("Sec-WebSocket-Protocol").empty() == false) {
				//Client has specified a protocol. We will determine if we support it.

					webSocketServerProtocol = this->getServerProtocolByName(webSocketClientHeader.getFieldByName("Sec-WebSocket-Protocol"));

					if(webSocketServerProtocol == NULL) {
						//We do not support the protocol. Kill the connection.

						closeDescriptor(webSocketDescriptor);
					}
				}

				if(!webSocketDescriptor->isClosed()) {

					std::string response = webSocketClientHeader.generateResponse(this->port, webSocketServerProtocol);

					descriptorsAuthenticatedList.push_back(webSocketDescriptor);

					webSocketDescriptor->socketWrite(response);

					webSocketDescriptor->authenticated = true;

					webSocketDescriptor->webSocketServerProtocol = webSocketServerProtocol;

					if(webSocketServerProtocol != NULL) {

						webSocketServerProtocol->onAuthentication(this, webSocketDescriptor);

						webSocketServerProtocol->descriptorList.push_back(webSocketDescriptor);
					}
				}

				descriptorIter = descriptorsReceivingClientHeaderList.erase(descriptorIter);

				continue;
			}
			catch(WebSocketException exception) {

				closeDescriptor(webSocketDescriptor);

				++descriptorIter;

				continue;
			}
		}
		
		++descriptorIter;
	}

	descriptorIter = this->descriptorsAuthenticatedList.begin();
	while(descriptorIter != descriptorsAuthenticatedList.end()) {

		WebSocketDescriptor *webSocketDescriptor = (*descriptorIter);

		while(webSocketDescriptor->inputBufferQueue.empty() == false) {

			std::string message = webSocketDescriptor->inputBufferQueue.front();

			webSocketDescriptor->inputBufferQueue.pop_front();

			if(webSocketDescriptor->webSocketServerProtocol != NULL) {

				webSocketDescriptor->webSocketServerProtocol->onMessageReceived(this, webSocketDescriptor, message);
			}
		}

		++descriptorIter;
	}
}

std::list<WebSocketDescriptor *> WebSocketServer::getDescriptorList()
{
	return descriptorList;
}

void WebSocketServer::closeDescriptor(WebSocketDescriptor *webSocketDescriptor)
{
	closesocket(webSocketDescriptor->getClientSocket());
	webSocketDescriptor->clientSocket = SOCKET_ERROR;
}

void WebSocketServer::addServerProtocol(WebSocketServerProtocol *webSocketServerProtocol)
{
	this->webSocketServerProtocols[ webSocketServerProtocol->getName() ] = webSocketServerProtocol;
}

WebSocketServerProtocol *WebSocketServer::getServerProtocolByName(const std::string &protocolName)
{
	if(this->webSocketServerProtocols.find(protocolName) != this->webSocketServerProtocols.end()) {

		return this->webSocketServerProtocols[ protocolName ];
	}

	return NULL;
}


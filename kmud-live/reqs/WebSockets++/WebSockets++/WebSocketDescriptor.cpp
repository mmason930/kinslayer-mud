/*******************************************************************
 *                                                                 *
 * WebSocketDescriptor.cpp                                         *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#include "WebSocketDescriptor.h"
#include "WebSocketException.h"

#include <sstream>
#include <iostream>

WebSocketDescriptor::WebSocketDescriptor()
{
	this->clientSocket = SOCKET_ERROR;
	this->messageStartNullByte = false;
	this->authenticated = false;
	this->messageReadPos = 0;
	this->webSocketServerProtocol = NULL;
}

WebSocketDescriptor::WebSocketDescriptor(const std::string &ipAddress, SocketHandle clientSocket)
{
	this->ipAddress = ipAddress;
	this->clientSocket = clientSocket;
	this->messageStartNullByte = false;
	this->authenticated = false;
	this->messageReadPos = 0;
	this->webSocketServerProtocol = NULL;
}

WebSocketDescriptor::~WebSocketDescriptor()
{

}

void WebSocketDescriptor::send(const std::string &packet)
{
	this->outputBufferQueue.push_back(packet);
}

void WebSocketDescriptor::processOutgoingPackets()
{
	while(this->outputBufferQueue.empty() == false) {

		std::stringstream outgoingMessageStream;

		outgoingMessageStream << ((unsigned char)0x00);
		outgoingMessageStream << (this->outputBufferQueue.front());
		outgoingMessageStream << ((unsigned char)0xFF);

		this->socketWrite(outgoingMessageStream.str());

		this->outputBufferQueue.pop_front();
	}
}

std::string WebSocketDescriptor::getIpAddress()
{
	return ipAddress;
}

SocketHandle WebSocketDescriptor::getClientSocket()
{
	return clientSocket;
}

bool WebSocketDescriptor::isClosed()
{
	return clientSocket == SOCKET_ERROR;
}

void WebSocketDescriptor::socketWrite(const std::string &data)
{
	int bytesWritten;

	bytesWritten = ::send(clientSocket, data.c_str(), data.size(), 0);

	if(bytesWritten < 0) {

		throw WebSocketException("Writing to socket returned negative bytes written.");
	}
}

void WebSocketDescriptor::writeOutputQueue()
{
	while(this->outputBufferQueue.empty() == false)
	{
		std::string &outputPacket = this->outputBufferQueue.back();
		
		socketWrite(outputPacket);

		this->outputBufferQueue.pop_back();
	}
}

void WebSocketDescriptor::readMessageFirstByte()
{
	unsigned char firstByte = unprocessedInputBuffer[0];

	if(firstByte & 128) {
		
		if(firstByte != 0xFF) {

			throw WebSocketException("Expecting 0xFF byte.");
		}

		this->messageStartNullByte = false;

		throw WebSocketException("");
	}
	else {

		if(firstByte != 0x00) {

			throw WebSocketException("Expecting 0x00 byte.");
		}

		this->messageStartNullByte = true;

		this->messageReadPos = 1;
	}
}

void WebSocketDescriptor::readMessageFromClient()
{
	while(this->messageReadPos < this->unprocessedInputBuffer.size()) {

		unsigned char character = (unsigned char)this->unprocessedInputBuffer[this->messageReadPos];
		if((this->messageStartNullByte && character == 0xFF) || (!this->messageStartNullByte && character == 0x00)) {
			//The end of the message has been reached.

			inputBufferQueue.push_back(this->unprocessedInputBuffer.substr(1, this->messageReadPos - 1));

			this->unprocessedInputBuffer.erase(0, this->messageReadPos+1);

			this->messageReadPos = 0;

			if(this->unprocessedInputBuffer.empty() == false) {
				//There is another message waiting.

				readMessageFirstByte();

				continue;
			}
		}

		++this->messageReadPos;
	}
}

void WebSocketDescriptor::socketRead()
{
	int retVal;
	char packetBuffer[2048];
	bool hasError = false;

	retVal = recv(clientSocket, packetBuffer, sizeof(packetBuffer) - 1, 0);

	if(retVal > 0)
	{
		packetBuffer[retVal] = '\0';
		unprocessedInputBuffer += std::string(packetBuffer, retVal);

		if(authenticated) {
			//If this descriptor has been authenticated, then we are receiving a message from the server.
			//Otherwise, the message we are expecting is the header, so we will process that at a later time.

			if(this->messageReadPos != 0) {
				//What we have received is a continuation of a message.

				readMessageFromClient();//Continue where we left off.
			}
			else {
				//Most likely a new message(or a disconnect command).

				std::string::size_type pos = 0;

				readMessageFirstByte();

				readMessageFromClient();
			}
		}
	}
	else if(retVal == 0)
		hasError = true;
#ifdef WIN32
	else if(WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN)
		hasError = true;
#else
	else if(errno == EAGAIN || errno == EINTR)
		hasError = true;
#endif
	else
	{
		hasError = true;
	}

	if(hasError)
	{
		throw WebSocketException("Error when reading from client.");
	}
}

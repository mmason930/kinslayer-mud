#include "kuListener.h"
#include "kuDescriptor.h"

#include <list>
#include <map>
#include <string>
#include <stdarg.h>
#include <cstring>
#include <cstdio>
#include <iostream>

int kuDescriptor::d_TopUID = 0;

kuDescriptor::kuDescriptor(class kuListener *server)
{
	this->server = server;
	d_UID = kuDescriptor::d_TopUID++;
}

kuDescriptor::~kuDescriptor()
{
	if(this->sock != INVALID_SOCKET)
		this->socketClose();
}

void kuDescriptor::nonBlock()
{
#if defined(WIN32)
	unsigned long val = 1;
	int i = 0;
	if(i = ioctlsocket(this->sock, FIONBIO, &val))
	{
		return;
	}
#else
	fcntl(this->sock, F_SETFL, O_NDELAY);
	{
		return;
	}
#endif
}

bool kuDescriptor::hasCommand()
{
	return input.find("\r") != std::string::npos || input.find("\n") != std::string::npos;
}

std::string kuDescriptor::getCommand()
{
	std::string::size_type endPos, i, size;
	size = input.size();

	for(i = 0;i < size && input[i] != '\r' && input[i] != '\n';++i);

	if( (size == 0) || (i != size && input[i] != '\r' && input[i] != '\n') )
		return std::string("");

	endPos = i;

	if( (i + 1 < size) && ((input[i] == '\n' && input[i+1] == '\r') || (input[i] == '\r' && input[i+1] == '\n')) )
		++endPos;

	std::string Line = input.substr(0, i);
	input.erase(0, endPos+1);

	return Line;
}

int kuDescriptor::socketRead()
{
	char dataReadFromSocket[2048];
	int total = 0, recvReturnValue = 0;
	memset(dataReadFromSocket, 0, sizeof(dataReadFromSocket));

	recvReturnValue = ::recv(this->sock, dataReadFromSocket, sizeof(dataReadFromSocket), 0);

	if(recvReturnValue > 0)
	{
		total += recvReturnValue;
		this->input.append(dataReadFromSocket, recvReturnValue);
	}
	else if(recvReturnValue == 0)
		return -1;
#ifdef WIN32
	else if(WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN)
		return -1;
#else
	else if(errno == EAGAIN || errno == EINTR)
		return -1;
#endif
	else
	{
		return -1;
	}
	return recvReturnValue;
}

void kuDescriptor::eraseInput(std::string::size_type offset, std::string::size_type length)
{
	this->input.erase(offset, length);
}

int kuDescriptor::socketClose()
{
	if(this->sock != INVALID_SOCKET) {
		closesocket(this->sock);
		this->sock = INVALID_SOCKET;
	}
	return 1;
}

bool kuDescriptor::hasHost()
{
	return false;
}

void kuDescriptor::send(const char *str)
{
	this->output += std::string(str);
}

void kuDescriptor::send(const std::string &str)
{
	this->output += str;
}

int kuDescriptor::socketWrite()
{
	const char *outputBuffer = output.c_str();
	int bytesWritten = 0;

	if (!*outputBuffer)
		return 0;

	server->handleBeforeSocketWriteCallback(this);

	if((bytesWritten = ::send(this->sock, outputBuffer, output.size(), 0)) < 0)
	{
		this->socketClose();
		return -1;
	}
	
	server->handleAfterSocketWriteCallback(this, std::string(outputBuffer, bytesWritten));

	this->output.erase(0, bytesWritten);
}

void kuDescriptor::clearInput()
{
	input.clear();
}

bool kuDescriptor::socketIsClosed()
{
	return (this->sock == INVALID_SOCKET);
}

int kuDescriptor::getUid()
{
	return d_UID;
}

std::string kuDescriptor::getInputBuffer()
{
	return this->input;
}

/************** ADDRESS CLASS *****************/
Address::Address()
{
	memset(&udp_Sin, 0, sizeof(udp_Sin));
}
sockaddr_in *Address::GetSaAddress()
{
	return &udp_Sin;
}
sockaddr *Address::GetSinAddress()
{
	return (sockaddr *)&udp_Sin;
}
const std::string Address::GetIP()
{
	return inet_ntoa(this->udp_Sin.sin_addr);
}
const std::string Address::GetHost()
{
	hostent *host;

	if(!(host = gethostbyname(this->GetIP().c_str())))
		return "";
	return host->h_name;
}
const u_short Address::GetPort()
{
	return this->udp_Sin.sin_port;
}

void kuDescriptor::disconnect() {

	if(sock > 0) {
#ifdef WIN32
		closesocket(this->sock);
#else
		close(sock);
#endif
		if(server != NULL) {

			server->handleCloseDescriptor(this);
		}
	}
}

//Write a message to the socket immediately.
void kuDescriptor::socketWriteInstant(const std::string &message)
{
	//send() is not guaranteed to write the full message to the connected socket.
	//Keep attempting to send until the full message has been written, or until an error has occurred.
	int bytesSent = 0, bytesRemaining = message.size();
	const char *buffer = message.c_str(), *bufferReadPosition;
	while (bytesRemaining)
	{
		bufferReadPosition = (buffer + bytesSent);

		if ((bytesSent = ::send(sock, bufferReadPosition, bytesRemaining, 0)) < 0) {
			//Under zero return value indicates error.
			socketClose();
			return;
		}

		bytesRemaining -= bytesSent;
	}
}

int kuDescriptor::getOutputBufferSize()
{
	return this->output.size();
}

/********************************************************************
 *                        ( B E T A )                               *
 *                                                                  *
 * mmason - 11/23/2009 - Implement the kuListener class.            *
 *   Dependencies are found in kuSockets.h                          *
 *   This class is responsible for binding and listening to a port, *
 *   as well as accepting and processing incoming connections.      *
 ********************************************************************/

#include "kuDescriptor.h"
#include "kuListener.h"

#include <iostream>

kuListener::kuListener(const int port, e_SocketType socketType)
{
	closeDescriptorCallback = NULL;
	dataForCloseDescriptorCallback = NULL;
	openDescriptorCallback = NULL;
	dataForOpenDescriptorCallback = NULL;
	beforeSocketWriteCallback = NULL;
	dataForBeforeSocketWriteCallback = NULL;
	afterSocketWriteCallback = NULL;
	dataForAfterSocketWriteCallback = NULL;
	dataForSocketReadCallback = NULL;
	socketReadCallback = NULL;
	this->socketType = socketType;
	nulltime.tv_sec = 0;
	nulltime.tv_usec = 0;
	this->listening = false;
	this->bound = false;
	this->socket = SOCKET_ERROR;
	this->port = static_cast<u_short>(port);
	int opt = 1;//True
#ifdef WIN32
	int i = 0;
	WSADATA wsaData;
	if( (i = WSAStartup(MAKEWORD(1,1), &wsaData)))
	{
//		WSAReportError("Listener::Listener() : WSAStartup()", i);
//		exit(1);
	}
#endif

	if (socketType == TCP)
		this->socket = this->createTCPSocket();
	else if (socketType == UDP)
		this->socket = this->createUDPSocket();
	else
		this->socket = this->createTCPSocket();

	if (setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
	{
		return;
	}

	if ((this->socket) == INVALID_SOCKET)
	{
		return;
	}
	
	//Bind the socket
	if(this->bind(port) == INVALID_SOCKET)
	{
		return;
	}
	//Listen, only required for TCP sockets.
	if (socketType == TCP && this->listen() == INVALID_SOCKET)
	{
		return;
	}
}

kuListener::~kuListener()
{
	for (auto descriptorIdToDescriptorPaid : descriptorMap)
	{
		descriptorIdToDescriptorPaid.second->socketClose();
		delete descriptorIdToDescriptorPaid.second;
	}

	this->close();
}

/********* Calling the listen() function ************/
int kuListener::listen()
{
	int i = 0;

	if ((i = ::listen(this->socket, 5)) < 0)
	{
		this->listening = false;
		return -1;
	}

	this->listening = true;
	return i;
}
/**************************************************/


/********* Calling the bind() function ************/
int kuListener::bind(const int port)
{
	int i = 0;
	struct sockaddr_in sin;

	sin.sin_family	= AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port	= htons(port);

	if ((i = ::bind(this->socket, (struct sockaddr *)(&sin), sizeof(sin))) == INVALID_SOCKET)
	{
		this->bound = false;
		return INVALID_SOCKET;
	}
	this->bound = true;
	return i;
}
/****************************************************/


/********* Calling the accept() function ************/

std::pair< kuDescriptor *, bool > kuListener::accept()
{
	SOCKET sock = 0;
	kuDescriptor *d = 0;
	std::pair< kuDescriptor *, bool > lND;

	socklen_t size = sizeof(struct sockaddr_in);

	d = new kuDescriptor( this );
	if ((sock = ::accept(this->socket, (sockaddr *)&d->sin, &size)) == INVALID_SOCKET)
	{
		delete (d);
		lND.first = (0);
		lND.second = (false);
	}
	else
	{
		d->sock = sock;
		d->nonBlock();
		d->ipAddress = inet_ntoa(d->sin.sin_addr);

		descriptorMap[d->getUid()] = d;

		lND.first = (d);
		lND.second = (true);
	}
	return lND;
}

//Is there a new connection to be grabbed?
bool kuListener::canAccept()
{
	fd_set pending;
	timeval nt = {0,0};

	FD_ZERO(&pending);
	FD_SET(this->socket, &pending);

	if ((::select(this->socket + 1, &pending, 0, 0, &nt)) == INVALID_SOCKET)
	{
		return false;
	}
	if (FD_ISSET(this->socket, &pending))
	{
		return true;
	}
	return false;
}

void kuListener::close()
{
	if( this->getSocket() != INVALID_SOCKET ) {
		closesocket(this->socket);
	}
	this->bound = false;
	this->listening = false;
}
/****************************************************/

u_short kuListener::getPort()
{
	return (this->port);
}

bool kuListener::isBound()
{
	return (this->bound);
}

bool kuListener::isListening()
{
	return (this->listening);
}

SOCKET kuListener::getSocket()
{
	return (socket);
}

/**** Check to see if there are any new hosts waiting to be accepted. If so, we will grab them
      and append them to our descriptor list. Return a list of any newly accepted descriptors. ****/
std::list< kuDescriptor * > kuListener::acceptNewHosts()
{
	std::list< kuDescriptor * > lNewDescriptors;
	FD_ZERO(&inset );
	FD_ZERO(&outset);
	FD_ZERO(&excset);
	FD_SET(this->socket, &inset);
	FD_SET(this->socket, &outset);
	FD_SET(this->socket, &excset);

	if (this->canAccept())
	{
		std::pair< kuDescriptor *, bool > lND = this->accept();

		if( lND.second == true ) {
			lNewDescriptors.push_back( lND.first );

			if(this->openDescriptorCallback != NULL) {

				this->openDescriptorCallback(this->dataForOpenDescriptorCallback, this, lND.first);
			}
		}
	}
	return lNewDescriptors;
}

void kuListener::pulse()
{
	SOCKET max = 0;
	for (auto descriptorPair : descriptorMap)
	{
		if (descriptorPair.second->socketIsClosed())
			continue;
		max = descriptorPair.second->sock > max ? descriptorPair.second->sock : max;
		FD_SET(descriptorPair.second->sock, &inset);
		FD_SET(descriptorPair.second->sock, &outset);
		FD_SET(descriptorPair.second->sock, &excset);
	}
	::select(max + 1, &inset, &outset, &excset, &nulltime);

	for (auto descriptorPair : descriptorMap)
	{
		if (!descriptorPair.second->socketIsClosed() && FD_ISSET(descriptorPair.second->sock, &inset))
		{
			if (descriptorPair.second->socketRead() == -1)
				descriptorPair.second->socketClose();
		}
		if (!descriptorPair.second->socketIsClosed() && FD_ISSET(descriptorPair.second->sock, &outset))
		{
			descriptorPair.second->socketWrite();
		}
		if (!descriptorPair.second->socketIsClosed() && FD_ISSET(descriptorPair.second->sock, &excset))
		{
			descriptorPair.second->socketClose();
		}
	}
	for(auto iter = descriptorMap.begin();iter != descriptorMap.end();)
	{
		if( (*iter).second->socketIsClosed() )
		{
			if( this->closeDescriptorCallback != NULL ) {
				this->closeDescriptorCallback(this->dataForCloseDescriptorCallback, this, (*iter).second);
			}
			delete (*iter).second;
			descriptorMap.erase( iter++ );
		}
		else
			++iter;
	}
}

std::list< kuDescriptor * > kuListener::getDescriptors()
{
	std::list< kuDescriptor * > lDesc;
	for (dMapType::iterator dIter = descriptorMap.begin(); dIter != descriptorMap.end(); ++dIter)
		lDesc.push_back( (*dIter).second );
	return lDesc;
}

/********* Calling the socket() function ************/
int kuListener::select(const int MaxDescs, fd_set *inset, fd_set *outset, fd_set *excset)
{
	timeval nulltime = {0,0};
	
	if((::select(MaxDescs + 1, inset, outset, excset, &nulltime)) < 0)
	{
		return -1;
	}
	return 1;
}
/********* Calling the socket() function for TCP ************/
SOCKET kuListener::createTCPSocket()
{
	SOCKET sock = 0;

	if ( (sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		return -1;
	}
	return sock;
}
/****************************************************/

/********* Calling the socket() function for UDP ************/
SOCKET kuListener::createUDPSocket()
{
	SOCKET sock = 0;

	if ( (sock = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	return sock;
}
/****************************************************/

bool kuListener::enableKeepAlive()
{
	char optval;
	socklen_t optlen = sizeof(optval);
	if (getsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
		return false;
	}

	if(optval)
		return true;//Early out. The option has already been enabled.

	optval = 1;
	optlen = sizeof(optval);
	if (setsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
		return false;
	}

	if (getsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
		return false;
	}

	if(optval)
		return true;//The setting succeeded.
	return false;//The option failed.
}

kuDescriptor *kuListener::getDescriptorById( const int uid )
{
	return(descriptorMap.count(uid) == 0 ? (0) : descriptorMap[uid]);
}

void kuListener::setCloseDescriptorCallback( void (*closeDescriptorCallback)(void*, kuListener*, kuDescriptor*) )
{
	this->closeDescriptorCallback = closeDescriptorCallback;
}

void kuListener::setDataForCloseDescriptorCallback( void *data )
{
	this->dataForCloseDescriptorCallback = data;
}

void kuListener::setOpenDescriptorCallback( void (*openDescriptorCallback)(void*, kuListener*, kuDescriptor*) )
{
	this->openDescriptorCallback = openDescriptorCallback;
}

void kuListener::setSocketReadCallback( void (*socketReadCallback)(void*, kuListener*, kuDescriptor*, const std::string &output) )
{
	this->socketReadCallback = socketReadCallback;
}


void kuListener::setDataForOpenDescriptorCallback( void *data )
{
	this->dataForOpenDescriptorCallback = data;
}

void kuListener::setBeforeSocketWriteCallback( void (*socketWriteCallback)(void*, kuListener*, kuDescriptor*) )
{
	this->beforeSocketWriteCallback = socketWriteCallback;
}

void kuListener::setAfterSocketWriteCallback( void (*socketWriteCallback)(void*, kuListener*, kuDescriptor*, const std::string &output) )
{
	this->afterSocketWriteCallback = socketWriteCallback;
}


void kuListener::setDataForBeforeSocketWriteCallback( void *data )
{
	this->dataForAfterSocketWriteCallback = data;
}

void kuListener::setDataForAfterSocketWriteCallback( void *data )
{
	this->dataForBeforeSocketWriteCallback = data;
}

void kuListener::setDataForSocketReadCallback( void *data )
{
	this->dataForSocketReadCallback = data;
}

void kuListener::handleCloseDescriptor(kuDescriptor *descriptor)
{
	if(closeDescriptorCallback != NULL) {

		closeDescriptorCallback(dataForCloseDescriptorCallback, this, descriptor);
	}
}

void kuListener::handleBeforeSocketWriteCallback(kuDescriptor *descriptor)
{
	if(beforeSocketWriteCallback != NULL) {

		beforeSocketWriteCallback(dataForBeforeSocketWriteCallback, this, descriptor);
	}
}

void kuListener::handleAfterSocketWriteCallback(kuDescriptor *descriptor, const std::string &output)
{
	if(afterSocketWriteCallback != NULL) {

		afterSocketWriteCallback(dataForAfterSocketWriteCallback, this, descriptor, output);
	}
}

void kuListener::handleSocketReadCallback(kuDescriptor *descriptor, const std::string &input)
{
	if(socketReadCallback != NULL) {

		socketReadCallback(dataForSocketReadCallback, this, descriptor, input);
	}
}

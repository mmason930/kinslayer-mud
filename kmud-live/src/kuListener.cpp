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

kuListener::kuListener(const int port, e_SocketType t)
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
	l_type = t;
	nulltime.tv_sec = 0;
	nulltime.tv_usec = 0;
	l_isListening = false;
	l_isBound = false;
	l_Sock = SOCKET_ERROR;
	l_Port = static_cast<u_short>(port);
	int opt;
#ifdef WIN32
	int i = 0;
	WSADATA wsaData;
	if( (i = WSAStartup(MAKEWORD(1,1), &wsaData)))
	{
//		WSAReportError("Listener::Listener() : WSAStartup()", i);
//		exit(1);
	}
#endif

	if(t == TCP)
		this->l_Sock = this->l_TCPSocket();
	else if(t == UDP)
		this->l_Sock = this->l_UDPSocket();
	else
		this->l_Sock = this->l_TCPSocket();

	if ( setsockopt( this->l_Sock, SOL_SOCKET, SO_REUSEADDR, ( char * ) & opt, sizeof( opt ) ) < 0 )
	{
		return;
	}

	if ((this->l_Sock) == INVALID_SOCKET)
	{
		return;
	}
	
	//Bind the socket
	if(this->l_Bind(port) == INVALID_SOCKET)
	{
		return;
	}
	//Listen, only required for TCP sockets.
	if(t == TCP && this->l_Listen() == INVALID_SOCKET)
	{
		return;
	}
}

kuListener::~kuListener()
{
	l_Close();
}

/********* Calling the listen() function ************/
int kuListener::l_Listen()
{
	int i = 0;

	if( (i = listen(this->l_Sock, 5) ) < 0)
	{
		l_isListening = false;
		return -1;
	}
	l_isListening = true;
	return i;
}
/**************************************************/


/********* Calling the bind() function ************/
int kuListener::l_Bind(const int port)
{
	int i = 0;
	struct sockaddr_in sin;

	sin.sin_family	= AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port	= htons(port);

	if( (i = bind(this->l_Sock, (struct sockaddr *)(&sin), sizeof(sin)) )  == INVALID_SOCKET)
	{
		l_isBound = false;
		return INVALID_SOCKET;
	}
	l_isBound = true;
	return i;
}
/****************************************************/


/********* Calling the accept() function ************/

std::pair< kuDescriptor *, bool > kuListener::l_Accept()
{
	SOCKET sock = 0;
	kuDescriptor *d = 0;
	std::pair< kuDescriptor *, bool > lND;

	socklen_t size = sizeof(struct sockaddr_in);

	d = new kuDescriptor( this );
	if( (sock = accept(this->l_Sock, (sockaddr *)&d->sin, &size) ) == INVALID_SOCKET)
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

		mDescriptors[ d->getUid() ] = d;

		lND.first = (d);
		lND.second = (true);
	}
	return lND;
}

//Is there a new connection to be grabbed?
bool kuListener::l_CanAccept()
{
	fd_set pending;
	timeval nt = {0,0};

	FD_ZERO(&pending);
	FD_SET(this->l_Sock, &pending);

	if( (select(this->l_Sock + 1, &pending, 0, 0, &nt) ) == INVALID_SOCKET )
	{
		return false;
	}
	if(FD_ISSET(this->l_Sock, &pending))
	{
		return true;
	}
	return false;
}

void kuListener::l_Close()
{
	if( this->l_GetSocket() != INVALID_SOCKET ) {
		closesocket( this->l_Sock );
	}
	this->l_isBound = false;
	this->l_isListening = false;
}
/****************************************************/

u_short kuListener::l_GetPort()
{
	return (l_Port);
}
bool kuListener::l_IsBound()
{
	return (l_isBound);
}
bool kuListener::l_IsListening()
{
	return (l_isListening);
}
SOCKET kuListener::l_GetSocket()
{
	return (l_Sock);
}

/**** Check to see if there are any new hosts waiting to be accepted. If so, we will grab them
      and append them to our descriptor list. Return a list of any newly accepted descriptors. ****/
std::list< kuDescriptor * > kuListener::l_AcceptNewHosts()
{
	std::list< kuDescriptor * > lNewDescriptors;
	FD_ZERO(&inset );
	FD_ZERO(&outset);
	FD_ZERO(&excset);
	FD_SET(this->l_Sock, &inset);
	FD_SET(this->l_Sock, &outset);
	FD_SET(this->l_Sock, &excset);

	if(this->l_CanAccept())
	{
		std::pair< kuDescriptor *, bool > lND = this->l_Accept();

		if( lND.second == true ) {
			lNewDescriptors.push_back( lND.first );

			if(this->openDescriptorCallback != NULL) {

				this->openDescriptorCallback(this->dataForOpenDescriptorCallback, lND.first);
			}
		}
	}
	return lNewDescriptors;
}
void kuListener::l_Pulse()
{
	SOCKET max = 0;
	for(dMapType::iterator dIter = mDescriptors.begin();dIter != mDescriptors.end();++dIter)
	{
		if( (*dIter).second->socketIsClosed() )
			continue;
		max = (*dIter).second->sock > max ? (*dIter).second->sock : max;
		FD_SET( (*dIter).second->sock, &inset );
		FD_SET( (*dIter).second->sock, &outset);
		FD_SET( (*dIter).second->sock, &excset);
	}
	select(max + 1, &inset, &outset, &excset, &nulltime);

	for(dMapType::iterator dIter = mDescriptors.begin();dIter != mDescriptors.end();++dIter)
	{
		if( !(*dIter).second->socketIsClosed() && FD_ISSET( (*dIter).second->sock, &inset ) )
		{
			if( (*dIter).second->socketRead() == -1 )
				(*dIter).second->socketClose();
		}
		if( !(*dIter).second->socketIsClosed() && FD_ISSET( (*dIter).second->sock, &outset ) )
		{
			(*dIter).second->socketWrite();
		}
		if( !(*dIter).second->socketIsClosed() && FD_ISSET( (*dIter).second->sock, &excset ) )
		{
			(*dIter).second->socketClose();
		}
	}
	for(dMapType::iterator dIter = mDescriptors.begin();dIter != mDescriptors.end();)
	{
		if( (*dIter).second->socketIsClosed() )
		{
			if( this->closeDescriptorCallback != NULL ) {
				this->closeDescriptorCallback(this->dataForCloseDescriptorCallback, (*dIter).second);
			}
			delete (*dIter).second;
			mDescriptors.erase( dIter++ );
		}
		else
			++dIter;
	}
}

std::list< kuDescriptor * > kuListener::l_GetDescriptors()
{
	std::list< kuDescriptor * > lDesc;
	for( dMapType::iterator dIter = mDescriptors.begin();dIter != mDescriptors.end();++dIter )
		lDesc.push_back( (*dIter).second );
	return lDesc;
}

/********* Calling the socket() function ************/
int kuListener::l_Select(const int MaxDescs, fd_set *inset, fd_set *outset, fd_set *excset)
{
	timeval nulltime = {0,0};
	
	if((select(MaxDescs + 1, inset, outset, excset, &nulltime)) < 0)
	{
		return -1;
	}
	return 1;
}
/********* Calling the socket() function for TCP ************/
SOCKET kuListener::l_TCPSocket()
{
	SOCKET sock = 0;

	if ( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		return -1;
	}
	return sock;
}
/****************************************************/

/********* Calling the socket() function for UDP ************/
SOCKET kuListener::l_UDPSocket()
{
	SOCKET sock = 0;

	if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	return sock;
}
/****************************************************/

kuDescriptor *kuListener::l_GetDesc( const int uid )
{
	return( mDescriptors.count( uid ) == 0 ? (0) : mDescriptors[ uid ] );
}

void kuListener::setCloseDescriptorCallback( void (*closeDescriptorCallback)(void*, kuDescriptor*) )
{
	this->closeDescriptorCallback = closeDescriptorCallback;
}

void kuListener::setDataForCloseDescriptorCallback( void *data )
{
	this->dataForCloseDescriptorCallback = data;
}

void kuListener::setOpenDescriptorCallback( void (*openDescriptorCallback)(void*, kuDescriptor*) )
{
	this->openDescriptorCallback = openDescriptorCallback;
}

void kuListener::setSocketReadCallback( void (*socketReadCallback)(void*, kuDescriptor*, const std::string &output) )
{
	this->socketReadCallback = socketReadCallback;
}


void kuListener::setDataForOpenDescriptorCallback( void *data )
{
	this->dataForOpenDescriptorCallback = data;
}

void kuListener::setBeforeSocketWriteCallback( void (*socketWriteCallback)(void*, kuDescriptor*, const std::string &output) )
{
	this->beforeSocketWriteCallback = socketWriteCallback;
}

void kuListener::setAfterSocketWriteCallback( void (*socketWriteCallback)(void*, kuDescriptor*, const std::string &output) )
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

		closeDescriptorCallback(dataForCloseDescriptorCallback, descriptor);
	}
}

void kuListener::handleBeforeSocketWriteCallback(kuDescriptor *descriptor, const std::string &output)
{
	if(beforeSocketWriteCallback != NULL) {

		beforeSocketWriteCallback(dataForBeforeSocketWriteCallback, descriptor, output);
	}
}

void kuListener::handleAfterSocketWriteCallback(kuDescriptor *descriptor, const std::string &output)
{
	if(afterSocketWriteCallback != NULL) {

		afterSocketWriteCallback(dataForAfterSocketWriteCallback, descriptor, output);
	}
}

void kuListener::handleSocketReadCallback(kuDescriptor *descriptor, const std::string &input)
{
	if(socketReadCallback != NULL) {

		socketReadCallback(dataForSocketReadCallback, descriptor, input);
	}
}

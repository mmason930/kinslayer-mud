/*******************************************************************
 *                                                                 *
 * WebSocketServer.h                                             *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#ifndef WEB_SOCKET_SERVER_H
#define WEB_SOCKET_SERVER_H

#include <list>
#include <map>

#ifndef WEB_SOCKET_INCLUDES

#define WEB_SOCKET_INCLUDES

	#ifdef WIN32
		#include <sys\types.h>
		#include <winsock.h>
		#include <direct.h>
		#include <windows.h>
		#include <errno.h>
	#else
		#include <arpa/inet.h>
		#include <sys/types.h>
		#include <dirent.h>
		#include <netdb.h>
		#include <fcntl.h>
		#include <netinet/in.h>
		#include <netinet/ip.h>
		#include <netinet/ip_icmp.h>
		#include <netinet/in_systm.h>
		#include <unistd.h>
		#include <pthread.h>
		#include <sys/ioctl.h>
		#include <sys/types.h>
		#include <sys/param.h>
		#include <sys/socket.h>
		#include <sys/file.h>
		#include <sys/time.h>
		#include <errno.h>
		#include <arpa/telnet.h>
		#include <signal.h>

		#define closesocket(sock) close(sock)
		#define SOCKET_ERROR (-1)
		#define INVALID_SOCKET (-1)
	#endif
		
	typedef int SocketHandle;
#endif


class WebSocketDescriptor;

class WebSocketServer
{
private:
	unsigned short port;
	fd_set inset, outset, excset;
	timeval nulltime;
	SocketHandle listeningSocket;

	std::map<std::string, class WebSocketServerProtocol *> webSocketServerProtocols;

	int getLastError();
	std::list<WebSocketDescriptor *> descriptorList;
	std::list<WebSocketDescriptor *> descriptorsReceivingClientHeaderList;
	std::list<WebSocketDescriptor *> descriptorsAuthenticatedList;
public:
	WebSocketServer();
	~WebSocketServer();

	void setup(unsigned short port);
	void service();

	std::list<WebSocketDescriptor *> getDescriptorList();
	void closeDescriptor(WebSocketDescriptor *webSocketDescriptor);
	void addServerProtocol(class WebSocketServerProtocol *webSocketServerProtocol);
	class WebSocketServerProtocol *getServerProtocolByName(const std::string &protocolName);
};

#endif

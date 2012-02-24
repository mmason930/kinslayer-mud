/*******************************************************************
 *                                                                 *
 * WebSocketDescriptor.h                                           *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#ifndef WEB_SOCKET_DESCRIPTOR_H
#define WEB_SOCKET_DESCRIPTOR_H

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


#include <string>
#include <list>

class WebSocketDescriptor
{
	friend class WebSocketServer;
private:
	std::string ipAddress;
	
	std::list<std::string> inputBufferQueue;
	std::list<std::string> outputBufferQueue;
	class WebSocketServerProtocol *webSocketServerProtocol;

	std::string unprocessedInputBuffer;

	SocketHandle clientSocket;
	
	bool authenticated;
	bool messageStartNullByte;
	std::string::size_type messageReadPos;
	
	void socketWrite(const std::string &data);
	void socketRead();
	void writeOutputQueue();
	void processOutgoingPackets();
public:
	WebSocketDescriptor();
	WebSocketDescriptor(const std::string &ipAddress, SocketHandle clientSocket);
	~WebSocketDescriptor();

	std::string getIpAddress();

	std::string popCommandFromInputBufferQueue();
	void send(const std::string &packet);
	SocketHandle getClientSocket();
	bool isClosed();

	void readMessageFromClient();
	void readMessageFirstByte();
};

#endif

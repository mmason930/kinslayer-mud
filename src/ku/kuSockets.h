#ifndef KU_SOCKETS_H
#define KU_SOCKETS_H

/**************************************************************
 *                                                            *
 * kuSockets.h - mmason - 11/23/2009                          *
 *      - defines & includes supporting components for        *
 *        the networking utilities included in this library.  *
 *                                                            *
 **************************************************************/

#ifdef WIN32 /*** Windows ***/
	#include <sys\types.h>
	#include <winsock.h>
	#include <direct.h>
	#include <windows.h>
	#include <errno.h>
	#pragma comment(lib, "wsock32.lib")
#ifndef socklen_t
	typedef int socklen_t;
#endif

#else /*** Linux ***/

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

	typedef int socket_t;
	typedef socket_t SOCKET;

	int WSAGetLastError();

	#define closesocket(sock) ::close(sock)
	#define SOCKET_ERROR (-1)
	#define INVALID_SOCKET (-1)

#endif /*** / WIN32 ***/


// select() uses a fixed-size descriptor bitmap on POSIX systems.
inline bool kuSocketCanSelect(SOCKET socket)
{
#ifdef WIN32
	return socket != INVALID_SOCKET;
#else
	return socket >= 0 && socket < FD_SETSIZE;
#endif
}

#ifdef MSG_NOSIGNAL
constexpr int KU_SEND_FLAGS = MSG_NOSIGNAL;
#else
constexpr int KU_SEND_FLAGS = 0;
#endif

#endif


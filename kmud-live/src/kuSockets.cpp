
#include "kuSockets.h"

#ifndef WIN32
int WSAGetLastError()
{
	return errno;
}
#endif


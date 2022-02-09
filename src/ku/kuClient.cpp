#include <sstream>
#include <cstring>

#include "kuClient.h"

kuClient::kuClient()
{
	sock = 0;
	connected = false;
	hasHost = HOSTERR;
	port = 0;
	hostData = NULL;
	hostName.clear();
}

kuClient::~kuClient()
{
	disconnect();
}

int kuClient::receive( std::stringstream &buf )
{
	char buffer[2048];
	timeval nulltime = {0,0};
	int retval = 0;

	fd_set inset, outset, excset;

	FD_ZERO(&inset);
	FD_ZERO(&outset);
	FD_ZERO(&excset);
	FD_SET(this->sock, &inset);
	FD_SET(this->sock, &excset);

	if(select(this->sock + 1, &inset, &outset, &excset, &nulltime) < 0)
	{
		return -1;
		disconnect();
	}

	if(!(FD_ISSET(this->sock, &inset)))
		return 0;

	retval = recv(this->sock, buffer, sizeof(buffer)-1, 0);

	if(retval > 0)
	{
		buffer[retval] = '\0';
		buf << buffer;
		return retval;
	}
	else if(retval == 0) {
		disconnect();
		return -1;
	}
#ifdef WIN32
	else if(WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN)
		return 0;
#else
	else if(errno == EAGAIN || errno == EINTR)
		return 0;
#endif
	disconnect();
	return -1;
}
bool kuClient::send(const std::string &data)
{
	int i = 0;

	if( (i = ::send(this->sock, data.c_str(), data.size(), 0)) < 0)
	{
		disconnect();
		return false;
	}
	return true;
}

bool kuClient::connect(const std::string &h, const int p)
{
	this->hasHost	= HOSTERR;
	this->hostData	= 0;
	this->connected = false;
	this->hostName	= h;
	this->port		= p;
	hostent	*host;
	char	*ip;
	sockaddr_in sin;

#ifdef WIN32
	int i = 0;
	WSADATA wsaData;
	if( (i = WSAStartup(MAKEWORD(1,1), &wsaData)))
	{
		disconnect();
		return false;
	}
#endif

	if(!(host = gethostbyname(hostName.c_str())))
	{
		hasHost = HOSTERR;
		disconnect();
		return false;
	}
	hostName = host->h_name;

	memset(&sin, 0, sizeof(sockaddr_in));

	sin.sin_family	= AF_INET;
	sin.sin_port	= htons(port);

	ip = inet_ntoa (*(struct in_addr *)*host->h_addr_list); 
	sin.sin_addr.s_addr = inet_addr(ip);

	if( (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		disconnect();
		hasHost = HOSTERR;
		return false;
	}

	if( (::connect(sock, (const sockaddr *)&sin, sizeof(sin)) < 0) )
	{
		disconnect();
		hasHost = HOSTERR;
		return false;
	}

	hasHost = HOSTOK;
	connected = true;
	return true;
}

void kuClient::disconnect() {

	if( sock ) {
		closesocket(sock);
		sock = 0;
	}
	connected = false;
	hasHost = HOSTERR;
	port = 0;
	hostData = NULL;
	hostName.clear();
}

bool kuClient::isConnected() {

	return connected;
}

#include <sstream>
#include <cstring>

#include "kuClient.h"

kuClient::kuClient()
{
	sock = INVALID_SOCKET;
	connected = false;
    pendingOutput.clear();
    outputOffset = 0;
	hasHost = HOSTERR;
	port = 0;
	hostData = nullptr;
	hostName.clear();
}

kuClient::~kuClient()
{
	disconnect();
}

int kuClient::receive( std::stringstream &buf )
{
	if (!connected || !kuSocketCanSelect(sock) || !flushOutput())
		return -1;
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
		if (errno == EINTR) return 0;
		disconnect();
		return -1;
	}

	if(!(FD_ISSET(this->sock, &inset)))
		return 0;

	retval = recv(this->sock, buffer, sizeof(buffer)-1, 0);

	if(retval > 0)
	{
		buf.write(buffer, retval);
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
    constexpr std::size_t limit = 4 * 1024 * 1024;
    if (!connected) return false;
    if (data.size() > limit - pendingOutputSize()) { disconnect(); return false; }
    if (outputOffset) { pendingOutput.erase(0, outputOffset); outputOffset = 0; }
    pendingOutput.append(data);
    return flushOutput();
}

bool kuClient::flushOutput()
{
    if (!connected) return false;
    // Bound the work per gateway iteration; preserve bytes on short writes/EAGAIN.
    std::size_t budget = 256 * 1024;
    while (pendingOutputSize() && budget) {
        const auto count = std::min(pendingOutputSize(), budget);
        const auto sent = ::send(sock, pendingOutput.data() + outputOffset, count, KU_SEND_FLAGS);
        if (sent > 0) { outputOffset += sent; budget -= sent; continue; }
#ifdef WIN32
        const auto error = WSAGetLastError();
        if (sent < 0 && error == WSAEINTR) continue;
        if (sent < 0 && error == WSAEWOULDBLOCK) return true;
#else
        if (sent < 0 && errno == EINTR) continue;
        if (sent < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) return true;
#endif
        disconnect();
        return false;
    }
    if (!pendingOutputSize()) { pendingOutput.clear(); outputOffset = 0; }
    return true;
}

bool kuClient::connect(const std::string &h, const int p)
{
	disconnect();
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

	if (!kuSocketCanSelect(sock))
	{
		disconnect();
		return false;
	}

	if( (::connect(sock, (const sockaddr *)&sin, sizeof(sin)) < 0) )
	{
		disconnect();
		hasHost = HOSTERR;
		return false;
	}

	#ifdef WIN32
    u_long nonblocking = 1;
    if (ioctlsocket(sock, FIONBIO, &nonblocking) != 0) { disconnect(); return false; }
#else
    const int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) { disconnect(); return false; }
#endif
    hasHost = HOSTOK;
	connected = true;
	return true;
}

void kuClient::disconnect() {

	if( sock != INVALID_SOCKET ) {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
	connected = false;
    pendingOutput.clear();
    outputOffset = 0;
	hasHost = HOSTERR;
	port = 0;
	hostData = nullptr;
	hostName.clear();
}

bool kuClient::isConnected() {

	return connected;
}

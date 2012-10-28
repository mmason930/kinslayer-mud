#ifndef WEB_SOCKET_CLIENT_HEADER_HIXIE76
#define WEB_SOCKET_CLIENT_HEADER_HIXIE76

#include <string>
#include "WebSocketClientHeader.h"

class WebSocketClientHeaderHixie76 : public WebSocketClientHeader
{
protected:
	void read(std::string &packet);
public:
	std::string generateResponse(unsigned short port, const char *webSocketServerProtocolName);
};

#endif

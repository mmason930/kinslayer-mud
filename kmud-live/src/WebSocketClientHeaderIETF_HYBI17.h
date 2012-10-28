#ifndef WEB_SOCKET_CLIENT_HEADER_IETF_HYBI17
#define WEB_SOCKET_CLIENT_HEADER_IETF_HYBI17

#include <string>
#include "WebSocketClientHeader.h"

class WebSocketClientHeaderIETF_HYBI17 : public WebSocketClientHeader
{
protected:
	void read(std::string &packet);
public:
	std::string generateResponse(unsigned short port, const char *webSocketServerProtocolName);
};

#endif

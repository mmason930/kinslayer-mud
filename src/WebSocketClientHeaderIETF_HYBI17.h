#ifndef WEB_SOCKET_CLIENT_HEADER_IETF_HYBI17
#define WEB_SOCKET_CLIENT_HEADER_IETF_HYBI17

#include <string>
#include "WebSocketClientHeader.h"

// Note that Kinslayer implemented multiple websocket specs before the it was
// standardized with RFC 6455. We've removed all implementations except for hybi-17.
// This was the last websocket protocol draft before RFC 6455. To date, our
// implementation has held up to the official standard, but we should revisit
// the official spec.

// See: https://datatracker.ietf.org/doc/html/draft-ietf-hybi-thewebsocketprotocol-17
// See: https://datatracker.ietf.org/doc/html/rfc6455
class WebSocketClientHeaderIETF_HYBI17 : public WebSocketClientHeader
{
protected:
	void read(std::string &packet);
public:
	std::string generateResponse(unsigned short port, const char *webSocketServerProtocolName);
};

#endif

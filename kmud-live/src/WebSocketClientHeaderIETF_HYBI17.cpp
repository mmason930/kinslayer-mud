#include <string>
#include <sstream>

#include "StringUtil.h"

#include "WebSocketException.h"
#include "WebSocketClientHeaderIETF_HYBI17.h"

std::string WebSocketClientHeaderIETF_HYBI17::generateResponse(unsigned short port, const char *webSocketServerProtocolName)
{
	bool secureFlag = false;

	std::string::size_type pos;

	std::string message;

	std::string secWebSocketAccept = fields["Sec-WebSocket-Key"] + std::string("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	secWebSocketAccept = StringUtil::SHA1(secWebSocketAccept);
	secWebSocketAccept = StringUtil::base64Encode(secWebSocketAccept);

	message.append("HTTP/1.1 101 Switching Protocols\r\n");
	message.append("Upgrade: WebSocket\r\n");
	message.append("Connection: Upgrade\r\n");
	message.append(std::string("Sec-WebSocket-Accept: ") + secWebSocketAccept + std::string("\r\n"));
//	message.append(std::string("Sec-WebSocket-Version: ") + fields["Sec-WebSocket-Version"] + std::string("\r\n"));
//	message.append(std::string("Sec-WebSocket-Extensions: ") + fields["Sec-WebSocket-Extensions"] + std::string("\r\n"));

	message.append((webSocketServerProtocolName ? (std::string("Sec-WebSocket-Protocol: ") + std::string("lws-mirror-protocol") + std::string("\r\n")) : ""));
	//message.append((webSocketServerProtocolName ? (std::string("Sec-WebSocket-Protocol: ") + std::string(webSocketServerProtocolName) + std::string("\r\n")) : ""));
	message.append("\r\n");

	return message;

}

void WebSocketClientHeaderIETF_HYBI17::read(std::string &packet)
{
	std::string line;
	std::string::size_type startPos = 0, crlfPos;

	line = WebSocketClientHeader::getLine(packet, startPos, crlfPos);

	readFirstLine(line);

	//Read all of the fields. Field ending is denoted by a pair of CRLF.

	startPos = crlfPos + 2;

	do {
		
		line = WebSocketClientHeader::getLine(packet, startPos, crlfPos);

		readFieldLine(line);

		startPos = crlfPos + 2;

		if( (startPos + 2) > packet.size() ) {

			throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
		}

	} while(packet[startPos] != '\r' && packet[startPos + 1] != '\n');

	//Skip over the last crlf.
	startPos += 2;

	packet.erase(0, startPos);
}

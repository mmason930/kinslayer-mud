/*******************************************************************
 *                                                                 *
 * WebSocketClientHeader.cpp                                       *
 *                                                                 *
 * Author: Michael Mason <mikemason930@gmail.com>, (C) June 2011   *
 *                                                                 *
 * Copyright: See COPYING file that comes with this distribution   *
 *                                                                 *
 *******************************************************************/

#include "WebSocketClientHeader.h"
#include "WebSocketException.h"
#include "WebSocketServerProtocol.h"

#include <sstream>
#include <stdexcept>
#include <vector>

void MD5(const unsigned char *input, int ilen, unsigned char *output);

#define INVALID_HEADER_EXCEPTION_MESSAGE "Invalid header sent by client"

WebSocketClientHeader::WebSocketClientHeader(void)
{
}

WebSocketClientHeader::~WebSocketClientHeader(void)
{
}

std::string WebSocketClientHeader::getLine(const std::string &packet, std::string::size_type startPos, std::string::size_type &crlfPos)
{
	std::string::size_type pos = packet.find('\r', startPos);
	
	if(pos != std::string::npos && (pos+1) < packet.size() && packet[pos+1] == '\n') {

		crlfPos = pos;
		return packet.substr(startPos, crlfPos - startPos);
	}

	throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
}

void WebSocketClientHeader::read(std::string &packet)
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

		if( (startPos + 2) >= packet.size() ) {

			throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
		}

	} while(packet[startPos] != '\r' && packet[startPos + 1] != '\n');

	//Skip over the last crlf.
	startPos += 2;

	//The remainder is the final line, consisting of eight random bytes.
	while(startPos < packet.size()) {

		secretKey.push_back(packet[startPos++]);
	}

	if(secretKey.size() > 8) {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	packet.erase(0, startPos);
}

void WebSocketClientHeader::readFieldLine(const std::string &line)
{
	std::string::size_type pos = 0;
	std::string name;

	try {
		
		//The field name ends with a ": "
		while(line.at(pos) != ':' && line.at(pos + 1) != ' ') {

			if( (line.at(pos) < 0x0021 || line.at(pos) > 0x0039) && (line.at(pos) < 0x003B || line.at(pos) > 0x007E) ) {

				throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
			}

			name.push_back(line[pos]);

			++pos;
		}
	}
	catch(std::out_of_range e) {
	//This exception will only be thrown if we never hit a ": " sequence in this line.

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	//Validate field name length. It must be one or more characters in length.
	if(name.empty()) {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	pos += 2;//Skip over the ": "

	//The value may be zero or more characters in length, and consists of the remainder of the line.
	this->fields[name] = line.substr(pos);
}

void WebSocketClientHeader::readFirstLine(const std::string &line)
{
	std::string::size_type pos = 0, posBefore, index;

	//Read the method.
	posBefore = pos;
	pos = line.find(' ', pos);

	if(pos == std::string::npos) {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	this->method = line.substr(posBefore, pos - posBefore);

	//Validate the method(must be equal to "GET").
	if(this->method != "GET") {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	//Read the resource.
	if(++pos >= line.size() || line[pos] != '/') {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	posBefore = pos;
	pos = line.find(' ', pos);

	if(pos == std::string::npos) {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	resource = line.substr(posBefore, pos - posBefore);

	//Validate the resource(only characters between U+0021 and U+007E are permitted).
	for(index = 0;index < method.size();++index) {

		if(method[index] < 0x0021 || method[index] > 0x007E) {

			throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
		}
	}

	//The ending can be safely ignored(we know the ending CRLF was stripped successfully).
}

std::string WebSocketClientHeader::generateResponse(unsigned short port, WebSocketServerProtocol *webSocketServerProtocol)
{
	std::stringstream portStream;
	bool secureFlag = false;

	unsigned long long keyNumber1 = 0;
	unsigned long long keyNumber2 = 0;
	unsigned long part1;
	unsigned long part2;
	unsigned int spaces1 = 0;
	unsigned int spaces2 = 0;
	unsigned char challenge[16];
	unsigned char response[17];

	std::string::size_type pos;

	std::string message;

	portStream << port;

	pos = 0;
	while(pos < fields["Sec-WebSocket-Key1"].size()) {

		char symbol = fields["Sec-WebSocket-Key1"][pos];
		if(symbol >= 0x0030 && symbol <= 0x0039) {
			
			keyNumber1 = keyNumber1 * 10 + (symbol - 0x0030);
		}

		spaces1 += (symbol == 0x0020);

		++pos;
	}

	pos = 0;
	while(pos < fields["Sec-WebSocket-Key2"].size()) {

		char symbol = fields["Sec-WebSocket-Key2"][pos];
		if(symbol >= 0x0030 && symbol <= 0x0039) {

			keyNumber2 = keyNumber2 * 10 + (symbol - 0x0030);
		}
		
		spaces2 += (symbol == 0x0020);

		++pos;
	}

	if(spaces1 == 0 || spaces2 == 0 || keyNumber1 % spaces1 != 0 || keyNumber2 % spaces2 != 0) {

		throw WebSocketException(INVALID_HEADER_EXCEPTION_MESSAGE);
	}

	part1 = keyNumber1 / spaces1;
	part2 = keyNumber2 / spaces2;

	challenge[0] = (part1 >> 24);
	challenge[1] = (part1 >> 16);
	challenge[2] = (part1 >>  8);
	challenge[3] = (part1);
	challenge[4] = (part2 >> 24);
	challenge[5] = (part2 >> 16);
	challenge[6] = (part2 >>  8);
	challenge[7] = (part2);

	memcpy(challenge + 8, secretKey.c_str(), 8);
	
	MD5(challenge, 16, (unsigned char *)&response);

	response[16] = '\0';

	message.append("HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
	message.append("Upgrade: WebSocket\r\n");
	message.append("Connection: Upgrade\r\n");
	message.append(std::string("ws") + (std::string(secureFlag ? "s" : "")) + std::string("://") + fields["Host"] + this->resource + std::string("\r\n"));
	message.append(std::string("Sec-WebSocket-Location: ") + std::string("ws") + (std::string(secureFlag ? "s" : "")) + std::string("://") + fields["Host"] + this->resource + std::string("\r\n"));
	message.append(std::string("Sec-WebSocket-Origin: ") + fields["Origin"] + std::string("\r\n"));
	message.append((webSocketServerProtocol ? (std::string("Sec-WebSocket-Protocol: ") + webSocketServerProtocol->getName() + std::string("\r\n")) : ""));
	message.append("\r\n");
	message.append((char *)response, 16);

	return message;

}

std::string WebSocketClientHeader::getFieldByName(const std::string &fieldName)
{
	return fields[fieldName];
}

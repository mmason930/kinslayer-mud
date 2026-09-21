#include "WebSocketDataFrame.h"

#include <sstream>
#include <memory>
#include "WebSocketException.h"

WebSocketDataFrame::WebSocketDataFrame()
{
	fin = rsv1 = rsv2 = rsv3 = isMasked = false;
	opcode = 0;
}
WebSocketDataFrame::~WebSocketDataFrame()
{
}

bool WebSocketDataFrame::getFin() const
{
	return fin;
}
void WebSocketDataFrame::setFin(const bool fin)
{
	this->fin = fin;
}

bool WebSocketDataFrame::getRsv1() const
{
	return rsv1;
}
void WebSocketDataFrame::setRsv1(const bool rsv1)
{
	this->rsv1 = rsv1;
}

bool WebSocketDataFrame::getRsv2() const
{
	return rsv2;
}
void WebSocketDataFrame::setRsv2(const bool rsv2)
{
	this->rsv2 = rsv2;
}

bool WebSocketDataFrame::getRsv3() const
{
	return rsv3;
}
void WebSocketDataFrame::setRsv3(const bool rsv3)
{
	this->rsv3 = rsv3;
}

unsigned char WebSocketDataFrame::getOpCode() const
{
	return opcode;
}
void WebSocketDataFrame::setOpCode(const unsigned char opcode)
{
	this->opcode = opcode;
}

std::string WebSocketDataFrame::getPayloadData() const
{
	return payloadData;
}
void WebSocketDataFrame::setPayloadData(const std::string payloadData)
{
	this->payloadData = payloadData;
}

bool WebSocketDataFrame::getIsMasked() const
{
	return isMasked;
}

void WebSocketDataFrame::setIsMasked(bool isMasked)
{
	this->isMasked = isMasked;
}

std::vector<unsigned char> WebSocketDataFrame::getMaskCode() const
{
	return maskCode;
}

void WebSocketDataFrame::setMaskCode(const std::vector<unsigned char> &maskCode)
{
	this->maskCode = maskCode;
}

WebSocketDataFrame *WebSocketDataFrame::parse(const std::string &input, unsigned int &bytesRead, bool requireMask)
{
    bytesRead = 0;
    if (input.size() < 2) return nullptr;
    const auto first = static_cast<unsigned char>(input[0]);
    const auto second = static_cast<unsigned char>(input[1]);
    const bool fin = (first & 0x80) != 0, masked = (second & 0x80) != 0;
    const unsigned char opcode = first & 0x0f;
    if ((first & 0x70) || (opcode != 0 && opcode != 1 && opcode != 2 && opcode != 8 && opcode != 9 && opcode != 10))
        throw WebSocketException("Invalid WebSocket frame flags");
    if (requireMask && !masked) throw WebSocketException("Client frames must be masked");
    const bool control = opcode >= 8;
    const unsigned char marker = second & 0x7f;
    if (control && (!fin || marker > 125)) throw WebSocketException("Invalid control frame");
    std::size_t position = 2;
    uint64_t length = marker;
    if (marker >= 126) {
        const std::size_t width = marker == 126 ? 2 : 8;
        if (input.size() - position < width) return nullptr;
        if (width == 8 && (static_cast<unsigned char>(input[position]) & 0x80))
            throw WebSocketException("Invalid 64-bit frame length");
        length = 0;
        for (std::size_t i = 0; i < width; ++i)
            length = (length << 8) | static_cast<unsigned char>(input[position++]);
        if ((width == 2 && length < 126) || (width == 8 && length <= 65535))
            throw WebSocketException("Noncanonical frame length");
    }
    if (length > maxPayloadSize) throw WebSocketException("WebSocket frame too large");
    std::vector<unsigned char> key;
    if (masked) {
        if (input.size() - position < 4) return nullptr;
        for (int i = 0; i < 4; ++i) key.push_back(static_cast<unsigned char>(input[position++]));
    }
    if (length > input.size() - position) return nullptr;
    auto frame = std::make_unique<WebSocketDataFrame>();
    frame->fin = fin;
    frame->opcode = opcode;
    frame->isMasked = masked;
    frame->maskCode = key;
    frame->payloadData = input.substr(position, length);
    if (masked)
        for (std::size_t i = 0; i < length; ++i) frame->payloadData[i] ^= key[i % 4];
    if (opcode == 8 && length == 1) throw WebSocketException("Invalid close frame");
    bytesRead = static_cast<unsigned int>(position + length);
    return frame.release();
}

void sendToStringStreamAsBinary(std::stringstream &stream, const unsigned long long packet)
{
	stream << (unsigned char)(packet >> 56)
	       << (unsigned char)(packet >> 48)
		   << (unsigned char)(packet >> 40)
		   << (unsigned char)(packet >> 32)
		   << (unsigned char)(packet >> 24)
		   << (unsigned char)(packet >> 16)
		   << (unsigned char)(packet >>  8)
		   << (unsigned char)(packet);
}

void sendToStringStreamAsBinary(std::stringstream &stream, const unsigned int packet)
{
	stream << (unsigned char)(packet >> 24)
		   << (unsigned char)(packet >> 16)
		   << (unsigned char)(packet >>  8)
		   << (unsigned char)(packet);
}

void sendToStringStreamAsBinary(std::stringstream &stream, const unsigned short packet)
{
	stream << (unsigned char)(packet >>  8)
		   << (unsigned char)(packet);
}

std::string WebSocketDataFrame::prepareNetworkPacket()
{
	std::stringstream outputStream;
	unsigned char outputByte1;
	unsigned short outputByte2;
	unsigned int outputByte4;
	unsigned long long outputByte8;

	outputByte1= (unsigned char) (getFin()    << 7)
		       + (unsigned char) (getRsv1()   << 6)
			   + (unsigned char) (getRsv2()   << 5)
			   + (unsigned char) (getRsv3()   << 4)
			   + (unsigned char) (getOpCode());

	outputStream << outputByte1;

	unsigned long long payloadSize;

	if(payloadData.size() < 126)
		payloadSize = payloadData.size();
	else if(payloadData.size() >= 126 && payloadData.size() <= 65535)
		payloadSize = 126;
	else
		payloadSize = 127;

	outputByte1 = (((unsigned char)getIsMasked()) << 7)
			    + ((unsigned char)payloadSize);
	
	outputStream << outputByte1;

	if(payloadSize == 126)
	{
		outputByte2 = (unsigned short)payloadData.size();
		sendToStringStreamAsBinary(outputStream, outputByte2);
	}
	else if(payloadSize == 127)
	{
		outputByte8 = (unsigned long long)payloadData.size();
		sendToStringStreamAsBinary(outputStream, outputByte8);
	}

	if(getIsMasked())
	{
        if (maskCode.size() != 4) throw WebSocketException("Invalid mask key");
		outputStream << this->maskCode[ 0 ]
					 << this->maskCode[ 1 ]
					 << this->maskCode[ 2 ]
					 << this->maskCode[ 3 ];
	}

	if (getIsMasked()) {
        std::string encoded = payloadData;
        for (std::size_t i = 0; i < encoded.size(); ++i) encoded[i] ^= maskCode[i % 4];
        outputStream << encoded;
    } else outputStream << payloadData;

	return outputStream.str();
}


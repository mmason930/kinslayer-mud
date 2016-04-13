#include "WebSocketDataFrame.h"

#include <string>
#include <sstream>

WebSocketDataFrame::WebSocketDataFrame()
{
	fin = rsv1 = rsv2 = rsv3 = false;
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

WebSocketDataFrame *WebSocketDataFrame::parse(const std::string &input, unsigned int &bytesRead)
{
	WebSocketDataFrame *webSocketDataFrame;
	bytesRead = 0;

	if(input.size() < 2)
		return NULL;

	int readPos = 0;
	bool fin = ((unsigned char)input[ readPos ]) & (1);
	bool rsv1 = ((unsigned char)input[ readPos ]) & (1 << 1);
	bool rsv2 = ((unsigned char)input[ readPos ]) & (1 << 2);
	bool rsv3 = ((unsigned char)input[ readPos ]) & (1 << 3);

	unsigned char opcode = ((unsigned char)input[ readPos ]) & ~(240);

	if(opcode == 0x8)
	{
		webSocketDataFrame = new WebSocketDataFrame();
		webSocketDataFrame->setOpCode(opcode);
		return webSocketDataFrame;
	}

	++readPos;

	bool mask = ((unsigned char)input[ readPos ]) & (1 << 7);
	unsigned long long payloadLength = (unsigned char) (input[readPos] & ~(1 << 7));

	++readPos;

	if(payloadLength == 126)
	{//If length == 126, then the actual payload length is the next 2 bytes interpreted as unsigned integers.

		if(readPos + 2 > input.size())
			return NULL;
		
		payloadLength = 0;
		
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos    ]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 1]);

		readPos += 2;
	}
	else if(payloadLength == 127)
	{//If length == 127, then the actual payload length is the next 8 bytes interpreted as an unsiged 64-bit integer.

		if(readPos + 8 > input.size())
			return NULL;

		payloadLength = 0;

		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 7]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 6]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 5]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 4]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 3]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 2]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos + 1]);
		payloadLength = (payloadLength << 8) + ((unsigned char)input[readPos    ]);

		readPos += 8;
	}

	unsigned char maskingKey[4];

	if(mask)
	{
		if(readPos + 4 > input.size())
			return NULL;

		maskingKey[0] = (unsigned char)input[readPos    ];
		maskingKey[1] = (unsigned char)input[readPos + 1];
		maskingKey[2] = (unsigned char)input[readPos + 2];
		maskingKey[3] = (unsigned char)input[readPos + 3];

		readPos += 4;
	}

	if(readPos + payloadLength > input.size())
		return NULL;

	std::string payloadData = input.substr(readPos, payloadLength);

	if(payloadData.size() == payloadLength)
	{
		if(mask)
		{
			for(std::string::size_type i = 0;i < payloadData.size();++i)
			{
				payloadData[ i ] = payloadData[ i ] ^ maskingKey[ i % 4 ];
			}
		}
	}
	else
		return NULL;

	readPos += payloadLength;
	bytesRead = readPos;
	
	webSocketDataFrame = new WebSocketDataFrame();

	webSocketDataFrame->setFin(fin);
	webSocketDataFrame->setRsv1(rsv1);
	webSocketDataFrame->setRsv2(rsv2);
	webSocketDataFrame->setRsv3(rsv3);
	webSocketDataFrame->setOpCode(opcode);
	webSocketDataFrame->setPayloadData(payloadData);

	return webSocketDataFrame;
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
	else if(payloadData.size() >= 126 && payloadData.size() <= 65536)
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
		outputStream << this->maskCode[ 0 ]
					 << this->maskCode[ 1 ]
					 << this->maskCode[ 2 ]
					 << this->maskCode[ 3 ];
	}

	outputStream << this->getPayloadData();

	return outputStream.str();
}


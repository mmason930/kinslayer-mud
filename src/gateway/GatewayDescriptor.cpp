#include <sstream>
#include "GatewayDescriptor.h"
#include "../websocket/WebSocketDataFrame.h"
#include "../websocket/WebSocketException.h"

#include "../jsoncpp/json.h"

GatewayDescriptor::GatewayDescriptor()
{
	serverConnection = NULL;
	clientConnection = NULL;
	gatewayListener = NULL;
}
GatewayDescriptor::~GatewayDescriptor()
{
}

void GatewayDescriptor::setClientConnection(kuDescriptor *clientConnection)
{
	this->clientConnection = clientConnection;
}
void GatewayDescriptor::setServerConnection(kuClient *serverConnection)
{
	this->serverConnection = serverConnection;
}

void GatewayDescriptor::setSession(const std::string &session)
{
	this->session = session;
}
std::string GatewayDescriptor::getSession()
{
	return this->session;
}

void GatewayDescriptor::sendToClient(const std::string &packet)
{
	if(this->getGatewayListener()->getType() == GATEWAY_LISTENER_TYPE_WEBSOCKET && this->getStatus() != GatewayDescriptorStatus::handshaking)
	{
		WebSocketDataFrame dataFrame;

		dataFrame.setFin(true);
		dataFrame.setIsMasked(false);
		dataFrame.setRsv1(false);
		dataFrame.setRsv2(false);
		dataFrame.setRsv3(false);
		dataFrame.setOpCode(0x01);

		
		char *cleansedPacket = new char[ packet.size() + 1 ];
		std::string::size_type writeIndex = 0;
		std::string::size_type readIndex = 0;
		for(std::string::size_type readIndex = 0;readIndex < packet.size();++readIndex)
		{
			if( ((int)(packet[ readIndex ])) > 0 )
			{
				cleansedPacket[ writeIndex ] = packet[ readIndex ];
				++writeIndex;
			}
		}
		cleansedPacket[ writeIndex ] = '\0';
		dataFrame.setPayloadData(cleansedPacket);

		delete[] cleansedPacket;

		clientConnection->socketWriteInstant(dataFrame.prepareNetworkPacket());
	}
	else if(this->getGatewayListener()->getType() == GATEWAY_LISTENER_TYPE_WEBSOCKET)
	{
		clientConnection->socketWriteInstant(packet);
	}
	else
	{
		clientConnection->send(packet.c_str());
	}
}
void GatewayDescriptor::sendToServer(const std::string &packet)
{
	serverConnection->send(packet);
}

void GatewayDescriptor::sendOutputMessageToClient(const std::string &packet)
{
	if(this->getGatewayListener()->getType() == GATEWAY_LISTENER_TYPE_WEBSOCKET)
	{
		Json::Value rootValue;
		rootValue["method"] = "Output";
		rootValue["data"] = packet;

		Json::FastWriter writer;
		this->sendToClient(writer.write(rootValue) + (char)0x06);
	}
	else
	{
		this->sendToClient(packet);
	}
}

std::string GatewayDescriptor::pullFromClient()
{
	std::string input;
	if(this->getGatewayListener()->getType() == GATEWAY_LISTENER_TYPE_WEBSOCKET && getStatus() != GatewayDescriptorStatus::handshaking)
	{
		input = clientConnection->getInputBuffer();
		if(input.size() > 0)
		{
			unsigned int bytesRead = 0;
			WebSocketDataFrame *webSocketDataFrame = WebSocketDataFrame::parse(input, bytesRead);

			if(webSocketDataFrame != NULL)
			{
				if(webSocketDataFrame->getOpCode() == 0x8)
				{
					delete webSocketDataFrame;
					throw WebSocketException("Socket Closed");
				}
				clientConnection->eraseInput(0, bytesRead);
				input = webSocketDataFrame->getPayloadData();
				delete webSocketDataFrame;
				return input;
			}
		}
	}
	else
	{
		input = clientConnection->getInputBuffer();

		clientConnection->clearInput();

		return input;
	}

	return "";
}

std::string GatewayDescriptor::pullFromServer()
{
	std::stringstream buffer;

	serverConnection->receive(buffer);

	return buffer.str();
}

bool GatewayDescriptor::connect(const std::string host, const int port)
{
	if(this->serverConnection != NULL) {

		delete this->serverConnection;
	}

	serverConnection = new kuClient();
	serverConnection->connect(host, port);

	return true;
}

kuDescriptor *GatewayDescriptor::getClientConnection()
{
	return clientConnection;
}

kuClient *GatewayDescriptor::getServerConnection()
{
	return serverConnection;
}

GatewayDescriptorStatus *GatewayDescriptor::getStatus()
{
	return status;
}

void GatewayDescriptor::setStatus(GatewayDescriptorStatus *status)
{
	this->status = status;
}

std::string GatewayDescriptor::getRandomId()
{
	return randomId;
}

void GatewayDescriptor::setRandomId(const std::string &randomId)
{
	this->randomId = randomId;
}

GatewayListener *GatewayDescriptor::getGatewayListener()
{
	return gatewayListener;
}

void GatewayDescriptor::setGatewayListener(GatewayListener *gatewayListener)
{
	this->gatewayListener = gatewayListener;
}

std::string GatewayDescriptor::getCurrentInputBuffer() const
{
	return currentInputBuffer;
}

void GatewayDescriptor::clearCurrentInputBuffer()
{
	this->currentInputBuffer.clear();
}

void GatewayDescriptor::appendToCurrentInputBuffer(const std::string &buffer)
{
	this->currentInputBuffer += buffer;
}

GatewayDescriptorType *GatewayDescriptor::getType() const
{
	return type;
}

void GatewayDescriptor::setType(GatewayDescriptorType *type)
{
	this->type = type;
}

std::optional<std::string> GatewayDescriptor::getProxyForwardedIpAddress() const
{
	return proxyForwardedIpAddress;
}

void GatewayDescriptor::setProxyForwardedIpAddress(const std::optional<std::string> &proxyForwardedIpAddress)
{
	this->proxyForwardedIpAddress = proxyForwardedIpAddress;
}
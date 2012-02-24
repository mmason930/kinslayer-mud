#include <sstream>
#include "GatewayDescriptor.h"


GatewayDescriptor::GatewayDescriptor()
{
	serverConnection = NULL;
	clientConnection = NULL;
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
	clientConnection->send(packet.c_str());
}
void GatewayDescriptor::sendToServer(const std::string &packet)
{
	serverConnection->send(packet);
}

std::string GatewayDescriptor::pullFromClient()
{
	std::string input = clientConnection->getInputBuffer();

	clientConnection->clearInput();

	return input;
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

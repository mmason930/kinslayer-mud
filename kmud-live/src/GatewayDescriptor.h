#ifndef GATEWAY_DESCRIPTOR_H
#define GATEWAY_DESCRIPTOR_H

#include "kuDescriptor.h"
#include "kuClient.h"

#include "GatewayDescriptorStatus.h"

class GatewayDescriptor
{
private:

	std::string session;
	kuDescriptor *clientConnection;
	kuClient *serverConnection;
	GatewayDescriptorStatus *status;
	std::string randomId;

public:

	GatewayDescriptor();
	~GatewayDescriptor();

	void setClientConnection(kuDescriptor *clientConnection);
	void setServerConnection(kuClient *serverConnection);

	void setSession(const std::string &session);
	std::string getSession();

	void sendToClient(const std::string &packet);
	void sendToServer(const std::string &packet);

	std::string pullFromClient();
	std::string pullFromServer();

	bool connect(const std::string host, const int port);

	kuDescriptor *getClientConnection();
	kuClient *getServerConnection();

	GatewayDescriptorStatus *getStatus();
	void setStatus(GatewayDescriptorStatus *status);

	std::string getRandomId();
	void setRandomId(const std::string &randomId);
};

#endif

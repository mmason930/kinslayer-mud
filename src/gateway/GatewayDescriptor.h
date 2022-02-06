#ifndef GATEWAY_DESCRIPTOR_H
#define GATEWAY_DESCRIPTOR_H

#include "../ku/kuDescriptor.h"
#include "../ku/kuClient.h"

#include "GatewayListener.h"
#include "GatewayDescriptorStatus.h"
#include "GatewayDescriptorType.h"

#include <optional>

class GatewayDescriptor
{
private:

	std::string session;
	kuDescriptor *clientConnection;
	kuClient *serverConnection;
	GatewayListener *gatewayListener;
	GatewayDescriptorStatus *status;
	GatewayDescriptorType *type;
	std::string randomId;
	std::string currentInputBuffer;
	std::optional<std::string> proxyForwardedIpAddress;

public:

	GatewayDescriptor();
	~GatewayDescriptor();

	void setClientConnection(kuDescriptor *clientConnection);
	void setServerConnection(kuClient *serverConnection);

	void setSession(const std::string &session);
	std::string getSession();

	void sendToClient(const std::string &packet);
	void sendToServer(const std::string &packet);

	void sendOutputMessageToClient(const std::string &packet);

	std::string pullFromClient();
	std::string pullFromServer();

	bool connect(const std::string host, const int port);

	kuDescriptor *getClientConnection();
	kuClient *getServerConnection();

	GatewayDescriptorStatus *getStatus();
	void setStatus(GatewayDescriptorStatus *status);

	std::string getRandomId();
	void setRandomId(const std::string &randomId);

	GatewayListener *getGatewayListener();
	void setGatewayListener(GatewayListener *gatewayListener);

	std::string getCurrentInputBuffer() const;
	void clearCurrentInputBuffer();
	void appendToCurrentInputBuffer(const std::string &buffer);

	GatewayDescriptorType *getType() const;
	void setType(GatewayDescriptorType *type);

	std::optional<std::string> getProxyForwardedIpAddress() const;
	void setProxyForwardedIpAddress(const std::optional<std::string> &proxyForwardedIpAddress);
};

#endif

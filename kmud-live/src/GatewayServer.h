#ifndef GATEWAY_SERVER_H
#define GATEWAY_SERVER_H

#include <list>
#include "kuListener.h"
#include "DateTime.h"

#include "GatewayDescriptor.h"

class GatewayServer
{
private:

	std::list<GatewayDescriptor*> descriptors;
	std::list<kuListener *> listeners;
//	kuListener *listener;

	kuClient *motherConnectionToServer;

	std::string inputBufferFromServer;

	std::string serverHost;
	int serverPort;
	int listeningPort;
	int secondaryPort;

	DateTime lastPingSentToGameServer;
	DateTime lastPingResponseFromGameServer;

	bool crashed;
	bool rebooting;

	void processInputFromMotherConnectionToServer();

public:

	GatewayServer();
	GatewayServer(const int port, const int *secondaryPort, const std::string &serverHost, const int serverPort);
	~GatewayServer();

	std::string getServerHost();
	int getServerPort();

	void pingGameServer();
	DateTime getLastPingSentToServerDatetime();
	DateTime getLastPingResponseFromGameServerDatetime();

	bool hasCommand();
	std::string getCommand();

	void setup(const int port, const int *secondaryGatewayPort, const std::string &serverHost, const int serverPort);
	void run();
	void shutdown();
	void setupAndRun(const int port, const int *secondaryPort,  const std::string &serverHost, const int serverPort);

	void attemptConnectionWithGameServer();
	void bindListener();

	std::list<GatewayDescriptor*> &getDescriptors();

	void removeDescriptor(kuDescriptor *descriptor);

	void disconnectDescriptorFromGameAndGateway(GatewayDescriptor *gatewayDescriptor);

	bool isConnectedToGameServer();

	kuClient *getMotherConnectionToServer();

	bool mudIsCrashed();
	bool mudIsRebooting();
	bool mudIsDown();

	void sendToDescriptors(const std::string &message);
	void handleGameShutdown();
};

#endif

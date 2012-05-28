#ifndef GATEWAY_SERVER_H
#define GATEWAY_SERVER_H

#include <list>
#include "kuListener.h"
#include "DateTime.h"

#include "GatewayDescriptor.h"
#include "MudStatus.h"

#define BASIC_CONFIG_PATH		"lib/misc/BasicConfig"

class GatewayServer
{
private:

	std::list<GatewayDescriptor*> descriptors;
	std::list<kuListener *> listeners;
//	kuListener *listener;

	kuClient *motherConnectionToServer;

	std::string inputBufferFromServer;
	std::string mudRootDirectoryPath;
	std::string mudExecutablePath;

	std::string serverHost;
	int serverPort;
	int listeningPort;
	int secondaryPort;
	unsigned int mudProcessId;
	bool shutdownOnReboot;

	DateTime lastPingSentToGameServer;
	DateTime lastPingResponseFromGameServer;
	DateTime statusLastModifiedDatetime;
	DateTime lastAttemptedMudStartDatetime;
	DateTime lastMudProcessIdCheckDatetime;

	bool crashed;
	bool rebooting;
	bool restartOnShutdown;

	MudStatus *mudStatus;

	void processInputFromMotherConnectionToServer();

public:

	GatewayServer();
	~GatewayServer();

	std::string getServerHost();
	int getServerPort();

	void pingGameServer();
	DateTime getLastPingSentToServerDatetime();
	DateTime getLastPingResponseFromGameServerDatetime();

	bool hasCommand();
	std::string getCommand();

	void setup();
	void run();
	void shutdown();
	void setupAndRun();

	void disconnectMotherConnectionFromGameServer();
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

	bool loadConfiguration();
	void reloadConfiguration();
	void sendToDescriptors(const std::string &message);
	void handleGameShutdown();
	std::string getMudRootDirectoryPath();
	bool getRestartOnShutdown();
	bool isMudProcessRunning();
	MudStatus *getMudStatus();
	void setMudStatus(MudStatus *mudStatus);
	void attemptMudStart();
};

#endif

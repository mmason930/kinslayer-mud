#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <functional>

#include "PlayerPortalServer.h"

//Eventually, we should store all globals in this class.
class Game
{
protected:
	PlayerPortalServer *playerPortalServer;
	std::string name;
	std::string version;
	std::string defaultDirectory;
	std::string playerLogsDirectory;
	std::map<std::string, std::string> basicConfig; //The configuration loaded from the BasicConfig file on the server.

public:

	Game();
	~Game();

	void setupPlayerPortalServer(const int port);
	void processPlayerPortalServer();
	void cleanupPlayerPortalServer();

	std::string getName();
	void setName(const std::string &name);

	void loadBasicConfig();
	std::string getBasicConfigValue(const std::string &configName);

	std::string getVersion();
	std::string getDefaultDirectory();
	std::string getPlayerLogsDirectory();

	bool hasBasicConfiguration(const std::string &basicConfigurationName);
	void setupFilesystem();
	bool playerPortalServerIsConnected();
	int getPlayerPortalPort();
	unsigned int getNumberOfPlayerPortalDescriptors();
	void sendToAll(std::function<std::string(class Character *target)>);
};

extern class Game *game;

#endif
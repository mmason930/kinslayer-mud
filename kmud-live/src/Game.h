#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <functional>

#include "PlayerPortalServer.h"
#include "utils/ThreadedLogFile.h"

//Eventually, we should store all globals in this class.
class Game
{
protected:
	PlayerPortalServer *playerPortalServer;
	std::string name;
	std::string version;
	std::string defaultDirectory;
	std::string playerLogsDirectory;
	std::string subversionRepositoryUrl;
	std::map<std::string, std::string> basicConfig; //The configuration loaded from the BasicConfig file on the server.
	int bootSubversionRevision; //The revision of the local filesystem at the time of boot.
	int bootScriptsDirectorySubversionRevision;//The revision of the 'kmud-live/scripts' directory at the time of boot.

	ThreadedLogFile lagLogFile;
public:

	Game();
	~Game();

	void setupPlayerPortalServer(const int port);
	void processPlayerPortalServer();
	void cleanupPlayerPortalServer();

	std::string getName() const;
	void setName(const std::string &name);

	std::string getSubversionRepositoryUrl() const;
	void setSubversionRepositoryUrl(const std::string &subversionRepositoryUrl);

	int getBootSubversionRevision() const;
	int getBootScriptsDirectorySubversionRevision() const;

	void loadBasicConfig();
	std::string getBasicConfigValue(const std::string &configName);

	std::string getVersion();
	std::string getDefaultDirectory();
	std::string getPlayerLogsDirectory();

	void loadSubversionInfo();

	bool hasBasicConfiguration(const std::string &basicConfigurationName);
	void setupFilesystem();
	bool playerPortalServerIsConnected();
	int getPlayerPortalPort();
	unsigned int getNumberOfPlayerPortalDescriptors();
	void sendToAll(std::function<std::string(class Character *target)>);

	void setupThreadedLogFiles();

	void logLag(const std::string &message);
};

extern class Game *game;

#endif
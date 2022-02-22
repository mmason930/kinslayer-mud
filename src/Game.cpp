#include <boost/filesystem.hpp>

#include "Game.h"


extern Character *character_list;

Game *game = NULL;

Game::Game()
{
	this->version = "KinslayerMUD, version 2.0";
	this->playerLogsDirectory = "plrlogs/";
	this->defaultDirectory = "lib";
}

Game::~Game()
{
	cleanupPlayerPortalServer();
}

void Game::setupPlayerPortalServer(const int port)
{
	Log("Setting up Player Portal Server...");
	this->playerPortalServer = new PlayerPortalServer(new kuListener(port, TCP));
}

void Game::cleanupPlayerPortalServer()
{
	delete playerPortalServer;
	playerPortalServer = NULL;
}


void Game::processPlayerPortalServer()
{
	playerPortalServer->process();
}

std::string Game::getName() const
{
	return this->name;
}

void Game::setName(const std::string &name)
{
	this->name = name;
}

bool Game::monitorRepo()
{
	return hasBasicConfiguration("Monitor Repository") && getBasicConfigValue("Monitor Repository") == "1";
}

std::string Game::getScriptPullCommand()
{
	return getBasicConfigValue("Script Pull Command");
}

void Game::loadBasicConfig()
{
	Log("Loading Basic Configuration...");
	basicConfig = MiscUtil::loadResourcesFromFile("misc/BasicConfig");
}

std::string Game::getBasicConfigValue(const std::string &configName) const
{
	return basicConfig.find(configName)->second;
}

bool Game::hasBasicConfiguration(const std::string &basicConfigurationName) const
{
	return basicConfig.find(basicConfigurationName) != basicConfig.end();
}

std::string Game::getVersion()
{
	return this->version;
}

std::string Game::getDefaultDirectory()
{
	return this->defaultDirectory;
}

std::string Game::getPlayerLogsDirectory()
{
	return this->playerLogsDirectory;
}

void Game::setupFilesystem()
{
	Log("Setting up filesystem...");
	bool sqlFileExists = false;

	if (!boost::filesystem::exists("js_files")) {

		Log("Creating lib/js_files...");
		boost::filesystem::create_directory("js_files");
	}

	if (!boost::filesystem::exists("misc")) {

		Log("Creating lib/misc...");

		boost::filesystem::create_directory("misc");
	}

	if (!boost::filesystem::exists(getPlayerLogsDirectory())) {

		Log("Creating lib/plrlogs...");
		boost::filesystem::create_directory("plrlogs");
	}

	if (!boost::filesystem::exists("misc/BasicConfig")) {

		Log("lib/misc/BasicConfig not found. This file is required. Please create it and fill it with the required configuration.");
		exit(1);
	}
}

bool Game::playerPortalServerIsConnected()
{
	return playerPortalServer->isConnected();
}

int Game::getPlayerPortalPort()
{
	return this->playerPortalServer->getPort();
}

unsigned int Game::getNumberOfPlayerPortalDescriptors()
{
	return playerPortalServer->numberOfDescriptors();
}

void Game::sendToAll(std::function<std::string(Character *target)> messageFunction)
{
	for(Character *target = character_list;target;target = target->next)
	{
		if(target->desc)
		{
			target->send(messageFunction(target).c_str());
		}
	}
}

Character *Game::getSignedInCharacterByUserId(int userId) const
{
	for(Character *target = character_list;target;target = target->next)
	{
		if(target->player.idnum == userId)
			return target;
	}

	return nullptr;
}

GuildEditorInterface *Game::getGuildEditorInterface() const
{
	return this->guildEditorInterface.get();
}


sql::Connection Game::getConnection() const
{
	return connection;
}

sql::Context Game::getContext() const
{
	return context;
}

void Game::setConnection(sql::Connection connection)
{
	this->connection = connection;
}

void Game::setContext(sql::Context contect)
{
	this->context = context;
}

void Game::setupEditorInterfaces()
{
	this->guildEditorInterface.reset(new GuildEditorInterface());
}

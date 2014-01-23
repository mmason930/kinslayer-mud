#include <boost/filesystem.hpp>

#include "Game.h"
#include "MiscUtil.h"
#include "utils.h"

Game *game = NULL;

Game::Game()
{
	this->version = "KinslayerMUD, version 0001";
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

std::string Game::getName()
{
	return this->name;
}

void Game::setName(const std::string &name)
{
	this->name = name;

}

void Game::loadBasicConfig()
{
	Log("Loading Basic Configuration...");
	basicConfig = MiscUtil::loadResourcesFromFile("misc/BasicConfig");
}

std::string Game::getBasicConfigValue(const std::string &configName)
{
	return basicConfig[configName];
}

bool Game::hasBasicConfiguration(const std::string &basicConfigurationName)
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

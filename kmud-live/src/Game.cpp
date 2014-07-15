#include <boost/filesystem.hpp>

#include "Game.h"
#include "MiscUtil.h"
#include "utils.h"
#include "structs.h"

#include "SystemUtil.h"

extern Character *character_list;

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

void Game::loadSubversionInfo()
{
	std::map<std::string, std::string> subversionInfoMap = SystemUtil::getSubversionInfoMap("../");

	auto subversionInfoMapIterator = subversionInfoMap.find("URL");

	if(subversionInfoMapIterator == subversionInfoMap.end())
	{
		Log("Could not determine subversion URL. Aborting.");
		exit(1);
	}

	this->setSubversionRepositoryUrl((*subversionInfoMapIterator).second);
	
	subversionInfoMapIterator = subversionInfoMap.find("Revision");

	if(subversionInfoMapIterator == subversionInfoMap.end())
	{
		Log("Could not determine subversion revision. Aborting.");
		exit(1);
	}

	if(!MiscUtil::isInt((*subversionInfoMapIterator).second))
	{
		Log("Revision `%s` is not a valid integer. Aborting.");
		exit(1);
	}

	this->bootSubversionRevision = atoi((*subversionInfoMapIterator).second.c_str());

	subversionInfoMap = SystemUtil::getSubversionInfoMap("./scripts/");

	subversionInfoMapIterator = subversionInfoMap.find("Revision");

	if(subversionInfoMapIterator == subversionInfoMap.end())
	{
		Log("Could not determine subversion revision for script directory. Aborting.");
		exit(1);
	}

	if(!MiscUtil::isInt((*subversionInfoMapIterator).second))
	{
		Log("Revision `%s` is not a valid integer. Aborting.");
		exit(1);
	}

	this->bootScriptsDirectorySubversionRevision = atoi( (*subversionInfoMapIterator).second.c_str() );

	Log("Subversion Revision For Script Directory: %d", bootScriptsDirectorySubversionRevision);
	Log("Subversion URL: %s", getSubversionRepositoryUrl().c_str());
	Log("Subversion Revision: %d", getBootSubversionRevision());
}

int Game::getBootSubversionRevision() const
{
	return bootSubversionRevision;
}

int Game::getBootScriptsDirectorySubversionRevision() const
{
	return bootScriptsDirectorySubversionRevision;
}

std::string Game::getName() const
{
	return this->name;
}

void Game::setName(const std::string &name)
{
	this->name = name;
}

std::string Game::getSubversionRepositoryUrl() const
{
	return subversionRepositoryUrl;
}

void Game::setSubversionRepositoryUrl(const std::string &subversionRepositoryUrl)
{
	this->subversionRepositoryUrl = subversionRepositoryUrl;
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
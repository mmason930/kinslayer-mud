#include "PlayerPortalServer.h"
#include "../utils.h"

void onPlayerPortalServerSocketClose(void *data, kuListener *listener, kuDescriptor *descriptor);

PlayerPortalServer::PlayerPortalServer(kuListener *listener)
{
	this->listener = listener;

	this->listener->setDataForCloseDescriptorCallback(this);
	this->listener->setCloseDescriptorCallback(onPlayerPortalServerSocketClose);

	commandProcessors["Load Help File"] = new PlayerPortalLoadHelpFileCommandProcessor();
	commandProcessors["Save Help File"] = new PlayerPortalSaveHelpFileCommandProcessor();
	commandProcessors["Load Item Flags"] = new PlayerPortalLoadItemFlagsCommandProcess();
	commandProcessors["Load Object List"] = new PlayerPortalLoadObjectListCommandProcess();
}

PlayerPortalServer::~PlayerPortalServer()
{
	for (PlayerPortalDescriptor *descriptor : descriptors)
	{
		delete descriptor;
	}

	for (auto commandProcessorPair : commandProcessors)
	{
		delete commandProcessorPair.second;
	}

	listener->close();
	delete listener;
}

int PlayerPortalServer::getPort()
{
	return this->listener->getPort();
}

void PlayerPortalServer::process()
{
	std::list<kuDescriptor *> newDescriptors = this->listener->acceptNewHosts();

	for(kuDescriptor *descriptor : newDescriptors)
	{
		PlayerPortalDescriptor *playerPortalDescriptor = new PlayerPortalDescriptor(descriptor, this);
		this->descriptors.push_back(playerPortalDescriptor);
		this->kuDescriptorUidToPlayerPortalDescriptorMap[descriptor->getUid()] = playerPortalDescriptor;
	}

	this->listener->pulse();

	for(PlayerPortalDescriptor *playerPortalDescriptor : descriptors)
	{
		playerPortalDescriptor->processInput();
	}
}

void PlayerPortalServer::processCommand(PlayerPortalDescriptor *playerPortalDescriptor, const Json::Value &command)
{
	if (playerPortalDescriptor->getStatus() != PlayerPortalDescriptorStatus::connected)
	{
		Log("Player Portal: Player attempting to process command before fully signing in.");
		return;
	}

	if (command["method"].isNull() || command["method"].isString() == false)
	{
		Log("Player Portal: Player attempting to process command with no `method` set.");
		return;
	}

	auto commandProcessorIter = commandProcessors.find(command["method"].asString());

	if (commandProcessorIter == commandProcessors.end())
	{
		Log("Player Portal: Command not found: `%s`", command["method"].asString().c_str());
		return;
	}

	try
	{
		(*commandProcessorIter).second->process(playerPortalDescriptor, command);
	}
	catch (std::exception &e)
	{
		MudLog(NRM, LVL_APPR, TRUE, "Could not process web socket command. Command: '%s', Error: %s", command["method"].asString().c_str(), e.what());
	}
}

void PlayerPortalServer::closeDescriptor(PlayerPortalDescriptor *playerPortalDescriptor)
{
	playerPortalDescriptor->close();

	this->descriptors.remove(playerPortalDescriptor);
	this->kuDescriptorUidToPlayerPortalDescriptorMap.erase(playerPortalDescriptor->getUid());
}

void PlayerPortalServer::onDescriptorClose(kuDescriptor *descriptor)
{
	PlayerPortalDescriptor *playerPortalDescriptor = this->kuDescriptorUidToPlayerPortalDescriptorMap[descriptor->getUid()];

	this->closeDescriptor(playerPortalDescriptor);
}

void onPlayerPortalServerSocketClose(void *data, kuListener *listener, kuDescriptor *descriptor)
{
	PlayerPortalServer *playerPortalServer = (PlayerPortalServer*)data;

	playerPortalServer->onDescriptorClose(descriptor);
}

bool PlayerPortalServer::isConnected()
{
	return this->listener->isListening();
}

unsigned int PlayerPortalServer::numberOfDescriptors()
{
	return descriptors.size();
}

#ifndef PLAYER_PORTAL_SERVER_H
#define PLAYER_PORTAL_SERVER_H

#include <list>
#include <map>

#include "PlayerPortalDescriptor.h"
#include "kuSockets.h"
#include "kuListener.h"
#include "kuClient.h"
#include "PlayerPortalCommandProcessor.h"
#include "jsoncpp/json.h"

class PlayerPortalDescriptor;
class PlayerPortalCommandProcessor;

class PlayerPortalServer
{
protected:
	kuListener *listener;
	std::list<PlayerPortalDescriptor *> descriptors;
	std::map<int, PlayerPortalDescriptor *> kuDescriptorUidToPlayerPortalDescriptorMap;
	void closeDescriptor(PlayerPortalDescriptor *playerPortalDescriptor);

	std::map<std::string, PlayerPortalCommandProcessor *> commandProcessors;

public:

	PlayerPortalServer(kuListener *listener);
	~PlayerPortalServer();

	void process();
	void processCommand(PlayerPortalDescriptor *playerPortalDescriptor, const Json::Value &command);
	void onDescriptorClose(kuDescriptor *descriptor);
	int getPort();
	bool isConnected();
	unsigned int numberOfDescriptors();
};

#endif
#ifndef PLAYER_PORTAL_DESCRIPTOR_H
#define PLAYER_PORTAL_DESCRIPTOR_H

#include "ku/kuSockets.h"
#include "ku/kuDescriptor.h"
#include "PlayerPortalServer.h"
#include "PlayerPortalDescriptorStatus.h"
#include "jsoncpp/json.h"

class PlayerPortalServer;
class PlayerPortalDescriptorStatus;

class PlayerPortalDescriptor
{
protected:
	kuDescriptor *descriptor;
	PlayerPortalServer *server;
	PlayerPortalDescriptorStatus *status;
	int userId;
	int uid;
	std::string inputBuffer;
public:

	PlayerPortalDescriptor(kuDescriptor *descriptor, PlayerPortalServer *server);
	~PlayerPortalDescriptor();

	PlayerPortalServer *getServer();
	PlayerPortalDescriptorStatus *getStatus();
	void setStatus(PlayerPortalDescriptorStatus *status);
	int getUserId();
	void setUserId(const int userId);
	int getUid();
	void close();
	void processInput();
	void processCommand(const Json::Value &command);
	void send(const std::string &output);
};

#endif
#ifndef PLAYER_PORTAL_COMMAND_PROCESSOR_H
#define PLAYER_PORTAL_COMMAND_PROCESSOR_H

#include <string>
#include "PlayerPortalDescriptor.h"
#include "jsoncpp/json.h"

class PlayerPortalDescriptor;

class PlayerPortalCommandProcessor
{
private:
public:

	PlayerPortalCommandProcessor();
	~PlayerPortalCommandProcessor();

	virtual void process(PlayerPortalDescriptor *descriptor, Json::Value command) = 0;
};

class PlayerPortalJavaScriptCommandProcessor : public PlayerPortalCommandProcessor
{
public:
	PlayerPortalJavaScriptCommandProcessor();
	~PlayerPortalJavaScriptCommandProcessor();
	void process(PlayerPortalDescriptor *descriptor, Json::Value command);
};

class PlayerPortalLoadHelpFileCommandProcessor : public PlayerPortalJavaScriptCommandProcessor {};
class PlayerPortalSaveHelpFileCommandProcessor : public PlayerPortalJavaScriptCommandProcessor {};

#endif
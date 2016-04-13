#ifndef PLAYER_PORTAL_DESCRIPTOR_STATUS_H
#define PLAYER_PORTAL_DESCRIPTOR_STATUS_H

#include <list>
#include "Enum.h"

class PlayerPortalDescriptorStatus : public Enum<PlayerPortalDescriptorStatus> {

private:
	PlayerPortalDescriptorStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static PlayerPortalDescriptorStatus *handshaking;
	static PlayerPortalDescriptorStatus *authenticating;
	static PlayerPortalDescriptorStatus *connected;
};


#endif
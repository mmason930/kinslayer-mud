#ifndef USER_LOGOUT_TYPE_H
#define USER_LOGOUT_TYPE_H

#include <list>
#include "Enum.h"

class PlayerPortalDescriptorStatus : public Enum {

private:

	static std::list<PlayerPortalDescriptorStatus*> enums;

	PlayerPortalDescriptorStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
		enums.push_back(this);
	}

public:

	static PlayerPortalDescriptorStatus *handshaking;
	static PlayerPortalDescriptorStatus *authenticating;
	static PlayerPortalDescriptorStatus *connected;
};


#endif
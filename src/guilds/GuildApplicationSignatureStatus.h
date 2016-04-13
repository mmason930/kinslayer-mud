#ifndef GUILD_APPLICATION_SIGNATURE_STATUS_H
#define GUILD_APPLICATION_SIGNATURE_STATUS_H

#include "../Enum.h"

class GuildApplicationSignatureStatus : public Enum<GuildApplicationSignatureStatus> {
	
private:
	GuildApplicationSignatureStatus(int value, const std::string &standardName, bool canBeRemovedBySubmitter, bool canBeApproved, bool canBeDenied) : Enum(value, standardName)
	{
		this->canBeRemovedBySubmitter = canBeRemovedBySubmitter;
		this->canBeApproved = canBeApproved;
		this->canBeDenied = canBeDenied;
	}
	
	bool canBeRemovedBySubmitter;
	bool canBeApproved;
	bool canBeDenied;
public:

	bool getCanBeRemovedBySubmitter() { return canBeRemovedBySubmitter; }
	bool getCanBeApproved() { return canBeApproved; }
	bool getCanBeDenied() { return canBeDenied; }

	static GuildApplicationSignatureStatus *pending;
	static GuildApplicationSignatureStatus *approved;
	static GuildApplicationSignatureStatus *denied;
	static GuildApplicationSignatureStatus *removed;
};

#endif
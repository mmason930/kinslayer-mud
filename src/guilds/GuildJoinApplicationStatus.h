#ifndef GUILD_JOIN_APPLICATION_STATUS_H
#define GUILD_JOIN_APPLICATION_STATUS_H

#include "../Enum.h"

#include <vector>
#include <algorithm>

class GuildJoinApplicationStatus : public Enum<GuildJoinApplicationStatus> {

private:
	GuildJoinApplicationStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static GuildJoinApplicationStatus *reviewing;
	static GuildJoinApplicationStatus *approved;
	static GuildJoinApplicationStatus *denied;
	static GuildJoinApplicationStatus *removed;

	static std::vector<GuildJoinApplicationStatus*> getStatusesSubmitterCanRemove()
	{
		return {reviewing};
	}

	bool submitterCanRemove()
	{
		auto validStatuses = GuildJoinApplicationStatus::getStatusesSubmitterCanRemove();
		return std::find(validStatuses.begin(), validStatuses.end(), this) != validStatuses.end();
	}
};

#endif

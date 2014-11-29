#ifndef GUILD_APPLICATION_STATUS_H
#define GUILD_APPLICATION_STATUS_H

#include "../Enum.h"

class GuildApplicationStatus : public Enum<GuildApplicationStatus> {

private:
	GuildApplicationStatus(int value, const std::string &standardName, bool requiresReview) : Enum(value, standardName)
	{
		this->requiresReview = requiresReview;
	}

	bool requiresReview;
public:

	bool getRequiresReview() const
	{
		return requiresReview;
	}

	static GuildApplicationStatus *pending;
	static GuildApplicationStatus *reviewing;
	static GuildApplicationStatus *approved;
	static GuildApplicationStatus *denied;
};

#endif

#ifndef GUILD_APPLICATION_STATUS_H
#define GUILD_APPLICATION_STATUS_H

#include "../Enum.h"

class GuildApplicationStatus : public Enum<GuildApplicationStatus> {

private:
	GuildApplicationStatus(int value, const std::string &standardName, bool requiresReview, bool applicationOver) : Enum(value, standardName)
	{
		this->requiresReview = requiresReview;
		this->applicationClosed = applicationClosed;
	}

	bool requiresReview;
	bool applicationClosed;
public:

	bool getRequiresReview() const
	{
		return requiresReview;
	}

	bool getApplicationClosed() const
	{
		return applicationClosed;
	}

	static GuildApplicationStatus *pending;
	static GuildApplicationStatus *reviewing;
	static GuildApplicationStatus *approved;
	static GuildApplicationStatus *denied;
};

#endif

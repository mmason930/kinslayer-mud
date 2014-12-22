#ifndef GUILD_APPLICATION_STATUS_H
#define GUILD_APPLICATION_STATUS_H

#include "../Enum.h"

#include <vector>

class GuildApplicationStatus : public Enum<GuildApplicationStatus> {

private:
	GuildApplicationStatus(int value, const std::string &standardName, bool requiresReview, bool applicationClosed) : Enum(value, standardName)
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

	static std::vector<GuildApplicationStatus*> allExceptDenied()
	{
		std::vector<GuildApplicationStatus*> statuses = {};

		for(auto iter = getStartIterator();iter != getEndIterator();++iter)
		{
			if(*iter != denied)
				statuses.push_back(*iter);
		}

		return statuses;
	}

	static GuildApplicationStatus *pending;
	static GuildApplicationStatus *reviewing;
	static GuildApplicationStatus *approved;
	static GuildApplicationStatus *denied;
};

#endif

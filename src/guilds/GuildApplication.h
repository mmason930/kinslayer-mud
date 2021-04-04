#ifndef GUILD_APPLICATION_H
#define GUILD_APPLICATION_H

#include <string>
#include <optional>

#include "../DateTime.h"
#include "../DataObjectWithIntId.h"
#include "GuildApplicationStatus.h"

class GuildApplication : public DataObjectWithIntId
{
private:
protected:
	std::string guildName;
	std::string guildDescription;
	DateTime submittedDatetime;
	int submittedByUserId;
	int submittedByUserRace;
	GuildApplicationStatus *status;
	int coppersCharged;
	std::optional<DateTime> completedDatetime;
	std::optional<int> reviewerUserId;
	std::optional<std::string> deniedReason;
	std::optional<int> guildId;
public:

	GuildApplication() : DataObjectWithIntId() {};
	~GuildApplication() {};

	std::string getGuildName() const;
	void setGuildName(const std::string &guildName);

	std::string getGuildDescription() const;
	void setGuildDescription(const std::string &guildDescription);

	DateTime getSubmittedDatetime() const;
	void setSubmittedDatetime(const DateTime &submittedDatetime);

	int getSubmittedByUserId() const;
	void setSubmittedByUserId(const int submittedByUserId);

	int getSubmittedByUserRace() const;
	void setSubmittedByUserRace(int submittedByUserRace);

	GuildApplicationStatus *getStatus() const;
	void setStatus(GuildApplicationStatus *status);

	int getCoppersCharged() const;
	void setCoppersCharged(const int coppersCharged);

	std::optional<DateTime> getCompletedDatetime() const;
	void setCompletedDatetime(const std::optional<DateTime> &completedDatetime);

	std::optional<int> getReviewerUserId() const;
	void setReviewerUserId(const std::optional<int> reviewerUserId);

	std::optional<std::string> getDeniedReason() const;
	void setDeniedReason(const std::optional<std::string> deniedReason);

	std::optional<int> getGuildId() const;
	void setGuildId(const std::optional<int> &guildId);
};

#endif
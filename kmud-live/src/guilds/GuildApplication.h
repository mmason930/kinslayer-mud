#ifndef GUILD_APPLICATION_H
#define GUILD_APPLICATION_H

#include <string>
#include <boost/optional.hpp>

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
	GuildApplicationStatus *status;
	int coppersCharged;
	boost::optional<DateTime> completedDatetime;
	boost::optional<int> reviewerUserId;
	boost::optional<std::string> deniedReason;
	boost::optional<int> guildId;
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

	GuildApplicationStatus *getStatus() const;
	void setStatus(GuildApplicationStatus *status);

	int getCoppersCharged() const;
	void setCoppersCharged(const int coppersCharged);

	boost::optional<DateTime> getCompletedDatetime() const;
	void setCompletedDatetime(const boost::optional<DateTime> &completedDatetime);
	
	boost::optional<int> getReviewerUserId() const;
	void setReviewerUserId(const boost::optional<int> reviewerUserId);

	boost::optional<std::string> getDeniedReason() const;
	void setDeniedReason(const boost::optional<std::string> deniedReason);

	boost::optional<int> getGuildId() const;
	void setGuildId(const boost::optional<int> &guildId);
};

#endif
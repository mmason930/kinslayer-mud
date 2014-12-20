#ifndef GUILD_JOIN_APPLICATION_H
#define GUILD_JOIN_APPLICATION_H

#include <boost/optional.hpp>

#include "../DataObjectWithIntId.h"
#include "../DateTime.h"

class GuildJoinApplicationStatus;

class GuildJoinApplication : public DataObjectWithIntId
{
private:
protected:

	int userId;
	int guildId;
	DateTime createdDatetime;
	GuildJoinApplicationStatus *status;
	DateTime statusLastModifiedDatetime;
	int statusLastModifiedByUserId;
	std::string messageToGuild;
	boost::optional<std::string> messageFromGuild;
public:

	int getUserId() const;
	void setUserId(int userId);

	int getGuildId() const;
	void setGuildId(int guildId);

	DateTime getCreatedDatetime() const;
	void setCreatedDatetime(const DateTime &dateTime);

	GuildJoinApplicationStatus *getStatus() const;
	void setStatus(GuildJoinApplicationStatus *status);

	DateTime getStatusLastModifiedDatetime() const;
	void setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime);

	int getStatusLastModifiedByUserId() const;
	void setStatusLastModifiedByUserId(int statusLastModifiedByUserId);

	std::string getMessageToGuild() const;
	void setMessageToGuild(const std::string &messageToGuild);

	boost::optional<std::string> getMessageFromGuild() const;
	void setMessageFromGuild(const boost::optional<std::string> &messageFromGuild);
};

#endif

#ifndef USER_GUILD_H
#define USER_GUILD_H

#include "../DataObjectWithIntId.h"
#include "../DateTime.h"

#include "UserGuildStatus.h"

class UserGuild : public DataObjectWithIntId
{
private:
protected:
	int userId;
	int guildId;
	DateTime joinedDatetime;
	UserGuildStatus *status;
	DateTime statusLastModifiedDatetime;
	boost::optional<int> guildRankId;
public:

	UserGuild() : DataObjectWithIntId() {};
	~UserGuild() {}

	int getUserId() const;
	void setUserId(int userId);

	int getGuildId() const;
	void setGuildId(int guildId);

	DateTime getJoinedDatetime() const;
	void setJoinedDatetime(const DateTime &joinedDatetime);

	UserGuildStatus *getStatus() const;
	void setStatus(UserGuildStatus *status);

	DateTime getStatusLastModifiedDatetime() const;
	void setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime);

	boost::optional<int> getGuildRankId() const;
	void setGuildRankId(const boost::optional<int> &guildRankId);
};

#endif
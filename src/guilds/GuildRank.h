#ifndef GUILD_RANK_H
#define GUILD_RANK_H

#include <string>

#include "../DataObjectWithIntId.h"
#include "../DateTime.h"
#include "GuildRankStatus.h"

class GuildRank : public DataObjectWithIntId
{
private:
protected:
	int guildId;
	std::string name;
	DateTime createdDatetime;
	GuildRankStatus *status;
	DateTime statusLastModifiedDatetime;
	int statusLastModifiedByUserId;
public:

	int getGuildId() const;
	void setGuildId(int guildId);

	std::string getName() const;
	void setName(const std::string &name);

	DateTime getCreatedDatetime() const;
	void setCreatedDatetime(const DateTime &createdDatetime);

	GuildRankStatus *getStatus() const;
	void setStatus(GuildRankStatus *status);

	DateTime getStatusLastModifiedDatetime() const;
	void setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime);

	int getStatusLastModifiedByUserId() const;
	void setStatusLastModifiedByUserId(int statusLastModifiedByUserId);

	GuildRank& operator = (const GuildRank *guildRank);
};

#endif
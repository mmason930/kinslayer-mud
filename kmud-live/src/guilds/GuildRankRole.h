#ifndef GUILD_RANK_ROLE_H
#define GUILD_RANK_ROLE_H

#include "../DataObjectWithIntId.h"
#include "GuildPrivilege.h"

class GuildRankRole : public DataObjectWithIntId
{
private:
protected:
	int guildRankId;
	GuildPrivilege *guildPrivilege;
public:

	int getGuildRankId() const;
	void setGuildRankId(int guildRankId);

	GuildPrivilege *getGuildPrivilege() const;
	void setGuildPrivilege(GuildPrivilege *guildPrivilege);
};

#endif

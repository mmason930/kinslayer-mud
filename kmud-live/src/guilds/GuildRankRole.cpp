#include "../conf.h"
#include "../sysdep.h"

#include "GuildRankRole.h"

int GuildRankRole::getGuildRankId() const
{
	return guildRankId;
}

void GuildRankRole::setGuildRankId(int guildRankId)
{
	this->guildRankId = guildRankId;
}

GuildPrivilege *GuildRankRole::getGuildPrivilege() const
{
	return guildPrivilege;
}

void GuildRankRole::setGuildPrivilege(GuildPrivilege *guildPrivilege)
{
	this->guildPrivilege = guildPrivilege;
}
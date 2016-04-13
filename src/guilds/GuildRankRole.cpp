#include "../conf.h"
#include "../sysdep.h"

#include "GuildRankRole.h"

GuildRankRole::GuildRankRole()
{
	setId(-1);
}

GuildRankRole::GuildRankRole(const GuildRankRole &guildRankRole)
{
	this->setGuildPrivilege(guildRankRole.getGuildPrivilege());
	this->setGuildRankId(guildRankRole.getGuildRankId());
	this->setId(guildRankRole.getId());
}

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
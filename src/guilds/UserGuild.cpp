#include "../conf.h"
#include "../sysdep.h"

#include "UserGuild.h"


int UserGuild::getUserId() const
{
	return userId;
}

void UserGuild::setUserId(int userId)
{
	this->userId = userId;
}

int UserGuild::getGuildId() const
{
	return guildId;
}

void UserGuild::setGuildId(int guildId)
{
	this->guildId = guildId;
}

DateTime UserGuild::getJoinedDatetime() const
{
	return joinedDatetime;
}

void UserGuild::setJoinedDatetime(const DateTime &joinedDatetime)
{
	this->joinedDatetime = joinedDatetime;
}

UserGuildStatus *UserGuild::getStatus() const
{
	return status;
}

void UserGuild::setStatus(UserGuildStatus *status)
{
	this->status = status;
}

DateTime UserGuild::getStatusLastModifiedDatetime() const
{
	return statusLastModifiedDatetime;
}

void UserGuild::setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime)
{
	this->statusLastModifiedDatetime = statusLastModifiedDatetime;
}

boost::optional<int> UserGuild::getGuildRankId() const
{
	return guildRankId;
}

void UserGuild::setGuildRankId(const boost::optional<int> &guildRankId)
{
	this->guildRankId = guildRankId;
}

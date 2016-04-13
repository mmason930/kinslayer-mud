#include "../conf.h"
#include "../sysdep.h"

#include "GuildJoinApplication.h"
#include "GuildJoinApplicationStatus.h"

int GuildJoinApplication::getUserId() const
{
	return userId;
}

void GuildJoinApplication::setUserId(int userId)
{
	this->userId = userId;
}

int GuildJoinApplication::getGuildId() const
{
	return guildId;
}

void GuildJoinApplication::setGuildId(int guildId)
{
	this->guildId = guildId;
}

DateTime GuildJoinApplication::getCreatedDatetime() const
{
	return createdDatetime;
}

void GuildJoinApplication::setCreatedDatetime(const DateTime &dateTime)
{
	this->createdDatetime = createdDatetime;
}

GuildJoinApplicationStatus *GuildJoinApplication::getStatus() const
{
	return status;
}

void GuildJoinApplication::setStatus(GuildJoinApplicationStatus *status)
{
	this->status = status;
}

DateTime GuildJoinApplication::getStatusLastModifiedDatetime() const
{
	return statusLastModifiedDatetime;
}

void GuildJoinApplication::setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime)
{
	this->statusLastModifiedDatetime = statusLastModifiedDatetime;
}

int GuildJoinApplication::getStatusLastModifiedByUserId() const
{
	return statusLastModifiedByUserId;
}

void GuildJoinApplication::setStatusLastModifiedByUserId(int statusLastModifiedByUserId)
{
	this->statusLastModifiedByUserId = statusLastModifiedByUserId;
}

std::string GuildJoinApplication::getMessageToGuild() const
{
	return messageToGuild;
}

void GuildJoinApplication::setMessageToGuild(const std::string &messageToGuild)
{
	this->messageToGuild = messageToGuild;
}

boost::optional<std::string> GuildJoinApplication::getMessageFromGuild() const
{
	return messageFromGuild;
}

void GuildJoinApplication::setMessageFromGuild(const boost::optional<std::string> &messageFromGuild)
{
	this->messageFromGuild = messageFromGuild;
}

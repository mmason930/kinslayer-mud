
#include "GuildRank.h"

int GuildRank::getGuildId() const
{
	return guildId;
}

void GuildRank::setGuildId(int guildId)
{
	this->guildId = guildId;
}

std::string GuildRank::getName() const
{
	return name;
}

void GuildRank::setName(const std::string &name)
{
	this->name = name;
}

DateTime GuildRank::getCreatedDatetime() const
{
	return createdDatetime;
}

void GuildRank::setCreatedDatetime(const DateTime &createdDatetime)
{
	this->createdDatetime = createdDatetime;
}

GuildRankStatus *GuildRank::getStatus() const
{
	return status;
}

void GuildRank::setStatus(GuildRankStatus *status)
{
	this->status = status;
}

DateTime GuildRank::getStatusLastModifiedDatetime() const
{
	return statusLastModifiedDatetime;
}

void GuildRank::setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime)
{
	this->statusLastModifiedDatetime = statusLastModifiedDatetime;
}

int GuildRank::getStatusLastModifiedByUserId() const
{
	return statusLastModifiedByUserId;
}

void GuildRank::setStatusLastModifiedByUserId(int statusLastModifiedByUserId)
{
	this->statusLastModifiedByUserId = statusLastModifiedByUserId;
}

GuildRank& GuildRank::operator = (const GuildRank *guildRank)
{
	setCreatedDatetime(guildRank->getCreatedDatetime());
	setGuildId(guildRank->getGuildId());
	setId(guildRank->getId());
	setName(guildRank->getName());
	setStatus(guildRank->getStatus());
	setStatusLastModifiedDatetime(guildRank->getStatusLastModifiedDatetime());
	setStatusLastModifiedByUserId(guildRank->getStatusLastModifiedByUserId());

	return *this;
}

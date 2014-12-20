#include "GuildApplicationSignature.h"

int GuildApplicationSignature::getUserId() const
{
	return userId;
}

void GuildApplicationSignature::setUserId(int userId)
{
	this->userId = userId;
}

int GuildApplicationSignature::getGuildApplicationId() const
{
	return guildApplicationId;
}

void GuildApplicationSignature::setGuildApplicationId(int guildApplicationId)
{
	this->guildApplicationId = guildApplicationId;
}

GuildApplicationSignatureStatus *GuildApplicationSignature::getStatus() const
{
	return status;
}

void GuildApplicationSignature::setStatus(GuildApplicationSignatureStatus *status)
{
	this->status = status;
}

DateTime GuildApplicationSignature::getCreatedDatetime() const
{
	return createdDatetime;
}

void GuildApplicationSignature::setCreatedDatetime(const DateTime &createdDatetime)
{
	this->createdDatetime = createdDatetime;
}

DateTime GuildApplicationSignature::getStatusLastModifiedDatetime() const
{
	return statusLastModifiedDatetime;
}

void GuildApplicationSignature::setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime)
{
	this->statusLastModifiedDatetime = statusLastModifiedDatetime;
}

int GuildApplicationSignature::getStatusChangedByUserId() const
{
	return statusChangedByUserId;
}

void GuildApplicationSignature::setStatusChangedByUserId(int statusChangedByUserId)
{
	this->statusChangedByUserId = statusChangedByUserId;
}

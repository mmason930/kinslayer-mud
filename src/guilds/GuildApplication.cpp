#include "GuildApplication.h"

std::string GuildApplication::getGuildName() const
{
	return guildName;
}

void GuildApplication::setGuildName(const std::string &guildName)
{
	this->guildName = guildName;
}

std::string GuildApplication::getGuildDescription() const
{
	return guildDescription;
}

void GuildApplication::setGuildDescription(const std::string &guildDescription)
{
	this->guildDescription = guildDescription;
}

DateTime GuildApplication::getSubmittedDatetime() const
{
	return submittedDatetime;
}

void GuildApplication::setSubmittedDatetime(const DateTime &submittedDatetime)
{
	this->submittedDatetime = submittedDatetime;
}

int GuildApplication::getSubmittedByUserId() const
{
	return submittedByUserId;
}

void GuildApplication::setSubmittedByUserId(const int submittedByUserId)
{
	this->submittedByUserId = submittedByUserId;
}

int GuildApplication::getSubmittedByUserRace() const
{
	return submittedByUserRace;
}

void GuildApplication::setSubmittedByUserRace(int submittedByUserRace)
{
	this->submittedByUserRace = submittedByUserRace;
}

GuildApplicationStatus *GuildApplication::getStatus() const
{
	return status;
}

void GuildApplication::setStatus(GuildApplicationStatus *status)
{
	this->status = status;
}

int GuildApplication::getCoppersCharged() const
{
	return coppersCharged;
}

void GuildApplication::setCoppersCharged(const int coppersCharged)
{
	this->coppersCharged = coppersCharged;
}

std::optional<DateTime> GuildApplication::getCompletedDatetime() const
{
	return completedDatetime;
}

void GuildApplication::setCompletedDatetime(const std::optional<DateTime> &completedDatetime)
{
	this->completedDatetime = completedDatetime;
}

std::optional<int> GuildApplication::getReviewerUserId() const
{
	return reviewerUserId;
}

void GuildApplication::setReviewerUserId(const std::optional<int> reviewerUserId)
{
	this->reviewerUserId = reviewerUserId;
}

std::optional<std::string> GuildApplication::getDeniedReason() const
{
	return deniedReason;
}

void GuildApplication::setDeniedReason(const std::optional<std::string> deniedReason)
{
	this->deniedReason = deniedReason;
}

std::optional<int> GuildApplication::getGuildId() const
{
	return guildId;
}

void GuildApplication::setGuildId(const std::optional<int> &guildId)
{
	this->guildId = guildId;
}

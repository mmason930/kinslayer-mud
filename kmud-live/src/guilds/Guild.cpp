#include "Guild.h"

std::string Guild::getName() const {
	return name;
}

void Guild::setName(const std::string &name) {
	this->name = name;
}

DateTime Guild::getCreatedDatetime() const {
		return createdDatetime;
}

void Guild::setCreatedDatetime(const DateTime &createdDatetime) {
	this->createdDatetime = createdDatetime;
}

GuildStatus *Guild::getStatus() const {
	return status;
}

void Guild::setStatus(GuildStatus *status) {
	this->status = status;
}

DateTime Guild::getStatusLastModifiedDatetime() const {
	return statusLastModifiedDatetime;
}

void Guild::setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime) {
	this->statusLastModifiedDatetime = statusLastModifiedDatetime;
}

int Guild::getCreatedByUserId() const {
	return createdByUserId;
}

void Guild::setCreatedByUserId(const int createdByUserId) {
	this->createdByUserId = createdByUserId;
}

#include "../conf.h"

#include "Script.h"


Script::Script()
{
	id = -1;
}

Script::Script(const int id, const std::string &methodName)
{
	setId(id);
	setMethodName(methodName);
}

Script::~Script()
{}

std::string Script::getMethodName() const
{
	return methodName;
}

void Script::setMethodName(const std::string &methodName)
{
	this->methodName = methodName;
}

int Script::getCreatedByUserId() const
{
	return createdByUserId;
}

void Script::setCreatedByUserId(const int createdByUserId)
{
	this->createdByUserId = createdByUserId;
}

DateTime Script::getCreatedDatetime() const
{
	return createdDatetime;
}

void Script::setCreatedDatetime(const DateTime &createdDatetime)
{
	this->createdDatetime = createdDatetime;
}

int Script::getLastModifiedByUserId() const
{
	return lastModifiedByUserId;
}

void Script::setLastModifiedByUserId(const int lastModifiedByUserId)
{
	this->lastModifiedByUserId = lastModifiedByUserId;
}


DateTime Script::getLastModifiedDatetime() const
{
	return lastModifiedDatetime;
}

void Script::setLastModifiedDatetime(const DateTime &lastModifiedDatetime)
{
	this->lastModifiedDatetime = lastModifiedDatetime;
}

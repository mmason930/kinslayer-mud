#include "../conf.h"
#include "../sysdep.h"

#include "DotModeTarget.h"
#include "DotModeTargetType.h"

DotModeTarget::DotModeTarget()
{
	setType(nullptr);
}

DotModeTarget::DotModeTarget(DotModeTargetType *type, const boost::optional<int> &targetNumber)
{
	setType(type);
	setTargetNumber(targetNumber);
}

DotModeTarget::DotModeTarget(DotModeTargetType *type)
{
	setType(type);
}

DotModeTargetType *DotModeTarget::getType() const
{
	return type;
}

void DotModeTarget::setType(DotModeTargetType *type)
{
	this->type = type;
}

boost::optional<int> DotModeTarget::getTargetNumber() const
{
	return targetNumber;
}

void DotModeTarget::setTargetNumber(const boost::optional<int> &targetNumber)
{
	this->targetNumber = targetNumber;
}

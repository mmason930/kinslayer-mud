
#include "DotModeTarget.h"

DotModeTarget::DotModeTarget()
{
	setType(nullptr);
}

DotModeTarget::DotModeTarget(DotModeTargetType *type, const std::optional<int> &targetNumber)
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

std::optional<int> DotModeTarget::getTargetNumber() const
{
	return targetNumber;
}

void DotModeTarget::setTargetNumber(const std::optional<int> &targetNumber)
{
	this->targetNumber = targetNumber;
}

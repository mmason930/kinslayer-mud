#include "../conf.h"
#include "../sysdep.h"

#include "DotMode.h"
#include "DotModeTarget.h"
#include "DotModeArea.h"

DotMode::DotMode(const std::optional<DotModeTarget> &target, DotModeArea *area, const std::optional<std::string> &targetName)
{
	setTarget(target);
	setArea(area);
	setTargetName(targetName);
}

DotMode::DotMode()
{
	setArea(nullptr);
}

std::optional<DotModeTarget> DotMode::getTarget() const
{
	return target;
}

void DotMode::setTarget(const std::optional<DotModeTarget> &target)
{
	this->target = target;
}

DotModeArea *DotMode::getArea() const
{
	return area;
}

void DotMode::setArea(DotModeArea *area)
{
	this->area = area;
}

std::optional<std::string> DotMode::getTargetName() const
{
	return targetName;
}

void DotMode::setTargetName(const std::optional<std::string> &targetName)
{
	this->targetName = targetName;
}

DotModeTargetType *DotMode::getTargetType()
{
	return target ? target.value().getType() : nullptr;
}

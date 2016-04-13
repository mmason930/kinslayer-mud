#include "../conf.h"
#include "../sysdep.h"

#include "DotMode.h"
#include "DotModeTarget.h"
#include "DotModeArea.h"

DotMode::DotMode(const boost::optional<DotModeTarget> &target, DotModeArea *area, const boost::optional<std::string> &targetName)
{
	setTarget(target);
	setArea(area);
	setTargetName(targetName);
}

DotMode::DotMode()
{
	setArea(nullptr);
}

boost::optional<DotModeTarget> DotMode::getTarget() const
{
	return target;
}

void DotMode::setTarget(const boost::optional<DotModeTarget> &target)
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

boost::optional<std::string> DotMode::getTargetName() const
{
	return targetName;
}

void DotMode::setTargetName(const boost::optional<std::string> &targetName)
{
	this->targetName = targetName;
}

DotModeTargetType *DotMode::getTargetType()
{
	return target ? target.get().getType() : nullptr;
}

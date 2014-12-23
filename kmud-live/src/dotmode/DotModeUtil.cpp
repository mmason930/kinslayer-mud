#include "../conf.h"
#include "../sysdep.h"

#include "../MiscUtil.h"
#include "../StringUtil.h"
#include "../utils.h"

#include "DotModeUtil.h"
#include "DotModeTarget.h"
#include "DotModeArea.h"


DotModeUtil *DotModeUtil::self = nullptr;

DotModeUtil::DotModeUtil()
{
}

DotModeUtil::~DotModeUtil()
{
}

DotModeUtil *DotModeUtil::get()
{
	return self == nullptr ? (self = new DotModeUtil()) : self;
}

void DotModeUtil::destroy()
{
	DotModeUtil *selfPointer = self;

	self = nullptr;

	delete selfPointer;
}

boost::optional<DotModeTarget> DotModeUtil::parseDotModeTarget(const std::string &argument) const
{
	int targetNumber;
	if(!str_cmp(argument, "all"))
		return DotModeTarget(DotModeTargetType::all);
	else if(MiscUtil::isInt(argument) && (targetNumber = MiscUtil::convert<int>(argument)) < 0)
			return DotModeTarget(DotModeTargetType::specific, targetNumber);

	return boost::optional<DotModeTarget>();
}

DotModeArea *DotModeUtil::parseDotModeArea(const std::string argument) const
{
	return DotModeArea::getEnumByAbbreviation(argument);
}

boost::optional<DotMode> DotModeUtil::parseDotMode(const std::string &argument) const
{
	std::vector<std::string> components = StringUtil::splitToVector(argument, '.');
	
	DotModeArea *dotModeArea = nullptr;
	boost::optional<DotModeTarget> dotModeTarget;

	if(components.empty())
		return boost::optional<DotMode>();

	auto numberOfComponents = components.size();

	if(numberOfComponents > 3)
		return boost::optional<DotMode>();

	if(numberOfComponents == 1)
		return DotMode(boost::optional<DotModeTarget>(), nullptr, components[0]);
	
	else if(numberOfComponents == 2)
	{
		//For this scenario, the first component can be either target or area.
		//We have to do some guessing work here. 

		if(!(dotModeTarget = parseDotModeTarget(components[0])) && !(dotModeArea = parseDotModeArea(components[0])))
				return boost::optional<DotMode>();
	}
	else if(numberOfComponents == 3)
	{
		dotModeArea = parseDotModeArea(components[0]);
		dotModeTarget = parseDotModeTarget(components[0]);
	}

	return DotMode(dotModeTarget, dotModeArea, components[0]);
}

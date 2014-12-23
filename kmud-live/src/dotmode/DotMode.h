#ifndef DOT_MODE_H
#define DOT_MODE_H

#include <string>
#include <vector>
#include <boost/optional.hpp>

#include "DotModeTarget.h"
#include "DotModeArea.h"

class DotMode
{
private:
protected:
	
	boost::optional<DotModeTarget> target;
	DotModeArea *area;
	boost::optional<std::string> targetName;
public:

	DotMode(const boost::optional<DotModeTarget> &target, DotModeArea *area, const boost::optional<std::string> &targetName);
	DotMode();

	boost::optional<DotModeTarget> getTarget() const;
	void setTarget(const boost::optional<DotModeTarget> &target);

	DotModeArea *getArea() const;
	void setArea(DotModeArea *area);

	boost::optional<std::string> getTargetName() const;
	void setTargetName(const boost::optional<std::string> &targetName);

	DotModeTargetType *getTargetType();
};

#endif

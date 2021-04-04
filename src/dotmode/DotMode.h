#ifndef DOT_MODE_H
#define DOT_MODE_H

#include <string>
#include <vector>
#include <optional>

#include "DotModeTarget.h"
#include "DotModeArea.h"

class DotMode
{
private:
protected:

	std::optional<DotModeTarget> target;
	DotModeArea *area;
	std::optional<std::string> targetName;
public:

	DotMode(const std::optional<DotModeTarget> &target, DotModeArea *area, const std::optional<std::string> &targetName);
	DotMode();

	std::optional<DotModeTarget> getTarget() const;
	void setTarget(const std::optional<DotModeTarget> &target);

	DotModeArea *getArea() const;
	void setArea(DotModeArea *area);

	std::optional<std::string> getTargetName() const;
	void setTargetName(const std::optional<std::string> &targetName);

	DotModeTargetType *getTargetType();
};

#endif

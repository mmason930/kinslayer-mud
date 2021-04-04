#ifndef DOT_MODE_TARGET_H
#define DOT_MODE_TARGET_H

#include <optional>
#include "DotModeTargetType.h"

class DotModeTarget
{
private:
protected:
	DotModeTargetType *type;
	std::optional<int> targetNumber;
public:

	DotModeTarget();
	DotModeTarget(DotModeTargetType *type, const std::optional<int> &targetNumber);
	DotModeTarget(DotModeTargetType *type);

	DotModeTargetType *getType() const;
	void setType(DotModeTargetType *type);

	std::optional<int> getTargetNumber() const;
	void setTargetNumber(const std::optional<int> &targetNumber);
};

#endif

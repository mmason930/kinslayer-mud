#ifndef DOT_MODE_TARGET_H
#define DOT_MODE_TARGET_H

#include <boost/optional.hpp>
#include "DotModeTargetType.h"

class DotModeTarget
{
private:
protected:
	DotModeTargetType *type;
	boost::optional<int> targetNumber;
public:

	DotModeTarget();
	DotModeTarget(DotModeTargetType *type, const boost::optional<int> &targetNumber);
	DotModeTarget(DotModeTargetType *type);

	DotModeTargetType *getType() const;
	void setType(DotModeTargetType *type);

	boost::optional<int> getTargetNumber() const;
	void setTargetNumber(const boost::optional<int> &targetNumber);
};

#endif

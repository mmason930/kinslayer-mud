#ifndef DOT_MODE_TARGET_TYPE_H
#define DOT_MODE_TARGET_TYPE_H

#include "../Enum.h"

class DotModeTargetType : public Enum<DotModeTargetType> {

private:
	DotModeTargetType(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:
	
	static DotModeTargetType *specific;
	static DotModeTargetType *all;
};

#endif

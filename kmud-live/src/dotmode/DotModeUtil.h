#ifndef DOT_MODE_UTIL_H
#define DOT_MODE_UTIL_H

#include <boost/optional.hpp>

#include "DotMode.h"
#include "DotModeArea.h"
#include "DotModeTarget.h"
#include "DotModeTargetType.h"

class DotModeUtil
{
private:
	static DotModeUtil *self;
	
	DotModeUtil();
	~DotModeUtil();
protected:
public:

	boost::optional<DotMode> parseDotMode(const std::string &argument) const;
	boost::optional<DotModeTarget> parseDotModeTarget(const std::string &argument) const;
	DotModeArea *parseDotModeArea(const std::string argument) const;

	static DotModeUtil *get();
	static void destroy();
};

#endif

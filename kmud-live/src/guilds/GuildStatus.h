#ifndef GUILDE_STATUS_H
#define GUILDE_STATUS_H

#include "../Enum.h"

class GuildStatus : public Enum<GuildStatus> {

private:
	GuildStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:
	static GuildStatus *active;
	static GuildStatus *abandoned;
};

#endif

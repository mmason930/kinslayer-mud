#ifndef GUILD_RANK_STATUS
#define GUILD_RANK_STATUS

#include "../Enum.h"

class GuildRankStatus : public Enum<GuildRankStatus> {

private:
	GuildRankStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static GuildRankStatus *active;
	static GuildRankStatus *removed;
};

#endif

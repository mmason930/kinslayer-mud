#ifndef GUILD_PRIVILEGE_H
#define GUILD_PRIVILEGE_H

#include "../Enum.h"

class GuildPrivilege : public Enum<GuildPrivilege> {

private:
	GuildPrivilege(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static GuildPrivilege *approveNewGuildMembers;
	static GuildPrivilege *removeGuildMembers;
	static GuildPrivilege *manageRanks;
};

#endif

#include "GuildPrivilege.h"

GuildPrivilege *GuildPrivilege::approveNewGuildMembers = new GuildPrivilege(0, "Approve New Guild Members");
GuildPrivilege *GuildPrivilege::removeGuildMembers = new GuildPrivilege(1, "Remove Guild Members");
GuildPrivilege *GuildPrivilege::manageRanks = new GuildPrivilege(2, "Manage Ranks");
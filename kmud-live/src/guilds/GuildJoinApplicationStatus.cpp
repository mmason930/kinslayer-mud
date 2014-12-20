#include "GuildJoinApplicationStatus.h"

GuildJoinApplicationStatus *GuildJoinApplicationStatus::reviewing = new GuildJoinApplicationStatus(0, "Reviewing");
GuildJoinApplicationStatus *GuildJoinApplicationStatus::approved = new GuildJoinApplicationStatus(1, "Approved");
GuildJoinApplicationStatus *GuildJoinApplicationStatus::denied = new GuildJoinApplicationStatus(2, "Denied");
GuildJoinApplicationStatus *GuildJoinApplicationStatus::removed = new GuildJoinApplicationStatus(3, "Removed");

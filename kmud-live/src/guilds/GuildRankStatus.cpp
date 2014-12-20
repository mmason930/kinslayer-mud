#include "GuildRankStatus.h"

GuildRankStatus *GuildRankStatus::active = new GuildRankStatus(0, "Active");
GuildRankStatus *GuildRankStatus::removed = new GuildRankStatus(1, "Removed");

#include "GuildStatus.h"

//template < >
//std::list<Enum<GuildStatus>*> Enum<GuildStatus>::enums = std::list<Enum<GuildStatus>*>();

GuildStatus *GuildStatus::active = new GuildStatus(0, "Active");
GuildStatus *GuildStatus::abandoned = new GuildStatus(1, "Abandoned");

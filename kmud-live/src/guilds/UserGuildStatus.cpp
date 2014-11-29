#include "UserGuildStatus.h"

UserGuildStatus *UserGuildStatus::active  = new UserGuildStatus(0, "Active" , true);
UserGuildStatus *UserGuildStatus::removed = new UserGuildStatus(1, "Removed", false);
UserGuildStatus *UserGuildStatus::quit    = new UserGuildStatus(2, "Quit"   , false);
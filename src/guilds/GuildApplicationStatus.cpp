#include "GuildApplicationStatus.h"

GuildApplicationStatus *GuildApplicationStatus::pending = new GuildApplicationStatus  (0, "Pending"  , false, false);
GuildApplicationStatus *GuildApplicationStatus::reviewing = new GuildApplicationStatus(1, "Reviewing", true , false);
GuildApplicationStatus *GuildApplicationStatus::approved = new GuildApplicationStatus (2, "Approved" , false, true);
GuildApplicationStatus *GuildApplicationStatus::denied = new GuildApplicationStatus   (3, "Denied"   , false, true);

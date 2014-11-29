#include "GuildApplicationStatus.h"

GuildApplicationStatus *GuildApplicationStatus::pending = new GuildApplicationStatus(0, "Pending", true);
GuildApplicationStatus *GuildApplicationStatus::reviewing = new GuildApplicationStatus(1, "Reviewing", true);
GuildApplicationStatus *GuildApplicationStatus::approved = new GuildApplicationStatus(2, "Approved", false);
GuildApplicationStatus *GuildApplicationStatus::denied = new GuildApplicationStatus(3, "Denied", false);

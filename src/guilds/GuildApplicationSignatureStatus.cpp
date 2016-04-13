#include "GuildApplicationSignatureStatus.h"

GuildApplicationSignatureStatus *GuildApplicationSignatureStatus::pending = new GuildApplicationSignatureStatus(0, "Pending", true, true, true);
GuildApplicationSignatureStatus *GuildApplicationSignatureStatus::approved = new GuildApplicationSignatureStatus(1, "Approved", true, false, true);
GuildApplicationSignatureStatus *GuildApplicationSignatureStatus::denied = new GuildApplicationSignatureStatus(2, "Denied", false, false, false);
GuildApplicationSignatureStatus *GuildApplicationSignatureStatus::removed = new GuildApplicationSignatureStatus(3, "Removed", false, false, false);

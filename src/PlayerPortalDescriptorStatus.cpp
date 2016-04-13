#include "PlayerPortalDescriptorStatus.h"

PlayerPortalDescriptorStatus *PlayerPortalDescriptorStatus::handshaking = new PlayerPortalDescriptorStatus(0, "Handshaking");
PlayerPortalDescriptorStatus *PlayerPortalDescriptorStatus::authenticating = new PlayerPortalDescriptorStatus(1, "Authenticating");
PlayerPortalDescriptorStatus *PlayerPortalDescriptorStatus::connected = new PlayerPortalDescriptorStatus(2, "Connected");
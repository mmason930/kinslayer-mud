#include "GatewayDescriptorStatus.h"

GatewayDescriptorStatus *GatewayDescriptorStatus::awaitingConnection = new GatewayDescriptorStatus(0, "Awaiting Connection");
GatewayDescriptorStatus *GatewayDescriptorStatus::retrievingSession = new GatewayDescriptorStatus(1, "Retrieving Session");
GatewayDescriptorStatus *GatewayDescriptorStatus::connected = new GatewayDescriptorStatus(3, "Connected");
GatewayDescriptorStatus *GatewayDescriptorStatus::disconnected = new GatewayDescriptorStatus(4, "Disconnected");
GatewayDescriptorStatus *GatewayDescriptorStatus::handshaking = new GatewayDescriptorStatus(5, "Handshaking");

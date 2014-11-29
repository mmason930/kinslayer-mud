#include "GatewayDescriptorType.h"

GatewayDescriptorType *GatewayDescriptorType::unknown = new GatewayDescriptorType(0, "Unknown");
GatewayDescriptorType *GatewayDescriptorType::rawTCP = new GatewayDescriptorType(1, "Raw TCP");
GatewayDescriptorType *GatewayDescriptorType::websocket = new GatewayDescriptorType(2, "WebSocket");

#include "GatewayDescriptorType.h"

template < >
std::list<Enum<GatewayDescriptorType>*> Enum<GatewayDescriptorType>::enums = std::list<Enum<GatewayDescriptorType>*>();

GatewayDescriptorType *GatewayDescriptorType::unknown = new GatewayDescriptorType(0, "Unknown");
GatewayDescriptorType *GatewayDescriptorType::rawTCP = new GatewayDescriptorType(1, "Raw TCP");
GatewayDescriptorType *GatewayDescriptorType::websocket = new GatewayDescriptorType(2, "WebSocket");

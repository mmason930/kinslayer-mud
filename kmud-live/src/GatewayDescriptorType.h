#ifndef GATEWAY_DESCRIPTOR_TYPE_H
#define GATEWAY_DESCRIPTOR_TYPE_H

#include "Enum.h"

class GatewayDescriptorType : public Enum<GatewayDescriptorType> {

private:
	GatewayDescriptorType(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:
	
	static GatewayDescriptorType *unknown;
	static GatewayDescriptorType *rawTCP;
	static GatewayDescriptorType *websocket;
};


#endif



#ifndef GATEWAY_DESCRIPTOR_STATUS_H
#define GATEWAY_DESCRIPTOR_STATUS_H

#include "Enum.h"

class GatewayDescriptorStatus : public Enum {

private:

	static std::list<GatewayDescriptorStatus*> enums;

	GatewayDescriptorStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
		enums.push_back(this);
	}

public:


	static GatewayDescriptorStatus *awaitingConnection;
	static GatewayDescriptorStatus *retrievingSession;
	static GatewayDescriptorStatus *connected;
	static GatewayDescriptorStatus *disconnected;
	static GatewayDescriptorStatus *handshaking;
};


#endif



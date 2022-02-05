#ifndef GATEWAY_DESCRIPTOR_STATUS_H
#define GATEWAY_DESCRIPTOR_STATUS_H

#include "Enum.h"

class GatewayDescriptorStatus : public Enum<GatewayDescriptorStatus> {

private:

	GatewayDescriptorStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:
	static GatewayDescriptorStatus *awaitingConnection;
	static GatewayDescriptorStatus *retrievingSession;
	static GatewayDescriptorStatus *connected;
	static GatewayDescriptorStatus *disconnected;
	static GatewayDescriptorStatus *handshaking;
};


#endif

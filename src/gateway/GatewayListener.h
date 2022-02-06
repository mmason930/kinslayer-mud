#ifndef GATEWAY_LISTENER_H
#define GATEWAY_LISTENER_H

#include "../ku/kuSockets.h"
#include "../ku/kuListener.h"

const int GATEWAY_LISTENER_TYPE_NORMAL = 0;
const int GATEWAY_LISTENER_TYPE_WEBSOCKET = 1;

class GatewayListener
{
private:
	kuListener *listener;
	int type;
public:
	GatewayListener();
	~GatewayListener();

	kuListener *getListener();
	void setListener(kuListener *listener);

	int getType();
	void setType(const int type);
};

#endif

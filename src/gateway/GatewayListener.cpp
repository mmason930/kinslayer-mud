#include "GatewayListener.h"

GatewayListener::GatewayListener()
{
	listener = NULL;
}
GatewayListener::~GatewayListener()
{
	if(listener != NULL)
		delete listener;
}

kuListener *GatewayListener::getListener()
{
	return listener;
}

void GatewayListener::setListener(kuListener *listener)
{
	this->listener = listener;
}

int GatewayListener::getType()
{
	return type;
}

void GatewayListener::setType(const int type)
{
	this->type = type;
}

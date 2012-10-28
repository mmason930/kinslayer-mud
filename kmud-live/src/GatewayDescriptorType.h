#ifndef GATEWAY_DESCRIPTOR_TYPE_H
#define GATEWAY_DESCRIPTOR_TYPE_H

#include "Enum.h"

class GatewayDescriptorType : public Enum {

private:

	static std::list<GatewayDescriptorType*> enums;

	GatewayDescriptorType(int value, const std::string &standardName) : Enum(value, standardName)
	{
		enums.push_back(this);
	}

public:

	static Enum *getEnumByValue(int v)
	{
		for(auto iter = enums.begin();iter != enums.end();++iter)
		{
			Enum *e = (*iter);
			if( e->getValue() == v )
			{
				return e;
			}
		}
		
		return NULL;
	}
	
	static GatewayDescriptorType *unknown;
	static GatewayDescriptorType *rawTCP;
	static GatewayDescriptorType *websocket;
};


#endif



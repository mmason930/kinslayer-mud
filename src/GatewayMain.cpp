#include "GatewayServer.h"

#include <fstream>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "StringUtil.h"
#include "MiscUtil.h"

int main( void )
{
	GatewayServer gatewayServer;

	srand((unsigned int) (time(0)+clock()/2) );

	if(!gatewayServer.loadConfiguration())
		exit(1);
	gatewayServer.setup();

	gatewayServer.run();
}

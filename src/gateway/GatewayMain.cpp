#include "GatewayServer.h"

#include <fstream>
#ifndef WIN32
#include <csignal>
#endif
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../StringUtil.h"
#include "../MiscUtil.h"

int main( void )
{
#ifndef WIN32
	std::signal(SIGPIPE, SIG_IGN);
#endif
	GatewayServer gatewayServer;

	time_t currentTime = time(0);
	srand((unsigned int) (currentTime + clock()/2) );

	if(!gatewayServer.loadConfiguration())
		exit(1);
	gatewayServer.setup();

	gatewayServer.run();
}

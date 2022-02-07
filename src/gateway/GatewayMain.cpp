


int main( void )
{
	GatewayServer gatewayServer;

	time_t currentTime = time(0);
	srand((unsigned int) (currentTime + clock()/2) );

	if(!gatewayServer.loadConfiguration())
		exit(1);
	gatewayServer.setup();

	gatewayServer.run();
}

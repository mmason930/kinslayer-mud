#include <iostream>
#include <thread>

#include "DateTime.h"
#include "MiscUtil.h"
#include "SystemUtil.h"

#include "GatewayServer.h"
#include "GatewayListener.h"
#include "GatewayDescriptorStatus.h"
#include "GatewayDescriptorType.h"
#include "StringUtil.h"

#include "WebSocketClientHeader.h"
#include "WebSocketException.h"

#ifndef WIN32
#include <sys/types.h>
#include <sys/wait.h>
#endif

bool debugBoolean = false;

std::vector<std::string> crashReasonVector;

std::string makeTimestamp() {

	return "[" + DateTime::parse("%Y-%m-%d %H:%M:%S", DateTime()) + "]";
}

GatewayServer::GatewayServer()
{
	this->lastPingResponseFromGameServer = DateTime(0);
	this->lastPingSentToGameServer = DateTime(0);
	this->serverPort = 0;
	this->motherConnectionToServer = NULL;
	this->restartOnShutdown = false;
	this->mudProcessId = 0;
	this->mudStatus = MudStatus::notRunning;
	this->statusLastModifiedDatetime = DateTime();
	this->lastAttemptedMudStartDatetime = DateTime(0);
	this->lastMudProcessIdCheckDatetime = DateTime(0);
	this->shutdownOnReboot = false;
}

GatewayServer::~GatewayServer()
{
}

std::string GatewayServer::getServerHost()
{
	return serverHost;
}

int GatewayServer::getServerPort()
{
	return serverPort;
}

GatewayListener *GatewayServer::getGatewayListener(kuListener *listener)
{
	for(auto iter = listeners.begin();iter != listeners.end();++iter)
	{
		GatewayListener *gatewayListener = (*iter);
		if(gatewayListener->getListener() == listener)
			return gatewayListener;
	}

	return NULL;
}

void closeDescriptorCallback(void *data, kuListener *listener, kuDescriptor *descriptor)
{
	GatewayServer *gatewayServer = static_cast<GatewayServer*>(data);

	gatewayServer->removeDescriptor(descriptor);

	std::cout << makeTimestamp() << " Descriptor closed [" << descriptor->getIp() << "]" << std::endl;
}

//descriptor is the connection from the player to the gateway server.
void openDescriptorCallback(void *data, kuListener *listener, kuDescriptor *descriptor)
{
	GatewayServer *gatewayServer = static_cast<GatewayServer*>(data);
	GatewayDescriptor *gatewayDescriptor = new GatewayDescriptor();
	GatewayListener *gatewayListener = gatewayServer->getGatewayListener(listener);
	
	gatewayDescriptor->setClientConnection(descriptor);
	gatewayDescriptor->setRandomId(StringUtil::getRandomString(40));
	gatewayDescriptor->setGatewayListener(gatewayListener);

	if(gatewayListener->getType() == GATEWAY_LISTENER_TYPE_WEBSOCKET)
	{
		gatewayDescriptor->setStatus(GatewayDescriptorStatus::handshaking);
		gatewayDescriptor->setType(GatewayDescriptorType::websocket);
	}
	else
	{
		gatewayDescriptor->setStatus(GatewayDescriptorStatus::awaitingConnection);
		gatewayDescriptor->setType(GatewayDescriptorType::rawTCP);
	}

	gatewayServer->getDescriptors().push_back(gatewayDescriptor);

	std::cout << makeTimestamp() << " Descriptor opened [" << descriptor->getIp() << "]" << std::endl;
}

kuClient *GatewayServer::getMotherConnectionToServer()
{
	return motherConnectionToServer;
}

bool GatewayServer::loadConfiguration()
{
	std::string configFilePath = BASIC_CONFIG_PATH;

	std::cout << "Loading configuration from " << configFilePath << std::endl;
	
	std::map<std::string, std::string> resources = MiscUtil::loadResourcesFromFile(configFilePath);

	if(resources.find("Gateway Port") == resources.end()) {

		std::cout << "No Gateway Port specified in configuration file." << std::endl;
		return false;
	}
	if(resources.find("MUD Host") == resources.end()) {

		std::cout << "No MUD Host specified in configuration file." << std::endl;
		return false;
	}
	if(resources.find("MUD Port") == resources.end()) {

		std::cout << "No MUD Port specified in configuration file." << std::endl;
		return false;
	}
	if(resources.find("MUD Root Directory") == resources.end()) {

		std::cout << "No MUD Root Directory specified in confuration file." << std::endl;
		return false;
	}
	if(resources.find("Restart On Shutdown") == resources.end()) {

		std::cout << "No `Restart On Shutdown` specified in confuration file." << std::endl;
		return false;
	}
	if(resources.find("MUD Executable Path") == resources.end()) {

		std::cout << "No `MUD Executable Path` specified in confuration file." << std::endl;
		return false;
	}

	this->secondaryPort = 0;
	if(resources.find("Secondary Gateway Port") != resources.end())
	{
		this->secondaryPort = atoi(resources["Secondary Gateway Port"].c_str());
	}

	if(resources.find("Websocket Ports") != resources.end())
	{
		std::list<std::string> websocketPortsStringList = StringUtil::SplitToList<std::string>(resources["Websocket Ports"], ',');
		for(auto iter = websocketPortsStringList.begin();iter != websocketPortsStringList.end();++iter)
		{
			std::string portString = (*iter);
			if(!MiscUtil::isInt(portString))
			{
				std::cout << "Invalid Websocket Port specified: `" << portString << "`: Not a valid integer." << std::endl;
			}
			else
			{
				this->websocketPorts.push_back(atoi(portString.c_str()));
			}
		}
	}

	this->serverPort = atoi(resources["MUD Port"].c_str());
	this->serverHost = resources["MUD Host"];
	this->listeningPort = atoi(resources["Gateway Port"].c_str());
	this->mudRootDirectoryPath = resources["MUD Root Directory"];
	this->restartOnShutdown = atoi(resources["Restart On Shutdown"].c_str());
	this->mudExecutablePath = resources["MUD Executable Path"];
	return true;
}

void GatewayServer::setupAndRun()
{
	setup();

	run();
}

void GatewayServer::setup()
{
	crashReasonVector.clear();

	crashReasonVector.push_back("Galnor's terrible coding");
	crashReasonVector.push_back("Fogel's balance imbalance");
	crashReasonVector.push_back("Norum's fury");
	crashReasonVector.push_back("a spontaneous bunny raid");
	crashReasonVector.push_back("a nargdoor");
	crashReasonVector.push_back("divine intervention");
	crashReasonVector.push_back("WoTMUD spies");
	crashReasonVector.push_back("a tsunami");
	crashReasonVector.push_back("Westboro Baptist Church");
	crashReasonVector.push_back("Sarah Palin");
	crashReasonVector.push_back("a divide by zero");
	crashReasonVector.push_back("rampant roleplaying");
	crashReasonVector.push_back("Rick Astley");
	crashReasonVector.push_back("Norya's hacking");
	crashReasonVector.push_back("Gristlyn's horrible typing");
	crashReasonVector.push_back("Phantasm's idiocy");
	crashReasonVector.push_back("Chase's brotacular scandal");
	crashReasonVector.push_back("Lord Jack's allegience to the Darkside");
	crashReasonVector.push_back("Rakkard's RAINBOW");
	crashReasonVector.push_back("your mom");
	crashReasonVector.push_back("Facebook's terrible chat interface");
	crashReasonVector.push_back("Google's attempts to buy us out");
	crashReasonVector.push_back("rampant abuse of loopholes");
	crashReasonVector.push_back("Lamgwin's political aspirations");
	crashReasonVector.push_back("Jonlin's unexpected return");
	crashReasonVector.push_back("the long overdue meltdown of the Valon Guard");
	crashReasonVector.push_back("Kno'mon clan leaders");
	crashReasonVector.push_back("something Evilina did");
	crashReasonVector.push_back("the MMORPG takeover");
	crashReasonVector.push_back("you. Yes, you,");
}

void GatewayServer::disconnectMotherConnectionFromGameServer()
{
	if(motherConnectionToServer != NULL) {

		motherConnectionToServer->disconnect();
		delete motherConnectionToServer;
	}

	motherConnectionToServer = NULL;
}

void GatewayServer::attemptConnectionWithGameServer()
{
	//Setup mother connection to the game server.

	if(motherConnectionToServer != NULL) {

		delete motherConnectionToServer;
	}

	motherConnectionToServer = new kuClient();
	motherConnectionToServer->connect(serverHost, serverPort);

	if(motherConnectionToServer->isConnected() == false) {

		delete motherConnectionToServer;
		motherConnectionToServer = NULL;
	}
	else {

		sendToDescriptors("The MUD has been restarted and is now booting...\r\n\r\n");

		crashed = false;
		rebooting = false;
		setMudStatus( MudStatus::booting );

		std::cout << makeTimestamp() << " Connection established. Sending validation token." << std::endl;
		this->mudProcessId = 0;

		motherConnectionToServer->send("Validate 78fd516c2825e7f463f045e609a8523e\r\n");
	}
}

void GatewayServer::bindListener()
{
	//Setup the listening server for incoming connections.
	std::list<int> portsToListenTo;
	portsToListenTo.push_back(listeningPort);
	if(secondaryPort)
		portsToListenTo.push_back(secondaryPort);

	for(auto iter = portsToListenTo.begin();iter != portsToListenTo.end();++iter)
	{
		int port = (*iter);
		std::cout << makeTimestamp() << " Setting up gateway listener(normal) on port " << port << "." << std::endl;
		kuListener * listener;

		GatewayListener *gatewayListener = new GatewayListener();
		listener = new kuListener(port, TCP);
		gatewayListener->setListener(listener);
		gatewayListener->setType(GATEWAY_LISTENER_TYPE_NORMAL);

		listener->setDataForCloseDescriptorCallback( static_cast<void*>(this) );

		listener->setCloseDescriptorCallback(closeDescriptorCallback);
	
		listener->setDataForOpenDescriptorCallback( static_cast<void*>(this) );

		listener->setOpenDescriptorCallback(openDescriptorCallback);

		if (!listener->enableKeepAlive())
		{
			std::cout << makeTimestamp() << " Failed to set keepalive on socket." << std::endl;
		}

		listeners.push_back(gatewayListener);
	}
	for(auto iter = websocketPorts.begin();iter != websocketPorts.end();++iter)
	{
		int port = (*iter);
		std::cout << makeTimestamp() << " Setting up gateway listener(websocket) on port " << port << "." << std::endl;
		kuListener * listener;

		GatewayListener *gatewayListener = new GatewayListener();
		listener = new kuListener(port, TCP);
		gatewayListener->setListener(listener);
		gatewayListener->setType(GATEWAY_LISTENER_TYPE_WEBSOCKET);

		listener->setDataForCloseDescriptorCallback( static_cast<void*>(this) );

		listener->setCloseDescriptorCallback(closeDescriptorCallback);
	
		listener->setDataForOpenDescriptorCallback( static_cast<void*>(this) );

		listener->setOpenDescriptorCallback(openDescriptorCallback);

		if (!listener->enableKeepAlive())
		{
			std::cout << makeTimestamp() << " Failed to set keepalive on socket." << std::endl;
		}

		listeners.push_back(gatewayListener);
	}
}

bool GatewayServer::isConnectedToGameServer()
{
	if(motherConnectionToServer == NULL) {

		return false;
	}

	if(!motherConnectionToServer->isConnected()) {

		return false;
	}

	return true;
}

void GatewayServer::processInputFromMotherConnectionToServer()
{
	std::list<GatewayDescriptor*>::iterator descriptorIter;

	std::stringstream inputBuffer;

	this->getMotherConnectionToServer()->receive(inputBuffer);

	inputBufferFromServer += inputBuffer.str();

	while(hasCommand()) {

		std::string commandLine = getCommand();

		try {

			std::vector<std::string> vArgs = StringUtil::getArgVector(commandLine);

			std::string command = vArgs.at(0);

			if(command == "Session") {

				std::string randomId = vArgs.at(1);
				std::string sessionKey = vArgs.at(2);

				std::cout << makeTimestamp() << " Session received: " << sessionKey << std::endl;

				descriptorIter = descriptors.begin();

				while(descriptorIter != descriptors.end()) {

					GatewayDescriptor *descriptor = (*descriptorIter);

					if(descriptor->getRandomId() == randomId) {

						descriptor->setSession(sessionKey);

						descriptor->connect(serverHost, serverPort);

						descriptor->sendToServer( "Session " + sessionKey + "\r\n" );

						descriptor->setStatus(GatewayDescriptorStatus::connected);

						break;
					}

					++descriptorIter;
				}
			}
			else if(command == "Close") {

				std::string sessionKey = vArgs.at(1);

				descriptorIter = descriptors.begin();

				std::cout << makeTimestamp() << " Close received: " << sessionKey << std::endl;

				while(descriptorIter != descriptors.end()) {

					GatewayDescriptor *descriptor = (*descriptorIter);

					if(descriptor->getSession() == sessionKey) {

						this->disconnectDescriptorFromGameAndGateway(descriptor);

						descriptors.erase(descriptorIter);

						break;
					}
					else {

						++descriptorIter;
					}
				}
			}
			else if(command == "PingResponse") {

				//TODO: Read any actual data sent along with this packet.

				std::cout << makeTimestamp() << " Ping response received from game server." << std::endl;

				lastPingResponseFromGameServer = DateTime();
			}
			else if(command == "FinishedBooting") {
				
				std::cout << makeTimestamp() << " The MUD has indicated that it has finished booting." << std::endl;
				
				setMudStatus( MudStatus::running );
			}
			else if(command == "ShutdownOnReboot") {

				std::cout << makeTimestamp() << " The MUD has told the Gateway to shut down when the MUD next reboots." << std::endl;

				this->shutdownOnReboot = true;
			}
			else if(command == "RestartOnReboot") {

				std::cout << makeTimestamp() << " The MUD has told the Gateway to restart when the MUD next reboots." << std::endl;

				this->shutdownOnReboot = false;
			}
			else if(command == "Reboot") {

				std::cout << makeTimestamp() << " Reboot command received. Putting players into awaitingConnection mode." << std::endl;

				sendToDescriptors("The MUD is shutting down...\r\n\r\n");

				crashed = false;
				rebooting = true;

				handleGameShutdown();
				setMudStatus(MudStatus::shuttingDown);
			}
			else if(command == "ProcessID") {

				int processId = atoi(vArgs.at(1).c_str());
				std::cout << makeTimestamp() << " The MUD has set its process ID to `" << processId << "`." << std::endl;
				this->mudProcessId = processId;

			}
		}
		catch(std::out_of_range e) {

			//...Error...
		}
	}
}

void GatewayServer::disconnectDescriptorFromGameAndGateway(GatewayDescriptor *gatewayDescriptor)
{
	gatewayDescriptor->getServerConnection()->disconnect();

	delete gatewayDescriptor->getServerConnection();

	debugBoolean = true;

	gatewayDescriptor->getClientConnection()->socketClose();
	gatewayDescriptor->setServerConnection(NULL);
	gatewayDescriptor->setClientConnection(NULL);
	gatewayDescriptor->setStatus(GatewayDescriptorStatus::disconnected);
}

void GatewayServer::handleGameShutdown()
{
	std::list<GatewayDescriptor*>::iterator iter;
	iter = descriptors.begin();

	while(iter != descriptors.end()) {

		GatewayDescriptor *descriptor = (*iter);
		descriptor->setStatus(GatewayDescriptorStatus::awaitingConnection);
		kuClient *connectionToGame = descriptor->getServerConnection();

		if(connectionToGame != NULL) {

			delete connectionToGame;
			descriptor->setServerConnection(NULL);
		}

		++iter;
	}
}
void GatewayServer::sendToDescriptors(const std::string &message)
{
	std::list<GatewayDescriptor*>::iterator iter;
	iter = descriptors.begin();

	while(iter != descriptors.end()) {

		GatewayDescriptor *descriptor = (*iter);

		descriptor->sendOutputMessageToClient(message);

		++iter;
	}
}

void GatewayServer::run()
{
	std::list<GatewayDescriptor*>::iterator descriptorIter;

	bindListener();
	bool isTerminated = false;

	while(!isTerminated) {

		for(auto gatewayListenerIter = listeners.begin();gatewayListenerIter != listeners.end();++gatewayListenerIter)
		{
			(*gatewayListenerIter)->getListener()->acceptNewHosts();
			(*gatewayListenerIter)->getListener()->pulse();
		}

		if(!isConnectedToGameServer())
		{
			if(getMudStatus() == MudStatus::running || getMudStatus() == MudStatus::booting) {
			//We lost connection but think the MUD is up. This means the MUD has most likely crashed.

				crashed = true;
				rebooting = false;

				//Disconnect all players and set them in 'awaiting connection' mode.

				std::string reason = crashReasonVector[ MiscUtil::random(0,crashReasonVector.size()-1) ];

				sendToDescriptors("The MUD has crashed! How terrible!\r\n\r\n"
								  "We suspect " + reason + " may be responsible.\r\n\r\n"
											  "Please standby while we reboot...\r\n\r\n");

				std::cout << makeTimestamp() << " Game has crashed. Putting players into awaitingConnection mode." << std::endl;

				handleGameShutdown();
				setMudStatus( MudStatus::notRunning );

				lastAttemptedMudStartDatetime = DateTime(0);//Reset this to far in the past so we attempt a reboot immediately.
			}
			attemptConnectionWithGameServer();
		}

		if(getMudStatus() == MudStatus::shuttingDown)
		{
			//Check to see if the MUD has shut down yet.
			if(SystemUtil::processExists( this->mudProcessId ) == false)
			{
				setMudStatus(MudStatus::notRunning);
				this->mudProcessId = 0;

				this->disconnectMotherConnectionFromGameServer();
				sendToDescriptors("The MUD has finished shutting down.\r\n\r\n");
				std::cout << makeTimestamp() << " The MUD process can no longer be found. Assuming MUD has completed with shutdown." << std::endl;

				if(shutdownOnReboot)
				{
					std::cout << makeTimestamp() << " The gateway is now preparing to shut down." << std::endl;
					isTerminated = true;
				}
			}
			else
			{
//				std::cout << makeTimestamp() << " MUD process ID `" << this->mudProcessId << "` still exists. Waiting for shutdown to complete..." << std::endl;
				//MUD is shutting down, but still running.
			}
		}

		
		//We need to reboot the MUD if configured to carry out the task.
		if(!isTerminated && getMudStatus() == MudStatus::notRunning)
		{
			long long secondsSinceLastAttemptedMudStart = (DateTime().getTime() - lastAttemptedMudStartDatetime.getTime());

			if(this->restartOnShutdown && secondsSinceLastAttemptedMudStart >= 60)
			{
				sendToDescriptors("Attempting to start the MUD...\r\n\r\n");
				this->attemptMudStart();
				lastAttemptedMudStartDatetime = DateTime();
				
				std::cout << makeTimestamp() << " Attempting to restart MUD. Current status is `" << getMudStatus()->getStandardName() << "`. Socket connection: " << (isConnectedToGameServer() == true ? "Yes" : "No") << std::endl;
			}
		}

		if(!isTerminated && getMudStatus() == MudStatus::booting)
		{
			if(this->mudProcessId != 0 && DateTime().getTime() - lastMudProcessIdCheckDatetime.getTime() > 5)
			{
				std::cout << makeTimestamp() << " Checking to see if MUD process is still alive...";

				if(SystemUtil::processExists(this->mudProcessId))
					std::cout << "yes" << std::endl;
				else
				{
					std::cout << "no. Treating as crash." << std::endl;
					this->crashed = true;
				}

				lastMudProcessIdCheckDatetime = DateTime();
			}
		}
		
		if(!isTerminated && getMudStatus() == MudStatus::running)
		{
			__int64 ts = lastPingSentToGameServer.getTime();
			__int64 tr = lastPingResponseFromGameServer.getTime();

			__int64 tn = DateTime().getTime();

			__int64 timeBetween = tn - ts;

//			if(lastPingSentToGameServer.getTime() == 0 || !lastPingSentToGameServer.after(lastPingResponseFromGameServer)) {
			if(ts == 0 || timeBetween >= 5) {

				pingGameServer();
			}
		}

		if(isConnectedToGameServer() && getMudStatus() != MudStatus::notRunning) {

			processInputFromMotherConnectionToServer();
		}

		for(descriptorIter = descriptors.begin();descriptorIter != descriptors.end();) {

			GatewayDescriptor *descriptor = (*descriptorIter);
			bool removedDescriptor = false;

			if(descriptor->getStatus() == GatewayDescriptorStatus::connected) {
				
				std::string inputFromClient, inputFromServer;
				try
				{
					 inputFromClient = descriptor->pullFromClient();
				}
				catch(WebSocketException webSocketException)
				{
					if(webSocketException.what() == "Socket Closed")
					{
						this->disconnectDescriptorFromGameAndGateway(descriptor);
						descriptorIter = descriptors.erase(descriptorIter);
						removedDescriptor = true;
					}
				}

				if(!removedDescriptor)
				{
					inputFromServer = descriptor->pullFromServer();

					if(inputFromServer.empty() == false) {

						descriptor->sendToClient(inputFromServer);
					}

					if(inputFromClient.empty() == false) {

						descriptor->sendToServer(inputFromClient);
					}
				}
			}
			else if(descriptor->getStatus() == GatewayDescriptorStatus::awaitingConnection && getMotherConnectionToServer() != NULL && !this->mudIsDown()) {

				kuClient *gameClient = getMotherConnectionToServer();

				std::stringstream buffer;

				buffer << "Host " << descriptor->getRandomId() << " " << descriptor->getClientConnection()->getIp()
					   << " " << descriptor->getType()->getValue() << "\r\n";

				gameClient->send(buffer.str());

				descriptor->setStatus(GatewayDescriptorStatus::retrievingSession);
			}
//			else if(descriptor->getStatus() == GatewayDescriptorStatus::handshaking) {
			else if(descriptor->getStatus() == GatewayDescriptorStatus::handshaking && getMotherConnectionToServer() != NULL && !this->mudIsDown()) {

				try {
					std::string dataFromWebSocketClient = descriptor->pullFromClient();

					descriptor->appendToCurrentInputBuffer(dataFromWebSocketClient);

					if(WebSocketClientHeader::isComplete(descriptor->getCurrentInputBuffer()))
					{
						dataFromWebSocketClient = descriptor->getCurrentInputBuffer();

						std::auto_ptr<WebSocketClientHeader> webSocketClientHeader(WebSocketClientHeader::allocateByInitialClientPacket(dataFromWebSocketClient));
						webSocketClientHeader->read(dataFromWebSocketClient);
						std::string response = webSocketClientHeader->generateResponse(descriptor->getGatewayListener()->getListener()->getPort(), "mud-protocol");

						descriptor->sendToClient(response);
						descriptor->setStatus(GatewayDescriptorStatus::awaitingConnection);

						std::cout << makeTimestamp() << " Client `" << descriptor->getSession() << "` is being put into awaitingConnection status." << std::endl;
					}
				}
				catch(WebSocketException webSocketException)
				{
					descriptor->setStatus(GatewayDescriptorStatus::disconnected);
					std::cout << "ERROR : WebSocketException caught while handshaking: " << webSocketException.what() << std::endl;
				}
			}

			if(!removedDescriptor)
				++descriptorIter;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool GatewayServer::hasCommand()
{
	return (inputBufferFromServer.find("\n") != std::string::npos);
}

std::string GatewayServer::getCommand()
{
	std::string::size_type endPos, i, size;

	size = inputBufferFromServer.size();

	for(i = 0;i < size && inputBufferFromServer[i] != '\r' && inputBufferFromServer[i] != '\n';++i);

	if( (size == 0) || (i == size && inputBufferFromServer[i] != '\r' && inputBufferFromServer[i] != '\n') )
		return std::string("");

	endPos = i;

	if( (size > i+1) && ((inputBufferFromServer[i] == '\n' && inputBufferFromServer[i+1] == '\r') || (inputBufferFromServer[i] == '\r' && inputBufferFromServer[i+1] == '\n')) )
		++endPos;

	std::string Line = inputBufferFromServer.substr(0, i);
	inputBufferFromServer.erase(0, endPos+1);

	return Line;
}

void GatewayServer::shutdown()
{
	while(listeners.empty() == false)
	{
		delete listeners.front();
		listeners.pop_front();
	}
}

std::list<GatewayDescriptor*> &GatewayServer::getDescriptors()
{
	return descriptors;
}

void GatewayServer::removeDescriptor(kuDescriptor *descriptor)
{
	for(std::list<GatewayDescriptor*>::iterator iter = descriptors.begin();iter != descriptors.end();++iter) {

		if( (*iter)->getClientConnection() == descriptor ) {

			if( (*iter)->getServerConnection() != NULL ) {

				(*iter)->getServerConnection()->disconnect();
			}

			delete (*iter);

			descriptors.erase(iter);

			return;
		}
	}
}

void GatewayServer::pingGameServer()
{
	if(motherConnectionToServer != NULL && (lastPingSentToGameServer.getTime() == 0 || lastPingSentToGameServer.compareTo(lastPingResponseFromGameServer) <= 0)) {

		std::cout << makeTimestamp() << " Pinging game server." << std::endl;

		motherConnectionToServer->send("Ping " + DateTime::parse("%Y-%m-%d %H:%M:%S", DateTime()) + "\n");

		this->lastPingSentToGameServer = DateTime();
	}
}

DateTime GatewayServer::getLastPingSentToServerDatetime()
{
	return lastPingSentToGameServer;
}

DateTime GatewayServer::getLastPingResponseFromGameServerDatetime()
{
	return lastPingResponseFromGameServer;
}
bool GatewayServer::mudIsCrashed()
{
	return crashed;
}

bool GatewayServer::mudIsRebooting()
{
	return rebooting;
}

bool GatewayServer::mudIsDown()
{
	return crashed || rebooting;
}

std::string GatewayServer::getMudRootDirectoryPath()
{
	return mudRootDirectoryPath;
}

bool GatewayServer::getRestartOnShutdown()
{
	return restartOnShutdown;
}

bool GatewayServer::isMudProcessRunning()
{
	return (SystemUtil::processExists(this->mudProcessId));
}

MudStatus *GatewayServer::getMudStatus()
{
	return mudStatus;
}

void GatewayServer::setMudStatus(MudStatus *mudStatus)
{
	this->mudStatus = mudStatus;
	this->statusLastModifiedDatetime = DateTime();
}

void GatewayServer::attemptMudStart()
{
	std::cout << makeTimestamp() << " Attempting to start the MUD." << std::endl;
	std::stringstream commandBuffer;
#ifdef WIN32
	commandBuffer << "START CMD /C CALL \"" << this->getMudRootDirectoryPath() << mudExecutablePath << "\"";
	system(commandBuffer.str().c_str());
#else
	commandBuffer << this->getMudRootDirectoryPath() << mudExecutablePath;

	pid_t temporaryProcessId, childProcessId;

	temporaryProcessId = fork();

	if(temporaryProcessId == 0)
	{//This is the child
		childProcessId = fork();
		if(childProcessId == 0)
		{//This is the actual child, which we will use to spawn the MUD process.
//			std::cout << "**MUD PROCESS** Child process starting MUD..." << std::endl;
			int retval = execlp(commandBuffer.str().c_str(), commandBuffer.str().c_str(), 0);

//			std::cout << "**MUD PROCESS** Child process finished starting MUD..." << std::endl;
//			std::cout << "**MUD PROCESS** Return Value: " << retval << std::endl;
//			std::cout << "**MUD PROCESS** Errno: " << strerror(errno) << std::endl;
			exit(1);
		}
		else if(childProcessId > 0)
		{//This is the parent process, which we will terminate to assign the MUD process's parent to the "init" process.
		 //The gateway process which we will keep alive will spawn on the second fork call.
//			std::cout << "**TEMPORARY PROCESS** Exiting immediately..." << std::endl;
			exit(0);
		}
		else
		{//Error, could not fork.
			std::cout << makeTimestamp() << " ERROR: Could not fork!" << std::endl;
		}
	}
	else if(temporaryProcessId > 0)
	{//Here we have the actual gateway process, which will safely exit out of this function and proceed functioning.
	 //First we will wait for the temporary process to terminate so that we can reap it, eliminating the dangling zombie process.
		int status;
		waitpid(childProcessId, &status, 0);
	}
	else
	{//Error, could not fork.
		std::cout << makeTimestamp() << " ERROR: Could not fork!" << std::endl;
	}
	
//	std::cout << "**GATEWAY PROCESS** Proceeding..." << std::endl;

#endif
}

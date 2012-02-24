#include <iostream>
#include <boost/thread.hpp>

#include "DateTime.h"
#include "MiscUtil.h"

#include "GatewayServer.h"
#include "GatewayDescriptorStatus.h"
#include "StringUtil.h"

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
}

GatewayServer::GatewayServer(const int port, const int *secondaryPort, const std::string &serverHost, const int serverPort)
{
	this->lastPingResponseFromGameServer = DateTime(0);
	this->lastPingSentToGameServer = DateTime(0);
	setup(port, secondaryPort, serverHost, serverPort);
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

void closeDescriptorCallback(void *data, kuDescriptor *descriptor)
{
	GatewayServer *gatewayServer = static_cast<GatewayServer*>(data);

	gatewayServer->removeDescriptor(descriptor);

	std::cout << makeTimestamp() << " Descriptor closed [" << descriptor->getIp() << "]" << std::endl;
}

//descriptor is the connection from the player to the gateway server.
void openDescriptorCallback(void *data, kuDescriptor *descriptor)
{
	GatewayServer *gatewayServer = static_cast<GatewayServer*>(data);

	GatewayDescriptor *gatewayDescriptor = new GatewayDescriptor();
	
	gatewayDescriptor->setClientConnection(descriptor);

	gatewayDescriptor->setStatus(GatewayDescriptorStatus::awaitingConnection);

	gatewayDescriptor->setRandomId(StringUtil::getRandomString(40));

	gatewayServer->getDescriptors().push_back(gatewayDescriptor);

	std::cout << makeTimestamp() << " Descriptor opened [" << descriptor->getIp() << "]" << std::endl;
}

kuClient *GatewayServer::getMotherConnectionToServer()
{
	return motherConnectionToServer;
}

void GatewayServer::setupAndRun(const int port, const int *secondaryPort, const std::string &serverHost, const int serverPort)
{
	setup(port, secondaryPort, serverHost, serverPort);

	run();
}

void GatewayServer::setup(const int port, const int *secondaryGatewayPort, const std::string &serverHost, const int serverPort)
{
	this->serverHost = serverHost;

	this->serverPort = serverPort;

	this->listeningPort = port;

	if(secondaryGatewayPort)
		this->secondaryPort = *secondaryGatewayPort;

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

		std::cout << makeTimestamp() << " Connection established. Sending validation token." << std::endl;

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
		std::cout << makeTimestamp() << " Setting up gateway listener on port " << port << "." << std::endl;
		kuListener * listener;

		listener = new kuListener(port, TCP);

		listener->setDataForCloseDescriptorCallback( static_cast<void*>(this) );

		listener->setCloseDescriptorCallback(closeDescriptorCallback);
	
		listener->setDataForOpenDescriptorCallback( static_cast<void*>(this) );

		listener->setOpenDescriptorCallback(openDescriptorCallback);

		listeners.push_back(listener);
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
			else if(command == "Reboot") {

				std::cout << makeTimestamp() << " Reboot command received. Putting players into awaitingConnection mode." << std::endl;

				sendToDescriptors("The MUD is shutting down...\r\n\r\n");

				crashed = false;
				rebooting = true;

				handleGameShutdown();
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

		descriptor->sendToClient(message);

		++iter;
	}
}

void GatewayServer::run()
{
	std::list<GatewayDescriptor*>::iterator descriptorIter;

	bindListener();

	while(true) {

		for(auto kuListenerIter = listeners.begin();kuListenerIter != listeners.end();++kuListenerIter)
		{
			(*kuListenerIter)->l_AcceptNewHosts();

			(*kuListenerIter)->l_Pulse();
		}

		if(!isConnectedToGameServer()) {

			if(mudIsDown() == false) {
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
			}

			attemptConnectionWithGameServer();
		}
		else {

			__int64 ts = lastPingSentToGameServer.getTime();
			__int64 tr = lastPingResponseFromGameServer.getTime();

			__int64 tn = DateTime().getTime();

			__int64 timeBetween = tn - ts;

//			if(lastPingSentToGameServer.getTime() == 0 || !lastPingSentToGameServer.after(lastPingResponseFromGameServer)) {
			if(ts == 0 || timeBetween >= 5) {

				pingGameServer();
			}

			processInputFromMotherConnectionToServer();
		}

		for(descriptorIter = descriptors.begin();descriptorIter != descriptors.end();++descriptorIter) {

			GatewayDescriptor *descriptor = (*descriptorIter);

			if(descriptor->getStatus() == GatewayDescriptorStatus::connected) {

				std::string inputFromClient = descriptor->pullFromClient();

				std::string inputFromServer = descriptor->pullFromServer();

				if(inputFromServer.empty() == false) {

					descriptor->sendToClient(inputFromServer);
				}

				if(inputFromClient.empty() == false) {

					descriptor->sendToServer(inputFromClient);
				}
			}
			else if(descriptor->getStatus() == GatewayDescriptorStatus::awaitingConnection && getMotherConnectionToServer() != NULL && !this->mudIsDown()) {

				kuClient *gameClient = getMotherConnectionToServer();

				std::stringstream buffer;

				buffer << "Host " << descriptor->getRandomId() << " " << descriptor->getClientConnection()->getIp() << "\r\n";

				gameClient->send(buffer.str());

				descriptor->setStatus(GatewayDescriptorStatus::retrievingSession);
			}
		}

		boost::this_thread::sleep( boost::posix_time::millisec( 10 ) );
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
	if(motherConnectionToServer != NULL) {

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

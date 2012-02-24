#include "GatewayServer.h"

#include <fstream>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "StringUtil.h"

std::map<std::string, std::string> loadResourcesFromFile(const std::string &fileName)
{
	std::ifstream inFile(fileName.c_str());

	std::map<std::string, std::string> mapping;

	if(inFile.is_open() == false) {

		return mapping;
	}

	while(inFile.eof() == false)
	{
		std::string line;
		
		std::getline(inFile, line);

		std::string::size_type pos = line.find(":");

		if(pos == std::string::npos) {

			continue;
		}

		std::string left = line.substr(0,pos);

		std::string right = line.substr(pos+1);
		
		StringUtil::trim(left);
		
		StringUtil::trim(right);

		mapping[left] = right;
	}

	inFile.close();

	return mapping;
}

int main( void )
{
	GatewayServer gatewayServer;

	std::string configFilePath = "lib/misc/gateway";

	std::cout << "Loading configuration from " << configFilePath << std::endl;
	
	std::map<std::string, std::string> resources = loadResourcesFromFile(configFilePath);

	if(resources.find("Gateway Port") == resources.end()) {

		std::cout << "No Gateway Port specified in configuration file." << std::endl;
		return 0;
	}
	if(resources.find("MUD Host") == resources.end()) {

		std::cout << "No MUD Host specified in configuration file." << std::endl;
		return 0;
	}
	if(resources.find("MUD Port") == resources.end()) {

		std::cout << "No MUD Port specified in configuration file." << std::endl;
		return 0;
	}

	int secondaryGatewayPort, *secondaryGatewayPortPtr = NULL;
	if(resources.find("Secondary Gateway Port") != resources.end())
	{
		secondaryGatewayPort = atoi(resources["Secondary Gateway Port"].c_str());
		secondaryGatewayPortPtr = &secondaryGatewayPort;
	}
	int mudPort = atoi(resources["MUD Port"].c_str());
	int gatewayPort = atoi(resources["Gateway Port"].c_str());
	std::string mudHost = resources["MUD Host"];

	srand((unsigned int) (time(0)+clock()/2) );

	gatewayServer.setup(gatewayPort, secondaryGatewayPortPtr, mudHost, mudPort);

	gatewayServer.run();
}

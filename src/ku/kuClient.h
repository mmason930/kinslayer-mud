#ifndef KU_CLIENT_H
#define KU_CLIENT_H

#include "kuSockets.h"

class kuClient
{
private:
    std::string pendingOutput;
    std::size_t outputOffset = 0;
public:

	int		port;
	std::string	hostName;
	hostent *hostData;
	int		hasHost;
	SOCKET	sock;
	bool connected;
public:

	kuClient();
	~kuClient(void);

	int receive( std::stringstream &buf );
	bool send(const std::string &data);
    bool flushOutput();
    std::size_t pendingOutputSize() const { return pendingOutput.size() - outputOffset; }
	bool connect(const std::string &h, const int p);
	bool isConnected();
	void disconnect();
};

#define HOSTOK		0
#define HOSTERR		1
#define HOSTLOOKUP	2

#endif
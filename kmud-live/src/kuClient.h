#include "kuSockets.h"

class kuClient
{
private:
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
	bool connect(const std::string &h, const int p);
	bool isConnected();
	void disconnect();
};

#define HOSTOK		0
#define HOSTERR		1
#define HOSTLOOKUP	2

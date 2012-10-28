#ifndef KU_DESCRIPTOR_H
#define KU_DESCRIPTOR_H

#include "kuSockets.h"
#include <string>

class kuDescriptor
{
//Variables
private:
	static int d_TopUID;
	int d_UID;
public:
	SOCKET sock;

	std::string host;
	std::string ipAddress;
	std::string toHost;
	class kuListener *server;

	sockaddr_in sin;

	std::string input;
	std::string output;

	kuDescriptor( class kuListener *server );
	~kuDescriptor();
//Methods
public:
	int getUid();

	void nonBlock();
	void send(const char *str);
	void send(const std::string &str);

	int  socketRead();
	int  socketWrite();
	int  socketClose();

	void socketWriteInstant(const std::string &message);

	std::string getCommand();

	bool hasHost();
	void clearInput();
	void eraseInput(std::string::size_type offset, std::string::size_type length);

	bool socketIsClosed();

	bool hasCommand();

	void disconnect();

	struct HostRetriever *hostRetriever;

	std::string getInputBuffer();

	const std::string &getHost()
	{
		return host;
	}
	const std::string &getIp()
	{
		return ipAddress;
	}

	int getOutputBufferSize();
};

class Address
{
private:
	sockaddr_in udp_Sin;
public:
	Address();
	void operator = (sockaddr_in &csin)
	{
		udp_Sin = csin;
	}
	void operator = (sockaddr &csa)
	{
		udp_Sin = *reinterpret_cast<sockaddr_in *>(&csa);
	}
	sockaddr_in *GetSaAddress();
	sockaddr *GetSinAddress();
	const std::string GetIP();
	const std::string GetHost();
	const u_short GetPort();
};

#endif


#ifndef KU_LISTENER_H
#define KU_LISTENER_H

#include "kuSockets.h"
#include "kuDescriptor.h"
#include <list>
#include <map>
#include <string>

enum e_SocketType
{
	UDP, TCP
};

typedef std::map< int, kuDescriptor * > dMapType;
typedef std::pair< int, kuDescriptor * > dPairType;

class kuListener
{
	//Variables
public:
	e_SocketType l_type;
private:
	u_short	l_Port;
	fd_set inset, outset, excset;
	timeval nulltime;
	void *dataForCloseDescriptorCallback;
	void *dataForOpenDescriptorCallback;
	void *dataForBeforeSocketWriteCallback;
	void *dataForAfterSocketWriteCallback;
	void *dataForSocketReadCallback;
	void (*openDescriptorCallback)(void*, kuDescriptor*);
	void (*closeDescriptorCallback)(void*, kuDescriptor*);
	void (*beforeSocketWriteCallback)(void *, kuDescriptor*, const std::string &output);
	void (*afterSocketWriteCallback)(void *, kuDescriptor*, const std::string &output);
	void (*socketReadCallback)(void *, kuDescriptor*, const std::string &input);

public:
	kuListener(const int port, e_SocketType t);
	~kuListener(void);

	void setCloseDescriptorCallback( void (*closeDescriptorCallback)(void*, kuDescriptor*) );
	void setDataForCloseDescriptorCallback( void *data );

	void setOpenDescriptorCallback( void (*openDescriptorCallback)(void*, kuDescriptor*) );
	void setDataForOpenDescriptorCallback( void *data );
	
	void setBeforeSocketWriteCallback( void (*socketWriteCallback)(void*, kuDescriptor*, const std::string &output) );
	void setDataForBeforeSocketWriteCallback( void *data );

	void setAfterSocketWriteCallback( void (*socketWriteCallback)(void*, kuDescriptor*, const std::string &output) );
	void setDataForAfterSocketWriteCallback( void *data );
	
	void setSocketReadCallback( void (*socketReadCallback)(void*, kuDescriptor*, const std::string &output) );
	void setDataForSocketReadCallback( void *data );

	void handleCloseDescriptor(kuDescriptor *descriptor);
	void handleBeforeSocketWriteCallback(kuDescriptor *descriptor, const std::string &output);
	void handleAfterSocketWriteCallback(kuDescriptor *descriptor, const std::string &output);
	void handleSocketReadCallback(kuDescriptor *descriptor, const std::string &input);

	std::list< kuDescriptor * > l_GetDescriptors();

	u_short l_GetPort();
	bool l_IsBound();
	bool l_IsListening();
	SOCKET l_GetSocket();
	bool l_CanAccept();
	std::pair< kuDescriptor *, bool > l_Accept();
	int l_Select(const int MaxDescs, fd_set *inset, fd_set *outset, fd_set *excset);

	kuDescriptor *l_GetDesc( const int uid );

	std::list< kuDescriptor * > l_AcceptNewHosts();
	void l_Pulse();
	void l_Close();
	bool l_EnableKeepAlive();
private:

	std::map< int, kuDescriptor * > mDescriptors;
	SOCKET l_TCPSocket();
	SOCKET l_UDPSocket();

	int l_Bind(const int port);
	int l_Listen();

	bool l_isBound;
	bool l_isListening;
	SOCKET	l_Sock;

};

#endif

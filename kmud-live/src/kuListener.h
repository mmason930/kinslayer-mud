#ifndef KU_LISTENER_H
#define KU_LISTENER_H

#include "kuSockets.h"
#include "kuDescriptor.h"
#include <list>
#include <map>
#include <string>
#include <functional>

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
	e_SocketType socketType;
private:
	u_short	port;
	fd_set inset, outset, excset;
	timeval nulltime;
	void *dataForCloseDescriptorCallback;
	void *dataForOpenDescriptorCallback;
	void *dataForBeforeSocketWriteCallback;
	void *dataForAfterSocketWriteCallback;
	void *dataForSocketReadCallback;
	void (*openDescriptorCallback)(void*, kuListener *, kuDescriptor*);
	void (*closeDescriptorCallback)(void*, kuListener *, kuDescriptor*);
	void (*beforeSocketWriteCallback)(void *, kuListener *, kuDescriptor*);
	void (*afterSocketWriteCallback)(void *, kuListener *, kuDescriptor*, const std::string &output);
	void (*socketReadCallback)(void *, kuListener *, kuDescriptor*, const std::string &input);

public:
	kuListener(const int port, e_SocketType socketType);
	~kuListener(void);

	void setCloseDescriptorCallback( void (*closeDescriptorCallback)(void*, kuListener*, kuDescriptor*) );
	void setDataForCloseDescriptorCallback( void *data );

	void setOpenDescriptorCallback( void (*openDescriptorCallback)(void*, kuListener*, kuDescriptor*) );
	void setDataForOpenDescriptorCallback( void *data );
	
	void setBeforeSocketWriteCallback( void (*socketWriteCallback)(void*, kuListener*, kuDescriptor*) );
	void setDataForBeforeSocketWriteCallback( void *data );

	void setAfterSocketWriteCallback( void (*socketWriteCallback)(void*, kuListener*, kuDescriptor*, const std::string &output) );
	void setDataForAfterSocketWriteCallback( void *data );
	
	void setSocketReadCallback( void (*socketReadCallback)(void*, kuListener*, kuDescriptor*, const std::string &output) );
	void setDataForSocketReadCallback( void *data );

	void handleCloseDescriptor(kuDescriptor *descriptor);
	void handleBeforeSocketWriteCallback(kuDescriptor *descriptor);
	void handleAfterSocketWriteCallback(kuDescriptor *descriptor, const std::string &output);
	void handleSocketReadCallback(kuDescriptor *descriptor, const std::string &input);

	std::list< kuDescriptor * > getDescriptors();

	u_short getPort();
	bool isBound();
	bool isListening();
	SOCKET getSocket();
	bool canAccept();
	std::pair< kuDescriptor *, bool > accept();
	int select(const int MaxDescs, fd_set *inset, fd_set *outset, fd_set *excset);

	kuDescriptor *getDescriptorById(const int uid);

	std::list< kuDescriptor * > acceptNewHosts();
	void pulse();
	void close();
	bool enableKeepAlive();
private:

	std::map< int, kuDescriptor * > descriptorMap;
	SOCKET createTCPSocket();
	SOCKET createUDPSocket();

	int bind(const int port);
	int listen();

	bool bound;
	bool listening;
	SOCKET socket;

};

#endif

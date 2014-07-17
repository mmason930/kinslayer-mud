#ifndef MUD_STATUS_H
#define MUD_STATUS_H

#include "Enum.h"

class MudStatus : public Enum<MudStatus> {

private:
	MudStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static MudStatus *notRunning;
	static MudStatus *runningNotConnected;
	static MudStatus *booting;
	static MudStatus *running;
	static MudStatus *shuttingDown;
	static MudStatus *hung;
};


#endif



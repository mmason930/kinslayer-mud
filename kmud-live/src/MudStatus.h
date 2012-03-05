#ifndef MUD_STATUS_H
#define MUD_STATUS_H

#include "Enum.h"

class MudStatus : public Enum {

private:

	static std::list<MudStatus*> enums;

	MudStatus(int value, const std::string &standardName) {

		this->value = value;
		this->standardName = standardName;

		enums.push_back(this);
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



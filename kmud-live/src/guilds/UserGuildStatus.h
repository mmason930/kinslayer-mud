#ifndef USER_GUILD_STATUS_H
#define USER_GUILD_STATUS_H

#include "../Enum.h"
#include <list>

class UserGuildStatus : public Enum<UserGuildStatus> {

private:
	UserGuildStatus(int value, const std::string &standardName, bool isActive) : Enum(value, standardName)
	{
		this->isActive = isActive;
	}

	bool isActive;

public:

	bool getIsActive() const { return isActive; }

	static std::list<UserGuildStatus*> getActiveStatuses()
	{
		std::list<UserGuildStatus*> statusList;
		for(auto iter = getStartIterator();iter != getEndIterator();++iter)
		{
			if( (*iter)->getIsActive() )
				statusList.push_back( (*iter) );
		}

		return statusList;
	}

	static UserGuildStatus *active;
	static UserGuildStatus *removed;
	static UserGuildStatus *quit;
};

#endif

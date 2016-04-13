#ifndef USER_LOGOUT_TYPE_H
#define USER_LOGOUT_TYPE_H

#include <list>
#include "Enum.h"

class UserLogoutType : public Enum<UserLogoutType> {

private:
	UserLogoutType(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static UserLogoutType *rent;
	static UserLogoutType *idle;
	static UserLogoutType *linklessIdle;
	static UserLogoutType *quit;
	static UserLogoutType *quitWithRent;
	static UserLogoutType *purge;
	static UserLogoutType *reboot;
	static UserLogoutType *scriptExtraction;
	static UserLogoutType *unknown;
	static UserLogoutType *notRealLogout;
};


#endif


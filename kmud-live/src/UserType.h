#ifndef USER_TYPE_H
#define USER_TYPE_H

#include <list>
#include "Enum.h"

class UserType : public Enum<UserType> {

private:
	UserType(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:
	static UserType *player;
	static UserType *mob;
};

#endif

#ifndef USER_TYPE_H
#define USER_TYPE_H

#include <list>
#include "Enum.h"

class UserType : public Enum {

private:

	static std::list<Enum*> enums;

	UserType(int value, const std::string &standardName) : Enum(value, standardName){

		enums.push_back(this);
	}

public:

	static Enum *getEnumByValue(int v)
	{
		for(auto iter = enums.begin();iter != enums.end();++iter)
		{
			Enum *e = (*iter);
			if( e->getValue() == v )
			{
				return e;
			}
		}
		
		return NULL;
	}

	static UserType *player;
	static UserType *mob;
};

#endif

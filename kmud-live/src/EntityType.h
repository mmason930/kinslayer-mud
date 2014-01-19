#ifndef USER_LOGOUT_TYPE_H
#define USER_LOGOUT_TYPE_H

#include <list>
#include "Enum.h"

class EntityType : public Enum {

private:

	static std::list<EntityType*> enums;

	EntityType(int value, const std::string &standardName) : Enum(value, standardName)
	{
		enums.push_back(this);
	}

public:

	static EntityType *mob;
	static EntityType *player;
	static EntityType *object;
	static EntityType *room;
};


#endif


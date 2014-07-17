#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

#include <list>
#include "Enum.h"

class EntityType : public Enum<EntityType> {

private:
	EntityType(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:

	static EntityType *mob;
	static EntityType *player;
	static EntityType *object;
	static EntityType *room;
};


#endif


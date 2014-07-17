#include "EntityType.h"

template < >
std::list<Enum<EntityType>*> Enum<EntityType>::enums = std::list<Enum<EntityType>*>();

EntityType *EntityType::mob = new EntityType(0, "Mob");
EntityType *EntityType::player = new EntityType(1, "Player");
EntityType *EntityType::object = new EntityType(2, "Object");
EntityType *EntityType::room = new EntityType(3, "Room");

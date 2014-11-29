#include "EntityType.h"

EntityType *EntityType::mob = new EntityType(0, "Mob");
EntityType *EntityType::player = new EntityType(1, "Player");
EntityType *EntityType::object = new EntityType(2, "Object");
EntityType *EntityType::room = new EntityType(3, "Room");

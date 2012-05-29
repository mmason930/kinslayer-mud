#include "UserType.h"

std::list<Enum*> UserType::enums;

UserType *UserType::player = new UserType(0, "Player");
UserType *UserType::mob = new UserType(1, "Mob");

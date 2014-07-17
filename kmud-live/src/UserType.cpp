#include "UserType.h"

template < >
std::list<Enum<UserType>*> Enum<UserType>::enums = std::list<Enum<UserType>*>();

UserType *UserType::player = new UserType(0, "Player");
UserType *UserType::mob = new UserType(1, "Mob");

#pragma once
#include "jsoncpp/json.h"
#include <string>
class Character;
Json::Value clientShopSnapshot(Character *ch);
std::string clientShopCommand(Character *ch, const Json::Value &request);

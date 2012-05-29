#ifndef CHARACTER_UTIL_UTIL_H
#define CHARACTER_UTIL_UTIL_H

#include <mysql/sqlDatabase.h>

class PlayerIndex;
class Character;

class CharacterUtil
{
private:
protected:
public:
	static Character *loadCharacter(const int userId);
	static Character *loadCharacter(const std::string &username);
	static void changeUserPassword(sql::Connection connection, const int userId, const std::string &userPassword);
	static Character *getOnlineCharacterById(const int userId);
	static PlayerIndex *getPlayerIndexByUserName(const std::string &userName);
	static PlayerIndex *getPlayerIndexByUserId(const int userId);
};

#endif

#ifndef CHARACTER_UTIL_UTIL_H
#define CHARACTER_UTIL_UTIL_H

#include <mysql/sqlDatabase.h>
#include <list>

class PlayerIndex;
class Character;
class UserEmailAddress;
class UserEmailAddressConfirmation;

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
	static UserEmailAddress *getUserEmailAddress(sql::Connection connection, const int userEmailAddressId);
	static UserEmailAddress *getUserEmailAddress(sql::Row &row);
	static std::list<UserEmailAddress *> getUserEmailAddresses(sql::Connection connection, const int userId);
	static void putUserEmailAddress(sql::Connection connection, UserEmailAddress *userEmailAddress);
	static UserEmailAddressConfirmation *getUserEmailAddressConfirmation(sql::Connection connection, const std::string confirmationKey);
	static UserEmailAddressConfirmation *getUserEmailAddressConfirmationByUserEmailAddressId(sql::Connection connection, const int userEmailAddressId);
	static UserEmailAddressConfirmation *getUserEmailAddressConfirmation(sql::Row row);
	static void putUserEmailAddressConfirmation(sql::Connection connection, UserEmailAddressConfirmation *userEmailAddressConfirmation);
	static void sendUserEmailAddressConfirmationEmail(sql::Connection connection, Character *character, const UserEmailAddress *userEmailAddress, const UserEmailAddressConfirmation *userEmailAddressConfirmation);
	static void freeUserEmailAddresses(std::list<UserEmailAddress*> userEmailAddresses);
	static std::list<class UserMacro *> getUserMacros(sql::Connection connection, const unsigned int userId);
	static class UserMacro *getUserMacro(sql::Row row);
	static UserMacro *getUserMacroMeetingCriteria(sql::Connection connection, const std::string &criteria);
	static UserMacro *getUserMacro(sql::Connection connection, int userMacroId);
	static UserMacro *getUserMacro(sql::Connection connection, int userId, const unsigned short &keyCode);
	static void deleteUserMacro(sql::Connection connection, int userId, const unsigned short &keyCode);
	static void deleteUserMacro(sql::Connection connection, int userMacroId);
	static void putUserMacro(sql::Connection connection, class UserMacro *);
	static void freeUserMacros(std::list<class UserMacro *> &userMacros);
	static bool isValidUserName(const std::string username);
};

#endif

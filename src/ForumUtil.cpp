#include <sstream>

#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "db.h"
#include "utils.h"

#include "CharacterUtil.h"
#include "ForumUtil.h"
#include "StringUtil.h"

void ForumUtil::addForumUser(const int userId)
{
	Character *character = CharacterUtil::loadCharacter(userId);

	addForumUser(character);

	delete character;
}
void ForumUtil::addForumUser(Character *character)
{
	std::stringstream queryBuffer;
	sql::Query query;

	int userType = 0;
	int groupId = 2;
	std::string userPermissions = "";
	int userPermissionsFrom = 0;
	std::string userIp = "";

	DateTime registrationDate = character->player.time.birth;
	std::string username = character->player.name;
	std::string usernameClean = StringUtil::allLower(username.c_str());
	std::string userPassword = character->player.passwd;
	int passwordChanged = 0;
	int userPasswordConvert = 0;
	std::string userEmail = "";
	std::string userEmailHash = "";
	std::string userBirthday = "";
	int userLastVisit = 0;
	int userLastMark = 0;
	int userLastPostTime = 0;

	queryBuffer
		<< "INSERT INTO phpbb_users(" 	
		<< " `user_id`,"
		<< " `user_type`,"
		<< " `group_id`,"
		<< " `user_permissions`,"
		<< " `user_perm_from`,"
		<< " `user_ip`,"
		<< " `user_regdate`,"
		<< " `username`,"
		<< " `username_clean`,"
		<< " `user_password`,"
		<< " `user_passchg`,"
		<< " `user_pass_convert`,"
		<< " `user_email`,"
		<< " `user_email_hash`,"
		<< " `user_birthday`,"
		<< " `user_lastvisit`,"
		<< " `user_lastmark`,"
		<< " `user_lastpost_time`"
		<< ") VALUES ("
		<< character->player.idnum << ","
		<< userType << ","
		<< groupId << ","
		<< sql::escapeQuoteString(userPermissions) << ","
		<< userPermissionsFrom << ","
		<< sql::escapeQuoteString(userIp) << ","
		<< registrationDate.getTime() << ","
		<< sql::escapeQuoteString(username) << ","
		<< sql::escapeQuoteString(usernameClean) << ","
		<< sql::escapeQuoteString(userPassword) << ","
		<< passwordChanged << ","
		<< userPasswordConvert << ","
		<< sql::escapeQuoteString(userEmail) << ","
		<< sql::escapeQuoteString(userEmailHash) << ","
		<< sql::escapeQuoteString(userBirthday) << ","
		<< userLastVisit << ","
		<< userLastMark << ","
		<< userLastPostTime
		<< ")";

	try {
		gameDatabase->sendRawQuery(queryBuffer.str());
	}
	catch(sql::QueryException e) {

		MudLog(BRF, LVL_APPR, TRUE, "Could not add user to forum user table. User Id: %d, Username: %s.\n%s", character->player.idnum, character->player.name.c_str(), e.getMessage().c_str());
		return;
	}
}

void ForumUtil::archiveAndRemoveDeletedForumUsers(sql::Connection connection)
{
	std::stringstream queryBuffer;
	
	queryBuffer << " INSERT IGNORE INTO phpbb_usersArchive"
				<< " SELECT"
				<< "   phpbb_users.*"
				<< " FROM"
				<< "   phpbb_users"
				<< " LEFT JOIN users ON phpbb_users.user_id = users.user_id"
				<< " WHERE users.user_id IS NULL";

	connection->sendRawQuery(queryBuffer.str());

	queryBuffer.str("");

	queryBuffer << " DELETE"
				<< "   phpbb_users.*"
				<< " FROM"
				<< "   phpbb_users"
				<< " LEFT JOIN users ON phpbb_users.user_id = users.user_id"
				<< " WHERE users.user_id IS NULL";

	connection->sendRawQuery(queryBuffer.str());

}
void ForumUtil::addUsersToForum(sql::Connection connection)
{
	//Query for all users who do not yet have a forum account.
	std::stringstream sql;

	sql << " SELECT users.user_id"
		<< " FROM users"
		<< " LEFT JOIN phpbb_users ON phpbb_users.user_id = users.user_id"
		<< " WHERE phpbb_users.user_id IS NULL";

	sql::Query query = connection->sendQuery(sql.str());
	sql::Row row;

	while(query->hasNextRow())
	{
		row = query->getRow();
		int userId = row.getInt("user_id");
		ForumUtil::addForumUser(userId);
	}
}

void ForumUtil::changeUserPassword(sql::Connection connection, const int userId, const std::string &userPassword)
{
	std::stringstream sql;

	sql << " UPDATE phpbb_users"
		<< " SET user_password = " << sql::escapeQuoteString(userPassword)
		<< " WHERE user_id = " << userId;

	connection->sendRawQuery(sql.str());
}

void ForumUtil::synchronizeUserConfigurationValues(sql::Connection connection)
{
	std::stringstream sql;
	sql::Query query;
	sql::Row row;
	bool hasAtLeastOneUser = false;

	int newestUserId;
	int numberOfUsers;
	std::string newestUsername;
	std::string newestUserColor;

	sql << " SELECT"
		<< "   `user_id``"
		<< "   `username`,"
		<< "   `user_colour`"
		<< " FROM phpbb_users"
		<< " ORDER BY user_regdate"
		<< " DESC LIMIT 1";

	query = connection->sendQuery(sql.str());

	if(query->hasNextRow())
	{
		row = query->getRow();
		newestUsername = row["username"];
		newestUserColor = row["user_colour"];
		newestUserId = row.getInt("user_id");

		hasAtLeastOneUser = true;
	}

	sql.str("");
	sql << " SELECT COUNT(*) AS num_users FROM phpbb_users";

	query = connection->sendQuery(sql.str());
	if(query->hasNextRow())
	{
		numberOfUsers = query->getRow().getInt("num_users");
	}

	
	if(hasAtLeastOneUser)
	{
		//Update the newest user ID
		sql.str("");
		sql << " UPDATE phpbb_config"
			<< " SET config_value = " << newestUserId
			<< " WHERE config_name = 'newest_user_id'";

		connection->sendRawQuery(sql.str());

		//Update the newest username
		sql.str("");
		sql << " UPDATE phpbb_config"
			<< " SET config_value = " << sql::escapeQuoteString(newestUsername)
			<< " WHERE config_name = 'newest_username'";

		connection->sendRawQuery(sql.str());

		//Update the newest user color.
		sql.str("");
		sql << " UPDATE phpbb_config"
			<< " SET config_value = " << sql::escapeQuoteString(newestUserColor)
			<< " WHERE config_name = 'newest_user_colour'";

		connection->sendRawQuery(sql.str());

		//Update the number of users.
		sql.str("");
		sql << " UPDATE phpbb_config"
			<< " SET config_value = " << numberOfUsers
			<< " WHERE config_name = 'num_users'";

		connection->sendRawQuery(sql.str());
	}
}

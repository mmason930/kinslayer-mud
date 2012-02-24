#ifndef FORUM_UTIL_UTIL_H
#define FORUM_UTIL_UTIL_H

class ForumUtil
{
private:
protected:
public:
	static void addForumUser(Character *character);
	static void addForumUser(const int userId);

	static void addUsersToForum(sql::Connection connection);
};

#endif

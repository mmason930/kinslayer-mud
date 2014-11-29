#ifndef GUILD_UTIL_H
#define GUILD_UTIL_H

#include <map>
#include <list>
#include <mysql/sqlDatabase.h>

class Guild;
class GuildApplication;
class UserGuild;

class GuildUtil
{
private:
	static GuildUtil *self;

	std::map<int, Guild *> guildMap;
	std::map<int, GuildApplication *> guildApplicationMap;
	std::map<int, UserGuild *> userGuildMap;
	std::map<int, std::list<UserGuild *> * > userIdToUserGuildsMap;

	GuildUtil();
	~GuildUtil();
protected:
	int coppersToCreateNewGuild;
	int maxGuildNameLength;
	int maxGuildDescriptionLength;
	int maxDeniedReasonLength;
	int minimumLevelToSubmitGuildApplication;
public:

	static GuildUtil *get();
	static void destroy();

	std::map<int, Guild *> getGuildMap(sql::Connection connection);
	Guild *getGuild(const sql::Row row) const;
	GuildApplication *getGuildApplication(const sql::Row row) const;
	void putGuildApplication(sql::Connection connection, GuildApplication *guildApplication) const;

	void putGuild(sql::Connection connection, Guild *guild) const;

	void loadGuildsFromDatabase(sql::Connection connection);
	void loadGuildApplicationsFromDatabase(sql::Connection connection);

	GuildApplication *submitGuildApplication(sql::Connection connection, int userId, const std::string &guildName, const std::string &guildDescription, const int coppersCharged);
	void denyGuildApplication(sql::Connection connection, int guildApplicationId, int deniedByUserId, const std::string &deniedReason);
	void approveGuildApplication(sql::Connection connection, int guildApplicationId, int approvedByUserId);
	Guild *createGuild(sql::Connection connection, int guildApplicationId);
	void initNewlyCreatedGuild(sql::Connection connection, int guildId);

	GuildApplication *findGuildApplication(const std::function<bool(GuildApplication *)> &predicate) const;

	UserGuild *getUserGuild(const sql::Row row) const;
	void putUserGuild(sql::Connection connection, UserGuild *userGuild) const;
	void loadUserGuildsFromDatabase(sql::Connection connection);

	std::list<UserGuild *> getUserGuildsByUserId(int userId) const;
	std::list<UserGuild *> getActiveUserGuildsByUserId(int userId) const;
	void addCharacterToGuild(sql::Connection connection, int guildId, int userId);

	std::list<GuildApplication *> getGuildApplicationsRequiringReview() const;
	GuildApplication *getGuildApplication(int guildApplicationId) const;

	std::map<int, Guild *> getGuildMap();

	int getCoppersToCreateNewGuild() const;
	int getMaxGuildNameLength() const;
	int getMaxGuildDescriptionLength() const;
	int getMaxDeniedReasonLength() const;
	int getMinimumLevelToSubmitGuildApplication() const;
};

#endif
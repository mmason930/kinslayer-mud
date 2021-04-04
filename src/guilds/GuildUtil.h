#ifndef GUILD_UTIL_H
#define GUILD_UTIL_H

#include <map>
#include <vector>
#include <mysql/sqlDatabase.h>

class Guild;
class GuildStatus;
class GuildApplication;
class GuildApplicationStatus;
class GuildApplicationSignature;
class GuildApplicationSignatureStatus;
class UserGuild;
class UserGuildStatus;
class GuildJoinApplication;
class GuildJoinApplicationStatus;
class GuildRank;
class GuildRankRole;
class GuildRankStatus;
class GuildPrivilege;

typedef void *(commandFunction)(Character *, char *, int, int);

class GuildUtil
{
private:
	static GuildUtil *self;

	std::map<int, Guild *> guildMap;

	std::map<int, GuildApplication *> guildApplicationMap;

	std::map<int, UserGuild *> userGuildMap;
	std::map<int, std::vector<UserGuild *> * > userIdToUserGuildsMap;
	std::map<int, std::vector<UserGuild *> * > guildIdToUserGuildsMap;

	std::map<int, GuildApplicationSignature *> guildApplicationSignatureMap;
	std::map<int, std::vector<GuildApplicationSignature *> * > userIdToGuildApplicationSignaturesMap;
	std::map<int, std::vector<GuildApplicationSignature *> * > guildApplicationIdToGuildApplicationSignaturesMap;

	std::map<int, GuildJoinApplication*> guildJoinApplicationMap;
	std::map<int, std::vector<GuildJoinApplication*> *> userIdToGuildJoinApplicationsMap;
	std::map<int, std::vector<GuildJoinApplication *> *> guildIdToGuildJoinApplicationsMap;

	std::map<int, GuildRank*> guildRankMap;
	std::map<int, std::vector<GuildRank*> *> guildIdToGuildRanksMap;

	std::map<int, GuildRankRole*> guildRankRoleMap;
	std::map<int, std::vector<GuildRankRole*> *> guildRankIdToGuildRankRolesMap;

	GuildUtil();
	~GuildUtil();
protected:
	int coppersToCreateNewGuild;
	int maxGuildNameLength;
	int maxGuildDescriptionLength;
	int maxDeniedReasonLength;
	int minimumLevelToSubmitGuildApplication;
	int numberOfRequiredSignaturesForNewGuild;
	int minimumLevelToJoinGuild;
public:

	static GuildUtil *get();
	static void destroy();

	std::map<int, Guild *> getGuildMap(sql::Connection connection);
	Guild *getGuild(const sql::Row row) const;
	Guild *getGuild(int guildId) const;
	GuildApplication *getGuildApplication(const sql::Row row) const;
	void putGuildApplication(sql::Connection connection, GuildApplication *guildApplication) const;

	void putGuild(sql::Connection connection, Guild *guild) const;

	void loadGuildsFromDatabase(sql::Connection connection);
	void loadGuildApplicationsFromDatabase(sql::Connection connection);

	GuildApplication *submitGuildApplication(sql::Connection connection, int userId, int userRace, const std::string &guildName, const std::string &guildDescription, const int coppersCharged);
	void denyGuildApplication(sql::Connection connection, int guildApplicationId, int deniedByUserId, const std::string &deniedReason);
	void approveGuildApplication(sql::Connection connection, int guildApplicationId, int approvedByUserId);
	Guild *createGuild(sql::Connection connection, int guildApplicationId);
	void initNewlyCreatedGuild(sql::Connection connection, int guildApplicationId, int guildId);

	GuildApplication *findGuildApplication(const std::function<bool(GuildApplication *)> &predicate) const;

	UserGuild *getUserGuild(const sql::Row row) const;
	void putUserGuild(sql::Connection connection, UserGuild *userGuild) const;
	void loadUserGuildsFromDatabase(sql::Connection connection);

	std::vector<UserGuild *> getUserGuildsByUserId(int userId, const std::vector<UserGuildStatus*> userGuildStatuses = std::vector<UserGuildStatus*>()) const;
	std::vector<UserGuild *> getActiveUserGuildsByUserId(int userId) const;
	std::vector<UserGuild *> getActiveUserGuildsByGuildId(int guildId) const;
	void addCharacterToGuild(sql::Connection connection, int guildId, int userId, bool announceToPlayer);

	GuildApplicationSignature *getGuildApplicationSignature(const sql::Row row) const;
	GuildApplicationSignature *getGuildApplicationSignature(int guildApplicationSignatureId) const;
	void putGuildApplicationSignature(sql::Connection connection, GuildApplicationSignature *guildApplicationSignature) const;
	void loadGuildApplicationSignaturesFromDatabase(sql::Connection connection);
	std::vector<GuildApplicationSignature*> getGuildApplicationSignaturesByUserId(int userId) const;
	std::vector<GuildApplicationSignature*> getGuildApplicationSignaturesByUserId(int userId, std::vector<GuildApplicationSignatureStatus*> statuses) const;
	std::vector<GuildApplicationSignature*> getGuildApplicationSignaturesByGuildApplicationId(int guildApplicationId) const;
	std::vector<GuildApplicationSignature*> getGuildApplicationSignaturesByGuildApplicationId(int guildApplicationId, std::vector<GuildApplicationSignatureStatus*> statuses) const;
	int getNumberOfValidGuildApplicationSignatures(int guildApplicationId) const;
	GuildApplicationSignature *submitGuildApplicationSignature(sql::Connection connection, int userId, int guildApplicationId);
	void removeGuildSignatureByUser(sql::Connection connection, int guildApplicationSignatureId);
	void approveSignature(sql::Connection connection, int guildApplicationSignatureId, int approvedByUserId);
	void denyGuildApplicationSignature(sql::Connection connection, int guildApplicationSignatureId, int deniedByUserId);

	//Guild Join Applications.
	void loadGuildJoinApplicationsFromDatabase(sql::Connection connection);
	GuildJoinApplication *getGuildJoinApplication(const sql::Row row) const;
	void putGuildJoinApplication(sql::Connection connection, GuildJoinApplication *guildJoinApplication) const;
	GuildJoinApplication *getGuildJoinApplication(int guildJoinApplicationId) const;
	std::vector<GuildJoinApplication *> getGuildJoinApplications(const std::optional<int> &userId, const std::optional<int> &guildId, const std::vector<GuildJoinApplicationStatus *> &statuses = std::vector<GuildJoinApplicationStatus *>());
	GuildJoinApplication *submitGuildJoinApplication(sql::Connection connection, int userId, int guildId, const std::string &messageToGuild);
	void removeGuildJoinApplication(sql::Connection connection, int removedByUserId, int guildJoinApplicationId);

	//Guild Ranks
	void loadGuildRanksFromDatabase(sql::Connection connection);
	GuildRank *getGuildRank(const sql::Row row) const;
	void putGuildRank(sql::Connection connection, GuildRank *guildRank) const;
	GuildRank *getGuildRank(int guildRankId) const;
	std::vector<GuildRank *> getGuildRanks(const std::optional<int> guildId, const std::vector<GuildRankStatus*> &statuses = std::vector<GuildRankStatus*>()) const;
	void removeGuildRank(sql::Connection connection, int userId, int guildRankId);
	void applyChangesToGuildRank(sql::Connection connection, GuildRank *guildRank, const std::map<int, GuildRankRole*> &guildPrivilegeIdToGuildRankRoleMap);
	void addGuildRank(sql::Connection connection, GuildRank *guildRank);

	//Guild Rank Roles
	void loadGuildRankRolesFromDatabase(sql::Connection connection);
	GuildRankRole *getGuildRankRole(const sql::Row row) const;
	void putGuildRankRole(sql::Connection connection, GuildRankRole *guildRankRole) const;
	std::vector<GuildRankRole *> getGuildRankRoles(std::optional<int> guildRankId) const;
	void addGuildRankRole(sql::Connection connection, GuildRankRole *guildRankRole);
	void removeGuildRankRole(sql::Connection connection, int guildRankRoleId);
	std::map<int, GuildRankRole*> getGuildPrivilegeIdToGuildRankRoleMap(int guildRankId) const;


	bool hasPrivilege(int guildId, int userId, GuildPrivilege *guildPrivilege) const;

	std::vector<GuildApplication *> getGuildApplicationsRequiringReview() const;
	std::vector<GuildApplication *> getGuildApplications(const std::optional<int> &userId = std::optional<int>(), const std::vector<GuildApplicationStatus *> statuses = std::vector<GuildApplicationStatus*>()) const;
	std::vector<GuildApplication *> getGuildApplicationsSorted() const;
	GuildApplication *getGuildApplication(int guildApplicationId) const;

	std::vector<Guild *> getGuilds(const std::vector<GuildStatus*> &statuses = std::vector<GuildStatus*>());
	std::map<int, Guild *> getGuildMap();
	int getNumberOfActiveGuildMembers(int guildId) const;

	void guildsCommandHandler(Character *ch, char *argument, int cmd, int subcmd);

	void removeUserFromGuild(sql::Connection connection, int userGuildId);

	int getCoppersToCreateNewGuild() const;
	int getMaxGuildNameLength() const;
	int getMaxGuildDescriptionLength() const;
	int getMaxDeniedReasonLength() const;
	int getMinimumLevelToSubmitGuildApplication() const;
	int getNumberOfRequiredSignaturesForNewGuild() const;
	int getMinimumLevelToJoinGuild() const;
};

#endif
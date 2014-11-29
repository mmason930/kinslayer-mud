#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../Game.h"
#include "../dbutils/QueryUtil.h"
#include "../dbutils/StoreDataObjectSQLBuilder.h"
#include "../EntityType.h"

#include "GuildUtil.h"
#include "Guild.h"
#include "GuildApplication.h"
#include "UserGuild.h"

GuildUtil *GuildUtil::self = nullptr;

GuildUtil::GuildUtil()
{
	this->coppersToCreateNewGuild = 100000; //10 gold.
	this->maxGuildNameLength = 40;
	this->maxGuildDescriptionLength = 255;
	this->maxDeniedReasonLength = 255;
	this->minimumLevelToSubmitGuildApplication = 30;
}

GuildUtil::~GuildUtil()
{
	for(auto iter : guildMap)
		delete iter.second;
	for(auto iter : guildApplicationMap)
		delete iter.second;
	for(auto iter : userGuildMap)
		delete iter.second;
	for(auto userGuilds : userIdToUserGuildsMap)
		delete userGuilds.second;
}

GuildUtil *GuildUtil::get()
{
	return !GuildUtil::self ? (GuildUtil::self = new GuildUtil()) : GuildUtil::self;
}

void GuildUtil::destroy()
{
	delete self;
}

Guild *GuildUtil::getGuild(const sql::Row row) const
{
	Guild *guild = new Guild();

	guild->setId(row.getInt("id"));
	guild->setName(row.getString("name"));
	guild->setCreatedDatetime(DateTime(row.getTimestamp("created_datetime")));
	guild->setCreatedByUserId(row.getInt("created_by_user_id"));
	guild->setStatus(GuildStatus::getEnumByValue(row.getInt("status")));
	guild->setStatusLastModifiedDatetime(DateTime(row.getTimestamp("status_last_modified_datetime")));
	
	return guild;
}

void GuildUtil::putGuild(sql::Connection connection, Guild *guild) const
{
	auto builder = std::auto_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "guild"));

	builder
		->put("id", guild->getId())
		->put("name", guild->getName())
		->put("created_datetime", guild->getCreatedDatetime())
		->put("created_by_user_id", guild->getCreatedByUserId())
		->put("status", guild->getStatus())
		->put("status_last_modified_datetime", guild->getStatusLastModifiedDatetime());
	
	builder->execute(*guild);
}

GuildApplication *GuildUtil::getGuildApplication(const sql::Row row) const
{
	GuildApplication *guildApplication = new GuildApplication();

	guildApplication->setId(row.getInt("id"));
	guildApplication->setGuildName(row.getString("guild_name"));
	guildApplication->setGuildDescription(row.getString("guild_description"));
	guildApplication->setSubmittedDatetime(DateTime(row.getTimestamp("submitted_datetime")));
	guildApplication->setSubmittedByUserId(row.getInt("submitted_by_user_id"));
	guildApplication->setStatus(GuildApplicationStatus::getEnumByValue(row.getInt("status")));
	guildApplication->setCompletedDatetime(QueryUtil::get()->getNullableDateTime(row, "completed_datetime"));
	guildApplication->setReviewerUserId(row.getNullableInt("reviewer_user_id"));
	guildApplication->setDeniedReason(row.getString("denied_reason"));
	guildApplication->setGuildId(row.getNullableInt("guild_id"));
	guildApplication->setCoppersCharged(row.getInt("coppers_charged"));
	
	return guildApplication;
}

void GuildUtil::putGuildApplication(sql::Connection connection, GuildApplication *guildApplication) const
{
	auto builder = std::auto_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "guildApplication"));

	builder
		->put("id", guildApplication->getId())
		->put("guild_name", guildApplication->getGuildName())
		->put("guild_description", guildApplication->getGuildDescription())
		->put("submitted_datetime", guildApplication->getSubmittedDatetime())
		->put("status", guildApplication->getStatus())
		->put("completed_datetime", guildApplication->getCompletedDatetime())
		->put("reviewer_user_id", guildApplication->getReviewerUserId())
		->put("denied_reason", guildApplication->getDeniedReason())
		->put("guild_id", guildApplication->getGuildId())
		->put("coppers_charged", guildApplication->getCoppersCharged());

	builder->execute(*guildApplication);
}

std::map<int, Guild *> GuildUtil::getGuildMap(sql::Connection connection)
{
	return QueryUtil::get()->loadDataObjectMapFromDatabase<Guild *, int>(
		connection,
		"guild",
		std::bind(&GuildUtil::getGuild, this, std::placeholders::_1),
		[](Guild *guild) -> int { return guild->getId(); }
	);
}

void GuildUtil::loadGuildsFromDatabase(sql::Connection connection)
{
	this->guildMap = getGuildMap(connection);
}

std::map<int, Guild *> GuildUtil::getGuildMap()
{
	return guildMap;
}

void GuildUtil::loadGuildApplicationsFromDatabase(sql::Connection connection)
{
	this->guildApplicationMap = QueryUtil::get()->loadDataObjectMapFromDatabase<GuildApplication *, int>(
		connection,
		"guildApplication",
		[=](sql::Row row) { return GuildUtil::get()->getGuildApplication(row); },
		//std::bind(&GuildUtil::getGuildApplication, this, std::placeholders::_1),
		[](GuildApplication *guildApplication) -> int { return guildApplication->getId(); }
	);
}

GuildApplication *GuildUtil::findGuildApplication(const std::function<bool(GuildApplication *)> &predicate) const
{
	std::find_if(guildApplicationMap.begin(), guildApplicationMap.end(),
		[&](const std::map<int, GuildApplication *>::value_type & itr) -> bool
		{
			return predicate(itr.second);
		}
	);

	return nullptr;
}

GuildApplication *GuildUtil::submitGuildApplication(sql::Connection connection, int userId, const std::string &guildName, const std::string &guildDescription, const int coppersCharged)
{
	GuildApplication *guildApplication = new GuildApplication();

	guildApplication->setSubmittedDatetime(DateTime());
	guildApplication->setGuildDescription(guildDescription);
	guildApplication->setGuildName(guildName);
	guildApplication->setSubmittedByUserId(userId);
	guildApplication->setStatus(GuildApplicationStatus::pending);
	guildApplication->setCoppersCharged(coppersCharged);
	
	putGuildApplication(connection, guildApplication);

	return guildApplicationMap[guildApplication->getId()] = guildApplication;
}

void GuildUtil::denyGuildApplication(sql::Connection connection, int guildApplicationId, int deniedByUserId, const std::string &deniedReason)
{
	GuildApplication *guildApplication = guildApplicationMap[guildApplicationId];

	guildApplication->setCompletedDatetime(DateTime());
	guildApplication->setDeniedReason(deniedReason);
	guildApplication->setReviewerUserId(deniedByUserId);
	guildApplication->setStatus(GuildApplicationStatus::denied);
	
	putGuildApplication(connection, guildApplication);
}

//Perform approval for Guild application.
void GuildUtil::approveGuildApplication(sql::Connection connection, int guildApplicationId, int approvedByUserId)
{
	GuildApplication *guildApplication = guildApplicationMap[guildApplicationId];

	guildApplication->setCompletedDatetime(DateTime());
	guildApplication->setReviewerUserId(approvedByUserId);
	guildApplication->setStatus(GuildApplicationStatus::approved);

	putGuildApplication(connection, guildApplication);
}

//Create and store new Guild from its Application.
Guild *GuildUtil::createGuild(sql::Connection connection, int guildApplicationId)
{
	GuildApplication *guildApplication = guildApplicationMap[guildApplicationId];
	Guild *guild = new Guild();

	guild->setCreatedByUserId(guildApplication->getSubmittedByUserId());
	guild->setCreatedDatetime(guildApplication->getCompletedDatetime().get());
	guild->setName(guildApplication->getGuildName());
	guild->setStatus(GuildStatus::active);
	guild->setStatusLastModifiedDatetime(guildApplication->getCompletedDatetime().get());

	putGuild(connection, guild);

	return guildMap[guild->getId()] = guild;
}

//For a newly created Guild, perform tasks such as adding members who submitted the application.
void GuildUtil::initNewlyCreatedGuild(sql::Connection connection, int guildId)
{
	Guild *guild = guildMap[guildId];

	this->addCharacterToGuild(connection, guildId, guild->getCreatedByUserId());
}

GuildApplication *GuildUtil::getGuildApplication(int guildApplicationId) const
{
	auto iter = guildApplicationMap.find(guildApplicationId);

	return iter == guildApplicationMap.end() ? nullptr : iter->second;
}

UserGuild *GuildUtil::getUserGuild(const sql::Row row) const
{
	UserGuild *userGuild = new UserGuild();

	userGuild->setId(row.getInt("id"));
	userGuild->setUserId(row.getInt("user_id"));
	userGuild->setGuildId(row.getInt("guild_id"));
	userGuild->setJoinedDatetime(DateTime(row.getTimestamp("joined_datetime")));
	userGuild->setStatus(UserGuildStatus::getEnumByValue(row.getInt("status")));
	userGuild->setStatusLastModifiedDatetime(DateTime(row.getTimestamp("status_last_modified_datetime")));

	return userGuild;
}

void GuildUtil::putUserGuild(sql::Connection connection, UserGuild *userGuild) const
{
	auto builder = std::auto_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "userGuild"));

	builder
		->put("user_id", userGuild->getUserId())
		->put("guild_id", userGuild->getGuildId())
		->put("joined_datetime", userGuild->getJoinedDatetime())
		->put("status", userGuild->getStatus())
		->put("status_last_modified_datetime", userGuild->getStatusLastModifiedDatetime());

	builder->execute(*userGuild);
}

void GuildUtil::loadUserGuildsFromDatabase(sql::Connection connection)
{
	this->userGuildMap = QueryUtil::get()->loadDataObjectMapFromDatabase<UserGuild *, int>(
		connection,
		"userGuild",
		std::bind(&GuildUtil::getUserGuild, this, std::placeholders::_1),
		[](UserGuild *userGuild) -> int { return userGuild->getId(); }
	);

	//Build the User ID to User Guild map.
	for(auto iter : userGuildMap) {
		auto userGuild = iter.second;
		auto userGuildListIter = userGuildMap.find(userGuild->getUserId());
		
		std::list<UserGuild *> *userGuildList = nullptr;

		if(userGuildListIter == userGuildMap.end())
		{
			userGuildList = new std::list<UserGuild *>();
			userIdToUserGuildsMap[userGuild->getUserId()] = userGuildList;
		}
		else
			userGuildList = userIdToUserGuildsMap[userGuild->getUserId()];
		
		userGuildList->push_back(userGuild);
	}
}

std::list<UserGuild *> GuildUtil::getUserGuildsByUserId(int userId) const
{
	auto iter = userIdToUserGuildsMap.find(userId);

	return iter == userIdToUserGuildsMap.end() ? std::list<UserGuild *>() : *(iter->second);
}

std::list<UserGuild *> GuildUtil::getActiveUserGuildsByUserId(int userId) const
{
	std::list<UserGuild *> userGuilds;

	for(UserGuild *userGuild : getUserGuildsByUserId(userId))
	{
		if(userGuild->getStatus()->getIsActive())
			userGuilds.push_back(userGuild);
	}

	return userGuilds;
}

void GuildUtil::addCharacterToGuild(sql::Connection connection, int guildId, int userId)
{
	UserGuild *userGuild = new UserGuild();

	userGuild->setGuildId(guildId);
	userGuild->setJoinedDatetime(DateTime());
	userGuild->setStatus(UserGuildStatus::active);
	userGuild->setStatusLastModifiedDatetime(userGuild->getJoinedDatetime());
	userGuild->setUserId(userId);

	Character *ch = game->getSignedInCharacterByUserId(userId);

	if(ch == nullptr || ch->getEntityType() == EntityType::player)
		putUserGuild(connection, userGuild);

	userGuildMap[userGuild->getId()] = userGuild;

	auto userGuilds = userIdToUserGuildsMap.find(userId) == userIdToUserGuildsMap.end() ? (userIdToUserGuildsMap[userId] = new std::list<UserGuild*>()) : userIdToUserGuildsMap[userId];

	userGuilds->push_back(userGuild);
}

std::list<GuildApplication *> GuildUtil::getGuildApplicationsRequiringReview() const
{
	std::list<GuildApplication *> guildApplications;
	for(auto iter : guildApplicationMap)
	{
		if(iter.second->getStatus()->getRequiresReview())
			guildApplications.push_back(iter.second);
	}

	return guildApplications;
}

int GuildUtil::getCoppersToCreateNewGuild() const
{
	return coppersToCreateNewGuild;
}

int GuildUtil::getMaxGuildNameLength() const
{
	return maxGuildNameLength;
}

int GuildUtil::getMaxGuildDescriptionLength() const
{
	return maxGuildDescriptionLength;
}

int GuildUtil::getMaxDeniedReasonLength() const
{
	return maxDeniedReasonLength;
}

int GuildUtil::getMinimumLevelToSubmitGuildApplication() const
{
	return minimumLevelToSubmitGuildApplication;
}

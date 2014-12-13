#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../utils.h"
#include "../Game.h"
#include "../dbutils/QueryUtil.h"
#include "../MiscUtil.h"
#include "../dbutils/StoreDataObjectSQLBuilder.h"
#include "../EntityType.h"
#include "../CharacterUtil.h"
#include "../screen.h"
#include "../StringUtil.h"

#include "GuildUtil.h"
#include "Guild.h"
#include "GuildApplication.h"
#include "GuildApplicationSignature.h"
#include "GuildApplicationSignatureStatus.h"
#include "UserGuild.h"
#include "GuildJoinApplication.h"
#include "GuildJoinApplicationStatus.h"

GuildUtil *GuildUtil::self = nullptr;

GuildUtil::GuildUtil()
{
	this->coppersToCreateNewGuild = 100000; //10 gold.
	this->maxGuildNameLength = 40;
	this->maxGuildDescriptionLength = 255;
	this->maxDeniedReasonLength = 255;
	this->minimumLevelToSubmitGuildApplication = 30;
	this->numberOfRequiredSignaturesForNewGuild = 3;
	this->minimumLevelToJoinGuild = 30;
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
	for(auto guildApplicationSignature : guildApplicationSignatureMap)
		delete guildApplicationSignature.second;
	for(auto guildApplicationSignatures : userIdToGuildApplicationSignaturesMap)
		delete guildApplicationSignatures.second;
	for(auto guildApplicationSignatures : guildApplicationIdToGuildApplicationSignaturesMap)
		delete guildApplicationSignatures.second;
	for(auto userGuilds : guildIdToUserGuildsMap)
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
	guild->setRace(row.getInt("race"));
	
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
		->put("status_last_modified_datetime", guild->getStatusLastModifiedDatetime())
		->put("race", guild->getRace());
	
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
	guildApplication->setSubmittedByUserRace(row.getInt("submitted_by_user_race"));
	guildApplication->setStatus(GuildApplicationStatus::getEnumByValue(row.getInt("status")));
	guildApplication->setCompletedDatetime(QueryUtil::get()->getNullableDateTime(row, "completed_datetime"));
	guildApplication->setReviewerUserId(row.getNullableInt("reviewer_user_id"));
	guildApplication->setDeniedReason(row.getNullableString("denied_reason"));
	guildApplication->setGuildId(row.getNullableInt("guild_id"));
	guildApplication->setCoppersCharged(row.getInt("coppers_charged"));
	
	return guildApplication;
}

void GuildUtil::putGuildApplication(sql::Connection connection, GuildApplication *guildApplication) const
{
	auto builder = std::auto_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "guildApplication"));

	builder
		->put("guild_name", guildApplication->getGuildName())
		->put("guild_description", guildApplication->getGuildDescription())
		->put("submitted_datetime", guildApplication->getSubmittedDatetime())
		->put("submitted_by_user_id", guildApplication->getSubmittedByUserId())
		->put("submitted_by_user_race", guildApplication->getSubmittedByUserRace())
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
		[=](const sql::Row row) -> Guild* { return this->getGuild(row); },
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
	auto element = std::find_if(guildApplicationMap.begin(), guildApplicationMap.end(),
		[&](const std::map<int, GuildApplication *>::value_type & itr) -> bool
		{
			return predicate(itr.second);
		}
	);

	return element == guildApplicationMap.end() ? nullptr : element->second;
}

GuildApplication *GuildUtil::submitGuildApplication(sql::Connection connection, int userId, int userRace, const std::string &guildName, const std::string &guildDescription, const int coppersCharged)
{
	GuildApplication *guildApplication = new GuildApplication();

	guildApplication->setSubmittedDatetime(DateTime());
	guildApplication->setGuildDescription(guildDescription);
	guildApplication->setGuildName(guildName);
	guildApplication->setSubmittedByUserId(userId);
	guildApplication->setStatus(GuildApplicationStatus::pending);
	guildApplication->setCoppersCharged(coppersCharged);
	guildApplication->setSubmittedByUserRace(userRace);
	
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

Guild *GuildUtil::getGuild(int guildId) const
{
	std::map<int, Guild*>::const_iterator iter = guildMap.find(guildId);

	return iter == guildMap.end() ? nullptr : iter->second;

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
	guild->setRace(guildApplication->getSubmittedByUserRace());

	putGuild(connection, guild);

	return guildMap[guild->getId()] = guild;
}

//For a newly created Guild, perform tasks such as adding members who submitted the application.
void GuildUtil::initNewlyCreatedGuild(sql::Connection connection, int guildApplicationId, int guildId)
{
	Guild *guild = guildMap[guildId];

	this->addCharacterToGuild(connection, guildId, guild->getCreatedByUserId(), true);

	//Put all players with approved signatures into the Guild.

	for(auto approvedSignature : this->getGuildApplicationSignaturesByGuildApplicationId(guildApplicationId, {GuildApplicationSignatureStatus::approved}))
	{
		this->addCharacterToGuild(connection, guildId, approvedSignature->getUserId(), true);
	}
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
		MiscUtil::pushToVectorMap(userIdToUserGuildsMap, userGuild->getUserId(), userGuild);
		MiscUtil::pushToVectorMap(guildIdToUserGuildsMap, userGuild->getGuildId(), userGuild);
	}
}

std::vector<Guild *> GuildUtil::getGuilds(const std::vector<GuildStatus*> &statuses)
{
	std::vector<Guild *> guilds;

	for(auto iter : guildMap)
	{
		if(statuses.empty() || std::find(statuses.begin(), statuses.end(), iter.second->getStatus()) != statuses.end())
			guilds.push_back(iter.second);
	}

	return guilds;
}

std::vector<UserGuild *> GuildUtil::getUserGuildsByUserId(int userId, const std::vector<UserGuildStatus*> userGuildStatuses) const
{
	auto iter = userIdToUserGuildsMap.find(userId);

	if(iter == userIdToUserGuildsMap.end())
		return {};

	std::vector<UserGuild*> userGuildList = *(iter->second);

	if(!userGuildStatuses.empty())
	{
		for(auto userGuildIter = userGuildList.begin();userGuildIter != userGuildList.end();)
		{
			auto userGuild = (*userGuildIter);
			if(std::find(userGuildStatuses.begin(), userGuildStatuses.end(), userGuild->getStatus()) == userGuildStatuses.end())
				userGuildIter = userGuildList.erase(userGuildIter);
			else
				++userGuildIter;
		}
	}

	return userGuildList;
}

std::vector<UserGuild *> GuildUtil::getActiveUserGuildsByUserId(int userId) const
{
	std::vector<UserGuild *> userGuilds;

	for(UserGuild *userGuild : getUserGuildsByUserId(userId))
	{
		if(userGuild->getStatus()->getIsActive())
			userGuilds.push_back(userGuild);
	}

	return userGuilds;
}

void GuildUtil::removeUserFromGuild(sql::Connection connection, int userGuildId)
{
	auto userGuild = userGuildMap[userGuildId];

	userGuild->setStatus(UserGuildStatus::removed);
	userGuild->setStatusLastModifiedDatetime(DateTime());

	putUserGuild(connection, userGuild);
}

void GuildUtil::addCharacterToGuild(sql::Connection connection, int guildId, int userId, bool announceToPlayer)
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

	MiscUtil::pushToVectorMap(userIdToUserGuildsMap, userGuild->getUserId(), userGuild);
	MiscUtil::pushToVectorMap(guildIdToUserGuildsMap, userGuild->getGuildId(), userGuild);

	if(announceToPlayer)
	{
		Character *playerAddedToGuild = CharacterUtil::getOnlineCharacterById(userId);
		Guild *guild = guildMap[guildId];

		if(playerAddedToGuild)
		{
			playerAddedToGuild->send("%s%sYou have been added to the Guild `%s`!%s\r\n\r\n",
				COLOR_BOLD(playerAddedToGuild, CL_NORMAL),
				COLOR_GREEN(playerAddedToGuild, CL_NORMAL),
				guild->getName().c_str(),
				COLOR_NORMAL(playerAddedToGuild, CL_NORMAL)
			);
		}
	}
}

std::vector<GuildApplication *> GuildUtil::getGuildApplicationsRequiringReview() const
{
	std::vector<GuildApplication *> guildApplications;
	for(auto iter : guildApplicationMap)
	{
		if(iter.second->getStatus()->getRequiresReview())
			guildApplications.push_back(iter.second);
	}

	return guildApplications;
}

std::vector<GuildApplication *> GuildUtil::getGuildApplications(const boost::optional<int> &userId, const std::vector<GuildApplicationStatus *> statuses) const
{
	std::vector<GuildApplication *> guildApplications;
	for(auto iter : guildApplicationMap)
	{
		auto application = iter.second;
		if(statuses.empty() || std::find(statuses.begin(), statuses.end(), application->getStatus()) != statuses.end())
		{
			if(!userId || userId.get() == application->getSubmittedByUserId())
				guildApplications.push_back(application);
		}
	}

	return guildApplications;
}

std::vector<GuildApplication *> GuildUtil::getGuildApplicationsSorted() const
{
	auto applications = getGuildApplications();
	std::sort(applications.begin(), applications.end(), [](GuildApplication*a, GuildApplication*b) -> bool { return a->getId() < b->getId(); });
	return applications;
}

GuildApplicationSignature *GuildUtil::getGuildApplicationSignature(const sql::Row row) const
{
	auto signature = new GuildApplicationSignature();

	signature->setId(row.getInt("id"));
	signature->setUserId(row.getInt("user_id"));
	signature->setGuildApplicationId(row.getInt("guild_application_id"));
	signature->setStatus(GuildApplicationSignatureStatus::getEnumByValue(row.getInt("status")));
	signature->setCreatedDatetime(DateTime(row.getTimestamp("created_datetime")));
	signature->setStatusLastModifiedDatetime(DateTime(row.getTimestamp("status_last_modified_datetime")));
	signature->setStatusChangedByUserId(row.getInt("status_changed_by_user_id"));

	return signature;
}

GuildApplicationSignature *GuildUtil::getGuildApplicationSignature(int guildApplicationSignatureId) const
{
	auto iter = guildApplicationSignatureMap.find(guildApplicationSignatureId);

	return iter == guildApplicationSignatureMap.end() ? nullptr : iter->second;
}
void GuildUtil::putGuildApplicationSignature(sql::Connection connection, GuildApplicationSignature *guildApplicationSignature) const
{
	auto builder = std::auto_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "guildApplicationSignature"));

	builder
		->put("user_id", guildApplicationSignature->getUserId())
		->put("guild_application_id", guildApplicationSignature->getGuildApplicationId())
		->put("status", guildApplicationSignature->getStatus())
		->put("created_datetime", guildApplicationSignature->getCreatedDatetime())
		->put("status_last_modified_datetime", guildApplicationSignature->getStatusLastModifiedDatetime())
		->put("status_changed_by_user_id", guildApplicationSignature->getStatusChangedByUserId());

	builder->execute(*guildApplicationSignature);
}

void GuildUtil::loadGuildApplicationSignaturesFromDatabase(sql::Connection connection)
{
	this->guildApplicationSignatureMap = QueryUtil::get()->loadDataObjectMapFromDatabase<GuildApplicationSignature*, int>(
		connection,
		"guildApplicationSignature",
		[=](const sql::Row row) { return this->getGuildApplicationSignature(row); },
		[](GuildApplicationSignature* signature) { return signature->getId(); }
	);

	for(auto iter : this->guildApplicationSignatureMap)
	{
		GuildApplicationSignature *signature = iter.second;

		MiscUtil::pushToVectorMap(userIdToGuildApplicationSignaturesMap, signature->getUserId(), signature);
		MiscUtil::pushToVectorMap(guildApplicationIdToGuildApplicationSignaturesMap, signature->getGuildApplicationId(), signature);
	}
}

std::vector<GuildApplicationSignature*> GuildUtil::getGuildApplicationSignaturesByUserId(const int userId) const
{
	return getGuildApplicationSignaturesByUserId(userId, {});
}

std::vector<GuildApplicationSignature*> GuildUtil::getGuildApplicationSignaturesByUserId(const int userId, std::vector<GuildApplicationSignatureStatus*> statuses) const
{
	std::vector<GuildApplicationSignature*> userSignatures;
	std::vector<GuildApplicationSignature*> signatures;

	auto userSignaturesIter = userIdToGuildApplicationSignaturesMap.find(userId);

	if(userSignaturesIter == userIdToGuildApplicationSignaturesMap.end())
		return {};

	userSignatures = *(userSignaturesIter->second);

	for(auto signature : userSignatures)
	{
		if(statuses.empty() || std::find(statuses.begin(), statuses.end(), signature->getStatus()) != statuses.end())
			signatures.push_back(signature);
	}

	return signatures;
}

std::vector<GuildApplicationSignature*> GuildUtil::getGuildApplicationSignaturesByGuildApplicationId(int guildApplicationId) const
{
	return getGuildApplicationSignaturesByGuildApplicationId(guildApplicationId, {});
}

std::vector<GuildApplicationSignature*> GuildUtil::getGuildApplicationSignaturesByGuildApplicationId(int guildApplicationId, std::vector<GuildApplicationSignatureStatus*> statuses) const
{
	std::vector<GuildApplicationSignature*> applicationSignatures;
	std::vector<GuildApplicationSignature*> signatures;

	auto applicationSignaturesIter = guildApplicationIdToGuildApplicationSignaturesMap.find(guildApplicationId);

	if(applicationSignaturesIter == guildApplicationIdToGuildApplicationSignaturesMap.end())
		return {};

	applicationSignatures = *(applicationSignaturesIter->second);

	for(auto signature : applicationSignatures)
	{
		if(statuses.empty() || std::find(statuses.begin(), statuses.end(), signature->getStatus()) != statuses.end())
			signatures.push_back(signature);
	}

	return signatures;
}

int GuildUtil::getNumberOfValidGuildApplicationSignatures(int guildApplicationId) const
{
	return (int)GuildUtil::get()->getGuildApplicationSignaturesByGuildApplicationId(guildApplicationId, {GuildApplicationSignatureStatus::approved}).size();
}

GuildApplicationSignature *GuildUtil::submitGuildApplicationSignature(sql::Connection connection, int userId, int guildApplicationId)
{
	//Create
	auto signature = new GuildApplicationSignature();

	signature->setUserId(userId);
	signature->setStatusChangedByUserId(userId);
	signature->setCreatedDatetime(DateTime());
	signature->setStatusLastModifiedDatetime(signature->getStatusLastModifiedDatetime());
	signature->setGuildApplicationId(guildApplicationId);
	signature->setStatus(GuildApplicationSignatureStatus::pending);

	//Store
	putGuildApplicationSignature(connection, signature);

	//Push to internal maps.
	this->guildApplicationSignatureMap[signature->getId()] = signature;

	MiscUtil::pushToVectorMap(this->userIdToGuildApplicationSignaturesMap, userId, signature);
	MiscUtil::pushToVectorMap(this->guildApplicationIdToGuildApplicationSignaturesMap, guildApplicationId, signature);
	
	return signature;
}

void GuildUtil::removeGuildSignatureByUser(sql::Connection connection, int guildApplicationSignatureId)
{
	auto signature = guildApplicationSignatureMap[guildApplicationSignatureId];

	signature->setStatus(GuildApplicationSignatureStatus::removed);
	signature->setStatusLastModifiedDatetime(DateTime());
	signature->setStatusChangedByUserId(signature->getUserId());
	
	putGuildApplicationSignature(connection, signature);
}

void GuildUtil::approveSignature(sql::Connection connection, int guildApplicationSignatureId, int approvedByUserId)
{
	auto signature = getGuildApplicationSignature(guildApplicationSignatureId);
	
	signature->setStatus(GuildApplicationSignatureStatus::approved);
	signature->setStatusChangedByUserId(approvedByUserId);
	signature->setStatusLastModifiedDatetime(DateTime());
	
	putGuildApplicationSignature(connection, signature);
}

void GuildUtil::denyGuildApplicationSignature(sql::Connection connection, int guildApplicationSignatureId, int deniedByUserId)
{
	auto signature = getGuildApplicationSignature(guildApplicationSignatureId);

	signature->setStatus(GuildApplicationSignatureStatus::denied);
	signature->setStatusChangedByUserId(deniedByUserId);
	signature->setStatusLastModifiedDatetime(DateTime());

	putGuildApplicationSignature(connection, signature);
}


//Guild Join Applications.
void GuildUtil::loadGuildJoinApplicationsFromDatabase(sql::Connection connection)
{
	this->guildJoinApplicationMap = QueryUtil::get()->loadDataObjectMapFromDatabase<GuildJoinApplication*,int>(
		connection,
		"guildJoinApplication",
		[=](const sql::Row row) -> GuildJoinApplication* { return this->getGuildJoinApplication(row); },
		[](GuildJoinApplication *guildJoinApplication) -> int { return guildJoinApplication->getId(); }
	);

	for(auto iter : guildJoinApplicationMap)
	{
		auto application = iter.second;
		MiscUtil::pushToVectorMap(userIdToGuildJoinApplicationsMap, application->getUserId(), application);
		MiscUtil::pushToVectorMap(guildIdToGuildJoinApplicationsMap, application->getGuildId(), application);
	}
}

GuildJoinApplication *GuildUtil::getGuildJoinApplication(const sql::Row row) const
{
	auto application = new GuildJoinApplication();

	application->setId(row.getInt("id"));
	application->setUserId(row.getInt("user_id"));
	application->setGuildId(row.getInt("guild_id"));
	application->setCreatedDatetime(DateTime(row.getTimestamp("created_datetime")));
	application->setStatus(GuildJoinApplicationStatus::getEnumByValue(row.getInt("status")));
	application->setStatusLastModifiedDatetime(DateTime(row.getTimestamp("status_last_modified_datetime")));
	application->setStatusLastModifiedByUserId(row.getInt("status_last_modified_by_user_id"));
	application->setMessageToGuild(row.getString("message_to_guild"));
	application->setMessageFromGuild(row.getNullableString("message_from_guild"));

	return application;
}

void GuildUtil::putGuildJoinApplication(sql::Connection connection, GuildJoinApplication *guildJoinApplication) const
{
	auto builder = std::shared_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "guildJoinApplication"));

	builder
		->put("user_id", guildJoinApplication->getUserId())
		->put("guild_id", guildJoinApplication->getGuildId())
		->put("created_datetime", guildJoinApplication->getCreatedDatetime())
		->put("status", guildJoinApplication->getStatus())
		->put("status_last_modified_datetime", guildJoinApplication->getStatusLastModifiedDatetime())
		->put("status_last_modified_by_user_id", guildJoinApplication->getStatusLastModifiedByUserId())
		->put("message_to_guild", guildJoinApplication->getMessageToGuild())
		->put("message_from_guild", guildJoinApplication->getMessageFromGuild());

	builder->execute(*guildJoinApplication);
}

GuildJoinApplication *GuildUtil::getGuildJoinApplication(int guildJoinApplicationId) const
{
	auto iter = guildJoinApplicationMap.find(guildJoinApplicationId);

	return iter == guildJoinApplicationMap.end() ? nullptr : iter->second;
}

std::vector<GuildJoinApplication *> GuildUtil::getGuildJoinApplications(const boost::optional<int> &userId, const boost::optional<int> &guildId, const std::vector<GuildJoinApplicationStatus *> &statuses)
{
	std::vector<GuildJoinApplication *> guildJoinApplications;

	if(userId && userIdToGuildJoinApplicationsMap[userId.get()] != nullptr)
		guildJoinApplications =  *(userIdToGuildJoinApplicationsMap[userId.get()]);
	else if(guildId && guildJoinApplications.empty() && guildIdToGuildJoinApplicationsMap[guildId.get()] != nullptr)
		guildJoinApplications = *(guildIdToGuildJoinApplicationsMap[guildId.get()]);

	if(!statuses.empty())
	{
		auto iter = std::remove_if(guildJoinApplications.begin(), guildJoinApplications.end(), [=](GuildJoinApplication *a) {
			return std::find(statuses.begin(), statuses.end(), a->getStatus()) == statuses.end();
		});

		guildJoinApplications.erase(iter, guildJoinApplications.end());
	}

	return guildJoinApplications;
}

GuildJoinApplication *GuildUtil::submitGuildJoinApplication(sql::Connection connection, int userId, int guildId, const std::string &messageToGuild)
{
	auto application = new GuildJoinApplication();

	DateTime referenceDatetime;

	application->setCreatedDatetime(referenceDatetime);
	application->setGuildId(guildId);
	application->setMessageToGuild(messageToGuild);
	application->setStatus(GuildJoinApplicationStatus::reviewing);
	application->setStatusLastModifiedByUserId(userId);
	application->setStatusLastModifiedDatetime(referenceDatetime);
	application->setUserId(userId);

	putGuildJoinApplication(connection, application);

	this->guildJoinApplicationMap[application->getId()] = application;

	MiscUtil::pushToVectorMap(this->userIdToGuildJoinApplicationsMap, application->getUserId(), application);
	MiscUtil::pushToVectorMap(this->guildIdToGuildJoinApplicationsMap, application->getGuildId(), application);

	//TODO: Send notifications out.

	return application;
}

void GuildUtil::removeGuildJoinApplication(sql::Connection connection, int removedByUserId, int guildJoinApplicationId)
{
	auto application = guildJoinApplicationMap[guildJoinApplicationId];

	application->setStatus(GuildJoinApplicationStatus::removed);
	application->setStatusLastModifiedByUserId(removedByUserId);
	application->setStatusLastModifiedDatetime(DateTime());

	putGuildJoinApplication(connection, application);
}

int GuildUtil::getNumberOfActiveGuildMembers(int guildId) const
{
	auto iter = guildIdToUserGuildsMap.find(guildId);

	return iter == guildIdToUserGuildsMap.end() ? 0 : iter->second->size();
}

void GuildUtil::guildsCommandHandler(Character *ch, char *argument, int cmd, int subcmd)
{
	game->getGuildEditorInterface()->setupNewInstance(ch);
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

int GuildUtil::getNumberOfRequiredSignaturesForNewGuild() const
{
	return numberOfRequiredSignaturesForNewGuild;
}

int GuildUtil::getMinimumLevelToJoinGuild() const
{
	return minimumLevelToJoinGuild;
}
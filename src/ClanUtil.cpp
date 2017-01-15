#include <sstream>

#include "conf.h"
#include "utils.h"
#include "ClanUtil.h"
#include "CharacterUtil.h"
#include "UserType.h"
#include "mobs.h"
#include "clans.h"
#include "Exception.h"
#include "DatabaseUtil.h"

ClanQuestPointTransaction *ClanUtil::getClanQuestPointTransaction(sql::Connection connection, const int clanQuestPointTransactionId)
{
	std::stringstream sql;
	sql::Query query;
	sql::Row row;

	sql << " SELECT *"
		<< " FROM clanQuestPointTransaction"
		<< " WHERE id = " << clanQuestPointTransactionId;

	query = connection->sendQuery(sql.str());
	while(query->hasNextRow())
	{
		return getClanQuestPointTransaction(query->getRow());
	}

	return NULL;
}

ClanQuestPointTransaction *ClanUtil::getClanQuestPointTransaction(const sql::Row &row)
{
	ClanQuestPointTransaction *clanQuestPointTransaction = new ClanQuestPointTransaction();

	clanQuestPointTransaction->setId(row.getInt("id"));
	clanQuestPointTransaction->setClanId(row.getInt("clan_id"));
	clanQuestPointTransaction->setUserId(row.getInt("user_id"));
	clanQuestPointTransaction->setAmount(row.getInt("amount"));
	clanQuestPointTransaction->setCreatedDatetime(row.getTimestamp("created_datetime"));
	clanQuestPointTransaction->setIssuedByUserType(UserType::getEnumByValue(row.getInt("issued_by_user_type")) );
	clanQuestPointTransaction->setIssuedByUserId(row.getInt("issued_by_user_id"));
	clanQuestPointTransaction->setReason(row.getString("reason"));

	return clanQuestPointTransaction;
}

void ClanUtil::putClanQuestPointTransaction(sql::Connection connection, ClanQuestPointTransaction *clanQuestPointTransaction)
{
	std::stringstream sql;

	if(clanQuestPointTransaction->isNew())
	{
		sql << " INSERT INTO clanQuestPointTransaction("
			<< "   `clan_id`,"
			<< "   `user_id`,"
			<< "   `amount`,"
			<< "   `created_datetime`,"
			<< "   `issued_by_user_type`,"
			<< "   `issued_by_user_id`,"
			<< "   `reason`"
			<< ") VALUES("
			<< clanQuestPointTransaction->getClanId() << ","
			<< clanQuestPointTransaction->getUserId() << ","
			<< clanQuestPointTransaction->getAmount() << ","
			<< sql::encodeQuoteDate(clanQuestPointTransaction->getCreatedDatetime().getTime()) << ","
			<< clanQuestPointTransaction->getIssuedByUserType()->getValue() << ","
			<< clanQuestPointTransaction->getIssuedByUserId() << ","
			<< sql::escapeQuoteString(clanQuestPointTransaction->getReason())
			<< ")";

		connection->sendRawQuery(sql.str());

		clanQuestPointTransaction->setId(connection->lastInsertID());
	}
	else
	{
		sql << " UPDATE clanQuestPointTransaction SET"
			<< "   clan_id = " << clanQuestPointTransaction->getClanId() << ","
			<< "   user_id = " << clanQuestPointTransaction->getUserId() << ","
			<< "   amount = " << clanQuestPointTransaction->getAmount() << ","
			<< "   created_datetime = " << sql::encodeQuoteDate(clanQuestPointTransaction->getCreatedDatetime().getTime()) << ","
			<< "   issued_by_user_type = " << clanQuestPointTransaction->getIssuedByUserType()->getValue() << ","
			<< "   issued_by_user_id = " << clanQuestPointTransaction->getIssuedByUserId() << ","
			<< "   reason = " << sql::escapeQuoteString(clanQuestPointTransaction->getReason())
			<< " WHERE id = " << clanQuestPointTransaction->getId();

		connection->sendRawQuery(sql.str());
	}
}

ClanQuestPointTransaction *ClanUtil::performQuestPointTransaction(sql::Connection connection, const int userId, const int clanId, const int amount, UserType *issuedByUserType, const int issuedByUserId, const std::string reason)
{
	std::unique_ptr<ClanQuestPointTransaction> clanQuestPointTransaction(new ClanQuestPointTransaction());

	//Attempt to perform the transaction to a user who is online.
	Character *character = CharacterUtil::getOnlineCharacterById(userId);
	Character *issuedByUser = NULL;
	UserClan *userClan = NULL;

	bool characterLoadedFromDatabase = false;

	if(amount == 0)
	{
		throw Exception("A clan quest point transaction with an amount of zero is not allowed.");
	}

	if(character == NULL)
	{//If the user is not online, attempt to load them from the database.

		characterLoadedFromDatabase = true;
		character = CharacterUtil::loadCharacter(userId);
	}

	if(character == NULL)
	{//If we do not have a valid pointer to the character, then they simply do not exist.

		throw Exception( std::string("The target character(userId = `" + MiscUtil::toString(userId) + "`) does not exist").c_str() );
	}

	//Load the issuer.
	if(issuedByUserType == UserType::mob)
	{
		issuedByUser = MobManager::GetManager().GetPrototypeByVnum(issuedByUserId);
	}
	else if(issuedByUserType == UserType::player)
	{
		issuedByUser = CharacterUtil::loadCharacter(issuedByUserId);
	}

	if(issuedByUser == NULL)
	{
		if(characterLoadedFromDatabase)
			delete character;

		throw Exception( std::string("The issuer(type = `" + issuedByUserType->getStandardName() + "`, ID = `" + MiscUtil::toString(issuedByUserId) + "`) could not be found.").c_str() );
	}

	userClan = character->getUserClan( clanId );

	if(userClan == NULL)
	{
		if(characterLoadedFromDatabase)
			delete character;

		throw Exception( std::string("The user is not a member of the specified clan(clanId = `" + MiscUtil::toString(clanId) + "`)").c_str() );
	}

	//Perform the transaction.
	userClan->setQuestPoints( userClan->getQuestPoints() + amount );

	//Create the transaction record.
	clanQuestPointTransaction->setUserId(userId);
	clanQuestPointTransaction->setClanId(clanId);
	clanQuestPointTransaction->setAmount(amount);
	clanQuestPointTransaction->setCreatedDatetime(DateTime());
	clanQuestPointTransaction->setIssuedByUserType(issuedByUserType);
	clanQuestPointTransaction->setIssuedByUserId(issuedByUserId);
	clanQuestPointTransaction->setReason(reason);

	ClanUtil::putClanQuestPointTransaction(connection, clanQuestPointTransaction.get());
	
	character->saveClans();

	if(characterLoadedFromDatabase)
		delete character;

	return clanQuestPointTransaction.release();
}

std::list<ClanQuestPointTransaction *> ClanUtil::getAllClanQuestPointTransactions(sql::Connection connection)
{
	std::stringstream sql;
	std::list<ClanQuestPointTransaction *> clanQuestPointTransactions;
	sql::Query query;
	sql::Row row;

	sql << " SELECT *"
		<< " FROM clanQuestPointTransaction"
		<< " ORDER BY id DESC";

	query = connection->sendQuery(sql.str());
	while(query->hasNextRow())
	{
		row = query->getRow();
		clanQuestPointTransactions.push_back( ClanUtil::getClanQuestPointTransaction(row) );
	}

	return clanQuestPointTransactions;
}

Clan *ClanUtil::getClan(const int clanId)
{//TODO: Convert this to a binary search.
	Clan *clan;

	for(clan = ClanList;clan;clan = clan->Next)
	{
		if(clan->vnum == clanId)
			return clan;
	}

	return NULL;
}

std::list<UserClan *> ClanUtil::getUserClans(sql::Connection connection, int userId)
{
	std::stringstream sql;
	std::list<UserClan *> userClans;
	sql::Query query;

	sql << " SELECT *"
		<< " FROM userClan"
		<< " WHERE user_id = " << userId
		<< " ORDER BY id";

	query = connection->sendQuery(sql.str());

	while(query->hasNextRow())
	{
		userClans.push_back(getUserClan(query->getRow()));
	}

	return userClans;
}

UserClan *ClanUtil::getUserClan(const sql::Row &row)
{
	UserClan *userClan = new UserClan();

	userClan->setId(row.getInt("id"));
	userClan->setUserId(row.getInt("user_id"));
	userClan->setClanId(row.getInt("clan_id"));
	userClan->setRank(row.getInt("rank"));
	userClan->setClannedDatetime(row.getTimestamp("clanned_datetime"));
	userClan->setLastRankedDatetime(row.getTimestamp("last_ranked_datetime"));
	userClan->setQuestPoints(row.getInt("quest_points"));
	userClan->setIsCouncil(row.getInt("is_council"));

	return userClan;
}

void ClanUtil::putUserClan(sql::Connection connection, UserClan *userClan)
{
	std::stringstream sql;

	if(userClan->isNew())
	{
		sql << " INSERT INTO userClan("
			<< "   `user_id`,"
			<< "   `clan_id`,"
			<< "   `rank`,"
			<< "   `clanned_datetime`,"
			<< "   `last_ranked_datetime`,"
			<< "   `quest_points`,"
			<< "   `is_council`"
			<< " ) VALUES("
			<< userClan->getUserId() << ","
			<< userClan->getClanId() << ","
			<< ((int)userClan->getRank()) << ","
			<< sql::encodeQuoteDate(userClan->getClannedDatetime().getTime()) << ","
			<< sql::encodeQuoteDate(userClan->getLastRankedDatetime().getTime()) << ","
			<< userClan->getQuestPoints() << ","
			<< (userClan->getIsCouncil() ? 1 : 0)
			<< ")";

		connection->sendRawQuery(sql.str());

		userClan->setId(connection->lastInsertID());
	}
	else
	{
		sql << " UPDATE userClan SET"
			<< "   user_id = " << userClan->getUserId() << ","
			<< "   clan_id = " << userClan->getClanId() << ","
			<< "   rank = " << ((int)userClan->getRank()) << ","
			<< "   clanned_datetime = " << sql::encodeQuoteDate(userClan->getClannedDatetime().getTime()) << ","
			<< "   last_ranked_datetime = " << sql::encodeQuoteDate(userClan->getLastRankedDatetime().getTime()) << ","
			<< "   quest_points = " << userClan->getQuestPoints() << ","
			<< "   is_council = " << (userClan->getIsCouncil() ? 1 : 0)
			<< " WHERE id = " << userClan->getId();

		connection->sendRawQuery(sql.str());
	}
}

UserClan *ClanUtil::getUserClanFromList(const std::list<UserClan *> &userClans, short clanId)
{
	for(auto userClanIter = userClans.begin();userClanIter != userClans.end();++userClanIter)
	{
		if((*userClanIter)->getClanId() == clanId)
			return (*userClanIter);
	}
	return NULL;
}

void ClanUtil::removeUserClansFromDatabase(sql::Connection connection, std::list<int> userClanIdList)
{
	std::stringstream sql;

	sql << " DELETE FROM userClan"
		<< " WHERE id IN" << DatabaseUtil::buildListSQL(userClanIdList, false, false);

	connection->sendRawQuery(sql.str());
}

void ClanUtil::removeUserClansFromDatabase(sql::Connection connection, std::list<UserClan *> userClanList)
{
	std::list<int> userClanIdList;

	for(auto iter = userClanList.begin();iter != userClanList.end();++iter)
	{
		UserClan *userClan = (*iter);
		userClanIdList.push_back(userClan->getId());
	}

	removeUserClansFromDatabase(connection, userClanIdList);
}

void ClanUtil::freeUserClans(std::list<UserClan *> &userClans)
{
	while(userClans.empty() == false)
	{
		delete userClans.front();
		userClans.pop_front();
	}
}

std::list<UserClan *> ClanUtil::cloneUserClansFromMobPrototype(Character *mobPrototype)
{
	std::list<UserClan *> userClanList;

	for(auto iter = mobPrototype->userClans.begin();iter != mobPrototype->userClans.end();++iter)
	{
		UserClan *userClan = UserClan::copyFromMobPrototype( (*iter) );
		userClanList.push_back(userClan);
	}

	return userClanList;
}

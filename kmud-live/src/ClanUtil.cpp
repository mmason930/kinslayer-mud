#include <sstream>

#include "conf.h"
#include "utils.h"
#include "ClanUtil.h"
#include "CharacterUtil.h"
#include "UserType.h"
#include "mobs.h"
#include "clans.h"

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

ClanQuestPointTransaction *ClanUtil::getClanQuestPointTransaction(sql::Row row)
{
	ClanQuestPointTransaction *clanQuestPointTransaction = new ClanQuestPointTransaction();

	clanQuestPointTransaction->setId(row.getInt("id"));
	clanQuestPointTransaction->setClanId(row.getInt("clan_id"));
	clanQuestPointTransaction->setUserId(row.getInt("user_id"));
	clanQuestPointTransaction->setAmount(row.getInt("amount"));
	clanQuestPointTransaction->setCreatedDatetime(row.getTimestamp("created_datetime"));
	clanQuestPointTransaction->setIssuedByUserType( (UserType*)UserType::getEnumByValue(row.getInt("issued_by_user_type")) );
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
	std::auto_ptr<ClanQuestPointTransaction> clanQuestPointTransaction(new ClanQuestPointTransaction());

	//Attempt to perform the transaction to a user who is online.
	Character *character = CharacterUtil::getOnlineCharacterById(userId);
	Character *issuedByUser = NULL;
	PlayerClan *userClan = NULL;

	bool characterLoadedFromDatabase = false;

	if(amount == 0)
	{
		throw std::exception("A clan quest point transaction with an amount of zero is not allowed.");
	}

	if(character == NULL)
	{//If the user is not online, attempt to load them from the database.

		characterLoadedFromDatabase = true;
		character = CharacterUtil::loadCharacter(userId);
	}

	if(character == NULL)
	{//If we do not have a valid pointer to the character, then they simply do not exist.

		throw std::exception( std::string("The target character(userId = `" + MiscUtil::toString(userId) + "`) does not exist").c_str() );
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

		throw std::exception( std::string("The issuer(type = `" + issuedByUserType->getStandardName() + "`, ID = `" + MiscUtil::toString(issuedByUserId) + "`) could not be found.").c_str() );
	}

	userClan = character->GetClan( clanId );

	if(userClan == NULL)
	{
		if(characterLoadedFromDatabase)
			delete character;

		throw std::exception( std::string("The user is not a member of the specified clan(clanId = `" + MiscUtil::toString(clanId) + "`)").c_str() );
	}

	//Perform the transaction.
	userClan->SetQuestPoints( userClan->GetQuestPoints() + amount );

	//Create the transaction record.
	clanQuestPointTransaction->setUserId(userId);
	clanQuestPointTransaction->setClanId(clanId);
	clanQuestPointTransaction->setAmount(amount);
	clanQuestPointTransaction->setCreatedDatetime(DateTime());
	clanQuestPointTransaction->setIssuedByUserType(issuedByUserType);
	clanQuestPointTransaction->setIssuedByUserId(issuedByUserId);
	clanQuestPointTransaction->setReason(reason);

	ClanUtil::putClanQuestPointTransaction(connection, clanQuestPointTransaction.get());
	
	character->SaveClans();

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

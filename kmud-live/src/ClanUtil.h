#ifndef CLAN_UTIL_H
#define CLAN_UTIL_H

#include "ClanQuestPointTransaction.h"
#include <mysql/sqlDatabase.h>
#include <list>

class Clan;

class ClanUtil
{
public:
	static ClanQuestPointTransaction *getClanQuestPointTransaction(sql::Connection connection, const int clanQuestPointTransactionId);
	static ClanQuestPointTransaction *getClanQuestPointTransaction(sql::Row row);
	static void putClanQuestPointTransaction(sql::Connection connection, ClanQuestPointTransaction *clanQuestPointTransaction);
	static ClanQuestPointTransaction *performQuestPointTransaction(sql::Connection connection, const int userId, const int clanId, const int amount, UserType *issuedByUserType, const int issuedByUserId, const std::string reason);
	static Clan *getClan(const int clanId);
	static std::list<ClanQuestPointTransaction *> getAllClanQuestPointTransactions(sql::Connection connection);
};

#endif

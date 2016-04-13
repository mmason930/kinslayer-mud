#ifndef CLAN_UTIL_H
#define CLAN_UTIL_H

#include "ClanQuestPointTransaction.h"
#include "UserClan.h"
#include <mysql/sqlDatabase.h>
#include <list>

class Clan;

class ClanUtil
{
public:
	static ClanQuestPointTransaction *getClanQuestPointTransaction(sql::Connection connection, const int clanQuestPointTransactionId);
	static ClanQuestPointTransaction *getClanQuestPointTransaction(const sql::Row &row);
	static void putClanQuestPointTransaction(sql::Connection connection, ClanQuestPointTransaction *clanQuestPointTransaction);
	static ClanQuestPointTransaction *performQuestPointTransaction(sql::Connection connection, const int userId, const int clanId, const int amount, UserType *issuedByUserType, const int issuedByUserId, const std::string reason);
	static Clan *getClan(const int clanId);
	static std::list<ClanQuestPointTransaction *> getAllClanQuestPointTransactions(sql::Connection connection);
	static std::list<UserClan *> getUserClans(sql::Connection connection, int userId);
	static UserClan *getUserClan(const sql::Row &row);
	static void putUserClan(sql::Connection connection, UserClan *userClan);
	static UserClan *getUserClanFromList(const std::list<UserClan *> &userClans, short clanId);
	static void freeUserClans(std::list<UserClan *> &userClans);
	static void removeUserClansFromDatabase(sql::Connection connection, std::list<UserClan *> userClanList);
	static void removeUserClansFromDatabase(sql::Connection connection, std::list<int> userClanIdList);
	static std::list<UserClan *> cloneUserClansFromMobPrototype(Character *mobPrototype);
};

#endif

#ifndef CLAN_QUEST_POINT_TRANSACTION_H
#define CLAN_QUEST_POINT_TRANSACTION_H

#include "UserType.h"
#include "DataObjectWithIntId.h"
#include "DateTime.h"
#include <string>

class ClanQuestPointTransaction : public DataObjectWithIntId
{
private:
	int userId;
	short int clanId;
	short int amount;
	DateTime createdDatetime;
	UserType *issuedByUserType;
	int issuedByUserId;
	std::string reason;
public:

	ClanQuestPointTransaction()
	{
		id = -1;
	}

	int getUserId()
	{
		return userId;
	}

	void setUserId(const int userId)
	{
		this->userId = userId;
	}

	short int getClanId()
	{
		return clanId;
	}

	void setClanId(const short int clanId)
	{
		this->clanId = clanId;
	}

	short int getAmount()
	{
		return amount;
	}

	void setAmount(const short int amount)
	{
		this->amount = amount;
	}

	DateTime getCreatedDatetime()
	{
		return createdDatetime;
	}

	void setCreatedDatetime(const DateTime &createdDatetime)
	{
		this->createdDatetime = createdDatetime;
	}

	UserType *getIssuedByUserType()
	{
		return issuedByUserType;
	}

	void setIssuedByUserType(UserType *issuedByUserType)
	{
		this->issuedByUserType = issuedByUserType;
	}

	int getIssuedByUserId()
	{
		return issuedByUserId;
	}

	void setIssuedByUserId(const int issuedByUserId)
	{
		this->issuedByUserId = issuedByUserId;
	}

	std::string getReason()
	{
		return reason;
	}

	void setReason(const std::string reason)
	{
		this->reason = reason;
	}
};

#endif

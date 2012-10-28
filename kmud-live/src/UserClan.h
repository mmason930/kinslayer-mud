#ifndef USER_CLAN_H
#define USER_CLAN_H

#include "DateTime.h"
#include "DataObjectWithIntId.h"

class UserClan : public DataObjectWithIntId
{
private:
	int userId;
	short int clanId;
	DateTime clannedDatetime;
	DateTime lastRankedDatetime;
	short int questPoints;
	unsigned char rank;
	bool isCouncil;
public:

	UserClan()
	{
		id = -1;
	}

	static UserClan *setupNewInstance(int userId, int clanId)
	{
		UserClan *userClan = new UserClan();

		userClan->setUserId(userId);
		userClan->setClanId(clanId);
		userClan->setClannedDatetime(DateTime());
		userClan->setLastRankedDatetime(DateTime());
		userClan->setQuestPoints(0);
		userClan->setRank(1);
		userClan->setIsCouncil(false);

		return userClan;
	}

	static UserClan *copyFromMobPrototype(UserClan *mobPrototypeUserClan)
	{
		UserClan *userClan = UserClan::setupNewInstance(mobPrototypeUserClan->getUserId(), mobPrototypeUserClan->getClanId());
		userClan->setIsCouncil(mobPrototypeUserClan->getIsCouncil());
		userClan->setRank(mobPrototypeUserClan->getRank());
		userClan->setQuestPoints(mobPrototypeUserClan->getQuestPoints());
		return userClan;
	}

	int getUserId() const
	{
		return userId;
	}

	void setUserId(int userId)
	{
		this->userId = userId;
	}

	short int getClanId() const
	{
		return clanId;
	}

	void setClanId(short int clanId)
	{
		this->clanId = clanId;
	}

	DateTime getClannedDatetime() const
	{
		return clannedDatetime;
	}

	void setClannedDatetime(DateTime clannedDatetime)
	{
		this->clannedDatetime = clannedDatetime;
	}

	DateTime getLastRankedDatetime() const
	{
		return lastRankedDatetime;
	}

	void setLastRankedDatetime(DateTime lastRankedDatetime)
	{
		this->lastRankedDatetime = lastRankedDatetime;
	}

	short int getQuestPoints() const
	{
		return questPoints;
	}

	void setQuestPoints(short int questPoints)
	{
		this->questPoints = questPoints;
	}

	unsigned char getRank() const
	{
		return rank;
	}

	void setRank(unsigned char rank)
	{
		this->rank = rank;
	}

	bool getIsCouncil() const
	{
		return isCouncil;
	}

	void setIsCouncil(bool isCouncil)
	{
		this->isCouncil = isCouncil;
	}
};

#endif

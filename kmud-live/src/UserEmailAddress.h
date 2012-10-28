#ifndef USER_EMAIL_H
#define USER_EMAIL_H

#include "DataObjectWithIntId.h"
#include "DateTime.h"

class UserEmailAddress : public DataObjectWithIntId
{
private:
	int userId;
	std::string emailAddress;
	bool confirmed;
	DateTime createdDatetime;
	DateTime confirmedDatetime;

public:

	UserEmailAddress()
	{
		id = -1;
	}
	int getUserId() const
	{
		return userId;
	}

	void setUserId(int userId)
	{
		this->userId = userId;
	}

	std::string getEmailAddress() const
	{
		return emailAddress;
	}

	void setEmailAddress(const std::string &emailAddress)
	{
		this->emailAddress = emailAddress;
	}

	bool getConfirmed() const
	{
		return confirmed;
	}

	void setConfirmed(bool confirmed)
	{
		this->confirmed = confirmed;
	}

	DateTime getCreatedDatetime() const
	{
		return createdDatetime;
	}

	void setCreatedDatetime(DateTime createdDatetime)
	{
		this->createdDatetime = createdDatetime;
	}

	DateTime getConfirmedDatetime() const
	{
		return confirmedDatetime;
	}

	void setConfirmedDatetime(DateTime confirmedDatetime)
	{
		this->confirmedDatetime = confirmedDatetime;
	}
};

#endif

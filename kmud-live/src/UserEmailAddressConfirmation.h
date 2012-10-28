#ifndef USER_EMAIL_ADDRESS_CONFIRMATION
#define USER_EMAIL_ADDRESS_CONFIRMATION

#include "DataObjectWithIntId.h"

class UserEmailAddressConfirmation : public DataObjectWithIntId
{
private:
	int userEmailAddressId;
	std::string confirmationKey;
public:

	UserEmailAddressConfirmation()
	{
		id = -1;
	}
	int getUserEmailAddressId() const
	{
		return userEmailAddressId;
	}

	void setUserEmailAddressId(int userEmailAddressId)
	{
		this->userEmailAddressId = userEmailAddressId;
	}

	std::string getConfirmationKey() const
	{
		return confirmationKey;
	}

	void setConfirmationKey(const std::string &confirmationKey)
	{
		this->confirmationKey = confirmationKey;
	}
};

#endif

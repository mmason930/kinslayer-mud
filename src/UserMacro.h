#ifndef USER_MACRO_H
#define USER_MACRO_H

#include "DataObjectWithIntId.h"
#include "DateTime.h"
#include <string>

class UserMacro : public DataObjectWithIntId
{
protected:
	unsigned int userId;
	unsigned short keyCode;
	std::string replacement;
	DateTime createdDatetime;
public:

	UserMacro()
	{
		id = -1;
	}

	unsigned int getUserId() const
	{
		return userId;
	}
	
	void setUserId(const unsigned int userId)
	{
		this->userId = userId;
	}

	unsigned short getKeyCode() const
	{
		return keyCode;
	}

	void setKeyCode(const unsigned short keyCode)
	{
		this->keyCode = keyCode;
	}

	std::string getReplacement() const
	{
		return replacement;
	}

	void setReplacement(const std::string &replacement)
	{
		this->replacement = replacement;
	}

	DateTime getCreatedDatetime() const
	{
		return createdDatetime;
	}

	void setCreatedDatetime(const DateTime &createdDatetime)
	{
		this->createdDatetime = createdDatetime;
	}
};

#endif

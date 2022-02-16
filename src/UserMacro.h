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
	std::optional<unsigned short> location;
	std::string replacement;
	std::string code;
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

	std::optional<unsigned short> getLocation() const
	{
		return location;
	}

	void setLocation(const std::optional<unsigned short> &location)
	{
		this->location = location;
	}

	std::string getReplacement() const
	{
		return replacement;
	}

	void setReplacement(const std::string &replacement)
	{
		this->replacement = replacement;
	}

	std::string getCode() const
	{
		return code;
	}

	void setCode(const std::string &code)
	{
		this->code = code;
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

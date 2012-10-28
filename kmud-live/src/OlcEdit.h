#ifndef OLC_EDIT_H
#define OLC_EDIT_H

#include "DataObjectWithIntId.h"
#include "DateTime.h"
#include "OlcEditType.h"

class OlcEdit : public DataObjectWithIntId
{
private:
	int userId;
	OlcEditType *type;
	DateTime createdDatetime;
public:

	int getUserId() const
	{
		return userId;
	}

	void setUserId(int userId)
	{
		this->userId = userId;
	}

	OlcEditType *getType() const
	{
		return type;
	}

	void setType(OlcEditType *type)
	{
		this->type = type;
	}

	DateTime getCreatedDatetime() const
	{
		return createdDatetime;
	}

	void setCreatedDatetime(DateTime createdDatetime)
	{
		this->createdDatetime = createdDatetime;
	}
};

#endif

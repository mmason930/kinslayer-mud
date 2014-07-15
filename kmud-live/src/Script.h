#ifndef JS_SCRIPT_H
#define JS_SCRIPT_H

#include <string>
#include "DateTime.h"

#include "DataObjectWithIntId.h"

class Script : public DataObjectWithIntId
{
private:
protected:

	std::string methodName;
	int createdByUserId;
	DateTime createdDatetime;
	int lastModifiedByUserId;
	DateTime lastModifiedDatetime;

public:

	Script();
	Script(const int id, const std::string &methodName);
	~Script();

	std::string getMethodName() const;
	void setMethodName(const std::string &methodName);

	int getCreatedByUserId() const;
	void setCreatedByUserId(const int createdByUserId);

	DateTime getCreatedDatetime() const;
	void setCreatedDatetime(const DateTime &createdDatetime);

	int getLastModifiedByUserId() const;
	void setLastModifiedByUserId(const int lastModifiedByUserId);

	DateTime getLastModifiedDatetime() const;
	void setLastModifiedDatetime(const DateTime &lastModifiedDatetime);
};

#endif
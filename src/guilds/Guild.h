#ifndef GUILD_H
#define GUILD_H

#include <string>
#include <memory>

#include "../DataObjectWithIntId.h"
#include "../DateTime.h"

#include "GuildStatus.h"

class Guild : public DataObjectWithIntId
{
private:
protected:
	std::string name;
	DateTime createdDatetime;
	GuildStatus *status;
	DateTime statusLastModifiedDatetime;
	int createdByUserId;
	int race;

public:

	std::string getName() const;
	void setName(const std::string &name);

	DateTime getCreatedDatetime() const;
	void setCreatedDatetime(const DateTime &createdDatetime);

	GuildStatus *getStatus() const;
	void setStatus(GuildStatus *status);

	DateTime getStatusLastModifiedDatetime() const;
	void setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime);

	int getCreatedByUserId() const;
	void setCreatedByUserId(const int createdByUserId);

	int getRace() const;
	void setRace(int race);
};

#endif
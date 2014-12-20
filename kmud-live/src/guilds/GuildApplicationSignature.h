#ifndef GUILD_APPLICATION_SIGNATURE_H
#define GUILD_APPLICATION_SIGNATURE_H

#include "../DataObjectWithIntId.h"
#include "../DateTime.h"

#include "GuildApplicationSignatureStatus.h"

class GuildApplicationSignature : public DataObjectWithIntId
{
private:
protected:
	int userId;
	int guildApplicationId;
	GuildApplicationSignatureStatus *status;
	DateTime createdDatetime;
	DateTime statusLastModifiedDatetime;
	int statusChangedByUserId;
public:

	GuildApplicationSignature() : DataObjectWithIntId() {}
	~GuildApplicationSignature() {}

	int getUserId() const;
	void setUserId(int userId);

	int getGuildApplicationId() const;
	void setGuildApplicationId(int guildApplicationId);

	GuildApplicationSignatureStatus *getStatus() const;
	void setStatus(GuildApplicationSignatureStatus *status);

	DateTime getCreatedDatetime() const;
	void setCreatedDatetime(const DateTime &createdDatetime);

	DateTime getStatusLastModifiedDatetime() const;
	void setStatusLastModifiedDatetime(const DateTime &statusLastModifiedDatetime);

	int getStatusChangedByUserId() const;
	void setStatusChangedByUserId(int statusChangedByUserId);

};

#endif

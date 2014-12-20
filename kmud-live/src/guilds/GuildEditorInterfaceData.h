#ifndef GUILD_EDITOR_INTERFACE_DATA_H
#define GUILD_EDITOR_INTERFACE_DATA_H

#include <string>
#include <map>

#include "../editor-interface/EditorInterfaceData.h"

class GuildRank;
class GuildRankRole;

class GuildEditorInterfaceData : public EditorInterfaceData
{
private:
protected:
	std::string newGuildName;
	std::string newGuildDescription;
	std::string applicationDenialReason;
	int guildApplicationId;
	int guildId;
	int guildJoinApplicationId;
	GuildRank *guildRank;
public:
	
	std::map<int, GuildRankRole*> guildPrivilegeIdToGuildRankRoleMap;

	std::string getNewGuildName() const { return newGuildName; }
	void setNewGuildName(const std::string &newGuildName) { this->newGuildName = newGuildName; }

	std::string getNewGuildDescription() const { return newGuildDescription; }
	void setNewGuildDescription(const std::string &newGuildDescription) { this->newGuildDescription = newGuildDescription; }

	int getGuildApplicationId() const { return guildApplicationId; }
	void setGuildApplicationId(int guildApplicationId) { this->guildApplicationId = guildApplicationId; }

	std::string getApplicationDenialReason() const { return applicationDenialReason; };
	void setApplicationDenialReason(const std::string &applicationDenialReason) { this->applicationDenialReason = applicationDenialReason; }

	int getGuildId() const { return guildId; }
	void setGuildId(int guildId) { this->guildId = guildId; }

	int getGuildJoinApplicationId() const { return guildJoinApplicationId; }
	void setGuildJoinApplicationId(int guildJoinApplicationId) { this->guildJoinApplicationId = guildJoinApplicationId; }

	GuildRank *getGuildRank() const { return guildRank; };
	void setGuildRank(GuildRank *guildRank) { this->guildRank = guildRank; }
};

#endif
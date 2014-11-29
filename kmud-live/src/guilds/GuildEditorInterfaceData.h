#ifndef GUILD_EDITOR_INTERFACE_DATA_H
#define GUILD_EDITOR_INTERFACE_DATA_H

#include <string>

#include "../editor-interface/EditorInterfaceData.h"

class GuildEditorInterfaceData : public EditorInterfaceData
{
private:
protected:
	std::string newGuildName;
	std::string newGuildDescription;
	int reviewingGuildApplicationId;
public:

	std::string getNewGuildName() const { return newGuildName; }
	void setNewGuildName(const std::string &newGuildName) { this->newGuildName = newGuildName; }

	std::string getNewGuildDescription() const { return newGuildDescription; }
	void setNewGuildDescription(const std::string &newGuildDescription) { this->newGuildDescription = newGuildDescription; }

	int getReviewingGuildApplicationId() const { return reviewingGuildApplicationId; }
	void setReviewingGuildApplicationId(int reviewingGuildApplicationId) { this->reviewingGuildApplicationId = reviewingGuildApplicationId; }
};

#endif
#ifndef GUILD_INTERFACE_H
#define GUILD_INTERFACE_H

#include <mysql/sqlDatabase.h>

#include "../editor-interface/EditorInterface.h"

#include "../Game.h"

class GuildEditorInterface : public EditorInterface
{
private:
protected:
public:

	GuildEditorInterface();

	EditorInterfaceData *createData() const;
};

#endif
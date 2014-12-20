#ifndef GUILD_INTERFACE_H
#define GUILD_INTERFACE_H

#include <mysql/sqlDatabase.h>
#include "GuildEditorInterfaceData.h"
#include "../editor-interface/EditorInterface.h"

#include "../Game.h"

class GuildEditorInterfaceData;

class GuildEditorInterface : public EditorInterface<GuildEditorInterfaceData>
{
private:
protected:
public:

	GuildEditorInterface();
};

#endif
#ifndef OLC_EDIT_ROOM_H
#define OLC_EDIT_ROOM_H

#include "OlcEdit.h"

class OlcEditRoom : public OlcEdit
{
private:
	int roomId;
public:

	int getRoomId() const;
	{
		return roomId;
	}

	void setRoomId(int roomId)
	{
		this->roomId = roomId;
	}
}

#endif

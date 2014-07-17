#ifndef ROOM_SECTOR_H
#define ROOM_SECTOR_H

#include "../Enum.h"

class RoomSector : public Enum<RoomSector> {

private:

	std::string constantName;
	int movementLoss;

	RoomSector(int value, const std::string &standardName, const std::string &constantName, const int movementLoss) : Enum(value, standardName)
	{
		this->constantName = constantName;
		this->movementLoss = movementLoss;
	}

public:
	std::string getConstantName() { return constantName; }
	int getMovementLoss() {	return movementLoss; }

	static RoomSector *inside;
	static RoomSector *city;
	static RoomSector *field;
	static RoomSector *forest;
	static RoomSector *hills;
	static RoomSector *mountain;
	static RoomSector *waterSwim;
	static RoomSector *waterNoSwim;
	static RoomSector *underwater;
	static RoomSector *flying;
	static RoomSector *road;
};

#endif

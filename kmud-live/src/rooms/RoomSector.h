#ifndef ROOM_SECTOR_H
#define ROOM_SECTOR_H

#include "../Enum.h"

class RoomSector : public Enum {

private:

	static std::list<RoomSector*> enums;
	std::string constantName;
	int movementLoss;

	RoomSector(int value, const std::string &standardName, const std::string &constantName, const int movementLoss) : Enum(value, standardName)
	{
		this->constantName = constantName;
		this->movementLoss = movementLoss;
		enums.push_back(this);
	}

public:

	static Enum *getEnumByValue(int v)
	{
		for (auto iter = enums.begin(); iter != enums.end(); ++iter)
		{
			Enum *e = (*iter);
			if (e->getValue() == v)
			{
				return e;
			}
		}

		return NULL;
	}

	std::string getConstantName() { return constantName; }
	int getMovementLoss() {	return movementLoss; }

	static std::list<RoomSector*>::iterator getStartIterator() { return enums.begin(); }
	static std::list<RoomSector*>::iterator getEndIterator() { return enums.end(); }

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

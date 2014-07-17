#include "RoomSector.h"

template < >
std::list<Enum<RoomSector>*> Enum<RoomSector>::enums = std::list<Enum<RoomSector>*>();

RoomSector *RoomSector::inside = new RoomSector(		 0, "Inside"		, "SECT_INSIDE"			, 2);
RoomSector *RoomSector::city = new RoomSector(			 1, "City"			, "SECT_CITY"			, 2);
RoomSector *RoomSector::field = new RoomSector(			 2, "Field"			, "SECT_FIELD"			, 2);
RoomSector *RoomSector::forest = new RoomSector(		 3, "Forest"		, "SECT_FOREST"			, 2);
RoomSector *RoomSector::hills = new RoomSector(			 4, "Hills"			, "SECT_HILLS"			, 2);
RoomSector *RoomSector::mountain = new RoomSector(		 5, "Mountain"		, "SECT_MOUNTAIN"		, 3);
RoomSector *RoomSector::waterSwim = new RoomSector(		 6, "Water Swim"	, "SECT_WATER_SWIM"		, 3);
RoomSector *RoomSector::waterNoSwim = new RoomSector(	 7, "Water No Swim"	, "SECT_WATER_NOSWIM"	, 3);
RoomSector *RoomSector::underwater = new RoomSector(	 8, "Underwater"	, "SECT_UNDERWATER"		, 5);
RoomSector *RoomSector::flying = new RoomSector(		 9, "Flying"		, "SECT_FLYING"			, 1);
RoomSector *RoomSector::road = new RoomSector(			10, "Road"			, "SECT_ROAD"			, 2);

#ifndef ROOM_H
#define ROOM_H

#include <future>

#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"

class RoomSector;
class Exit;

class Room : public JSBindable, public Entity
{
protected:
	room_vnum vnum;											// Rooms number	(vnum)
	sh_int zone;											// Room zone (for resetting)
	char *name;												// Rooms name
	RoomSector *sector;										// Sector (Road, Field, Mountain, etc...)
	Character *eavesdroppingWarder;							// The person who is warding the room against listeners
	int auctionVnum;										// Vnum of the room's associated auction
	bool deleted;											// Whether or not the room should be deleted on next zone save.
	sbyte light;												// Number of lightsources in room
public:
	struct UnorderedMapRowResult
	{
		sql::Query query;
		std::unordered_map<int, std::vector<sql::Row>> roomVnumToRowsMap;
	};

	struct VectorRowResult
	{
		sql::Query query;
		std::vector<sql::Row> rows;
	};

	static int nr_alloc;
	static int nr_dealloc;
	static std::set< int > corpseRooms;

	static std::future<VectorRowResult> roomQueryFuture;
	static std::future<UnorderedMapRowResult> exitQueryFuture;
	static std::future<UnorderedMapRowResult> jsFuture;
	static std::future<sql::Query> objectsFuture;

	char	*description;									// Shown when entered
	ExtraDescription *ex_description;				// for examine/look
	Exit *dir_option[NUM_OF_DIRS];				// Room exits
	int room_flags;											// DEATH,DARK ... etc
	SPECIAL(*func);
	std::vector<class Character*> eavesdropping;			// Characters eavesdropping on this room
	Object *contents;									// List of items in room
	Character *people;								// List of NPC / PC in room
	std::list<Track *> tracks;
	PokerTable *PTable;

	//Getter / Setters
	room_vnum getVnum() const;
	void setVnum(const room_vnum vnum);

	sh_int getZoneNumber() const;
	void setZoneNumber(const sh_int zoneNumber);

	const char *getName() const;
	void setName(const char *name);

	RoomSector *getSector() const;
	void setSector(RoomSector *sector);

	Character *getEavesdroppingWarder();
	void setEavesdroppingWarder(Character *eavesdroppingWarder);

	int getAuctionVnum() const;
	void setAuctionVnum(const int auctionVnum);

	bool isDeleted() const;
	void setDeleted(bool deleted);

	sbyte getLight() const;
	void setLight(sbyte light);


	//Miscellaneous Methods
	int getLinesInDescription();

	std::list< Character* > getPeople();

	bool isDark();
	bool IsPurged() { return false; }
	const char *getDisplayName();

	std::list< class Gate* > getGates();
	void removeGate(class Gate* _Gate);
	int getNumberOfGates();
	int getNumberOfExits();
	int getDistanceToRoom(Room* OtherRoom);
	int findFirstStep(Room* OtherRoom);
	std::list<int> pathToRoom(Room *OtherRoom);

	void setDoorBit(int dir, int bit);
	void setDoorBitOneSide(int dir, int bit);
	void removeDoorBit(int dir, int bit);
	void removeDoorBitOneSide(int dir, int bit);
	void copy(const Room *source, bool deep = true);
	void zero();
	void zero(bool free);
	Object *findFirstObject(const std::function<bool(class Object *obj)> &predicate);

	void addToBatch(sql::BatchInsertStatement &roomInsert, sql::BatchInsertStatement &exitInsert, sql::BatchInsertStatement &jsInsert);

	bool isTrackable();

	std::string fadeCode();
	std::string gateCode();

	class Zone * getZone();

	Room *operator =(Room &source);

	Room(const Room *source);
	Room(const Room &source);
	Room();
	~Room();

	Room *getNeighbor(const int dir);

	void freeLiveRoom();
	void deleteFromDatabase();

	static void preBootWorld();
	static void bootWorld(std::unordered_map<Room *, std::unordered_map<int, int>> &roomToExitToVnumMap);
	static Room *boot(const sql::Row &MyRow, const std::vector< sql::Row > &MyExits,
		const std::vector< sql::Row > &MyJS, std::unordered_map<Room *, std::unordered_map<int, int>> &roomToExitToVnumMap);
	static void renumberRoomExits(const std::unordered_map<Room *, std::unordered_map<int, int>> &roomToExitToVnumMap);

	std::list< Object* > loadItemList(bool recursive);
	void loadItems();
	void loadItems(const std::list< Object* > &lItems);
	void itemSave();
	void corpseSave();
	static void saveCorpseRooms();
	std::string getDisplayableId();
	Room *getRoom();
	class EntityType *getEntityType();

private:
};

#endif
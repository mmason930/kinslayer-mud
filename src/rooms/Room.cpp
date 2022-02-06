#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../db.h"
#include "../utils.h"
#include "../weaves.h"
#include "../md5.h"
#include "../EntityType.h"
#include "../zones.h"
#include "../constants.h"
#include "../weather.h"
#include "../threaded_jobs.h"
#include "../js/js.h"
#include "../poker.h"
#include "Room.h"
#include "RoomSector.h"
#include "Exit.h"

//Returns the number of lines in the room's description.
int Room::getLinesInDescription()
{
	int lines = 0;

	for (const char *ptr = description; *ptr; ++ptr)
	{
		if ((*ptr) == '\n')
			++lines;
	}

	return lines;
}

//Returns the number of exits in the room.
int Room::getNumberOfExits()
{
	int num = 0;
	for (int i = 0; i < NUM_OF_DIRS; ++i)
	if (this->getNeighbor(i))
			num++;
	return num;
}

int Room::getNumberOfGates()
{
	return GateManager::GetManager().GetGatesgetRoom(this).size();
}

void Room::removeGate(Gate* _Gate)
{
	GateManager::GetManager().RemoveGate(_Gate);
}

Object *Room::findFirstObject(const std::function<bool(class Object *obj)> &predicate)
{
	for (Object *obj = contents; obj; obj = obj->next_content)
	{
		if (predicate(obj))
			return obj;
	}

	return NULL;
}

bool Room::isTrackable()
{
	return (!ROOM_FLAGGED(this, ROOM_INDOORS) && !ROOM_FLAGGED(this, ROOM_NOTRACK));
}

std::string Room::getDisplayableId()
{
	return MiscUtil::toString(this->vnum);
}

Room *Room::getRoom()
{
	return this;
}

class EntityType *Room::getEntityType()
{
	return EntityType::room;
}

Room *Room::getNeighbor(const int dir)
{
	if (dir >= 0 && dir < NUM_OF_DIRS && this->dir_option[dir] != 0)
		return this->dir_option[dir]->getToRoom();
	return static_cast< Room* >(0);
}

std::list< Character* > Room::getPeople()
{
	std::list<Character*> lPeople;
	for (Character *p = this->people; p; p = p->next_in_room)
		lPeople.push_back(p);
	return lPeople;
}

std::string Room::fadeCode()
{
	static std::stringstream Code;
	Code.str("");
	Code << MD5::getHashFromString(itos(this->vnum).c_str());
	return Code.str();
}

std::string Room::gateCode()
{
	static std::stringstream Code;
	Code.str("");
	Code << MD5::getHashFromString(itos(this->vnum * 2).c_str());
	return Code.str();
}

//Reserve prototype-sensitive information.
void Room::freeLiveRoom()
{
	this->PTable = NULL;
	this->tracks.clear();
}

void Room::setDoorBit(int dir, int bit)
{
	if (this->dir_option[dir])
	{
		this->dir_option[dir]->setFlag(bit);

		if (this->dir_option[dir]->getToRoom() && this->dir_option[dir]->getToRoom()->dir_option[rev_dir[dir]]) {
			this->dir_option[dir]->getToRoom()->dir_option[rev_dir[dir]]->setFlag(bit);
		}
	}
}
void Room::setDoorBitOneSide(int dir, int bit)
{
	if (this->dir_option[dir])
		this->dir_option[dir]->setFlag(bit);
}
void Room::removeDoorBit(int dir, int bit)
{
	if (this->dir_option[dir])
	{
		this->dir_option[dir]->removeFlag(bit);

		if (this->dir_option[dir]->getToRoom() && this->dir_option[dir]->getToRoom()->dir_option[rev_dir[dir]]) {
			this->dir_option[dir]->getToRoom()->dir_option[rev_dir[dir]]->removeFlag(bit);
		}
	}
}
void Room::removeDoorBitOneSide(int dir, int bit)
{
	if (this->dir_option[dir])
		this->dir_option[dir]->removeFlag(bit);
}

Zone *Room::getZone()
{
	return ZoneManager::GetManager().GetZoneByRnum(zone);
}

bool Room::isDark()
{
	bool dark = false;

	//Rhollor 7.22.09 - Added check for a Lamp Post
	bool lampPresent = false;
	Object *lamp;

	for (lamp = this->contents; lamp; lamp = lamp->next_content)
	{
		if (lamp->getType() == ITEM_LAMPPOST)
			lampPresent = true;
	}

	if (this->light)
		return false;

	else if (lampPresent == true)
		dark = false;

	else if (ROOM_FLAGGED(this, ROOM_DARK))
		dark = true;

	else if (ROOM_FLAGGED(this, ROOM_LIT))
		dark = false;

	else if (getSector() == RoomSector::inside)
	{
		Zone *MyZone = this->getZone();
		if (MyZone == NULL)
		{
			MudLog(CMP, LVL_GOD, TRUE, "Room %d has an invalid zone.", this->vnum);
			dark = false;
		}
		else
		{
			switch (Weather::getSun())
			{
			case SUN_DARK:
				dark = true;
				break;
			case SUN_SET:
			case SUN_LIGHT:
			case SUN_RISE:
				dark = false;
				break;
				break;
			}
		}
	}

	return dark;
}

void Room::deleteFromDatabase()
{
	std::stringstream QueryBuffer;

	try {
		QueryBuffer << "DELETE FROM rooms WHERE vnum='" << this->vnum << "';";
		gameDatabase->sendRawQuery(QueryBuffer.str());

		QueryBuffer.str("");

		QueryBuffer << "DELETE FROM roomExit WHERE room_vnum='" << this->vnum << "';";
		gameDatabase->sendRawQuery(QueryBuffer.str());

		QueryBuffer.str("");

		QueryBuffer << "DELETE FROM js_attachments WHERE target_vnum='" << this->vnum << "' AND type='R';";
		gameDatabase->sendRawQuery(QueryBuffer.str());

	}
	catch (sql::QueryException e) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to delete room #%d from database: %s", this->vnum, e.getMessage().c_str());
		return;
	}
}

void Room::zero(bool free)
{
	if (!free)
	{
		this->zero();
		return;
	}
}
void Room::zero()
{
	this->vnum = NOWHERE;
	this->zone = 0;
	this->setSector(RoomSector::inside);
	this->name = NULL;
	this->description = NULL;
	this->ex_description = NULL;
	this->light = 0;
	this->contents = NULL;
	this->people = NULL;
	this->PTable = NULL;
	this->deleted = false;
	this->eavesdroppingWarder = NULL;
	this->auctionVnum = -1;
	this->room_flags = 0;
	this->func = (NULL);
	this->js_scripts = std::shared_ptr<std::vector<JSTrigger*> >(new std::vector< JSTrigger* >);

	memset(&this->dir_option, 0, sizeof(dir_option));
}

std::list< Gate* > Room::getGates()
{
	return GateManager::GetManager().GetGatesgetRoom(this);
}

void Room::copy(const Room *source, bool deep)
{
	if (deep == true)
	{
		this->contents = source->contents;
		this->people = source->people;
		this->PTable = source->PTable;
		for (std::list<Track*>::const_iterator tIter = source->tracks.begin(); tIter != source->tracks.end(); ++tIter)
		{
			this->tracks.push_back((*tIter)->Clone());
			this->tracks.back()->room = this;
		}
		//		this->Tracks		= source->Tracks;
		this->setDeleted(source->isDeleted());
	}
	this->func = source->func;
	this->light = source->light;
	this->room_flags = source->room_flags;
	//	memcpy(&this->room_flags, &source->room_flags, sizeof(int) * 4);
	this->setSector(source->getSector());
	this->zone = source->zone;
	this->setAuctionVnum(source->getAuctionVnum());
	this->js_scripts = source->js_scripts;

	this->name = str_dup(source->name ? source->name : "undefined");
	this->description = str_dup(source->description ? source->description : "undefined\r\n");

	int i;
	for (i = 0; i < NUM_OF_DIRS; ++i)
	{
		if (source->dir_option[i])
		{
			this->dir_option[i] = new Exit();
			*this->dir_option[i] = *source->dir_option[i];
		}
		else if (this->dir_option[i] != NULL)
		{
			delete this->dir_option[i];
			this->dir_option[i] = NULL;
		}
	}

	if (source->ex_description)
	{
		struct ExtraDescription *thist, *temp, *temp2;
		temp = new ExtraDescription;
		memset(temp, 0, sizeof(ExtraDescription));

		this->ex_description = temp;

		for (thist = source->ex_description; thist; thist = thist->next)
		{
			temp->keyword = thist->keyword ? str_dup(thist->keyword) : 0;
			temp->description = thist->description ? str_dup(thist->description) : 0;

			if (thist->next)
			{
				temp2 = new ExtraDescription;
				memset(temp2, 0, sizeof(ExtraDescription));
				temp->next = temp2;
				temp = temp2;
			}
			else
				temp->next = 0;
		}
	}
}
Room *Room::operator =(Room &source)
{
	this->copy(&source);
	return (&source);
}
Room::Room(const Room &source)
{
	++Room::nr_alloc;
	this->zero();
	this->copy(&source);
}

Room::Room(const Room *source)
{
	Room::nr_alloc++;
	this->zero();
	this->copy(source);
}

// Galnor - 3-24-2005 - Constructor for Rooms.
Room::Room()
{
	Room::nr_alloc++;
	this->zero();
}

//Room Destructor
Room::~Room()
{
	Room::nr_dealloc++;
	JSManager::get()->handleExtraction(this);
	std::list< Gate* > GatesInRoom = GateManager::GetManager().GetGatesgetRoom(this);
	for (std::list<Gate*>::iterator gIter = GatesInRoom.begin(); gIter != GatesInRoom.end(); ++gIter)
		GateManager::GetManager().RemoveGate((*gIter));
	//Delete the room's directions...
	for (unsigned int i = 0; i < NUM_OF_DIRS; ++i)
	{
		if (this->dir_option[i] != NULL)
			delete this->dir_option[i];
	}
	//Delete the room's name...
	delete[] this->name;
	//Delete the room's descriptions...
	delete[] this->description;
	ExtraDescription *ExDNext;
	for (ExtraDescription *ExD = this->ex_description; ExD; ExD = ExDNext)
	{
		ExDNext = ExD->next;
		delete ExD;
	}
	while (!this->tracks.empty())
	{
		delete (this->tracks.front());
		this->tracks.pop_front();
	}

	if (this->PTable)
		delete this->PTable;
}

void Room::bootWorld()
{

	class PassiveRoomQueryReleaseJob : public Job
	{
		sql::Query RoomQuery, ExitQuery, jsQuery, ObjectQuery;
	public:
		PassiveRoomQueryReleaseJob(sql::Query RoomQuery, sql::Query ExitQuery, sql::Query jsQuery, sql::Query ObjectQuery)
		{
			this->RoomQuery = RoomQuery;
			this->ExitQuery = ExitQuery;
			this->jsQuery = jsQuery;
			this->ObjectQuery = ObjectQuery;
		}
		virtual void performRoutine()
		{
			this->RoomQuery.reset();
			this->ExitQuery.reset();
			this->jsQuery.reset();
			this->ObjectQuery.reset();
		}
		virtual void performPostJobRoutine()
		{
			//...
		}
	};

	sql::Query RoomQuery, ExitQuery, jsQuery, ObjectQuery;
	std::list< sql::Row > MyExits, MyJS;
	std::list< Object* > MyObjects;

	Clock clock1;
	clock1.turnOn();
	try {
		std::stringstream roomQueryBuffer, exitQueryBuffer, objectQueryBuffer;

		roomQueryBuffer << " SELECT *"
			<< " FROM rooms"
			<< " ORDER BY vnum ASC";

		exitQueryBuffer << " SELECT *"
			<< " FROM roomExit"
			<< " ORDER BY room_vnum ASC";

		objectQueryBuffer << " SELECT id, holder_id"
			<< " FROM objects"
			<< " WHERE holder_type='R'"
			<< " AND holder_id IN(SELECT vnum FROM rooms)"
			<< " ORDER BY (holder_id+0) ASC";

		RoomQuery = gameDatabase->sendQuery(roomQueryBuffer.str());
		ExitQuery = gameDatabase->sendQuery(exitQueryBuffer.str());
		ObjectQuery = gameDatabase->sendQuery(objectQueryBuffer.str());
		jsQuery = gameDatabase->sendQuery("SELECT * FROM js_attachments WHERE type='R' ORDER BY target_vnum ASC,id ASC;");
	}
	catch (sql::QueryException e) {
		MudLog(BRF, LVL_APPR, TRUE, "Could not send query in Room::bootWorld() : %s", e.getMessage().c_str());
		exit(1);
	}

	while (RoomQuery->hasNextRow())
	{
		sql::Row MyRow = RoomQuery->getRow();
		MyExits.clear();
		MyJS.clear();
		MyObjects.clear();

		while (ExitQuery->hasNextRow() && ExitQuery->peekRow()["room_vnum"] == MyRow["vnum"])
			MyExits.push_back(ExitQuery->getRow());

		while (ObjectQuery->hasNextRow() && ObjectQuery->peekRow()["holder_id"] == MyRow["vnum"])
		{
			boost::uuids::string_generator uuidGenerator;
			boost::uuids::uuid objID = uuidGenerator(ObjectQuery->getRow()["id"].c_str());
			Object *obj = Object::loadSingleItem(objID, false);
			if (obj != NULL) {
				MyObjects.push_back(obj);
			}
		}
		while (jsQuery->hasNextRow() && jsQuery->peekRow()["target_vnum"] == MyRow["vnum"])
			MyJS.push_back(jsQuery->getRow());

		Room *r = Room::boot(MyRow, MyExits, MyJS, MyObjects);

		if (r != NULL)
			World.push_back(r);
	}

	/***
	* Cleanup of these resources is a monumental task. As such, it would be better to run it behind the
	* scenes with a separate thread. First, we copy the four boost::shared_ptr's into this class.
	* We then reset them so they bring their reference count down to 1. Once this happens, we can run
	* the thread and exit this function.
	*
	***/

	MyExits.clear();
	MyJS.clear();

	//We want a guarantee that the resources will be released.
	assert(RoomQuery.use_count() == 1);
	assert(ExitQuery.use_count() == 1);
	assert(jsQuery.use_count() == 1);
	assert(ObjectQuery.use_count() == 1);

	Job *cleanupJob = new PassiveRoomQueryReleaseJob(RoomQuery, ExitQuery, jsQuery, ObjectQuery);

	//Drop use count down to 1.
	RoomQuery.reset();
	ExitQuery.reset();
	jsQuery.reset();
	ObjectQuery.reset();

	//And deploy the thread...
	ThreadedJobManager::get().addJob(cleanupJob);
}

Room *Room::boot(const sql::Row &MyRow,
	const std::list< sql::Row > &MyExits,
	const std::list< sql::Row > &MyJS,
	const std::list< Object* > &MyObjects)
{
	static bool hasRoomExitIndexes = false;
	static int EXIT_ROOM_VNUM_INDEX;
	static int EXIT_TO_ROOM_INDEX;
	static int EXIT_GENERAL_DESCRIPTION_INDEX;
	static int EXIT_KEYWORD_INDEX;
	static int EXIT_EXIT_INFO_INDEX;
	static int EXIT_HIDDEN_LEVEL_INDEX;
	static int EXIT_PICK_REQ_INDEX;
	static int EXIT_KEY_VNUM_INDEX;
	static int EXIT_DIR_INDEX;

	Room *NewRoom = new Room();
	Zone *zone;
	if ((zone = ZoneManager::GetManager().GetZoneByVnum(MiscUtil::convert<unsigned int>(MyRow["znum"]))) == NULL)
	{
		MudLog(BRF, LVL_APPR, TRUE,
			"Attempting to load room with invalid zone vnum. Room: %d. Zone: %d.", atoi(MyRow["vnum"].c_str()),
			MiscUtil::convert<unsigned int>(MyRow["znum"]));
		return (NULL);
	}

	NewRoom->name = str_dup(MyRow["name"].c_str());
	NewRoom->description = str_dup(MyRow["description"].c_str());
	NewRoom->zone = zone->GetRnum();
	NewRoom->room_flags = atoi(MyRow["room_flags"].c_str());
	NewRoom->setAuctionVnum(atoi(MyRow["auction_vnum"].c_str()));
	NewRoom->setSector((RoomSector*)RoomSector::getEnumByValue(atoi(MyRow["sector"].c_str())));
	NewRoom->vnum = atoi(MyRow["vnum"].c_str());
	NewRoom->ex_description = ExtraDescription::Parse(MyRow["edescription"]);

	if (hasRoomExitIndexes == false && MyExits.empty() == false) {
		sql::Row row = MyExits.front();
		EXIT_ROOM_VNUM_INDEX = row.getIndexByField("room_vnum");
		EXIT_TO_ROOM_INDEX = row.getIndexByField("to_room");
		EXIT_GENERAL_DESCRIPTION_INDEX = row.getIndexByField("general_description");
		EXIT_KEYWORD_INDEX = row.getIndexByField("keyword");
		EXIT_EXIT_INFO_INDEX = row.getIndexByField("exit_info");
		EXIT_HIDDEN_LEVEL_INDEX = row.getIndexByField("hidden_level");
		EXIT_PICK_REQ_INDEX = row.getIndexByField("pick_req");
		EXIT_KEY_VNUM_INDEX = row.getIndexByField("key_vnum");
		EXIT_DIR_INDEX = row.getIndexByField("dir");
		hasRoomExitIndexes = true;
	}

	for (std::list< sql::Row >::const_iterator eRow = MyExits.begin(); eRow != MyExits.end(); ++eRow)
	{
		int dir = atoi((*eRow)[EXIT_DIR_INDEX].c_str());

		if (dir >= 0 && dir < NUM_OF_DIRS)
		{
			NewRoom->dir_option[dir] = new Exit();
			NewRoom->dir_option[dir]->setGeneralDescription((*eRow)[EXIT_GENERAL_DESCRIPTION_INDEX].c_str());
			NewRoom->dir_option[dir]->setKeywords((*eRow)[EXIT_KEYWORD_INDEX].c_str());
			NewRoom->dir_option[dir]->setExitInfo(atoi((*eRow)[EXIT_EXIT_INFO_INDEX].c_str()));
			NewRoom->dir_option[dir]->setPickRequirement((sbyte)(atoi((*eRow)[EXIT_PICK_REQ_INDEX].c_str())));
			NewRoom->dir_option[dir]->setHiddenLevel((sbyte)(atoi((*eRow)[EXIT_HIDDEN_LEVEL_INDEX].c_str())));
			NewRoom->dir_option[dir]->setKey(atoi((*eRow)[EXIT_KEY_VNUM_INDEX].c_str()));
			NewRoom->dir_option[dir]->setToRoom((Room*)(atoi((*eRow)[EXIT_TO_ROOM_INDEX].c_str())));
			//This last line is a HACK!
			//Instead of wasting memory on a seperate int to hold the vnum,
			//we'll just store it in the pointer until later.
		}
	}
	NewRoom->js_scripts = std::shared_ptr<std::vector<JSTrigger*> >(new std::vector<JSTrigger*>());
	for (std::list< sql::Row >::const_iterator jsIter = MyJS.begin(); jsIter != MyJS.end(); ++jsIter)
	{
		int vnum = atoi((*jsIter)["script_vnum"].c_str());
		JSTrigger* t = JSManager::get()->getTrigger(vnum);
		NewRoom->js_scripts->push_back(t);
	}

	if (!MyObjects.empty())
		NewRoom->loadItems(MyObjects);

	return (NewRoom);
}

void Room::addToBatch(sql::BatchInsertStatement &roomInsert, sql::BatchInsertStatement &exitInsert, sql::BatchInsertStatement &jsInsert)
{
	roomInsert.beginEntry();
	Zone *myZone = this->getZone();

	roomInsert.putLong(this->vnum);
	roomInsert.putLong(myZone ? myZone->getVnum() : -1);
	roomInsert.putString(this->name ? this->name : "");
	roomInsert.putString(this->description ? this->description : "");
	roomInsert.putInt(this->getSector()->getValue());
	roomInsert.putInt(this->room_flags);
	roomInsert.putInt(this->getAuctionVnum());
	roomInsert.putString(ExtraDescription::Serialize(this->ex_description));

	roomInsert.endEntry();

	for (unsigned int dir = 0; dir < NUM_OF_DIRS; ++dir)
	{
		if (this->dir_option[dir] == NULL || this->dir_option[dir]->getToRoom() == NULL)
			continue;
		Exit *exit = this->dir_option[dir];
		exitInsert.beginEntry();

		exitInsert.putInt(this->vnum);
		exitInsert.putInt(exit->getToRoom()->vnum);
		exitInsert.putString(exit->getGeneralDescription() ? exit->getGeneralDescription() : "");
		exitInsert.putString(exit->getKeywords() ? exit->getKeywords() : "");
		exitInsert.putInt(exit->getExitInfo());
		exitInsert.putInt(exit->getHiddenLevel());
		exitInsert.putInt(exit->getPickRequirement());
		exitInsert.putInt(exit->getKey());
		exitInsert.putInt(dir);

		exitInsert.endEntry();
	}

	for (unsigned int i = 0; i < this->js_scripts->size(); ++i) {
		jsInsert.beginEntry();

		jsInsert.putString("R");
		jsInsert.putLong(this->vnum);
		jsInsert.putLong(this->js_scripts->at(i)->vnum);

		jsInsert.endEntry();
	}
}


room_vnum Room::getVnum() const
{
	return vnum;
}

void Room::setVnum(const room_vnum vnum)
{
	this->vnum = vnum;
}

sh_int Room::getZoneNumber() const
{
	return zone;
}

void Room::setZoneNumber(const sh_int zoneNumber)
{
	this->zone = zoneNumber;
}

const char *Room::getName() const
{
	return name;
}

void Room::setName(const char *name)
{
	if(this->name)
		delete[] this->name;

	this->name = name ? str_dup(name) : NULL;
}

RoomSector *Room::getSector() const
{
	return sector;
}

void Room::setSector(RoomSector *sector)
{
	this->sector = sector;
}

Character *Room::getEavesdroppingWarder()
{
	return eavesdroppingWarder;
}

void Room::setEavesdroppingWarder(Character *eavesdroppingWarder)
{
	this->eavesdroppingWarder = eavesdroppingWarder;
}

int Room::getAuctionVnum() const
{
	return auctionVnum;
}

void Room::setAuctionVnum(const int auctionVnum)
{
	this->auctionVnum = auctionVnum;
}

bool Room::isDeleted() const
{
	return deleted;
}

void Room::setDeleted(bool deleted)
{
	this->deleted = deleted;
}

sbyte Room::getLight() const
{
	return light;
}

void Room::setLight(sbyte light)
{
	this->light = light;
}

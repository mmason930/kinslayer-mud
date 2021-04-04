#ifndef EXIT_H
#define EXIT_H

#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"

class Character;
class Room;

class Exit
{
protected:
	char *generalDescription;
	char *keywords;

	sh_int exitInfo;
	obj_vnum key;
	Room *toRoom;
	sbyte hiddenLevel;
	sbyte pickRequirement;

public:
	Exit();
	~Exit();

	Exit& operator =(const Exit& source);

	char **getPointerToGeneralDescription();
	const char *getGeneralDescription() const;
	void setGeneralDescription(const char *generalDescription);

	const char *getKeywords() const;
	void setKeywords(const char *keywords);

	sh_int getExitInfo() const;
	void setExitInfo(const sh_int exitInfo);

	obj_vnum getKey() const;
	void setKey(const obj_vnum key);

	Room *getToRoom() const;
	void setToRoom(Room *toRoom);

	sbyte getHiddenLevel() const;
	void setHiddenLevel(const sbyte hiddenLevel);

	sbyte getPickRequirement() const;
	void setPickRequirement(const sbyte pickRequirement);

	bool isFlagged(const int flag) const;

	bool isPickProof() const;
	bool isClosed() const;
	bool isOpen() const;
	bool canOpen() const;
	bool isDoor() const ;
	bool isLocked() const;
	bool canLock() const;
	bool canPick(Character *ch) const;
	bool canBeSeen(Character *ch) const;
	bool isRammable() const;
	void setRammable();
	bool isDisabled() const;
	bool isTemporary() const;
	void enable();
	void disable();
	void makeTemporary();
	void makePermanent();
	void unsetRammable();
	void toggleRammable();
	void clear();
	void toggleFlag(const int flag);
	void setFlag(const int flag);
	void removeFlag(const int flag);
};

#endif
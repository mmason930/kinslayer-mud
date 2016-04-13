#include "../conf.h"
#include "../sysdep.h"
#include "../structs.h"
#include "../utils.h"
#include "../spells.h"
#include "Exit.h"

bool Exit::isFlagged(const int flag) const
{
	return IS_SET(this->exitInfo, flag);
}

bool Exit::isPickProof() const
{
	return (this->pickRequirement == -1);
}
bool Exit::isClosed() const
{
	return isFlagged(EX_CLOSED);
}
bool Exit::isOpen() const
{
	return !isFlagged(EX_CLOSED);
}
bool Exit::canOpen() const
{
	return isFlagged(EX_ISDOOR);
}
bool Exit::isDoor() const
{
	return isFlagged(EX_ISDOOR);
}
bool Exit::isLocked() const
{
	return isFlagged(EX_LOCKED);
}
bool Exit::canPick(Character *ch) const
{
	return (GET_SKILL(ch, SKILL_PICK_LOCK) >= this->pickRequirement);
}
bool Exit::canLock() const
{
	return (key != -1);
}
bool Exit::canBeSeen(Character *ch) const
{
	return (GET_SKILL(ch, SKILL_SEARCH) >= this->hiddenLevel);
}
bool Exit::isRammable() const
{
	return (IS_SET(this->exitInfo, EX_RAMMABLE));
}
void Exit::setRammable()
{
	SET_BITK(exitInfo, EX_RAMMABLE);
}
void Exit::unsetRammable()
{
	REMOVE_BIT(exitInfo, EX_RAMMABLE);
}
void Exit::toggleRammable()
{
	TOGGLE_BIT(exitInfo, EX_RAMMABLE);
}
bool Exit::isDisabled() const
{
	return IS_SET(this->exitInfo, EX_DISABLED);
}
bool Exit::isTemporary() const
{
	return IS_SET(this->exitInfo, EX_TEMPORARY);
}
void Exit::makePermanent()
{
	REMOVE_BIT(exitInfo, EX_TEMPORARY);
}
void Exit::makeTemporary()
{
	SET_BITK(exitInfo, EX_TEMPORARY);
}

void Exit::enable()
{
	REMOVE_BIT(exitInfo, EX_DISABLED);
}

void Exit::disable()
{
	SET_BITK(exitInfo, EX_DISABLED);
}

const char *Exit::getGeneralDescription() const
{
	return generalDescription;
}

void Exit::setGeneralDescription(const char *generalDescription)
{
	if (this->generalDescription)
		delete[] this->generalDescription;

	this->generalDescription = (generalDescription && *generalDescription) ? str_dup(generalDescription) : NULL;
}

const char *Exit::getKeywords() const
{
	return keywords;
}

void Exit::setKeywords(const char *keywords)
{
	if (this->keywords)
		delete[] this->keywords;

	this->keywords = (keywords && *keywords) ? str_dup(keywords) : NULL;
}

sh_int Exit::getExitInfo() const
{
	return exitInfo;
}

void Exit::setExitInfo(const sh_int exitInfo)
{
	this->exitInfo = exitInfo;
}

obj_vnum Exit::getKey() const
{
	return key;
}

void Exit::setKey(const obj_vnum key)
{
	this->key = key;
}

Room *Exit::getToRoom() const
{
	return toRoom;
}

void Exit::setToRoom(Room *toRoom)
{
	this->toRoom = toRoom;
}

byte Exit::getHiddenLevel() const
{
	return hiddenLevel;
}

void Exit::setHiddenLevel(const byte hiddenLevel)
{
	this->hiddenLevel = hiddenLevel;
}

byte Exit::getPickRequirement() const
{
	return pickRequirement;
}

void Exit::setPickRequirement(const byte pickRequirement)
{
	this->pickRequirement = pickRequirement;
}

void Exit::toggleFlag(const int flag)
{
	TOGGLE_BIT(exitInfo, flag);
}

void Exit::setFlag(const int flag)
{
	SET_BITK(exitInfo, flag);
}

void Exit::removeFlag(const int flag)
{
	REMOVE_BIT(exitInfo, flag);
}

char **Exit::getPointerToGeneralDescription()
{
	return &(generalDescription);
}

void Exit::clear()
{
	this->generalDescription = 0;
	this->keywords = 0;
	this->exitInfo = 0;
	this->key = -1;
	this->toRoom = 0;
	this->hiddenLevel = 0;
	this->pickRequirement = 0;
	this->setRammable();
}

Exit::Exit()
{
	this->clear();
}

Exit::~Exit()
{
	if (this->keywords)
		delete[] this->keywords;
	if (this->generalDescription)
		delete[](this->generalDescription);
}

Exit& Exit::operator =(const Exit& source)
{
	setGeneralDescription(source.getGeneralDescription());
	setKeywords(source.getKeywords());
	setExitInfo(source.getExitInfo());
	setKey(source.getKey());
	setToRoom(source.getToRoom());
	setHiddenLevel(source.getHiddenLevel());
	setPickRequirement(source.getPickRequirement());

	return (*this);
}

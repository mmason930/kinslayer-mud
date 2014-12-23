#include "../conf.h"
#include "../sysdep.h"

#include "../structs.h"
#include "../utils.h"
#include "../handler.h"

#include "../rooms/Room.h"
#include "../Descriptor.h"
#include "../spells.h"
#include "../interpreter.h"
#include "../comm.h"
#include "../js_functions.h"

#include "ItemUtil.h"

#include "../dotmode/DotMode.h"

ItemUtil *ItemUtil::self = nullptr;

extern Object *object_list;

ItemUtil::ItemUtil()
{
}

ItemUtil::~ItemUtil()
{
}

ItemUtil *ItemUtil::get()
{
	return (self == nullptr) ? (self = new ItemUtil()) : self;
}

void ItemUtil::destroy()
{
	auto selfPointer = self;

	self = nullptr;

	delete selfPointer;
}

Object *ItemUtil::getObjectInListVis(Character * ch, const char *name, Object *objectList)
{
	Object *i;
	int j = 0, num = 0;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;

	if( ch->IsPurged() ) return (0);

	strcpy(tmp, name);

	if (!(num = GetNumber(&tmp)))
		return nullptr;

	for (i = objectList; i && (j <= num); i = i->next_content)
	{
		if(!i->getName())
			continue;

		// Fogel 8/18/09: Fixed a bug with taking items when another of the same type is hidden
		if (isname(tmp, i->getName()) && !i->hidden)
			if (++j == num)
				return i;
	}

	return nullptr;
}

Object *ItemUtil::getObjectInListByRealNumber(int number, Object *objectList)
{
	for (Object *i = objectList; i; i = i->next_content)
		if (GET_OBJ_RNUM(i) == number)
			return i;

	return nullptr;
}

Object *ItemUtil::getObjectById(const boost::uuids::uuid &targetId)
{
	for(Object *obj = object_list;obj;obj = obj->next)
		if(obj->objID == targetId && !obj->IsPurged())
			return obj;
	return nullptr;
}

Object *ItemUtil::getObjectInEquipmentList(Character *wearer, const char *name)
{
	if( wearer->IsPurged() )
		return nullptr;

	for(int i = 0;i < NUM_WEARS;++i)
	{
		if( isname(GET_EQ(wearer, i)->getName(), name) )
			return GET_EQ(wearer, i);
	}

	return nullptr;
}

Object *ItemUtil::getObjectInEquipVis(Character *ch, const char *arg, Object *equipment[], int *j)
{
	for ((*j) = 0; (*j) < NUM_WEARS; (*j)++)
	{
		if (equipment[(*j)])
		{
			if (isname(arg, equipment[(*j)]->getName()))
				return (equipment[(*j)]);
		}
	}

	return nullptr;
}

Object *ItemUtil::getObjectVis(Character *ch, const char *name)
{
	if( ch->IsPurged() )
		return nullptr;

	Object *i;
	int j = 0, num = 0;
	char tmpname[MAX_INPUT_LENGTH];
	char *tmp = tmpname;

	// scan items carried
	if ((i = ItemUtil::get()->getObjectInListVis(ch, name, ch->carrying)))
		return i;

	// scan room
	if ((i = ItemUtil::get()->getObjectInListVis(ch, name, ch->in_room->contents)))
		return i;

	strcpy(tmp, name);
	if (!(num = GetNumber(&tmp)))
		return nullptr;

	// ok.. no luck yet. scan the entire obj list.
	for (i = object_list; i && (j <= num); i = i->next)
		if ( !i->IsPurged() && ((i->getName() && isname(tmp, i->getName()))) )
			if (CAN_SEE_OBJ(ch, i))
				if (++j == num)
					return i;

	return nullptr;
}

Object *ItemUtil::getObjectInList(const char *name, Object *objectList)
{
	Object *i;

	for (Object *i = objectList; i; i = i->next_content)
		if (isname(name, i->getName()))
			return i;
	return nullptr;
}

int ItemUtil::canTakeObject( Character * ch, Object * obj )
{
	if ( IS_CARRYING_N( ch ) >= CAN_CARRY_N( ch ) )
	{
		Act( "$p: you can't carry that many items.", FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	else if ( ch->TooHeavyToPickUp( obj ) )
	{
		Act( "$p: you can't carry that much weight.", FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	else if ( !( CAN_WEAR( obj, ITEM_WEAR_TAKE ) ) )
	{
		Act( "$p: you can't take that!", FALSE, ch, obj, 0, TO_CHAR );
		return 0;
	}

	return 1;
}

int ItemUtil::performGetFromRoom(Character *ch, Object *obj, bool vaultSave)
{
	if ( canTakeObject( ch, obj ) &&  !obj->IsPurged() && !ch->IsPurged()
		&& js_object_get(ch, obj) && !obj->IsPurged() && !ch->IsPurged()
		)
	{
		obj->RemoveFromRoom();
		obj_to_char( obj, ch );
		if(!IS_OBJ_STAT((obj), ITEM_INVISIBLE) || GET_LEVEL(ch) >= LVL_IMMORT)
		{//RHOLLOR 05.03.09 remove messg when invis
			Act( "You get $p.", FALSE, ch, obj, 0, TO_CHAR );
			Act( "$n gets $p.", TRUE, ch, obj, 0, TO_ROOM, NULL, true);
		}
		return 1;
	}

	return 0;
}

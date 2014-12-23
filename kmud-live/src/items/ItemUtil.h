#ifndef ITEM_UTIL_H
#define ITEM_UTIL_H

#include <boost/uuid/uuid.hpp>

class DotMode;

class ItemUtil
{
private:

	static ItemUtil *self;

	ItemUtil();
	~ItemUtil();
protected:
public:

	static ItemUtil *get();
	static void destroy();

	Object *getObjectInListVis(Character *ch, const char *name, Object *objectList);
	Object *getObjectInListByRealNumber(int number, Object *objectList);
	Object *getObjectById(const boost::uuids::uuid &targetId);
	Object *getObjectInEquipmentList(Character *wearer, const char *name);
	Object *getObjectVis(Character *ch, const char *name);
	Object *getObjectInEquipVis(Character *ch, const char *arg, Object *equipment[], int *j);
	Object *getObjectInList(const char *name, Object *objectList);

	int performGetFromRoom(Character *ch, Object *obj, bool vaultSave = true);
	int canTakeObject(Character *ch, Object *obj);
};

#endif

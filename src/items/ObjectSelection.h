#pragma once
#include "../utils.h"
#include <boost/uuid/uuid_io.hpp>
// INVENTORY and EQUIPMENT know your own gear even in darkness. Mirror their
// item flags (including inventory's spam-mode rule), rather than room lighting.
inline bool visibleOwnedItem(Character *ch, Object *obj) {
    if (!ch || !obj || obj->IsPurged()) return false;
    const bool equipped = obj->worn_by == ch;
    if (!equipped && (obj->carried_by != ch || obj->hidden)) return false;
    return !IS_OBJ_STAT(obj, ITEM_INVISIBLE) ||
        (GET_LEVEL(ch) >= LVL_IMMORT && (equipped || !PRF_FLAGGED(ch, PRF_SPAM)));
}
inline bool canSelectObject(Character *ch, Object *obj) {
    return obj && ((obj->carried_by == ch || obj->worn_by == ch) ?
        visibleOwnedItem(ch, obj) : bool(CAN_SEE_OBJ(ch, obj)));
}
// An exact selector still searches only the inventory/equipment/room list the
// ordinary command is allowed to inspect. It never searches the whole world.
inline bool exactObjectSelector(const char *name, Object *obj) {
    return name && name[0] == '@' && obj && !obj->IsPurged() &&
        boost::uuids::to_string(obj->objID) == name + 1;
}

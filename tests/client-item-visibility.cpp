#include <cassert>
#include <iostream>
struct Character { int level=1; bool spam=false, light=false; };
struct Object {
    Character *carried_by=nullptr, *worn_by=nullptr;
    bool hidden=false, invisible=false, purged=false;
    bool IsPurged() const { return purged; }
};
#define IS_OBJ_STAT(obj, flag) ((obj)->invisible)
#define GET_LEVEL(ch) ((ch)->level)
#define PRF_FLAGGED(ch, flag) ((ch)->spam)
#define CAN_SEE_OBJ(ch, obj) ((ch)->light && !(obj)->invisible)
constexpr int LVL_IMMORT=100;
#include "client-visibility-under-test.inc"
int main() {
    Character player, other;
    Object carried, equipped, room;
    carried.carried_by=&player; equipped.worn_by=&player;
    assert(visibleOwnedItem(&player,&carried));
    assert(visibleOwnedItem(&player,&equipped));
    assert(canSelectObject(&player,&carried));
    assert(!canSelectObject(&player,&room));
    assert(!visibleOwnedItem(&other,&carried));
    carried.hidden=true;
    assert(!visibleOwnedItem(&player,&carried));
    carried.hidden=false; carried.invisible=true; equipped.invisible=true;
    assert(!visibleOwnedItem(&player,&carried));
    assert(!visibleOwnedItem(&player,&equipped));
    player.level=100;
    assert(visibleOwnedItem(&player,&carried));
    assert(visibleOwnedItem(&player,&equipped));
    player.spam=true;
    assert(!visibleOwnedItem(&player,&carried));
    assert(visibleOwnedItem(&player,&equipped));
    equipped.purged=true;
    assert(!visibleOwnedItem(&player,&equipped));
    std::cout << "Owned items: darkness, invisibility, hidden items, native inventory modes and room isolation pass.\n";
}

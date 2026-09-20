// The runner includes the real JSObject storage method against these fixtures.
#include <cassert>
#include <iostream>
#include <string>
namespace flusspferd {
struct string : std::string {
    using std::string::string;
    std::string to_string() const { return *this; }
};
}
struct Object {
    bool prototype = false, purged = false, saveSucceeds = true;
    int rareCount = 2, saves = 0;
    bool IsProto() const { return prototype; }
    bool IsPurged() const { return purged; }
    static bool saveItemToTopLevelHolder(char type, const std::string &id, Object *object) {
        assert(type == 'A'); assert(id == "1");
        ++object->saves; return object->saveSucceeds;
    }
    void Extract(bool lowerItemCount) {
        if (lowerItemCount) --rareCount;
        purged = true;
    }
};
struct JSObject {
    Object *real;
    bool storeAndExtract(flusspferd::string holderType, flusspferd::string holderId);
};
#include "auction-storage-under-test.inc"
int main() {
    Object item;
    JSObject wrapper{&item};
    item.saveSucceeds = false;
    assert(!wrapper.storeAndExtract("A", "1"));
    assert(!item.purged && item.rareCount == 2 && item.saves == 1);
    item.saveSucceeds = true;
    assert(wrapper.storeAndExtract("A", "1"));
    assert(item.purged && item.rareCount == 2 && item.saves == 2);
    assert(!wrapper.storeAndExtract("A", "1"));
    assert(item.saves == 2);
    item.purged = false;
    item.prototype = true;
    assert(!wrapper.storeAndExtract("A", "1"));
    item.prototype = false;
    assert(!wrapper.storeAndExtract("", "1"));
    assert(!wrapper.storeAndExtract("AB", "1"));
    assert(!wrapper.storeAndExtract("A", ""));
    assert(item.saves == 2);
    wrapper.real = nullptr;
    assert(!wrapper.storeAndExtract("A", "1"));
    std::cout << "PASS: successful storage preserves rare counts; failed saves retain live items; invalid objects/holders are rejected\n";
}

// Production function bodies are extracted by the runner. Only surrounding
// game/database dependencies are stubbed, so no game data is needed.
#include <cassert>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr size_t MAX_INPUT_LENGTH = 512;
constexpr int ITEM_DRINKCON = 17, NUM_LIQ_TYPES = 1, BRF = 0, LVL_APPR = 100, TRUE = 1;
const char *drinks[] = {"water"};
struct Character { void *desc = nullptr; };
struct Object
{
    int item_number = -1, type = 0, values[3]{};
    std::string description;
    const char *name = "jewelry";
    const char *GetSDesc() const { return description.c_str(); }
    int getType() const { return type; }
    int getVnum() const { return 123; }
};
#define GET_OBJ_RNUM(object) ((object)->item_number)
#define GET_OBJ_VAL(object, index) ((object)->values[index])
std::vector<int> ItemCount{4, 7};
class JSObject { public: Object *real = nullptr; int getCount(); };
struct ShopItemEntry { Object *obj; int numberAvailable; };
std::vector<ShopItemEntry> entries;
std::string page;
class StringUtil
{
public:
    static std::string cap(std::string text)
    {
        if (!text.empty()) text[0] = std::toupper(static_cast<unsigned char>(text[0]));
        return text;
    }
};
void MudLog(int, int, int, const char *, ...) {}
int buy_price(Object *, int) { return 12345; }
unsigned long CalcGold(int price) { return price / 10000; }
unsigned long CalcSilver(int price) { return (price % 10000) / 100; }
unsigned long CalcCopper(int price) { return price % 100; }
bool is_ok(Character *, Character *, int) { return true; }
void OneArgument(char *arg, char *out) { std::strcpy(out, arg); }
bool isname(const char *, const char *) { return true; }
std::vector<ShopItemEntry> getShopItemEntries(Character *, Character *, int) { return entries; }
void page_string(void *, char *text, int) { page = text; }

#include "item-display-under-test.inc"

int main()
{
    Object object;
    JSObject wrapper;
    assert(wrapper.getCount() == 0);
    wrapper.real = &object;
    assert(wrapper.getCount() == 1); // A corpse has no prototype index.
    object.item_number = 1;
    assert(wrapper.getCount() == 7);
    object.item_number = 100;
    assert(wrapper.getCount() == 1);

    object.description = std::string(500, 'x');
    object.type = ITEM_DRINKCON;
    object.values[1] = 1;
    Character buyer, keeper;
    const std::string line = list_object(&buyer, &object, -1, 1, 0);
    assert(line.find(std::string(499, 'x') + " of water") != std::string::npos);
    entries.assign(500, {&object, -1});
    char argument[] = "";
    shopping_list(argument, &buyer, &keeper, 0);
    assert(page.size() > 250000); // Previously overflowed the 15 KB listing buffer.
    assert(page.find("500)") != std::string::npos);
    entries.clear();
    shopping_list(argument, &buyer, &keeper, 0);
    assert(page == "Currently, there is nothing for sale.\r\n");
    std::cout << "PASS: corpse/prototype counts, long shop names, large catalogs, empty shops\n";
}

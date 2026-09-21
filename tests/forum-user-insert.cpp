// Compile the production addForumUser body against a schema-only test database.
#include <mysql/sqlDatabase.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <string>

struct Character {
    struct {
        int idnum = 12345;
        std::string name = "ForumRegression", passwd = "synthetic-password-hash";
        struct { time_t birth = 1600000000; } time;
    } player;
};
class DateTime {
    time_t value;
public:
    DateTime(time_t value) : value(value) {}
    time_t getTime() const { return value; }
};
struct StringUtil {
    static std::string allLower(const char *text) {
        std::string result(text);
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
        return result;
    }
};
enum { BRF, LVL_APPR, TRUE };
void MudLog(int, int, int, const char *, ...) { throw std::runtime_error("Production forum INSERT failed"); }
sql::Connection gameDatabase;
struct ForumUtil { static void addForumUser(Character *); };
#include "forum-user-under-test.inc"

int main() {
    const char *schema = std::getenv("DB_SCHEMA");
    if (!schema || !std::string(schema).starts_with("kinslayer_test_forum_")) return 2;
    gameDatabase = std::make_shared<sql::_Connection>(std::getenv("DB_HOSTNAME"), std::getenv("DB_USERNAME"), std::getenv("DB_PASSWORD"), schema);
    Character character;
    ForumUtil::addForumUser(&character);
    auto row = gameDatabase->sendQuery("SELECT user_id,username,user_type,group_id FROM phpbb_users WHERE user_id=12345")->getRow();
    assert(row.getInt("user_id") == 12345 && row["username"] == "ForumRegression");
    assert(row.getInt("user_type") == 0 && row.getInt("group_id") == 2);
    std::cout << "PASS: production forum-account INSERT against installed phpBB schema\n";
}

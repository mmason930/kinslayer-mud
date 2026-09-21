#include "utils/ForumArchive.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

int main()
{
    const char *schema = std::getenv("DB_SCHEMA");
    if (!schema || !std::string(schema).starts_with("kinslayer_test_forum_")) return 2;
    auto connection = std::make_shared<sql::_Connection>(std::getenv("DB_HOSTNAME"),
        std::getenv("DB_USERNAME"), std::getenv("DB_PASSWORD"), schema);
    auto run = [&](const std::string &sql) { connection->sendRawQuery(sql); };
    auto scalar = [&](const std::string &sql) { return connection->sendQuery(sql)->getRow().getInt(0); };
    auto mustFail = [&] {
        bool failed = false;
        try { ForumArchive::archive(connection); }
        catch (const sql::Exception &) { failed = true; }
        catch (const std::exception &) { failed = true; }
        assert(failed);
    };
    run("INSERT INTO users VALUES (10)");
    run("INSERT INTO phpbb_users (user_id,user_type,user_password) VALUES "
        "(1,3,'founder'),(8,0,'guest'),(10,0,'active'),(20,2,'bot'),(21,3,'founder2'),(30,0,'deleted'),(31,1,'newest')");
    run("INSERT INTO phpbb_usersArchive (user_id,user_type,user_password) VALUES (31,1,'older')");
    // Actual InnoDB rollback after a successful INSERT followed by failed DELETE.
    run("CREATE TRIGGER fail_delete BEFORE DELETE ON phpbb_users FOR EACH ROW SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='test failure'");
    mustFail();
    assert(scalar("SELECT COUNT(*) FROM phpbb_users") == 7);
    assert(scalar("SELECT COUNT(*) FROM phpbb_usersArchive") == 1);
    assert(scalar("SELECT COUNT(*) FROM phpbb_usersArchive WHERE user_password='older'") == 1);
    run("DROP TRIGGER fail_delete");
    run("CREATE TRIGGER fail_insert BEFORE INSERT ON phpbb_usersArchive FOR EACH ROW SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='test failure'");
    mustFail();
    assert(scalar("SELECT COUNT(*) FROM phpbb_users") == 7);
    run("DROP TRIGGER fail_insert");
    ForumArchive::archive(connection);
    assert(scalar("SELECT COUNT(*) FROM phpbb_users") == 5);
    assert(scalar("SELECT COUNT(*) FROM phpbb_usersArchive") == 2);
    assert(scalar("SELECT COUNT(*) FROM phpbb_usersArchive WHERE user_password='newest'") == 1);
    assert(scalar("SELECT COUNT(*) FROM phpbb_users WHERE user_id IN (1,8,10,20,21)") == 5);
    ForumArchive::archive(connection); // Idempotent empty pass.
    run("INSERT INTO phpbb_users (user_id,user_type,user_password) VALUES (32,0,'preserve')");
    run("ALTER TABLE phpbb_users ADD COLUMN future_column INT NOT NULL DEFAULT 0");
    mustFail();
    assert(scalar("SELECT COUNT(*) FROM phpbb_users WHERE user_id=32") == 1);
    run("ALTER TABLE phpbb_usersArchive ADD COLUMN future_column INT NOT NULL DEFAULT 0 FIRST");
    ForumArchive::archive(connection); // Explicit lists tolerate different column order.
    assert(scalar("SELECT COUNT(*) FROM phpbb_usersArchive WHERE user_id=32") == 1);
    run("ALTER TABLE phpbb_usersArchive ENGINE=MyISAM");
    mustFail();
    std::cout << "PASS: atomic archive/delete, insert/delete failure rollback, reserved identities, schema drift, reordered columns, engine checks\n";
}

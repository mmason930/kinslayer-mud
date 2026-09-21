#pragma once
#include <mysql/sqlDatabase.h>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// This operation is shared by the game and the isolated database regression test.
namespace ForumArchive {
inline void archive(sql::Connection connection)
{
    auto engines = connection->sendQuery("SELECT TABLE_NAME, ENGINE FROM information_schema.TABLES "
        "WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME IN ('users','phpbb_users','phpbb_usersArchive')");
    int count = 0;
    while (engines->hasNextRow()) {
        auto row = engines->getRow();
        if (row["ENGINE"] != "InnoDB")
            throw std::runtime_error("Forum archive requires InnoDB tables; run the archive migration");
        ++count;
    }
    if (count != 3) throw std::runtime_error("Forum archive tables are missing");

    // Refuse deletion after a future schema change until the archive is migrated.
    // Order need not match: every column is explicitly named in both statements.
    std::map<std::string, std::string> source, destination;
    auto columns = connection->sendQuery("SELECT TABLE_NAME,COLUMN_NAME,COLUMN_TYPE FROM information_schema.COLUMNS "
        "WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME IN ('phpbb_users','phpbb_usersArchive')");
    while (columns->hasNextRow()) {
        auto row = columns->getRow();
        const auto name = row["COLUMN_NAME"];
        if (name.empty() || name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos)
            throw std::runtime_error("Unexpected forum column name");
        (row["TABLE_NAME"] == "phpbb_users" ? source : destination)[name] = row["COLUMN_TYPE"];
    }
    if (source.empty() || source != destination)
        throw std::runtime_error("Forum archive schema mismatch; run tools/migrate-forum-archive.py --apply");

    connection->sendRawQuery("START TRANSACTION");
    try {
        // Keep the site's reserved Guest/founder identities and all bots/founders.
        auto users = connection->sendQuery("SELECT p.user_id FROM phpbb_users p "
            "WHERE p.user_id NOT IN (1,8) AND p.user_type IN (0,1) "
            "AND NOT EXISTS (SELECT 1 FROM users u WHERE u.user_id=p.user_id) FOR UPDATE");
        std::string ids;
        while (users->hasNextRow()) {
            if (!ids.empty()) ids += ',';
            ids += std::to_string(users->getRow().getInt("user_id"));
        }
        if (!ids.empty()) {
            std::string names, selected;
            for (const auto &[name, type] : source) {
                if (!names.empty()) { names += ','; selected += ','; }
                names += '`' + name + '`';
                selected += "p.`" + name + '`';
            }
            // Replace only matching IDs. An unrelated unique-key conflict must
            // abort, never overwrite a different archived account.
            connection->sendRawQuery("DELETE FROM phpbb_usersArchive WHERE user_id IN (" + ids + ")");
            connection->sendRawQuery("INSERT INTO phpbb_usersArchive (" + names + ") SELECT " + selected +
                " FROM phpbb_users p WHERE p.user_id IN (" + ids + ")");
            connection->sendRawQuery("DELETE FROM phpbb_users WHERE user_id IN (" + ids + ")");
        }
        connection->sendRawQuery("COMMIT");
    }
    catch (...) {
        try { connection->sendRawQuery("ROLLBACK"); } catch (...) {}
        throw;
    }
}
}

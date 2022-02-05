#ifndef DATABASE_MIGRATION_UTIL_H
#define DATABASE_MIGRATION_UTIL_H

#include <mysql/sqlDatabase.h>
#include "DatabaseMigration.h"

class DatabaseMigrationUtil
{
private:
	static DatabaseMigrationUtil *self;

	DatabaseMigrationUtil();
	~DatabaseMigrationUtil();

public:
	static DatabaseMigrationUtil *get();
	static void destroy();

	void putDatabaseMigration(sql::Connection connection, DatabaseMigration *migration) const;
	DatabaseMigration *getDatabaseMigration(const sql::Row row) const;
	std::map<std::string, DatabaseMigration*> getVersionToMigrationMap(sql::Connection connection) const;
	std::optional<std::string> getVersionNumberFromFilename(const std::string &filename) const;
};

#endif
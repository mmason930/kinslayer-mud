#include "../conf.h"
#include "../dbutils/StoreDataObjectSQLBuilder.h"

#include "DatabaseMigrationUtil.h"

void DatabaseMigrationUtil::putDatabaseMigration(sql::Connection connection, DatabaseMigration *migration) const
{
	auto builder = std::unique_ptr<StoreDataObjectSQLBuilder>(new StoreDataObjectSQLBuilder(connection, "databaseMigration"));

	builder
		->put("id", migration->getId())
		->put("filename", migration->getFilename())
		->put("version_number", migration->getVersionNumber())
		->put("created_datetime", migration->getCreatedDatetime())
		->put("started_datetime", migration->getStartedDatetime())
		->put("completed_datetime", migration->getCompletedDatetime())
		->put("status", migration->getStatus());
	
	builder->execute(*migration);
}

DatabaseMigration *DatabaseMigrationUtil::getDatabaseMigration(const sql::Row row) const
{
	DatabaseMigration *migration = new DatabaseMigration();

	migration->setId(row.getLongLong("id"));
	migration->setFilename(row.getString("filename"));
	migration->setVersionNumber(row.getString("version_number"));
	migration->setCreatedDatetime(DateTime(row.getTimestamp("created_datetime")));
	migration->setStartedDatetime(DateTime(row.getTimestamp("started_datetime")));
	migration->setCompletedDatetime(DateTime(row.getTimestamp("completed_datetime")));
	migration->setStatus(DatabaseMigrationStatus::getEnumByValue(row.getInt("status")));
	
	return migration;
}

std::map<std::string, DatabaseMigration*> DatabaseMigrationUtil::getVersionToMigrationMap(sql::Connection connection) const
{
	std::map<std::string, DatabaseMigration*> versionToMigrationMap;
	sql::Query query = connection->sendQuery("SELECT * FROM databaseMigration");
	
	while(query->hasNextRow())
	{
		DatabaseMigration *migration = getDatabaseMigration(query->getRow());
		versionToMigrationMap[migration->getVersionNumber()] = migration;
	}

	return versionToMigrationMap;
}

std::optional<std::string> DatabaseMigrationUtil::getVersionNumberFromFilename(const std::string &filename) const
{
	std::size_t separatorIndex = filename.find("--");

	if(separatorIndex == std::string::npos)
	{
		return std::optional<std::string>();
	}

	return std::optional<std::string>(filename.substr(0, separatorIndex));
}

void processMigrations(const std::string migrationDiration)
{
	// TODO: Verify that the database table exists. Bootstrap it if not.

	// TODO: Verify that directory exists. Create directory if not.

	/**
	 * TODO: Go through each file and:
	 *  > Get the version # from the filename. If it can't be found, skip it.
	 *  > Create the migration entry with status = PENDING
	 *  > Update status = RUNNING
	 *  > Run the SQL in the file
	 *  > Based on result, set to EXECUTED or FAILED
	 * 
	 * Need to see how to run multiple SQL queries with one query send.
	 */
}

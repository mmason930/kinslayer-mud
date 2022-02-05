#ifndef DATABASE_MIGRATION_STATUS_H
#define DATABASE_MIGRATION_STATUS_H

#include "../Enum.h"

class DatabaseMigrationStatus : public Enum<DatabaseMigrationStatus> {

private:

	DatabaseMigrationStatus(int value, const std::string &standardName) : Enum(value, standardName)
	{
	}

public:
	static DatabaseMigrationStatus *PENDING;
	static DatabaseMigrationStatus *RUNNING;
	static DatabaseMigrationStatus *FAILED;
	static DatabaseMigrationStatus *EXECUTED;
};

#endif

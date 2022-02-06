#ifndef DATABASE_MIGRATION_H
#define DATABASE_MIGRATION_H

#include "../DataObjectWithLongId.h"
#include "../DateTime.h"
#include "DatabaseMigrationStatus.h"
#include <string>

class DatabaseMigration : public DataObjectWithLongId
{
private:
	std::string filename;
	std::string versionNumber;
	DateTime createdDatetime;
	DateTime startedDatetime;
	DateTime completedDatetime;
	DatabaseMigrationStatus *status;
public:
	std::string getFilename()
	{
		return filename;
	}

	void setFilename(const std::string &filename)
	{
		this->filename = filename;
	}

	std::string getVersionNumber()
	{
		return versionNumber;
	}
	
	void setVersionNumber(const std::string &versionNumber)
	{
		this->versionNumber = versionNumber;
	}

	DateTime getCreatedDatetime()
	{
		return createdDatetime;
	}

	void setCreatedDatetime(const DateTime &createdDatetime)
	{
		this->createdDatetime = createdDatetime;
	}

	DateTime getStartedDatetime()
	{
		return startedDatetime;
	}

	void setStartedDatetime(const DateTime &startedDatetime)
	{
		this->startedDatetime = startedDatetime;
	}

	DateTime getCompletedDatetime()
	{
		return completedDatetime;
	}

	void setCompletedDatetime(const DateTime &completedDatetime)
	{
		this->completedDatetime = completedDatetime;
	}

	DatabaseMigrationStatus *getStatus()
	{
		return status;
	}

	void setStatus(DatabaseMigrationStatus *status)
	{
		this->status = status;
	}
};

#endif
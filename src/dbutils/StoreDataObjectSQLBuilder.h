#ifndef DATA_OBJECT_STORE_SQL_BUILDER_H
#define DATA_OBJECT_STORE_SQL_BUILDER_H

#include "StoreSQLBuilder.h"
#include "../DataObjectWithIntId.h"
#include "../DataObjectWithLongId.h"
#include "../SQLUtil.h"

#include <iostream>

class StoreDataObjectSQLBuilder : public StoreSQLBuilder {
	
private:
protected:
	sql::Connection connection;

public:
	StoreDataObjectSQLBuilder(sql::Connection connection, const std::string &tableName) : StoreSQLBuilder(tableName) {

		this->connection = connection;
	}

	int execute(DataObjectWithIntId *dataObjectWithIntId) {
		return execute(*dataObjectWithIntId);
	}
	int execute(DataObjectWithIntId &dataObjectWithIntId) {

		if(dataObjectWithIntId.isNew())
		{
			connection->sendQuery(generateInsert());
			dataObjectWithIntId.setId(connection->lastInsertID());
		}
		else
		{
			connection->sendQuery(generateUpdate(dataObjectWithIntId.getId()));
		}

		return dataObjectWithIntId.getId();
	}
	int execute(DataObjectWithLongId *dataObjectWithLongId) {
		return execute(*dataObjectWithLongId);
	}
	int execute(DataObjectWithLongId &dataObjectWithLongId) {

		if(dataObjectWithLongId.isNew())
		{
			connection->sendQuery(generateInsert());
			dataObjectWithLongId.setId(connection->lastInsertID());
		}
		else
		{
			connection->sendQuery(generateUpdate(dataObjectWithLongId.getId()));
		}

		return dataObjectWithLongId.getId();
	}
};

#endif
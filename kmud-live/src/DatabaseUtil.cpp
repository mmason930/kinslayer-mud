#include "DatabaseUtil.h"

std::string DatabaseUtil::getSingleResultFromQuery(sql::Connection &connection, const std::string &queryBuffer)
{
	sql::Query query;
	sql::Row row;
	query = connection->sendQuery(queryBuffer);

	if( query->hasNextRow() ) {
		return row[0];
	}
	return "";
}

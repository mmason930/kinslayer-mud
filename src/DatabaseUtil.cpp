#include "DatabaseUtil.h"

std::string DatabaseUtil::getSingleResultFromQuery(const sql::Connection &connection, const std::string &queryBuffer)
{
	sql::Query query = connection->sendQuery(queryBuffer);

	if( query->hasNextRow() ) {
		return query->getRow().getString(0);
	}
	return "";
}

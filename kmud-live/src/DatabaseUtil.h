#ifndef DATABASE_UTIL_UTIL_H
#define DATABASE_UTIL_UTIL_H

#include <mysql/sqlDatabase.h>

class DatabaseUtil
{
private:
protected:
public:
	//Throws: sql::QueryException
	static std::string getSingleResultFromQuery(sql::Connection &connection, const std::string &queryBuffer);
};

#endif

#ifndef DATABASE_UTIL_UTIL_H
#define DATABASE_UTIL_UTIL_H

#include <list>
#include <mysql/sqlDatabase.h>

#include "MiscUtil.h"

class DatabaseUtil
{
private:
protected:
public:
	//Throws: sql::QueryException
	static std::string getSingleResultFromQuery(const sql::Connection& connection, const std::string &queryBuffer);
	
	template<typename _T>
	static std::string buildListSQL(const std::list<_T> &collection, const bool quoteElements, const bool onEmptyAddNull)
	{
		if (onEmptyAddNull && collection.empty())
			return "(null)";

		std::stringstream sql;
		bool firstPass = true;
		sql << "(";

		for(auto iter = collection.begin();iter != collection.end();++iter)
		{
			if(firstPass)
				firstPass = false;
			else
				sql << ",";

			if (quoteElements)
			{
				sql << sql::escapeQuoteString(MiscUtil::toString((*iter)));
			}
			else
				sql << (*iter);
		}
		
		sql << ')';
		return sql.str();
    }
};

#endif

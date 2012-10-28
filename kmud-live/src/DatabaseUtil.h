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
	
	template<typename _T>
	static std::string buildListSQL(std::list<_T> collection, bool quoteElements, bool onEmptyAddNull)
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
				std::stringstream toStringConverterStream;
				toStringConverterStream << (*iter);
				sql << sql::escapeQuoteString(toStringConverterStream.str());
			}
			else
				sql << (*iter);
		}
		
		sql << ')';
		return sql.str();
    }
};

#endif

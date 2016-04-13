#ifndef QUERY_UTIL_H
#define QUERY_UTIL_H

#include <mysql/sqlDatabase.h>
#include <functional>
#include <boost/optional.hpp>
#include "../DateTime.h"

class QueryUtil
{
private:
	static QueryUtil *self;

	QueryUtil() {}
	~QueryUtil() {}
protected:
public:

	static QueryUtil *get()
	{
		return (self == nullptr ? (self = new QueryUtil()) : self);
	}

	static void destroy() {}
	boost::optional<DateTime> getNullableDateTime(const sql::Row &row, const std::string &columnName) const;

	template <typename ReturnType, typename MapKeyType>
	std::map<MapKeyType, ReturnType> loadDataObjectMapFromDatabase(sql::Connection connection, const std::string &tableName, const std::string &whereCriteria, const std::function<ReturnType(const sql::Row row)> &rowConverter, const std::function<MapKeyType(const ReturnType returnType)> &keyRetrievalFunction)
	{
		std::map<MapKeyType, ReturnType> map;
		std::stringstream queryBuffer;

		queryBuffer << " SELECT *"
					<< " FROM " << tableName
					<< " WHERE " << whereCriteria.empty() ? std::string("1") : whereCriteria;

		sql::Query query = connection->sendQuery(queryBuffer.str());
	
		while(query->hasNextRow())
		{
			sql::Row row = query->getRow();

			ReturnType returnType = rowConverter(row);
			MapKeyType mapKeyType = keyRetrievalFunction(returnType);

			map[ mapKeyType ] = returnType;
		}

		return map;
	}

	template <typename ReturnType, typename MapKeyType>
	std::map<MapKeyType, ReturnType> loadDataObjectMapFromDatabase(sql::Connection connection, const std::string &tableName, const std::function<ReturnType(const sql::Row row)> &rowConverter, const std::function<MapKeyType(const ReturnType returnType)> &keyRetrievalFunction)
	{
		return loadDataObjectMapFromDatabase(connection, tableName, std::string(""), rowConverter, keyRetrievalFunction);
	}

	template <typename ReturnType>
	ReturnType loadDataObjectFromDatabase(sql::Connection connection, const std::string &tableName, const std::string &whereCriteria, const std::function<ReturnType(const sql::Row row)> &rowConverter)
	{
	
		std::stringstream queryBuffer;

		queryBuffer << " SELECT *"
					<< " FROM " << tableName
					<< " WHERE " << (whereCriteria.empty() ? std::string("1") : whereCriteria);

		sql::Query query = connection->sendQuery(queryBuffer.str());
	
		if(query->hasNextRow())
		{
			return rowConverter(query->getRow());
		}

		return NULL;
	}
};

#endif
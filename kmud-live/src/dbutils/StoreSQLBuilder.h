#ifndef STORE_SQL_BUILDER_H
#define STORE_SQL_BUILDER_H

#include <list>
#include <tuple>
#include <mysql/sqlDatabase.h>
#include <boost/optional.hpp>

#include "../DateTime.h"
#include "../Enum.h"

class StoreSQLBuilder {

private:
protected:
	std::list<std::tuple<std::string, std::string>> columns;
	std::string tableName;
public:
	StoreSQLBuilder(const std::string &tableName) {
	
		this->tableName = tableName;
	}
	
	StoreSQLBuilder *putEscapedString(const std::string &columnName, const std::string &escapedString) {
    
		columns.push_back(std::tuple<std::string, std::string>(columnName, escapedString));
		return this;
	}
	
	StoreSQLBuilder *put(const std::string &columnName, const std::string &value) {
		
		return putEscapedString(columnName, sql::escapeQuoteString(value));
	}
	
	StoreSQLBuilder *put(const std::string &columnName, int value) {
		
		char intString[32];
		sprintf(intString, "%d", value);
		return putEscapedString(columnName, intString);
	}

	StoreSQLBuilder *put(const std::string &columnName, long value) {
		
		char intString[32];
		sprintf(intString, "%ld", value);
		return putEscapedString(columnName, intString);
	}

	StoreSQLBuilder *put(const std::string &columnName, unsigned long value) {
		
		char intString[32];
		sprintf(intString, "%lu", value);
		return putEscapedString(columnName, intString);
	}
	
	StoreSQLBuilder *put(const std::string &columnName, unsigned int value) {
		
		char intString[32];
		sprintf(intString, "%u", value);
		return putEscapedString(columnName, intString);
	}

	StoreSQLBuilder *put(const std::string &columnName, long long value) {
		
		char intString[32];
		sprintf(intString, "%lld", value);
		return putEscapedString(columnName, intString);
	}

	StoreSQLBuilder *put(const std::string &columnName, unsigned long long value) {
		
		char intString[32];
		sprintf(intString, "%llu", value);
		return putEscapedString(columnName, intString);
	}

	StoreSQLBuilder *put(const std::string &columnName, bool value) {
	
		return put(columnName, sql::encodeBooleanInt(value));
	}

	StoreSQLBuilder *put(const std::string &columnName, const DateTime &value) {
	
		return putEscapedString(columnName, sql::encodeQuoteDate(value.getTime()));
	}
	
	template<typename T>
	StoreSQLBuilder *put(const std::string &columnName, const boost::optional<T> &value) {

		return value ? put(columnName, value.get()) : putEscapedString(columnName, "NULL");
	}

	StoreSQLBuilder *put(const std::string &columnName, const char *str) {
	
		return str ? putEscapedString(columnName, sql::escapeQuoteString(std::string(str))) : putEscapedString(columnName, std::string("NULL"));
	}
	
	template<typename T>
	StoreSQLBuilder *put(const std::string &columnName, const Enum<T> *enumerator) {
	
		return enumerator ? put(columnName, enumerator->getValue()) : putEscapedString(columnName, "NULL");
	}

	std::string generateInsert() {
		
		std::stringstream stream, columnNameStream, columnValueStream;
    
		stream << "INSERT INTO `" << tableName << "`(";
		bool isFirst = true;

		for(std::tuple<std::string, std::string> column : columns) {
			  
			columnNameStream << (isFirst ? "" : ",") << "`" << std::get<0>(column) << "`";
			columnValueStream << (isFirst ? "" : ",") << std::get<1>(column);
			isFirst = false;
		}
		
		stream	<< columnNameStream.str()
				<< ")VALUES("
				<< columnValueStream.str()
				<< ")";
    
		return stream.str();
	}
	
	
	std::string generateUpdate(short id) {
		
		return generateUpdate("id", id);
	}
	
	std::string generateUpdate(int id) {
		
		return generateUpdate("id", id);
	}
	
	std::string generateUpdate(long id) {
		
		return generateUpdate("id", id);
	}
	
	std::string generateUpdate(const std::string &idColumnName, int id) {
		
		std::stringstream stream;
		stream << idColumnName << "=" << id;
		return generateUpdate(stream.str());
	}
  
	std::string generateUpdate(const std::string &idColumnName, long id) {
		
		std::stringstream stream;
		stream << idColumnName << "=" << id;
		return generateUpdate(stream.str());
	}
  
	std::string generateUpdate(const std::string &whereClause) {
    
		std::stringstream stream;

		stream << "UPDATE " << tableName << " SET ";
		 
		bool isFirst = true;
		for(std::tuple<std::string, std::string> column : columns) {
			
			stream << (isFirst ? "" : ",") << std::get<0>(column) << "=" << std::get<1>(column);
			isFirst = false;
		}
		
		stream << " WHERE " << whereClause;
		
		return stream.str();
	}
};

#endif
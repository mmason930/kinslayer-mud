#ifndef SQL_UTIL_H
#define SQL_UTIL_H

#include <string>
#include <sstream>
#include <iterator>
#include <mysql/sqlDatabase.h>

class SQLUtil
{
private:
protected:
public:

	template<typename IterType>
	static std::string buildListSQL(IterType beginIterator, IterType endIterator, bool quoteElements, bool nullOnEmpty)
	{
		std::stringstream queryBuffer;
		queryBuffer << "(";
		bool first = true;

		for(auto iter = beginIterator;iter != endIterator;++iter)
		{
			if(first)
				first = false;
			else
				queryBuffer << ',';

			if(quoteElements)
				queryBuffer << sql::escapeQuoteString((*iter));
			else
				queryBuffer << (*iter);

		}

		queryBuffer << ')';

		return queryBuffer.str();
	}
};

#endif

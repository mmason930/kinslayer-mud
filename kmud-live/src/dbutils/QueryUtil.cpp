#include "QueryUtil.h"

QueryUtil *QueryUtil::self = nullptr;

boost::optional<DateTime> QueryUtil::getNullableDateTime(const sql::Row &row, const std::string &columnName) const
{
	auto timeStamp = row.getNullableTimestamp(columnName);

	return !timeStamp.is_initialized() ? boost::optional<DateTime>() : DateTime(timeStamp.get());
}
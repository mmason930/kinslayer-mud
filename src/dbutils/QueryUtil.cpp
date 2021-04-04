#include "QueryUtil.h"

QueryUtil *QueryUtil::self = nullptr;

std::optional<DateTime> QueryUtil::getNullableDateTime(const sql::Row &row, const std::string &columnName) const
{
	return row.getNullableTimestamp(columnName);
}

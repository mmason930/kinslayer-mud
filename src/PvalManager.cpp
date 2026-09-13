#include "conf.h"
#include "structs.h"
#include "utils.h"
#include "PvalManager.h"
#include "db.h"

#include <thread>

PvalManager *PvalManager::Self = nullptr;

static std::string makeFullKey(const std::string &ownerType, const std::string &ownerID, const std::string &key)
{
	return ownerType + "~=~" + ownerID + "~=~" + key;
}

void PvalManager::preBoot()
{
	this->bootFuture = std::async(std::launch::async, []() -> std::unordered_map<std::string, PvalEntry>
	{
		std::unordered_map<std::string, PvalEntry> pvalEntryMap;
		sql::Connection connection = dbContext->createConnection();
		sql::Query MyQuery = connection->sendQuery("SELECT * FROM pvals");

		pvalEntryMap.reserve(MyQuery->numRows());
		while (MyQuery->hasNextRow())
		{
			sql::Row row = MyQuery->getRow();

			std::string fullKey = makeFullKey(row["owner_type"], row["owner_id"], row["sKey"]);

			pvalEntryMap.try_emplace(fullKey, row["value"], false, false);
		}

		return pvalEntryMap;
	});
}

void PvalManager::boot()
{
	pvals = bootFuture.get();

	// Spin up a background thread to clean up orphaned pvals
	std::thread cleanupThread([] {
		sql::Connection cleanupConn = dbContext->createConnection();
		try {
			cleanupConn->sendRawQuery(
				"DELETE FROM pvals WHERE owner_type='O' AND owner_id NOT IN(SELECT id FROM objects)");
			cleanupConn->sendRawQuery(
				"DELETE FROM pvals WHERE owner_type='C' AND owner_id NOT IN(SELECT user_id FROM users)");
		} catch (sql::QueryException &e) {
			Log("PvalManager: orphaned pval cleanup failed: %s", e.getMessage().c_str());
		}
	});
	cleanupThread.detach();
}

std::string PvalManager::getPval(const std::string &ownerType, const std::string &ownerID, const std::string &key)
{
	std::string fullKey = makeFullKey(ownerType, ownerID, key);
	auto it = pvals.find(fullKey);
	if (it != pvals.end() && !it->second.needsDelete)
		return it->second.value;
	return "";
}

void PvalManager::setPval(const std::string &ownerType, const std::string &ownerID, const std::string &key, const std::string &value, bool instant)
{
	std::string fullKey = makeFullKey(ownerType, ownerID, key);
	PvalEntry &entry = pvals[fullKey];
	entry.value = value;
	entry.needsDelete = false;

	if (instant) {
		std::string truncatedKey = key.substr(0, MAX_KEY_LENGTH);
		std::string query = "REPLACE INTO pvals SET "
			"owner_type='" + sql::escapeString(ownerType) + "', "
			"owner_id='" + sql::escapeString(ownerID) + "', "
			"sKey='" + sql::escapeString(truncatedKey) + "', "
			"value='" + sql::escapeString(value) + "'";
		try {
			gameDatabase->sendQuery(query);
		} catch (sql::QueryException &e) {
			MudLog(BRF, LVL_APPR, TRUE, "PvalManager::setPval : %s", e.getMessage().c_str());
		}
		entry.needsSave = false;
	} else {
		entry.needsSave = true;
	}
}

void PvalManager::deletePval(const std::string &ownerType, const std::string &ownerID, const std::string &key, bool instant)
{
	std::string fullKey = makeFullKey(ownerType, ownerID, key);
	auto it = pvals.find(fullKey);
	if (it == pvals.end())
		return;

	if (instant) {
		std::string truncatedKey = key.substr(0, MAX_KEY_LENGTH);
		std::string query = "DELETE FROM pvals WHERE "
			"owner_type='" + sql::escapeString(ownerType) + "' AND "
			"owner_id='" + sql::escapeString(ownerID) + "' AND "
			"sKey='" + sql::escapeString(truncatedKey) + "'";
		try {
			gameDatabase->sendQuery(query);
		} catch (sql::QueryException &e) {
			MudLog(BRF, LVL_APPR, TRUE, "PvalManager::deletePval : %s", e.getMessage().c_str());
		}
		pvals.erase(it);
	} else {
		it->second.needsDelete = true;
	}
}

void PvalManager::saveDirtyPvals()
{
	auto it = pvals.begin();
	while (it != pvals.end()) {
		if (it->second.needsDelete) {
			// Parse the full key to get components
			std::string fullKey = it->first;
			size_t pos1 = fullKey.find("~=~");
			size_t pos2 = fullKey.find("~=~", pos1 + 3);
			std::string ownerType = fullKey.substr(0, pos1);
			std::string ownerID = fullKey.substr(pos1 + 3, pos2 - pos1 - 3);
			std::string key = fullKey.substr(pos2 + 3);

			std::string query = "DELETE FROM pvals WHERE "
				"owner_type='" + sql::escapeString(ownerType) + "' AND "
				"owner_id='" + sql::escapeString(ownerID) + "' AND "
				"sKey='" + sql::escapeString(key) + "'";
			try {
				gameDatabase->sendQuery(query);
			} catch (sql::QueryException &e) {
				MudLog(BRF, LVL_APPR, TRUE, "PvalManager::saveDirtyPvals (delete) : %s", e.getMessage().c_str());
			}
			it = pvals.erase(it);
		} else if (it->second.needsSave) {
			std::string fullKey = it->first;
			size_t pos1 = fullKey.find("~=~");
			size_t pos2 = fullKey.find("~=~", pos1 + 3);
			std::string ownerType = fullKey.substr(0, pos1);
			std::string ownerID = fullKey.substr(pos1 + 3, pos2 - pos1 - 3);
			std::string key = fullKey.substr(pos2 + 3);

			std::string truncatedKey = key.substr(0, MAX_KEY_LENGTH);
			std::string query = "REPLACE INTO pvals SET "
				"owner_type='" + sql::escapeString(ownerType) + "', "
				"owner_id='" + sql::escapeString(ownerID) + "', "
				"sKey='" + sql::escapeString(truncatedKey) + "', "
				"value='" + sql::escapeString(it->second.value) + "'";
			try {
				gameDatabase->sendQuery(query);
			} catch (sql::QueryException &e) {
				MudLog(BRF, LVL_APPR, TRUE, "PvalManager::saveDirtyPvals (save) : %s", e.getMessage().c_str());
			}
			it->second.needsSave = false;
			++it;
		} else {
			++it;
		}
	}
}

int PvalManager::size()
{
	return (int)pvals.size();
}

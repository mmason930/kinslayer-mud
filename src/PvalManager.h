#ifndef PVAL_MANAGER_H
#define PVAL_MANAGER_H

#include <future>
#include <string>
#include <unordered_map>

struct PvalEntry {
	std::string value;
	bool needsSave;
	bool needsDelete;
};

class PvalManager {
private:
	static PvalManager *Self;
	std::future<std::unordered_map<std::string, PvalEntry>> bootFuture;
	std::unordered_map<std::string, PvalEntry> pvals;

	PvalManager() {}

	static const int MAX_KEY_LENGTH = 64;

public:
	static PvalManager *get() {
		if (!Self)
			Self = new PvalManager();
		return Self;
	}

	void preBoot();
	void boot();
	std::string getPval(const std::string &ownerType, const std::string &ownerID, const std::string &key);
	void setPval(const std::string &ownerType, const std::string &ownerID, const std::string &key, const std::string &value, bool instant);
	void deletePval(const std::string &ownerType, const std::string &ownerID, const std::string &key, bool instant);
	void saveDirtyPvals();
	int size();
};

#endif

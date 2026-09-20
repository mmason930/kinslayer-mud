#include "PvalManager.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
namespace sql {
struct QueryException : std::runtime_error {
    using std::runtime_error::runtime_error;
    std::string getMessage() const { return what(); }
};
std::string escapeString(const std::string &s) { return s; }
}
struct Database {
    bool fail = false;
    unsigned writes = 0;
    void sendQuery(const std::string &) {
        ++writes;
        if (fail) throw sql::QueryException("simulated database write failure");
    }
} database;
Database *gameDatabase = &database;
#define MudLog(...) ((void)0)
PvalManager *PvalManager::Self = nullptr;
#include "pval-under-test.inc"
int main() {
    auto pvals = PvalManager::get();
    assert(pvals->setPval("C", "1", "settings", "previous", true));
    database.fail = true;
    assert(!pvals->setPval("C", "1", "settings", "unsaved", true));
    assert(pvals->getPval("C", "1", "settings") == "previous");
    assert(!pvals->setPval("C", "1", "new-key", "unsaved", true));
    assert(pvals->getPval("C", "1", "new-key").empty());
    assert(pvals->setPval("C", "1", "pending", "queued", false));
    assert(pvals->getPval("C", "1", "pending") == "queued");
    assert(!pvals->setPval("C", "1", "pending", "replacement", true));
    assert(pvals->getPval("C", "1", "pending") == "queued");
    database.fail = false;
    std::string settings(12000, 'x');
    assert(pvals->setPval("C", "1", "settings", settings, true));
    assert(pvals->getPval("C", "1", "settings") == settings);
    std::cout << "Pvals: failed writes report failure and preserve cached state; retry and queued saves pass.\n";
}

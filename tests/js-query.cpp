// Exercise the real SQL result bindings in SpiderMonkey using only literal SELECTs.
// No game tables are read or written, and the game itself is not started.
#include "../src/js/JSQuery.h"
#include "../src/js/JSRow.h"
#include <js/GCAPI.h>
#include <cstdlib>
#include <stdexcept>

void RegisterJSQueryBindings();
void RegisterJSRowBindings();

// Supply only the wrapper routing needed by JSQuery::getRow/getpeekRow.
// The production query/row methods, registrations and engine are linked unchanged.
flusspferd::value lookupValue(JSBindable *value) {
    if (!value) return flusspferd::value();
    if (auto row = dynamic_cast<sqlJSRow *>(value))
        return flusspferd::create_native_object<JSRow>(flusspferd::object(), row);
    throw std::runtime_error("Unexpected wrapper in query test");
}

static const char *requiredEnv(const char *name) {
    const char *value = std::getenv(name);
    if (!value) throw std::runtime_error(std::string("Missing environment variable: ") + name);
    return value;
}

static void collect() {
    JS::PrepareForFullGC(flusspferd::g_cx);
    JS::NonIncrementalGC(flusspferd::g_cx, JS::GCOptions::Shrink, JS::GCReason::API);
}

static void runTests() {
    using namespace flusspferd;
    RegisterJSQueryBindings();
    RegisterJSRowBindings();
    load_class<JSQuery>();
    load_class<JSRow>();
    auto connection = sql::createContext(requiredEnv("DB_HOSTNAME"), requiredEnv("DB_USERNAME"),
        requiredEnv("DB_PASSWORD"), requiredEnv("DB_SCHEMA"))->createConnection();
    auto expose = [&](const char *name, const char *select) {
        global().set_property(name, create_native_object<JSQuery>(object(),
            new sqlJSQuery(connection->sendQuery(select))));
    };
    // Descending results match the communication search before reverseRows().
    expose("result", "SELECT 3 AS id UNION ALL SELECT 2 UNION ALL SELECT 1 ORDER BY id DESC");
    expose("empty", "SELECT 1 AS id WHERE FALSE");
    expose("single", "SELECT 42 AS id");
    auto globals = global();
    create_native_function(globals, "forceCollection", collect);
    evaluate(R"JS(
        function check(condition, message) {
            if (!condition) throw new Error(message);
        }
        function ids(query) {
            var values = [];
            while (query.hasNextRow) values.push(query.getRow.get("id"));
            return values.join(",");
        }
        check(typeof result.reverseRows === "function", "reverseRows missing from JSQuery");
        check(result.numRows === 3 && result.numFields === 1, "result metadata");
        check(result.getIndexByField("id") === 0 && result.getFieldByIndex(0) === "id", "field lookup");
        check(result.reverseRows() === undefined, "reverseRows return compatibility");
        check(ids(result) === "1,2,3", "communication results must display oldest first");
        check(!result.hasNextRow && result.numRows === 3, "exhausted result metadata");

        result.reverseRows();
        check(ids(result) === "3,2,1", "reversing an exhausted result must rewind");
        result.resetRowQueue();
        check(result.getRow.get("id") === "3", "reset after reversal");
        result.reverseRows();
        check(ids(result) === "1,2,3", "reversal must include already consumed rows");

        result.reverseRows();
        result.reverseRows();
        check(result.peekRow.get("id") === "1", "two reversals restore order");
        result.skipRow();
        check(result.getRow.getByIndex(0) === "2", "skipRow after reversal");
        check(result.peekRow.get("id") === "3", "peekRow must not consume");
        check(ids(result) === "3", "remaining row after peek");

        empty.reverseRows();
        check(empty.numRows === 0 && !empty.hasNextRow, "empty reversal");
        empty.resetRowQueue();
        empty.reverseRows();
        check(!empty.hasNextRow, "repeated empty reversal");
        single.reverseRows();
        check(ids(single) === "42", "single-row reversal");
        single.reverseRows();
        check(ids(single) === "42", "exhausted single-row reversal");

        result.resetRowQueue();
        var retained = result.getRow;
        result.reverseRows();
        check(retained.get("id") === "1", "reversal changed an existing row");
        result = null;
        forceCollection();
        check(retained.get("id") === "1", "row lost query ownership during GC");
        retained = empty = single = null;
        forceCollection();
    )JS", "js-query-regression.js");
    std::cout << "PASS: real JSQuery bindings, reversal/order, cursor reset, empty/single results, row lifetime\n";
}

int main() {
    try {
        flusspferd::current_context_scope context(flusspferd::context::create());
        runTests();
    } catch (const std::exception &error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}

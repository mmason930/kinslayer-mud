// Standalone embedding regression test. Never expose testing functions in-game.
#include "../src/js/flusspferd.hpp"
#include <js/GCAPI.h>
#include <js/Interrupt.h>
#include <mysql/sqlDatabase.h>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unordered_map>

// Public headers must retain the embedding application's C++ allocator.
// Catch an unpatched 153 installation even when sanitizers are not enabled.
#ifdef mozilla_cxxalloc_h
#error "Use the patched SpiderMonkey build: Mozilla global new/delete leaked into the embedding"
#endif

using namespace flusspferd;

FLUSSPFERD_CLASS_DESCRIPTION(Probe, ()) {
public:
    inline static unsigned finalized = 0;
    explicit Probe(const object &self) : base_type(self) {}
    ~Probe() override { ++finalized; }
};
FLUSSPFERD_CLASS_DESCRIPTION(OtherProbe, ()) {
public:
    explicit OtherProbe(const object &self) : base_type(self) {}
};

static void require(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}
static void collect() {
    JS::PrepareForFullGC(g_cx);
    JS::NonIncrementalGC(g_cx, JS::GCOptions::Shrink, JS::GCReason::API);
}
static string coerce_pair(string first, int second) {
    collect();
    return first;
}
static void throw_cpp() { throw std::runtime_error("native failure"); }
static void throw_sql() { throw sql::Exception("sql failure"); }
static void nested_js() {
    evaluate("for (;;) {}", "nested-timeout.js");
}

static void run_tests() {
    require(std::string(JS_GetImplementationVersion()) == "JavaScript-C153.0.4",
            "expected the pinned SpiderMonkey 153.0.4 runtime");
    // Test-only APIs let us prove actual optimized execution, not just options.
    global().set_property("testing", object(js::GetTestingFunctions(g_cx)));
    JS_SetGlobalJitCompilerOption(g_cx, JSJITCOMPILER_BASELINE_INTERPRETER_WARMUP_TRIGGER, 0);
    JS_SetGlobalJitCompilerOption(g_cx, JSJITCOMPILER_BASELINE_WARMUP_TRIGGER, 5);
    JS_SetGlobalJitCompilerOption(g_cx, JSJITCOMPILER_ION_NORMAL_WARMUP_TRIGGER, 20);
    JS_SetOffthreadIonCompilationEnabled(g_cx, false);
    auto globals = global();
    create_native_function(globals, "forceCollection", collect);
    create_native_function(globals, "coercePair", coerce_pair);
    create_native_function(globals, "throwCpp", throw_cpp);
    create_native_function(globals, "throwSql", throw_sql);
    create_native_function(globals, "nestedJs", nested_js);

    bool ion = evaluate(R"JS(
        var observedIon = false;
        function hot(n) {
            var sum = 0;
            for (var i = 0; i < n; ++i) {
                if (testing.inIon() === true) observedIon = true;
                sum += i;
            }
            return sum;
        }
        for (var j = 0; j < 100; ++j) hot(1000);
        observedIon;
    )JS").to_boolean();
    const char *disabled = std::getenv("KINSLAYER_DISABLE_JIT");
    require(ion == !(disabled && std::string(disabled) == "1"), "unexpected Ion execution status");
    std::cout << "Actual Ion execution: " << (ion ? "yes" : "no (fallback)") << '\n';

    std::unordered_map<unsigned, value> cache;
    array copied;
    {
        array original = create_array();
        for (unsigned i = 0; i < 1000; ++i) {
            auto record = create_object();
            record.set_property("id", value(static_cast<int>(i)));
            record.set_property("text", string("record-" + std::to_string(i)));
            original.push(record);
            cache.emplace(i, record);
            if (i % 50 == 0) { evaluate("testing.minorgc()"); collect(); }
        }
        copied = original;
    }
    array moved = std::move(copied);
    collect();
    value assigned = moved;
    value independentCopy = assigned;
    assigned = 7;
    require(independentCopy.is_object(), "assigning a copy changed shared storage");
    require(moved.length() == 1000, "array lifetime/copy failure");
    for (unsigned i = 0; i < 1000; ++i) {
        auto record = cache.at(i).to_object();
        require(record.get_object_ptr() == moved.get_element(i).to_object().get_object_ptr(), "cache identity lost");
        require(record.get_property("text").to_std_string() == "record-" + std::to_string(i), "string moved incorrectly");
    }
    require(evaluate(R"JS(
        coercePair('rooted-' + Math.random(), {valueOf() { testing.minorgc(); forceCollection(); return 1; }}).startsWith('rooted-')
    )JS").to_boolean(), "argument copy lost across coercion");

    auto &info = g_class_registry["Probe"];
    info.methods["read"] = [](void *ptr, JSContext *cx, unsigned, JS::Value *vp) {
        JS::CallArgs args = JS::CallArgsFromVp(0, vp);
        args.rval().set(static_cast<Probe*>(ptr)->get_property("payload").get_js_value());
        return true;
    };
    info.methods["callThrowing"] = [](void *, JSContext *, unsigned, JS::Value *) -> bool {
        global().call("throwOriginal");
        return true;
    };
    info.getters["bad"] = [](void *, JSContext *, JS::MutableHandleValue) -> bool {
        throw std::runtime_error("getter failure");
    };
    info.setters["bad"] = [](void *, JSContext *cx, JS::HandleValue) {
        JS_ReportErrorASCII(cx, "setter failure");
        return false;
    };
    {
        auto probe = create_native_object<Probe>(object());
        get_native<Probe>(probe).set_property("payload", 42);
        global().set_property("probe", probe);
        global().set_property("other", create_native_object<OtherProbe>(object()));
        collect();
        require(probe.call("read").get_int() == 42, "traced native back-reference failed");
        require(evaluate(R"JS(
            var originalError = {marker: 42};
            function throwOriginal() { throw originalError; }
            var failures = 0;
            try { probe.callThrowing(); } catch (e) { if (e === originalError) ++failures; }
            try { throwCpp(); } catch (e) { if (String(e).includes('native failure')) ++failures; }
            try { throwSql(); } catch (e) { if (String(e).includes('sql failure')) ++failures; }
            try { probe.bad; } catch (e) { if (String(e).includes('getter failure')) ++failures; }
            try { probe.bad = 1; } catch (e) { if (String(e).includes('setter failure')) ++failures; }
            try { probe.read.call({}); } catch (e) { ++failures; }
            try { probe.read.call(other); } catch (e) { ++failures; }
            try { coercePair('first', {valueOf() {throw originalError;}}); } catch (e) { if (e === originalError) ++failures; }
            failures === 8;
        )JS").to_boolean(), "native exception/receiver boundary failed");
        global().delete_property("probe");
        global().delete_property("other");
    }
    collect();
    require(Probe::finalized == 1, "native wrapper retained by self-root");

    // Match the game's same-realm evaluation and generator.next() reload model.
    evaluate(R"JS(
        var liveScript = function* () { yield 'old-wait'; return 'old-end'; };
        var callback = function () { return 'old-callback'; };
        var savedCallback = callback;
        var replacement = function () { return 1; };
        function caller() { return replacement(); }
        for (var i = 0; i < 10000; ++i) caller();
    )JS", "reload.js");
    object suspended = global().call("liveScript").to_object();
    require(suspended.call("next").to_object().get_property("value").to_std_string() == "old-wait", "initial yield failed");
    evaluate(R"JS(
        var liveScript = function* () { yield 'new-wait'; return 'new-end'; };
        var callback = function () { return 'new-callback'; };
        var replacement = function () { return 2; };
    )JS", "reload.js");
    collect();
    require(global().call("caller").get_int() == 2, "optimized caller missed replacement");
    require(global().call("savedCallback").to_std_string() == "old-callback", "queued callback changed");
    require(suspended.call("next").to_object().get_property("value").to_std_string() == "old-end", "suspended generator lost old code");
    object callable = evaluate("(function (arg) { return this.marker + arg; })").to_object();
    object receiver = create_object();
    receiver.set_property("marker", 40);
    collect();
    require(callable.call_function(receiver, 2).get_int() == 42, "callback receiver/argument lost");
    object fresh = global().call("liveScript").to_object();
    require(fresh.call("next").to_object().get_property("value").to_std_string() == "new-wait", "fresh generator missed reload");

    require(JS_AddInterruptCallback(g_cx, [](JSContext *) { return !execution_timed_out(); }), "interrupt registration failed");
    std::atomic<bool> stop{false};
    std::thread watchdog([&] {
        while (!stop.load()) {
            JS_RequestInterruptCallback(g_cx);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
    bool timedOut = false;
    try {
        execution_scope budget(std::chrono::milliseconds(50));
        evaluate("try { nestedJs(); } catch (e) { for (;;) {} }");
    } catch (const js_exception &e) {
        timedOut = !e.has_thrown;
    }
    stop = true;
    watchdog.join();
    require(timedOut, "nested deadline/uncatchable interruption failed");
    require(evaluate("6 * 7").get_int() == 42, "context unusable after interruption");
    std::cout << "PASS: moving GC, arrays, cache identity, coercion, native tracing/finalization, exceptions, reload, generators, interruption\n";
}

int main() {
    try {
        current_context_scope context(context::create());
        run_tests();
    } catch (const std::exception &e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
    std::cout << "PASS: context shutdown\n";
}

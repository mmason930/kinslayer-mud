/**
 * flusspferd.cpp - SpiderMonkey 153 compatibility shim implementation
 */

#include "flusspferd.hpp"
#include <js/Initialization.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/PropertyAndElement.h>
#include <js/Array.h>
#include <js/GCAPI.h>
#include <js/Object.h>
#include <js/Exception.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <mysql/sqlDatabase.h>

namespace flusspferd {

// ============================================================================
// Global state
// ============================================================================

JSContext *g_cx = nullptr;
JS::PersistentRootedObject *g_global = nullptr;

std::map<std::string, NativeClassInfo> g_class_registry;
std::map<std::string, void*> g_function_storage;

// Note: g_class_registries is a macro alias defined in flusspferd.hpp

static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS, &JS::DefaultGlobalClassOps
};

namespace {
thread_local unsigned execution_depth = 0;
thread_local std::chrono::steady_clock::time_point execution_deadline;
}

execution_scope::execution_scope(std::chrono::milliseconds budget) {
    if (execution_depth++ == 0) execution_deadline = std::chrono::steady_clock::now() + budget;
}
execution_scope::~execution_scope() { --execution_depth; }
bool execution_timed_out() {
    return execution_depth && std::chrono::steady_clock::now() >= execution_deadline;
}

[[noreturn]] void throw_js_failure(const std::string &operation) {
    JS::RootedValue thrown(g_cx);
    bool pending = JS_IsExceptionPending(g_cx) && JS_GetPendingException(g_cx, &thrown);
    std::string message = operation + " failed";
    if (pending) {
        JS_ClearPendingException(g_cx);
        JS::RootedString str(g_cx, JS::ToString(g_cx, thrown));
        if (str) {
            JS::UniqueChars text = JS_EncodeStringToUTF8(g_cx, str);
            if (text) message += std::string(": ") + text.get();
        }
        // Formatting must neither replace the original thrown value nor leave
        // another exception pending when the caller handles the C++ exception.
        JS_ClearPendingException(g_cx);
    }
    throw js_exception(message, value(thrown), pending);
}

bool handle_native_exception(JSContext *cx) noexcept {
    try {
        throw;
    } catch (const js_exception &e) {
        if (e.has_thrown) {
            JS::RootedValue thrown(cx, e.thrown.get_js_value());
            JS_SetPendingException(cx, thrown);
        } else {
            JS_ClearPendingException(cx); // Preserve uncatchable interruption.
        }
    } catch (const std::bad_alloc &) {
        JS_ReportOutOfMemory(cx);
    } catch (sql::Exception &e) {
        if (!JS_IsExceptionPending(cx)) JS_ReportErrorUTF8(cx, "%s", e.message.c_str());
    } catch (const std::exception &e) {
        if (!JS_IsExceptionPending(cx)) JS_ReportErrorUTF8(cx, "%s", e.what());
    } catch (...) {
        if (!JS_IsExceptionPending(cx)) JS_ReportErrorASCII(cx, "Unknown native exception");
    }
    return false;
}

// ============================================================================
// value implementation
// ============================================================================

value::value(const std::string &s) {
    if (!g_cx) {
        assign(JS::UndefinedValue());
        return;
    }
    JSString *str = JS_NewStringCopyUTF8N(g_cx, JS::UTF8Chars(s.c_str(), s.length()));
    if (!str) throw_js_failure("create string");
    assign(JS::StringValue(str));
}

value::value(const char *s) {
    if (!g_cx || !s) {
        assign(JS::UndefinedValue());
        return;
    }
    JSString *str = JS_NewStringCopyUTF8Z(g_cx, JS::ConstUTF8CharsZ(s));
    if (!str) throw_js_failure("create string");
    assign(JS::StringValue(str));
}

bool value::is_function() const {
    if (!get_js_value().isObject()) return false;
    JSObject *obj = &get_js_value().toObject();
    return obj && JS_ObjectIsFunction(obj);
}

double value::to_number() const {
    if (!g_cx) return 0.0;
    JS::RootedValue input(g_cx, get_js_value());
    execution_scope execution;
    double d;
    if (JS::ToNumber(g_cx, input, &d)) {
        return d;
    }
    throw_js_failure("ToNumber");
}

std::string value::to_std_string() const {
    execution_scope execution;
    if (!g_cx) return "";
    if (!get_js_value().isString()) {
        JS::RootedValue v(g_cx, get_js_value());
        JS::RootedString str(g_cx, JS::ToString(g_cx, v));
        if (!str) throw_js_failure("ToString");
        JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
        if (!chars) throw_js_failure("encode string");
        return std::string(chars.get());
    }
    
    JS::RootedString str(g_cx, get_js_value().toString());
    JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
    if (!chars) throw_js_failure("encode string");
    return std::string(chars.get());
}

string value::to_string() const {
    return string(*this);
}

object value::to_object() const {
    return object(*this);
}

object value::get_object() const {
    return object(*this);
}

// ============================================================================
// string implementation
// ============================================================================

string::string(const std::string &s) : value() {
    if (!g_cx) return;
    JSString *str = JS_NewStringCopyUTF8N(g_cx, JS::UTF8Chars(s.c_str(), s.length()));
    if (!str) throw_js_failure("create string");
    assign(JS::StringValue(str));
}

string::string(const char *s) : value() {
    if (!g_cx || !s) return;
    JSString *str = JS_NewStringCopyUTF8Z(g_cx, JS::ConstUTF8CharsZ(s));
    if (!str) throw_js_failure("create string");
    assign(JS::StringValue(str));
}

string::string(JSString *str) : value() {
    if (!str) throw_js_failure("create string");
    assign(JS::StringValue(str));
}

std::string string::to_std_string() const {
    execution_scope execution;
    if (!g_cx) return "";
    if (!get_js_value().isString()) {
        // Value might not be a JS string type (e.g., number passed to sqlEsc) -
        // convert it using JS::ToString, matching value::to_std_string() behavior
        JS::RootedValue v(g_cx, get_js_value());
        JS::RootedString str(g_cx, JS::ToString(g_cx, v));
        if (!str) throw_js_failure("ToString");
        JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
        if (!chars) throw_js_failure("encode string");
        return std::string(chars.get());
    }
    JS::RootedString str(g_cx, get_js_value().toString());
    JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
    if (!chars) throw_js_failure("encode string");
    return std::string(chars.get());
}

const char *string::c_str() const {
    // Note: This returns a temporary - caller should copy if needed
    static thread_local std::string temp;
    temp = to_std_string();
    return temp.c_str();
}

// ============================================================================
// object implementation
// ============================================================================

bool object::is_array() const {
    if (!g_cx || !get_js_value().isObject()) return false;
    bool isArray = false;
    JS::RootedObject obj(g_cx, &get_js_value().toObject());
    if (!JS::IsArrayObject(g_cx, obj, &isArray)) throw_js_failure("IsArrayObject");
    return isArray;
}

value object::get_property(const std::string &name) const {
    return get_property(name.c_str());
}

value object::get_property(const char *name) const {
    execution_scope execution;
    if (!g_cx || !g_global || is_null()) return value();
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue v(g_cx);
    if (JS_GetProperty(g_cx, obj, name, &v)) {
        return value(v);
    }
    throw_js_failure("get property or element");
}

void object::set_property(const std::string &name, const value &v) {
    set_property(name.c_str(), v);
}

void object::set_property(const char *name, const value &v) {
    execution_scope execution;
    if (!g_cx || !g_global || is_null()) return;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue jv(g_cx, v.get_js_value());
    if (!JS_SetProperty(g_cx, obj, name, jv)) throw_js_failure("set property");
}

bool object::has_property(const std::string &name) const {
    return has_property(name.c_str());
}

bool object::has_property(const char *name) const {
    execution_scope execution;
    if (!g_cx || !g_global || is_null()) return false;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    bool found = false;
    if (JS_HasProperty(g_cx, obj, name, &found)) {
        return found;
    }
    throw_js_failure("has property");
}

void object::delete_property(const std::string &name) {
    delete_property(name.c_str());
}

void object::delete_property(const char *name) {
    execution_scope execution;
    if (!g_cx || !g_global || is_null()) return;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::ObjectOpResult result;
    if (!JS_DeleteProperty(g_cx, obj, name, result)) throw_js_failure("delete property");
    if (!result.ok()) throw exception("Cannot delete property");
}

value object::call(const std::string &name) {
    if (!g_cx || !g_global || is_null()) return value();
    
    execution_scope execution;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue rval(g_cx);
    
    if (JS_CallFunctionName(g_cx, obj, name.c_str(), JS::HandleValueArray::empty(), &rval)) {
        return value(rval);
    }
    
    throw_js_failure(name);
    throw exception("Could not call function: " + name);
}

value object::call(const std::string &name, const value &arg) {
    if (!g_cx || !g_global || is_null()) return value();
    
    execution_scope execution;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue rval(g_cx);
    JS::RootedValueVector argv(g_cx);
    if (!argv.append(arg.get_js_value())) {
        throw exception("Failed to create argument array");
    }
    
    if (JS_CallFunctionName(g_cx, obj, name.c_str(), argv, &rval)) {
        return value(rval);
    }
    
    throw_js_failure(name);
    throw exception("Could not call function: " + name);
}

value object::call(const std::string &name, const value &arg1, const value &arg2) {
    if (!g_cx || !g_global || is_null()) return value();
    
    execution_scope execution;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue rval(g_cx);
    JS::RootedValueVector argv(g_cx);
    if (!argv.append(arg1.get_js_value()) || !argv.append(arg2.get_js_value())) {
        throw exception("Failed to create argument array");
    }
    
    if (JS_CallFunctionName(g_cx, obj, name.c_str(), argv, &rval)) {
        return value(rval);
    }
    
    throw_js_failure(name);
    throw exception("Could not call function: " + name);
}

value object::call_function(const object &receiver, const value &arg) {
    execution_scope execution;
    JS::RootedObject thisObj(g_cx, receiver.get_object_ptr());
    JS::RootedValue function(g_cx, get_js_value());
    JS::RootedValue argument(g_cx, arg.get_js_value());
    JS::RootedValue result(g_cx);
    if (!JS_CallFunctionValue(g_cx, thisObj, function, JS::HandleValueArray(argument), &result))
        throw_js_failure("call function");
    return value(result);
}

object object::prototype() const {
    if (!g_cx || is_null()) return object();
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedObject proto(g_cx);
    if (JS_GetPrototype(g_cx, obj, &proto)) {
        return object(proto);
    }
    throw_js_failure("get prototype");
}

object object::parent() const {
    // In modern SpiderMonkey, parent is not directly accessible
    // Return null for compatibility
    return object();
}

property_iterator object::begin() const {
    if (!g_cx || is_null()) return property_iterator();
    return property_iterator(g_cx, get_object_ptr());
}

property_iterator object::end() const {
    return property_iterator();
}

// ============================================================================
// array implementation
// ============================================================================

uint32_t array::length() const {
    if (!g_cx || is_null()) return 0;
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    uint32_t len = 0;
    if (!JS::GetArrayLength(g_cx, obj, &len)) throw_js_failure("array length");
    return len;
}

void array::set_length(uint32_t len) {
    if (!g_cx || is_null()) return;
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    if (!JS::SetArrayLength(g_cx, obj, len)) throw_js_failure("set array length");
}

value array::get_element(uint32_t index) const {
    execution_scope execution;
    if (!g_cx || is_null()) return value();
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue v(g_cx);
    if (JS_GetElement(g_cx, obj, index, &v)) {
        return value(v);
    }
    throw_js_failure("get property or element");
}

void array::set_element(uint32_t index, const value &v) {
    execution_scope execution;
    if (!g_cx || is_null()) return;
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue jv(g_cx, v.get_js_value());
    if (!JS_SetElement(g_cx, obj, index, jv)) throw_js_failure("set element");
}

void array::push(const value &v) {
    uint32_t len = length();
    set_element(len, v);
}

// ============================================================================
// root_value implementation
// ============================================================================

root_value::root_value() {
    if (g_cx) {
        rooted = std::make_unique<JS::PersistentRootedValue>(g_cx);
    }
}

root_value::root_value(const value &v) {
    if (g_cx) {
        rooted = std::make_unique<JS::PersistentRootedValue>(g_cx, v.get_js_value());
    }
}

root_value::~root_value() = default;

root_value &root_value::operator=(const value &v) {
    if (rooted) {
        *rooted = v.get_js_value();
    }
    return *this;
}

root_value::operator value() const {
    return get();
}

value root_value::get() const {
    if (rooted) {
        return value(rooted->get());
    }
    return value();
}

// ============================================================================
// root_object implementation
// ============================================================================

root_object::root_object() {
    if (g_cx) {
        rooted = std::make_unique<JS::PersistentRootedObject>(g_cx);
    }
}

root_object::root_object(const object &o) {
    if (g_cx) {
        rooted = std::make_unique<JS::PersistentRootedObject>(g_cx, o.get_object_ptr());
    }
}

root_object::~root_object() = default;

root_object &root_object::operator=(const object &o) {
    if (rooted) {
        *rooted = o.get_object_ptr();
    }
    return *this;
}

root_object::operator object() const {
    return get();
}

object root_object::get() const {
    if (rooted) {
        return object(rooted->get());
    }
    return object();
}

// ============================================================================
// context implementation
// ============================================================================

context context::create() {
    // Initialize SpiderMonkey if not already done
    static bool initialized = false;
    if (!initialized) {
        // JIT is enabled by default. This process-wide fallback is deliberately
        // read before JS_Init; changing it requires a process restart.
        const char *disableJit = std::getenv("KINSLAYER_DISABLE_JIT");
        const bool jitEnabled = !(disableJit && std::strcmp(disableJit, "1") == 0);
        if (!jitEnabled) JS::DisableJitBackend();
        std::fprintf(stderr, "%s: JIT %s\n", JS_GetImplementationVersion(), jitEnabled ? "enabled" : "disabled");
        if (!JS_Init()) {
            return context();
        }
        initialized = true;
    }
    
    // Create a new context with 1GB max heap
    JSContext *cx = JS_NewContext(1024L * 1024 * 1024);
    if (!cx) {
        return context();
    }

    // Configure GC parameters for a long-running game server
    JS_SetGCParameter(cx, JSGC_MAX_BYTES, 1024 * 1024 * 1024);  // 1GB max
    JS_SetGCParameter(cx, JSGC_INCREMENTAL_GC_ENABLED, 1);

    if (!JS::InitSelfHostedCode(cx)) {
        JS_DestroyContext(cx);
        return context();
    }
    
    context ctx;
    ctx.cx = cx;
    ctx.owns_context = true;
    
    return ctx;
}

void context::destroy() {
    if (owns_context && cx) {
        JS_DestroyContext(cx);
        cx = nullptr;
        g_cx = nullptr;
        JS_ShutDown();
    }
}

// ============================================================================
// current_context_scope implementation
// ============================================================================

current_context_scope::current_context_scope(const context &ctx) : context_(ctx), realm_(nullptr) {
    if (!ctx.is_valid()) throw exception("Failed to initialize SpiderMonkey context");
    g_cx = ctx.get();

    // Create the global object if it doesn't exist
    if (g_cx && !g_global) {
        JS::RealmOptions options;
        JSObject *globalObj = JS_NewGlobalObject(g_cx, &global_class, nullptr,
                                                  JS::FireOnNewGlobalHook, options);
        if (globalObj) {
            g_global = new JS::PersistentRootedObject(g_cx, globalObj);

            // Enter the global's realm and initialize standard classes
            JSAutoRealm ar(g_cx, globalObj);
            if (!JS::InitRealmStandardClasses(g_cx)) {
                delete g_global;
                g_global = nullptr;
            }
        }
    }

    // Keep the realm active for the lifetime of this scope
    if (g_cx && g_global && g_global->get()) {
        realm_ = new JSAutoRealm(g_cx, g_global->get());
    }
}

current_context_scope::~current_context_scope() {
    // All application-owned wrappers must be released before the realm owner.
    delete realm_;
    for (auto &entry : g_class_registry) {
        if (entry.second.release_prototype) entry.second.release_prototype();
    }
    delete g_global;
    g_global = nullptr;
    g_class_registry.clear();
    g_function_storage.clear();
    context_.destroy();
}

// ============================================================================
// property_iterator implementation
// ============================================================================

property_iterator::property_iterator() 
    : cx(nullptr), obj(nullptr), ids(nullptr), current_index(0), at_end(true) {}

property_iterator::property_iterator(JSContext *c, JSObject *o) 
    : cx(c), obj(new JS::PersistentRootedObject(c, o)), 
      ids(new JS::PersistentRooted<JS::IdVector>(c, JS::IdVector(c))), 
      current_index(0), at_end(false) {
    if (!JS_Enumerate(cx, *obj, &(*ids))) {
        delete ids;
        delete obj;
        throw_js_failure("enumerate properties");
    }
    if (ids->length() == 0) {
        at_end = true;
    }
}

property_iterator::property_iterator(property_iterator &&other) noexcept
    : cx(other.cx), obj(other.obj), ids(other.ids), 
      current_index(other.current_index), at_end(other.at_end) {
    other.cx = nullptr;
    other.obj = nullptr;
    other.ids = nullptr;
    other.at_end = true;
}

property_iterator &property_iterator::operator=(property_iterator &&other) noexcept {
    if (this != &other) {
        delete obj;
        delete ids;
        cx = other.cx;
        obj = other.obj;
        ids = other.ids;
        current_index = other.current_index;
        at_end = other.at_end;
        other.cx = nullptr;
        other.obj = nullptr;
        other.ids = nullptr;
        other.at_end = true;
    }
    return *this;
}

property_iterator::~property_iterator() {
    delete obj;
    delete ids;
}

property_iterator &property_iterator::operator++() {
    if (!at_end && ids) {
        current_index++;
        if (current_index >= ids->length()) {
            at_end = true;
        }
    }
    return *this;
}

bool property_iterator::operator!=(const property_iterator &other) const {
    if (at_end && other.at_end) return false;
    if (at_end != other.at_end) return true;
    return current_index != other.current_index;
}

bool property_iterator::operator==(const property_iterator &other) const {
    return !(*this != other);
}

string property_iterator::operator*() const {
    if (at_end || !ids || current_index >= ids->length()) return string();
    
    JS::RootedId id(cx, (*ids)[current_index]);
    JS::RootedValue idval(cx);
    if (!JS_IdToValue(cx, id, &idval)) {
        return string();
    }
    
    JS::RootedString str(cx, JS::ToString(cx, idval));
    if (!str) return string();
    
    return string(str);
}

// ============================================================================
// Global functions implementation
// ============================================================================

context current_context() {
    return context(g_cx);
}

object global() {
    if (!g_global) return object();
    return object(g_global->get());
}

value evaluate(const std::string &code, const char *filename, int lineno) {
    return evaluate(code.c_str(), filename, lineno);
}

value evaluate(const char *code, const char *filename, int lineno) {
    if (!g_cx || !g_global) return value();
    
    execution_scope execution;
    // Enter the global's realm
    JSAutoRealm ar(g_cx, g_global->get());
    
    JS::CompileOptions options(g_cx);
    options.setFileAndLine(filename, lineno);
    
    JS::SourceText<mozilla::Utf8Unit> srcBuf;
    if (!srcBuf.init(g_cx, code, strlen(code), JS::SourceOwnership::Borrowed)) {
        throw_js_failure("evaluate");
        throw exception("Failed to initialize source buffer");
    }
    
    JS::RootedValue rval(g_cx);
    
    if (!JS::Evaluate(g_cx, options, srcBuf, &rval)) {
        throw_js_failure("evaluate");
        throw exception("JavaScript evaluation failed");
    }
    
    return value(rval);
}

void gc() {
    if (g_cx) {
        JS_MaybeGC(g_cx);
    }
}

object create_object() {
    if (!g_cx || !g_global) return object();
    JSAutoRealm ar(g_cx, g_global->get());
    JSObject *obj = JS_NewPlainObject(g_cx);
    if (!obj) throw_js_failure("create object");
    return object(obj);
}

array create_array() {
    if (!g_cx || !g_global) return array();
    JSAutoRealm ar(g_cx, g_global->get());
    JSObject *arr = JS::NewArrayObject(g_cx, 0);
    if (!arr) throw_js_failure("create array");
    array result(arr);
    return result;
}

array create_array(uint32_t length) {
    if (!g_cx || !g_global) return array();
    JSAutoRealm ar(g_cx, g_global->get());
    JSObject *arr = JS::NewArrayObject(g_cx, length);
    if (!arr) throw_js_failure("create array");
    array result(arr);
    return result;
}

// ============================================================================
// Type conversion implementations
// ============================================================================

namespace detail {

JS::Value to_jsval<std::string>::convert(JSContext *cx, const std::string &v) {
    JSString *str = JS_NewStringCopyUTF8N(cx, JS::UTF8Chars(v.c_str(), v.length()));
    if (!str) throw_js_failure("create string");
    return JS::StringValue(str);
}

JS::Value to_jsval<const char*>::convert(JSContext *cx, const char* const &v) {
    if (!v) return JS::NullValue();
    JSString *str = JS_NewStringCopyUTF8Z(cx, JS::ConstUTF8CharsZ(v));
    if (!str) throw_js_failure("create string");
    return JS::StringValue(str);
}

std::string from_jsval<std::string>::convert(JSContext *cx, const JS::Value &v) {
    if (!v.isString()) {
        JS::RootedValue rv(cx, v);
        JS::RootedString str(cx, JS::ToString(cx, rv));
        if (!str) throw_js_failure("ToString");
        JS::UniqueChars chars = JS_EncodeStringToUTF8(cx, str);
        if (!chars) throw_js_failure("encode string");
        return std::string(chars.get());
    }
    
    JS::RootedString str(cx, v.toString());
    JS::UniqueChars chars = JS_EncodeStringToUTF8(cx, str);
    if (!chars) throw_js_failure("encode string");
    return std::string(chars.get());
}

} // namespace detail

// ============================================================================
// Property getter/setter dispatchers for native classes
// ============================================================================

bool property_getter_dispatch(JSContext *cx, unsigned argc, JS::Value *vp) try {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (!args.thisv().isObject()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedObject thisObj(cx, &args.thisv().toObject());
    void *priv = nullptr;

    // Get property name from callee's reserved slot 0
    JS::RootedObject callee(cx, &args.callee());
    const JS::Value &propNameVal = js::GetFunctionNativeReserved(callee, 0);
    if (!propNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString propNameStr(cx, propNameVal.toString());
    JS::UniqueChars propName = JS_EncodeStringToUTF8(cx, propNameStr);
    if (!propName) return false;

    // Get class name from callee's reserved slot 1
    const JS::Value &classNameVal = js::GetFunctionNativeReserved(callee, 1);
    if (!classNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString classNameStr(cx, classNameVal.toString());
    JS::UniqueChars className = JS_EncodeStringToUTF8(cx, classNameStr);
    if (!className) return false;

    // Look up getter in registry
    auto classIt = g_class_registry.find(className.get());
    if (classIt == g_class_registry.end()) {
        args.rval().setUndefined();
        return true;
    }

    if (JS::GetClass(thisObj) != classIt->second.jsclass) {
        JS_ReportErrorASCII(cx, "Incompatible native receiver");
        return false;
    }
    priv = sm_get_private(thisObj);
    if (!priv) {
        JS_ReportErrorASCII(cx, "Native object has no private data");
        return false;
    }

    auto getterIt = classIt->second.getters.find(propName.get());
    if (getterIt == classIt->second.getters.end()) {
        args.rval().setUndefined();
        return true;
    }

    // Call the getter
    JS::MutableHandleValue rval = args.rval();
    return getterIt->second(priv, cx, rval);
} catch (...) {
    return handle_native_exception(cx);
}

bool property_setter_dispatch(JSContext *cx, unsigned argc, JS::Value *vp) try {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (!args.thisv().isObject()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedObject thisObj(cx, &args.thisv().toObject());
    void *priv = nullptr;

    // Get property name from callee's reserved slot 0
    JS::RootedObject callee(cx, &args.callee());
    const JS::Value &propNameVal = js::GetFunctionNativeReserved(callee, 0);
    if (!propNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString propNameStr(cx, propNameVal.toString());
    JS::UniqueChars propName = JS_EncodeStringToUTF8(cx, propNameStr);
    if (!propName) return false;

    // Get class name from callee's reserved slot 1
    const JS::Value &classNameVal = js::GetFunctionNativeReserved(callee, 1);
    if (!classNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString classNameStr(cx, classNameVal.toString());
    JS::UniqueChars className = JS_EncodeStringToUTF8(cx, classNameStr);
    if (!className) return false;

    // Look up setter in registry
    auto classIt = g_class_registry.find(className.get());
    if (classIt == g_class_registry.end()) {
        args.rval().setUndefined();
        return true;
    }

    if (JS::GetClass(thisObj) != classIt->second.jsclass) {
        JS_ReportErrorASCII(cx, "Incompatible native receiver");
        return false;
    }
    priv = sm_get_private(thisObj);
    if (!priv) {
        JS_ReportErrorASCII(cx, "Native object has no private data");
        return false;
    }

    auto setterIt = classIt->second.setters.find(propName.get());
    if (setterIt == classIt->second.setters.end()) {
        args.rval().setUndefined();
        return true;
    }

    // Call the setter with the value argument
    if (argc > 0) {
        JS::HandleValue val = args[0];
        if (!setterIt->second(priv, cx, val)) return false;
    }

    args.rval().setUndefined();
    return true;
} catch (...) {
    return handle_native_exception(cx);
}

// ============================================================================
// Universal method dispatcher for native classes
// ============================================================================

bool universal_method_dispatch(JSContext *cx, unsigned argc, JS::Value *vp) try {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    // Get 'this' object
    if (!args.thisv().isObject()) {
        JS_ReportErrorASCII(cx, "Method called on non-object");
        return false;
    }

    JS::RootedObject thisObj(cx, &args.thisv().toObject());

    // Get the private data (native object pointer)
    void *priv = nullptr;

    // Get the method name from the callee's function native reserved slot
    JS::RootedObject callee(cx, &args.callee());
    const JS::Value &methodNameVal = js::GetFunctionNativeReserved(callee, 0);

    if (!methodNameVal.isString()) {
        JS_ReportErrorASCII(cx, "Invalid method name");
        return false;
    }

    JS::RootedString methodNameStr(cx, methodNameVal.toString());
    JS::UniqueChars methodName = JS_EncodeStringToUTF8(cx, methodNameStr);
    if (!methodName) {
        JS_ReportErrorASCII(cx, "Failed to get method name");
        return false;
    }

    // Get class name from function native reserved slot 1
    const JS::Value &classNameVal = js::GetFunctionNativeReserved(callee, 1);
    if (!classNameVal.isString()) {
        JS_ReportErrorASCII(cx, "Invalid class name");
        return false;
    }

    JS::RootedString classNameStr(cx, classNameVal.toString());
    JS::UniqueChars className = JS_EncodeStringToUTF8(cx, classNameStr);
    if (!className) {
        JS_ReportErrorASCII(cx, "Failed to get class name");
        return false;
    }
    
    // Look up the method in the registry
    auto classIt = g_class_registry.find(className.get());
    if (classIt == g_class_registry.end()) {
        JS_ReportErrorASCII(cx, "Class not found in registry");
        return false;
    }
    
    if (JS::GetClass(thisObj) != classIt->second.jsclass) {
        JS_ReportErrorASCII(cx, "Incompatible native receiver");
        return false;
    }
    priv = sm_get_private(thisObj);
    if (!priv) {
        JS_ReportErrorASCII(cx, "Native object has no private data");
        return false;
    }

    auto methodIt = classIt->second.methods.find(methodName.get());
    if (methodIt == classIt->second.methods.end()) {
        JS_ReportErrorASCII(cx, "Method '%s' not found", methodName.get());
        return false;
    }
    
    // Call the method
    return methodIt->second(priv, cx, argc, vp);
} catch (...) {
    return handle_native_exception(cx);
}

} // namespace flusspferd

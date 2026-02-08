/**
 * flusspferd.cpp - SpiderMonkey 131 compatibility shim implementation
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

namespace flusspferd {

// ============================================================================
// Global state
// ============================================================================

JSContext *g_cx = nullptr;
JS::PersistentRootedObject *g_global = nullptr;

std::map<std::string, NativeClassInfo> g_class_registry;
std::map<std::string, void*> g_function_storage;

// Note: g_class_registries is a macro alias defined in flusspferd.hpp

// Thread-local storage for last exception message
static thread_local std::string g_last_exception_message;
static thread_local bool g_has_last_exception = false;

// Global class definition for the global object
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    &JS::DefaultGlobalClassOps
};

// ============================================================================
// Error reporting and exception handling
// ============================================================================

static void clear_last_exception() {
    g_last_exception_message.clear();
    g_has_last_exception = false;
}

// Helper to check for pending exception and throw it
static void check_and_throw_pending_exception(const std::string &funcName) {
    if (!g_cx) return;

    if (JS_IsExceptionPending(g_cx)) {
        JS::RootedValue exc(g_cx);
        if (JS_GetPendingException(g_cx, &exc)) {
            JS_ClearPendingException(g_cx);
            clear_last_exception();

            if (exc.isObject()) {
                JS::RootedObject excObj(g_cx, &exc.toObject());
                const JSClass *cls = JS::GetClass(excObj);

                // Check for StopIteration
                if (cls && cls->name && strcmp(cls->name, "StopIteration") == 0) {
                    throw exception("[object StopIteration]");
                }

                // For Error objects, extract the message and other properties
                JS::RootedValue msgVal(g_cx);
                if (JS_GetProperty(g_cx, excObj, "message", &msgVal) && msgVal.isString()) {
                    JS::RootedString msgStr(g_cx, msgVal.toString());
                    JS::UniqueChars msgChars = JS_EncodeStringToUTF8(g_cx, msgStr);
                    if (msgChars) {
                        std::string errorMsg;
                        if (cls && cls->name) {
                            errorMsg = cls->name;
                            errorMsg += ": ";
                        }
                        errorMsg += msgChars.get();

                        // Try to get lineNumber and fileName for better diagnostics
                        JS::RootedValue lineVal(g_cx);
                        if (JS_GetProperty(g_cx, excObj, "lineNumber", &lineVal) && lineVal.isInt32()) {
                            errorMsg += " (line ";
                            errorMsg += std::to_string(lineVal.toInt32());
                            errorMsg += ")";
                        }

                        throw exception(errorMsg);
                    }
                }
            }

            // Fallback: convert exception to string
            JS::RootedString str(g_cx, JS::ToString(g_cx, exc));
            if (str) {
                JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
                if (chars) {
                    throw exception(chars.get());
                }
            }
            throw exception("uncaught exception: [unknown]");
        }
    }

    // Check captured exception from error reporter
    if (g_has_last_exception) {
        std::string msg = g_last_exception_message;
        clear_last_exception();
        throw exception(msg);
    }
}

// ============================================================================
// value implementation
// ============================================================================

value::value(const std::string &s) {
    if (!g_cx) {
        val = JS::UndefinedValue();
        return;
    }
    JSString *str = JS_NewStringCopyUTF8N(g_cx, JS::UTF8Chars(s.c_str(), s.length()));
    val = str ? JS::StringValue(str) : JS::UndefinedValue();
}

value::value(const char *s) {
    if (!g_cx || !s) {
        val = JS::UndefinedValue();
        return;
    }
    JSString *str = JS_NewStringCopyUTF8Z(g_cx, JS::ConstUTF8CharsZ(s));
    val = str ? JS::StringValue(str) : JS::UndefinedValue();
}

bool value::is_function() const {
    if (!val.isObject()) return false;
    JSObject *obj = &val.toObject();
    return obj && JS_ObjectIsFunction(obj);
}

double value::to_number() const {
    if (!g_cx) return 0.0;
    double d;
    if (JS::ToNumber(g_cx, JS::HandleValue::fromMarkedLocation(&val), &d)) {
        return d;
    }
    return 0.0;
}

std::string value::to_std_string() const {
    if (!g_cx) return "";
    if (!val.isString()) {
        JS::RootedValue v(g_cx, val);
        JS::RootedString str(g_cx, JS::ToString(g_cx, v));
        if (!str) return "";
        JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
        return chars ? std::string(chars.get()) : "";
    }
    
    JS::RootedString str(g_cx, val.toString());
    JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
    return chars ? std::string(chars.get()) : "";
}

string value::to_string() const {
    return string(val);
}

object value::to_object() const {
    return object(val);
}

object value::get_object() const {
    return object(val);
}

// ============================================================================
// string implementation
// ============================================================================

string::string(const std::string &s) : value() {
    if (!g_cx) return;
    JSString *str = JS_NewStringCopyUTF8N(g_cx, JS::UTF8Chars(s.c_str(), s.length()));
    val = str ? JS::StringValue(str) : JS::UndefinedValue();
}

string::string(const char *s) : value() {
    if (!g_cx || !s) return;
    JSString *str = JS_NewStringCopyUTF8Z(g_cx, JS::ConstUTF8CharsZ(s));
    val = str ? JS::StringValue(str) : JS::UndefinedValue();
}

string::string(JSString *str) : value() {
    val = str ? JS::StringValue(str) : JS::UndefinedValue();
}

std::string string::to_std_string() const {
    if (!g_cx || !val.isString()) return "";
    JS::RootedString str(g_cx, val.toString());
    JS::UniqueChars chars = JS_EncodeStringToUTF8(g_cx, str);
    return chars ? std::string(chars.get()) : "";
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
    if (!g_cx || !val.isObject()) return false;
    bool isArray = false;
    JS::RootedObject obj(g_cx, &val.toObject());
    JS::IsArrayObject(g_cx, obj, &isArray);
    return isArray;
}

value object::get_property(const std::string &name) const {
    return get_property(name.c_str());
}

value object::get_property(const char *name) const {
    if (!g_cx || !g_global || is_null()) return value();
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue v(g_cx);
    if (JS_GetProperty(g_cx, obj, name, &v)) {
        return value(v);
    }
    return value();
}

void object::set_property(const std::string &name, const value &v) {
    set_property(name.c_str(), v);
}

void object::set_property(const char *name, const value &v) {
    if (!g_cx || !g_global || is_null()) return;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue jv(g_cx, v.val);
    JS_SetProperty(g_cx, obj, name, jv);
}

bool object::has_property(const std::string &name) const {
    return has_property(name.c_str());
}

bool object::has_property(const char *name) const {
    if (!g_cx || !g_global || is_null()) return false;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    bool found = false;
    if (JS_HasProperty(g_cx, obj, name, &found)) {
        return found;
    }
    return false;
}

void object::delete_property(const std::string &name) {
    delete_property(name.c_str());
}

void object::delete_property(const char *name) {
    if (!g_cx || !g_global || is_null()) return;
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::ObjectOpResult result;
    JS_DeleteProperty(g_cx, obj, name, result);
}

value object::call(const std::string &name) {
    if (!g_cx || !g_global || is_null()) return value();
    
    clear_last_exception();
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue rval(g_cx);
    
    if (JS_CallFunctionName(g_cx, obj, name.c_str(), JS::HandleValueArray::empty(), &rval)) {
        return value(rval);
    }
    
    check_and_throw_pending_exception(name);
    throw exception("Could not call function: " + name);
}

value object::call(const std::string &name, const value &arg) {
    if (!g_cx || !g_global || is_null()) return value();
    
    clear_last_exception();
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue rval(g_cx);
    JS::RootedValueVector argv(g_cx);
    if (!argv.append(arg.val)) {
        throw exception("Failed to create argument array");
    }
    
    if (JS_CallFunctionName(g_cx, obj, name.c_str(), argv, &rval)) {
        return value(rval);
    }
    
    check_and_throw_pending_exception(name);
    throw exception("Could not call function: " + name);
}

value object::call(const std::string &name, const value &arg1, const value &arg2) {
    if (!g_cx || !g_global || is_null()) return value();
    
    clear_last_exception();
    
    JSAutoRealm ar(g_cx, g_global->get());
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue rval(g_cx);
    JS::RootedValueVector argv(g_cx);
    if (!argv.append(arg1.val) || !argv.append(arg2.val)) {
        throw exception("Failed to create argument array");
    }
    
    if (JS_CallFunctionName(g_cx, obj, name.c_str(), argv, &rval)) {
        return value(rval);
    }
    
    check_and_throw_pending_exception(name);
    throw exception("Could not call function: " + name);
}

object object::prototype() const {
    if (!g_cx || is_null()) return object();
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedObject proto(g_cx);
    if (JS_GetPrototype(g_cx, obj, &proto)) {
        return object(proto);
    }
    return object();
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
    JS::GetArrayLength(g_cx, obj, &len);
    return len;
}

void array::set_length(uint32_t len) {
    if (!g_cx || is_null()) return;
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::SetArrayLength(g_cx, obj, len);
}

value array::get_element(uint32_t index) const {
    if (!g_cx || is_null()) return value();
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue v(g_cx);
    if (JS_GetElement(g_cx, obj, index, &v)) {
        return value(v);
    }
    return value();
}

void array::set_element(uint32_t index, const value &v) {
    if (!g_cx || is_null()) return;
    
    JS::RootedObject obj(g_cx, get_object_ptr());
    JS::RootedValue jv(g_cx, v.val);
    JS_SetElement(g_cx, obj, index, jv);
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
        rooted = std::make_unique<JS::PersistentRootedValue>(g_cx, v.val);
    }
}

root_value::~root_value() = default;

root_value &root_value::operator=(const value &v) {
    if (rooted) {
        *rooted = v.val;
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
        // Disable JIT backend - use interpreter only.
        // Our compatibility shim uses unrooted JS::Value in many places,
        // which is unsafe with the JIT's exact GC rooting requirements.
        JS::DisableJitBackend();
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
    }
}

// ============================================================================
// current_context_scope implementation
// ============================================================================

current_context_scope::current_context_scope(const context &ctx) : realm_(nullptr) {
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
    delete realm_;
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
        at_end = true;
        return;
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
    
    // Enter the global's realm
    JSAutoRealm ar(g_cx, g_global->get());
    
    JS::CompileOptions options(g_cx);
    options.setFileAndLine(filename, lineno);
    
    JS::SourceText<mozilla::Utf8Unit> srcBuf;
    if (!srcBuf.init(g_cx, code, strlen(code), JS::SourceOwnership::Borrowed)) {
        check_and_throw_pending_exception("evaluate");
        throw exception("Failed to initialize source buffer");
    }
    
    JS::RootedValue rval(g_cx);
    
    if (!JS::Evaluate(g_cx, options, srcBuf, &rval)) {
        check_and_throw_pending_exception("evaluate");
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
    return object(obj);
}

array create_array() {
    if (!g_cx || !g_global) return array();
    JSAutoRealm ar(g_cx, g_global->get());
    JSObject *arr = JS::NewArrayObject(g_cx, 0);
    if (!arr) return array();
    array result(arr);
    result.set_auto_root();
    return result;
}

array create_array(uint32_t length) {
    if (!g_cx || !g_global) return array();
    JSAutoRealm ar(g_cx, g_global->get());
    JSObject *arr = JS::NewArrayObject(g_cx, length);
    if (!arr) return array();
    array result(arr);
    result.set_auto_root();
    return result;
}

// ============================================================================
// Type conversion implementations
// ============================================================================

namespace detail {

JS::Value to_jsval<std::string>::convert(JSContext *cx, const std::string &v) {
    JSString *str = JS_NewStringCopyUTF8N(cx, JS::UTF8Chars(v.c_str(), v.length()));
    return str ? JS::StringValue(str) : JS::UndefinedValue();
}

JS::Value to_jsval<const char*>::convert(JSContext *cx, const char* const &v) {
    if (!v) return JS::NullValue();
    JSString *str = JS_NewStringCopyUTF8Z(cx, JS::ConstUTF8CharsZ(v));
    return str ? JS::StringValue(str) : JS::UndefinedValue();
}

std::string from_jsval<std::string>::convert(JSContext *cx, const JS::Value &v) {
    if (!v.isString()) {
        JS::RootedValue rv(cx, v);
        JS::RootedString str(cx, JS::ToString(cx, rv));
        if (!str) return "";
        JS::UniqueChars chars = JS_EncodeStringToUTF8(cx, str);
        return chars ? std::string(chars.get()) : "";
    }
    
    JS::RootedString str(cx, v.toString());
    JS::UniqueChars chars = JS_EncodeStringToUTF8(cx, str);
    return chars ? std::string(chars.get()) : "";
}

} // namespace detail

// ============================================================================
// Property getter/setter dispatchers for native classes
// ============================================================================

bool property_getter_dispatch(JSContext *cx, unsigned argc, JS::Value *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (!args.thisv().isObject()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedObject thisObj(cx, &args.thisv().toObject());
    void *priv = sm_get_private(thisObj);
    if (!priv) {
        args.rval().setUndefined();
        return true;
    }

    // Get property name from callee's reserved slot 0
    JSObject *callee = &args.callee();
    const JS::Value &propNameVal = js::GetFunctionNativeReserved(callee, 0);
    if (!propNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString propNameStr(cx, propNameVal.toString());
    JS::UniqueChars propName = JS_EncodeStringToUTF8(cx, propNameStr);
    if (!propName) {
        args.rval().setUndefined();
        return true;
    }

    // Get class name from callee's reserved slot 1
    const JS::Value &classNameVal = js::GetFunctionNativeReserved(callee, 1);
    if (!classNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString classNameStr(cx, classNameVal.toString());
    JS::UniqueChars className = JS_EncodeStringToUTF8(cx, classNameStr);
    if (!className) {
        args.rval().setUndefined();
        return true;
    }

    // Look up getter in registry
    auto classIt = g_class_registry.find(className.get());
    if (classIt == g_class_registry.end()) {
        args.rval().setUndefined();
        return true;
    }

    auto getterIt = classIt->second.getters.find(propName.get());
    if (getterIt == classIt->second.getters.end()) {
        args.rval().setUndefined();
        return true;
    }

    // Call the getter
    JS::MutableHandleValue rval = args.rval();
    return getterIt->second(priv, cx, rval);
}

bool property_setter_dispatch(JSContext *cx, unsigned argc, JS::Value *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (!args.thisv().isObject()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedObject thisObj(cx, &args.thisv().toObject());
    void *priv = sm_get_private(thisObj);
    if (!priv) {
        args.rval().setUndefined();
        return true;
    }

    // Get property name from callee's reserved slot 0
    JSObject *callee = &args.callee();
    const JS::Value &propNameVal = js::GetFunctionNativeReserved(callee, 0);
    if (!propNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString propNameStr(cx, propNameVal.toString());
    JS::UniqueChars propName = JS_EncodeStringToUTF8(cx, propNameStr);
    if (!propName) {
        args.rval().setUndefined();
        return true;
    }

    // Get class name from callee's reserved slot 1
    const JS::Value &classNameVal = js::GetFunctionNativeReserved(callee, 1);
    if (!classNameVal.isString()) {
        args.rval().setUndefined();
        return true;
    }

    JS::RootedString classNameStr(cx, classNameVal.toString());
    JS::UniqueChars className = JS_EncodeStringToUTF8(cx, classNameStr);
    if (!className) {
        args.rval().setUndefined();
        return true;
    }

    // Look up setter in registry
    auto classIt = g_class_registry.find(className.get());
    if (classIt == g_class_registry.end()) {
        args.rval().setUndefined();
        return true;
    }

    auto setterIt = classIt->second.setters.find(propName.get());
    if (setterIt == classIt->second.setters.end()) {
        args.rval().setUndefined();
        return true;
    }

    // Call the setter with the value argument
    if (argc > 0) {
        JS::HandleValue val = args[0];
        setterIt->second(priv, cx, val);
    }

    args.rval().setUndefined();
    return true;
}

// ============================================================================
// Universal method dispatcher for native classes
// ============================================================================

bool universal_method_dispatch(JSContext *cx, unsigned argc, JS::Value *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    // Get 'this' object
    if (!args.thisv().isObject()) {
        JS_ReportErrorASCII(cx, "Method called on non-object");
        return false;
    }

    JS::RootedObject thisObj(cx, &args.thisv().toObject());

    // Get the private data (native object pointer)
    void *priv = sm_get_private(thisObj);
    if (!priv) {
        JS_ReportErrorASCII(cx, "No private data on object");
        return false;
    }

    // Get the method name from the callee's function native reserved slot
    JSObject *callee = &args.callee();
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
    
    auto methodIt = classIt->second.methods.find(methodName.get());
    if (methodIt == classIt->second.methods.end()) {
        JS_ReportErrorASCII(cx, "Method '%s' not found", methodName.get());
        return false;
    }
    
    // Call the method
    return methodIt->second(priv, cx, argc, vp);
}

} // namespace flusspferd

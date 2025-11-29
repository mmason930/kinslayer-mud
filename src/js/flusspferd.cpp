/**
 * flusspferd.cpp - SpiderMonkey 1.8.5 compatibility shim implementation
 */

#include "flusspferd.hpp"
#include <cstring>
#include <sstream>

namespace flusspferd {

// ============================================================================
// Global state
// ============================================================================

JSContext *g_cx = nullptr;
JSRuntime *g_rt = nullptr;
JSObject *g_global = nullptr;

std::map<std::string, NativeClassInfo> g_class_registry;
std::map<std::string, void*> g_function_storage;
std::map<std::string, ClassRegistry> g_class_registries;

// Global class for the global object
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub,
    JS_PropertyStub,
    JS_PropertyStub,
    JS_PropertyStub,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    nullptr,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

// ============================================================================
// Error reporting callback
// ============================================================================

static void errorReporter(JSContext *cx, const char *message, JSErrorReport *report) {
    std::stringstream ss;
    ss << "JavaScript Error: ";
    if (report && report->filename) {
        ss << report->filename << ":" << report->lineno << ": ";
    }
    ss << message;
    
    fprintf(stderr, "%s\n", ss.str().c_str());
}

// ============================================================================
// value implementation
// ============================================================================

value::value(const std::string &s) {
    JSString *str = JS_NewStringCopyN(g_cx, s.c_str(), s.length());
    val = str ? STRING_TO_JSVAL(str) : JSVAL_VOID;
}

value::value(const char *s) {
    if (s) {
        JSString *str = JS_NewStringCopyZ(g_cx, s);
        val = str ? STRING_TO_JSVAL(str) : JSVAL_VOID;
    } else {
        val = JSVAL_NULL;
    }
}

bool value::is_function() const {
    if (!is_object()) return false;
    JSObject *obj = JSVAL_TO_OBJECT(val);
    return obj && JS_ObjectIsFunction(g_cx, obj);
}

double value::to_number() const {
    jsdouble d;
    if (JS_ValueToNumber(g_cx, val, &d)) {
        return d;
    }
    return 0.0;
}

std::string value::to_std_string() const {
    JSString *str = JS_ValueToString(g_cx, val);
    if (!str) return "";
    
    char *cstr = JS_EncodeString(g_cx, str);
    if (!cstr) return "";
    
    std::string result(cstr);
    JS_free(g_cx, cstr);
    return result;
}

object value::to_object() const {
    if (is_null() || is_undefined()) {
        return object();
    }
    JSObject *obj;
    if (JS_ValueToObject(g_cx, val, &obj)) {
        return object(obj);
    }
    return object();
}

object value::get_object() const {
    return to_object();
}

// ============================================================================
// string implementation
// ============================================================================

string::string(const std::string &s) {
    JSString *str = JS_NewStringCopyN(g_cx, s.c_str(), s.length());
    val = str ? STRING_TO_JSVAL(str) : JSVAL_VOID;
}

string::string(const char *s) {
    if (s) {
        JSString *str = JS_NewStringCopyZ(g_cx, s);
        val = str ? STRING_TO_JSVAL(str) : JSVAL_VOID;
    } else {
        val = JSVAL_NULL;
    }
}

std::string string::to_std_string() const {
    return value::to_std_string();
}

const char *string::c_str() const {
    if (!is_string()) return "";
    JSString *str = JSVAL_TO_STRING(val);
    return JS_EncodeString(g_cx, str);
}

// ============================================================================
// object implementation
// ============================================================================

value object::get_property(const std::string &name) const {
    return get_property(name.c_str());
}

value object::get_property(const char *name) const {
    if (is_null()) return value();
    
    jsval v;
    if (JS_GetProperty(g_cx, get_object_ptr(), name, &v)) {
        return value(v);
    }
    return value();
}

void object::set_property(const std::string &name, const value &v) {
    set_property(name.c_str(), v);
}

void object::set_property(const char *name, const value &v) {
    if (is_null()) return;
    
    jsval jv = v.val;
    JS_SetProperty(g_cx, get_object_ptr(), name, &jv);
}

bool object::has_property(const std::string &name) const {
    return has_property(name.c_str());
}

bool object::has_property(const char *name) const {
    if (is_null()) return false;
    
    JSBool found;
    if (JS_HasProperty(g_cx, get_object_ptr(), name, &found)) {
        return found == JS_TRUE;
    }
    return false;
}

void object::delete_property(const std::string &name) {
    delete_property(name.c_str());
}

void object::delete_property(const char *name) {
    if (is_null()) return;
    
    jsval v;
    JS_DeleteProperty2(g_cx, get_object_ptr(), name, &v);
}

bool object::is_array() const {
    if (is_null()) return false;
    return JS_IsArrayObject(g_cx, get_object_ptr()) == JS_TRUE;
}

value object::call(const std::string &name) {
    if (is_null()) return value();
    
    jsval rval;
    if (JS_CallFunctionName(g_cx, get_object_ptr(), name.c_str(), 0, nullptr, &rval)) {
        return value(rval);
    }
    
    return value();
}

value object::call(const std::string &name, const value &arg) {
    if (is_null()) return value();
    
    jsval func;
    if (!JS_GetProperty(g_cx, get_object_ptr(), name.c_str(), &func)) {
        return value();
    }
    
    jsval argv[1] = { arg.val };
    jsval rval;
    
    if (JS_CallFunctionValue(g_cx, get_object_ptr(), func, 1, argv, &rval)) {
        return value(rval);
    }
    
    return value();
}

value object::call(const std::string &name, const value &arg1, const value &arg2) {
    if (is_null()) return value();
    
    jsval func;
    if (!JS_GetProperty(g_cx, get_object_ptr(), name.c_str(), &func)) {
        return value();
    }
    
    jsval argv[2] = { arg1.val, arg2.val };
    jsval rval;
    
    if (JS_CallFunctionValue(g_cx, get_object_ptr(), func, 2, argv, &rval)) {
        return value(rval);
    }
    
    return value();
}

object object::prototype() const {
    if (is_null()) return object();
    return object(JS_GetPrototype(g_cx, get_object_ptr()));
}

object object::parent() const {
    if (is_null()) return object();
    return object(JS_GetParent(g_cx, get_object_ptr()));
}

property_iterator object::begin() const {
    if (is_null()) return property_iterator();
    return property_iterator(g_cx, get_object_ptr());
}

property_iterator object::end() const {
    return property_iterator();
}

// ============================================================================
// array implementation
// ============================================================================

jsuint array::length() const {
    if (is_null()) return 0;
    
    jsuint len;
    if (JS_GetArrayLength(g_cx, get_object_ptr(), &len)) {
        return len;
    }
    return 0;
}

void array::set_length(jsuint len) {
    if (is_null()) return;
    JS_SetArrayLength(g_cx, get_object_ptr(), len);
}

value array::get_element(jsuint index) const {
    if (is_null()) return value();
    
    jsval v;
    if (JS_GetElement(g_cx, get_object_ptr(), index, &v)) {
        return value(v);
    }
    return value();
}

void array::set_element(jsuint index, const value &v) {
    if (is_null()) return;
    
    jsval jv = v.val;
    JS_SetElement(g_cx, get_object_ptr(), index, &jv);
}

void array::push(const value &v) {
    jsuint len = length();
    set_element(len, v);
}

// ============================================================================
// context implementation
// ============================================================================

context context::create() {
    context ctx;
    
    // Create runtime with 32MB heap
    ctx.rt = JS_NewRuntime(32 * 1024 * 1024);
    if (!ctx.rt) {
        throw exception("Failed to create JS runtime");
    }
    
    // Create context with 8KB stack
    ctx.cx = JS_NewContext(ctx.rt, 8192);
    if (!ctx.cx) {
        JS_DestroyRuntime(ctx.rt);
        throw exception("Failed to create JS context");
    }
    
    ctx.owns_context = true;
    
    // Set up error reporting
    JS_SetErrorReporter(ctx.cx, errorReporter);
    
    // Set options - use JSOPTION_JIT for 1.8.5
    JS_SetOptions(ctx.cx, JSOPTION_VAROBJFIX | JSOPTION_JIT);
    JS_SetVersion(ctx.cx, JSVERSION_LATEST);
    
    // Create global object
    JSObject *global = JS_NewObject(ctx.cx, &global_class, nullptr, nullptr);
    if (!global) {
        JS_DestroyContext(ctx.cx);
        JS_DestroyRuntime(ctx.rt);
        throw exception("Failed to create global object");
    }
    
    // Set as global for this context
    JS_SetGlobalObject(ctx.cx, global);
    
    // Initialize standard classes
    if (!JS_InitStandardClasses(ctx.cx, global)) {
        JS_DestroyContext(ctx.cx);
        JS_DestroyRuntime(ctx.rt);
        throw exception("Failed to initialize standard classes");
    }
    
    // Set global state
    g_cx = ctx.cx;
    g_rt = ctx.rt;
    g_global = global;
    
    return ctx;
}

// ============================================================================
// current_context_scope implementation
// ============================================================================

current_context_scope::current_context_scope(const context &ctx) {
    // Context is already set up in context::create()
}

current_context_scope::~current_context_scope() {
    // Cleanup happens in context destruction
}

// ============================================================================
// property_iterator implementation
// ============================================================================

property_iterator::property_iterator(JSContext *c, JSObject *o) 
    : cx(c), obj(o), iter(nullptr), at_end(false) {
    iter = JS_NewPropertyIterator(cx, obj);
    if (!iter) {
        at_end = true;
        return;
    }
    advance();
}

property_iterator::~property_iterator() {
    // Iterator is GC'd
}

void property_iterator::advance() {
    if (at_end || !iter) return;
    
    if (!JS_NextProperty(cx, iter, &current_id)) {
        at_end = true;
        return;
    }
    
    jsval idval;
    if (!JS_IdToValue(cx, current_id, &idval) || JSVAL_IS_VOID(idval)) {
        at_end = true;
    }
}

property_iterator &property_iterator::operator++() {
    advance();
    return *this;
}

bool property_iterator::operator!=(const property_iterator &other) const {
    if (at_end && other.at_end) return false;
    if (at_end != other.at_end) return true;
    return iter != other.iter;
}

bool property_iterator::operator==(const property_iterator &other) const {
    return !(*this != other);
}

string property_iterator::operator*() const {
    if (at_end) return string();
    
    jsval idval;
    if (!JS_IdToValue(cx, current_id, &idval)) {
        return string();
    }
    
    return string(idval);
}

// ============================================================================
// Global functions implementation
// ============================================================================

context current_context() {
    return context(g_cx, g_rt);
}

object global() {
    return object(g_global);
}

value evaluate(const std::string &code, const char *filename, int lineno) {
    return evaluate(code.c_str(), filename, lineno);
}

value evaluate(const char *code, const char *filename, int lineno) {
    jsval rval;
    
    if (!JS_EvaluateScript(g_cx, g_global, code, strlen(code), filename, lineno, &rval)) {
        jsval exc;
        if (JS_GetPendingException(g_cx, &exc)) {
            JS_ClearPendingException(g_cx);
            
            std::string msg = "JavaScript error";
            JSString *str = JS_ValueToString(g_cx, exc);
            if (str) {
                char *cstr = JS_EncodeString(g_cx, str);
                if (cstr) {
                    msg = cstr;
                    JS_free(g_cx, cstr);
                }
            }
            throw exception(msg);
        }
        throw exception("JavaScript evaluation failed");
    }
    
    return value(rval);
}

void gc() {
    JS_GC(g_cx);
}

object create_object() {
    JSObject *obj = JS_NewObject(g_cx, nullptr, nullptr, nullptr);
    return object(obj);
}

array create_array() {
    JSObject *arr = JS_NewArrayObject(g_cx, 0, nullptr);
    return array(arr);
}

array create_array(jsuint length) {
    JSObject *arr = JS_NewArrayObject(g_cx, length, nullptr);
    return array(arr);
}

// ============================================================================
// detail:: type conversion specializations for flusspferd types
// ============================================================================

namespace detail {

template<>
string js_to_cpp<string>(JSContext *cx, jsval v) {
    return string(v);
}

template<>
value js_to_cpp<value>(JSContext *cx, jsval v) {
    return value(v);
}

template<>
object js_to_cpp<object>(JSContext *cx, jsval v) {
    return object(v);
}

template<>
jsval cpp_to_js<string>(JSContext *cx, const string &v) {
    return v.val;
}

template<>
jsval cpp_to_js<value>(JSContext *cx, const value &v) {
    return v.val;
}

template<>
jsval cpp_to_js<object>(JSContext *cx, const object &v) {
    return v.val;
}

template<>
jsval cpp_to_js<array>(JSContext *cx, const array &v) {
    return v.val;
}

} // namespace detail

// ============================================================================
// Universal method dispatch - called when a native method is invoked from JS
// ============================================================================

JSBool universal_method_dispatch(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    // Get the callee to find the function name
    jsval callee = argv[-2];
    if (!JSVAL_IS_OBJECT(callee)) {
        JS_ReportError(cx, "Could not get callee");
        return JS_FALSE;
    }
    
    JSFunction *fun = JS_ValueToFunction(cx, callee);
    if (!fun) {
        JS_ReportError(cx, "Could not get function");
        return JS_FALSE;
    }
    
    JSString *nameStr = JS_GetFunctionId(fun);
    if (!nameStr) {
        JS_ReportError(cx, "Could not get function name");
        return JS_FALSE;
    }
    
    char *methodName = JS_EncodeString(cx, nameStr);
    if (!methodName) {
        JS_ReportError(cx, "Could not encode function name");
        return JS_FALSE;
    }
    
    // Get the object's class name
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || !cls->name) {
        JS_free(cx, methodName);
        JS_ReportError(cx, "Could not get class name");
        return JS_FALSE;
    }
    
    std::string className = cls->name;
    
    // Look up the method in our registry
    auto classIt = g_class_registries.find(className);
    if (classIt == g_class_registries.end()) {
        fprintf(stderr, "JS Warning: Class '%s' not found in registry when calling method '%s'\n", 
                className.c_str(), methodName);
        JS_free(cx, methodName);
        *rval = JSVAL_VOID;
        return JS_TRUE;
    }
    
    auto methodIt = classIt->second.methods.find(methodName);
    if (methodIt == classIt->second.methods.end()) {
        fprintf(stderr, "JS Warning: Method '%s' not found for class '%s'\n", 
                methodName, className.c_str());
        JS_free(cx, methodName);
        *rval = JSVAL_VOID;
        return JS_TRUE;
    }
    
    JS_free(cx, methodName);
    
    // Get the native pointer
    void *native_ptr = JS_GetPrivate(cx, obj);
    if (!native_ptr) {
        JS_ReportError(cx, "Native object is null");
        return JS_FALSE;
    }
    
    // Call the dispatcher
    try {
        *rval = methodIt->second(native_ptr, cx, argc, argv);
        return JS_TRUE;
    } catch (const std::exception &e) {
        JS_ReportError(cx, "%s", e.what());
        return JS_FALSE;
    }
}

} // namespace flusspferd

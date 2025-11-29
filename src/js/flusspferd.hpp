/**
 * flusspferd.hpp - SpiderMonkey 1.8.5 compatibility shim
 * 
 * This header provides a Flusspferd-compatible API using raw SpiderMonkey 1.8.5.
 * It allows existing code that was written for Flusspferd to work directly with SpiderMonkey.
 */

#ifndef KINSLAYER_FLUSSPFERD_HPP
#define KINSLAYER_FLUSSPFERD_HPP

#include <js/jsapi.h>
#include <string>
#include <stdexcept>
#include <functional>
#include <map>
#include <vector>
#include <cassert>

namespace flusspferd {

// Forward declarations
class value;
class object;
class array;
class string;
class context;
class root_object;
class root_value;
class call_context;
class property_iterator;

// Global context and runtime - initialized by JSEnvironment
extern JSContext *g_cx;
extern JSRuntime *g_rt;
extern JSObject *g_global;

// ============================================================================
// Exception class
// ============================================================================
class exception : public std::runtime_error {
public:
    explicit exception(const std::string &msg) : std::runtime_error(msg) {}
    explicit exception(const char *msg) : std::runtime_error(msg) {}
};

// ============================================================================
// value class - wraps jsval
// ============================================================================
class value {
public:
    jsval val;
    
    value() : val(JSVAL_VOID) {}
    value(jsval v) : val(v) {}
    value(int i) : val(INT_TO_JSVAL(i)) {}
    value(double d) { JS_NewNumberValue(g_cx, d, &val); }
    value(bool b) : val(BOOLEAN_TO_JSVAL(b)) {}
    value(const std::string &s);
    value(const char *s);
    value(JSObject *obj) : val(obj ? OBJECT_TO_JSVAL(obj) : JSVAL_NULL) {}
    
    bool is_undefined() const { return JSVAL_IS_VOID(val); }
    bool is_null() const { return JSVAL_IS_NULL(val); }
    bool is_undefined_or_null() const { return is_undefined() || is_null(); }
    bool is_boolean() const { return JSVAL_IS_BOOLEAN(val); }
    bool is_bool() const { return is_boolean(); }
    bool is_int() const { return JSVAL_IS_INT(val); }
    bool is_double() const { return JSVAL_IS_DOUBLE(val); }
    bool is_number() const { return JSVAL_IS_NUMBER(val); }
    bool is_string() const { return JSVAL_IS_STRING(val); }
    bool is_object() const { return !JSVAL_IS_PRIMITIVE(val); }
    bool is_function() const;
    
    int get_int() const { return JSVAL_TO_INT(val); }
    bool get_bool() const { return to_boolean(); }
    double get_double() const { return to_number(); }
    double to_number() const;
    bool to_boolean() const { return JS_ValueToBoolean(g_cx, val, nullptr) ? JSVAL_TO_BOOLEAN(val) : false; }
    std::string to_std_string() const;
    string to_string() const;  // Forward declared, implemented after string class
    object to_object() const;
    object get_object() const;
    
    operator jsval() const { return val; }
};

// ============================================================================
// string class - wraps JSString*
// ============================================================================
class string : public value {
public:
    string() : value() {}
    string(jsval v) : value(v) {}
    string(const std::string &s);
    string(const char *s);
    string(JSString *str) : value(str ? STRING_TO_JSVAL(str) : JSVAL_VOID) {}
    string(const value &v) : value(v) {}  // Conversion from value
    
    std::string to_std_string() const;
    std::string to_string() const { return to_std_string(); }  // Alias for compatibility
    const char *c_str() const;
    bool empty() const { return to_std_string().empty(); }
    size_t size() const { return to_std_string().size(); }
    size_t length() const { return size(); }
    
    // Static concat method
    static string concat(const string &a, const string &b) {
        return string(a.to_std_string() + b.to_std_string());
    }
    static string concat(const string &a, const char *b) {
        return string(a.to_std_string() + b);
    }
    
    operator std::string() const { return to_std_string(); }
};

// Implement value::to_string() now that string is defined
inline string value::to_string() const { return string(*this); }

// ============================================================================
// object class - wraps JSObject*
// ============================================================================
class object : public value {
public:
    object() : value(JSVAL_NULL) {}
    object(jsval v) : value(v) {}
    object(JSObject *obj) : value(obj) {}
    
    JSObject *get_object_ptr() const { 
        return is_null() ? nullptr : JSVAL_TO_OBJECT(val); 
    }
    
    bool is_null() const { return JSVAL_IS_NULL(val) || JSVAL_IS_VOID(val); }
    bool is_valid() const { return !is_null(); }
    bool is_array() const;
    
    // Property access
    value get_property(const std::string &name) const;
    value get_property(const char *name) const;
    void set_property(const std::string &name, const value &v);
    void set_property(const char *name, const value &v);
    bool has_property(const std::string &name) const;
    bool has_property(const char *name) const;
    void delete_property(const std::string &name);
    void delete_property(const char *name);
    
    // Method calls - use only std::string to avoid ambiguity
    value call(const std::string &name);
    value call(const std::string &name, const value &arg);
    value call(const std::string &name, const value &arg1, const value &arg2);
    
    // Prototype access
    object prototype() const;
    object parent() const;
    
    // Iteration
    property_iterator begin() const;
    property_iterator end() const;
};

// ============================================================================
// array class - wraps JSObject* (array)
// ============================================================================
class array : public object {
public:
    array() : object() {}
    array(jsval v) : object(v) {}
    array(JSObject *obj) : object(obj) {}
    array(const object &obj) : object(obj.val) {}  // Allow conversion from object
    
    jsuint length() const;
    void set_length(jsuint len);
    size_t size() const { return static_cast<size_t>(length()); }
    
    value get_element(jsuint index) const;
    void set_element(jsuint index, const value &v);
    void push(const value &v);
};

// ============================================================================
// root_value class - prevents GC of a value
// ============================================================================
class root_value {
public:
    jsval val;
    
    root_value() : val(JSVAL_VOID) {
        if (g_cx) JS_AddRoot(g_cx, &val);
    }
    
    root_value(const value &v) : val(v.val) {
        if (g_cx) JS_AddRoot(g_cx, &val);
    }
    
    ~root_value() {
        if (g_cx) JS_RemoveRoot(g_cx, &val);
    }
    
    root_value &operator=(const value &v) {
        val = v.val;
        return *this;
    }
    
    operator value() const { return value(val); }
    operator jsval() const { return val; }
};

// ============================================================================
// root_object class - prevents GC of an object
// ============================================================================
class root_object : public object {
private:
    bool rooted;
    
public:
    root_object() : object(), rooted(false) {
        if (g_cx) {
            JS_AddRoot(g_cx, &val);
            rooted = true;
        }
    }
    
    root_object(const object &o) : object(o), rooted(false) {
        if (g_cx) {
            JS_AddRoot(g_cx, &val);
            rooted = true;
        }
    }
    
    ~root_object() {
        if (rooted && g_cx) {
            JS_RemoveRoot(g_cx, &val);
        }
    }
    
    root_object &operator=(const object &o) {
        val = o.val;
        return *this;
    }
    
    root_object &operator=(JSObject *obj) {
        val = obj ? OBJECT_TO_JSVAL(obj) : JSVAL_NULL;
        return *this;
    }
};

// ============================================================================
// context class - wraps JSContext*
// ============================================================================
class context {
private:
    JSContext *cx;
    JSRuntime *rt;
    bool owns_context;
    
public:
    context() : cx(nullptr), rt(nullptr), owns_context(false) {}
    context(JSContext *c, JSRuntime *r) : cx(c), rt(r), owns_context(false) {}
    
    static context create();
    
    bool is_valid() const { return cx != nullptr; }
    JSContext *get() const { return cx; }
    JSRuntime *get_runtime() const { return rt; }
    
    void destroy() {
        if (owns_context && cx) {
            JS_DestroyContext(cx);
            cx = nullptr;
        }
        if (owns_context && rt) {
            JS_DestroyRuntime(rt);
            rt = nullptr;
        }
    }
};

// ============================================================================
// current_context_scope - RAII context management
// ============================================================================
class current_context_scope {
public:
    explicit current_context_scope(const context &ctx);
    ~current_context_scope();
    
    current_context_scope(const current_context_scope&) = delete;
    current_context_scope &operator=(const current_context_scope&) = delete;
};

// ============================================================================
// call_context - used in constructors
// ============================================================================
class call_context {
public:
    JSContext *cx;
    uintN argc;
    jsval *argv;
    jsval *rval;
    
    call_context(JSContext *c, uintN ac, jsval *av, jsval *rv)
        : cx(c), argc(ac), argv(av), rval(rv) {}
    
    value arg(unsigned int i) const {
        if (i < argc) return value(argv[i]);
        return value();
    }
};

// ============================================================================
// property_iterator - iterate object properties
// ============================================================================
class property_iterator {
private:
    JSContext *cx;
    JSObject *obj;
    JSObject *iter;
    jsid current_id;
    bool at_end;
    
    void advance();
    
public:
    property_iterator() : cx(nullptr), obj(nullptr), iter(nullptr), at_end(true) {}
    property_iterator(JSContext *c, JSObject *o);
    ~property_iterator();
    
    property_iterator &operator++();
    bool operator!=(const property_iterator &other) const;
    bool operator==(const property_iterator &other) const;
    string operator*() const;
};

// ============================================================================
// Global functions
// ============================================================================

// Get the current context
context current_context();

// Get the global object
object global();

// Evaluate JavaScript code
value evaluate(const std::string &code, const char *filename = "<eval>", int lineno = 1);
value evaluate(const char *code, const char *filename = "<eval>", int lineno = 1);

// Garbage collection
void gc();

// Create objects
object create_object();
array create_array();
array create_array(jsuint length);

// ============================================================================
// Native function support - Type conversion helpers
// ============================================================================

namespace detail {

// Convert jsval to C++ type
template<typename T>
struct from_jsval {
    static T convert(JSContext *cx, jsval v);
};

template<>
struct from_jsval<int> {
    static int convert(JSContext *cx, jsval v) {
        if (JSVAL_IS_INT(v)) return JSVAL_TO_INT(v);
        jsdouble d;
        JS_ValueToNumber(cx, v, &d);
        return static_cast<int>(d);
    }
};

template<>
struct from_jsval<unsigned int> {
    static unsigned int convert(JSContext *cx, jsval v) {
        return static_cast<unsigned int>(from_jsval<int>::convert(cx, v));
    }
};

template<>
struct from_jsval<double> {
    static double convert(JSContext *cx, jsval v) {
        jsdouble d;
        JS_ValueToNumber(cx, v, &d);
        return d;
    }
};

template<>
struct from_jsval<float> {
    static float convert(JSContext *cx, jsval v) {
        return static_cast<float>(from_jsval<double>::convert(cx, v));
    }
};

template<>
struct from_jsval<bool> {
    static bool convert(JSContext *cx, jsval v) {
        JSBool b;
        JS_ValueToBoolean(cx, v, &b);
        return b == JS_TRUE;
    }
};

template<>
struct from_jsval<std::string> {
    static std::string convert(JSContext *cx, jsval v) {
        JSString *str = JS_ValueToString(cx, v);
        if (!str) return "";
        char *cstr = JS_EncodeString(cx, str);
        if (!cstr) return "";
        std::string result(cstr);
        JS_free(cx, cstr);
        return result;
    }
};

template<>
struct from_jsval<string> {
    static string convert(JSContext *cx, jsval v) {
        return string(v);
    }
};

template<>
struct from_jsval<value> {
    static value convert(JSContext *cx, jsval v) {
        return value(v);
    }
};

template<>
struct from_jsval<object> {
    static object convert(JSContext *cx, jsval v) {
        return object(v);
    }
};

template<>
struct from_jsval<array> {
    static array convert(JSContext *cx, jsval v) {
        return array(v);
    }
};

template<>
struct from_jsval<JSObject*> {
    static JSObject* convert(JSContext *cx, jsval v) {
        if (JSVAL_IS_OBJECT(v)) {
            return JSVAL_TO_OBJECT(v);
        }
        return nullptr;
    }
};

// Convert C++ type to jsval
template<typename T>
struct to_jsval {
    static jsval convert(JSContext *cx, const T &v);
};

template<>
struct to_jsval<int> {
    static jsval convert(JSContext *cx, int v) {
        return INT_TO_JSVAL(v);
    }
};

template<>
struct to_jsval<unsigned int> {
    static jsval convert(JSContext *cx, unsigned int v) {
        return INT_TO_JSVAL(static_cast<int>(v));
    }
};

template<>
struct to_jsval<long long> {
    static jsval convert(JSContext *cx, long long v) {
        jsval rv;
        JS_NewNumberValue(cx, static_cast<double>(v), &rv);
        return rv;
    }
};

template<>
struct to_jsval<unsigned long long> {
    static jsval convert(JSContext *cx, unsigned long long v) {
        jsval rv;
        JS_NewNumberValue(cx, static_cast<double>(v), &rv);
        return rv;
    }
};

template<>
struct to_jsval<unsigned long> {
    static jsval convert(JSContext *cx, unsigned long v) {
        jsval rv;
        JS_NewNumberValue(cx, static_cast<double>(v), &rv);
        return rv;
    }
};

template<>
struct to_jsval<long> {
    static jsval convert(JSContext *cx, long v) {
        jsval rv;
        JS_NewNumberValue(cx, static_cast<double>(v), &rv);
        return rv;
    }
};

template<>
struct to_jsval<double> {
    static jsval convert(JSContext *cx, double v) {
        jsval rv;
        JS_NewNumberValue(cx, v, &rv);
        return rv;
    }
};

template<>
struct to_jsval<float> {
    static jsval convert(JSContext *cx, float v) {
        return to_jsval<double>::convert(cx, static_cast<double>(v));
    }
};

template<>
struct to_jsval<bool> {
    static jsval convert(JSContext *cx, bool v) {
        return BOOLEAN_TO_JSVAL(v);
    }
};

template<>
struct to_jsval<std::string> {
    static jsval convert(JSContext *cx, const std::string &v) {
        JSString *str = JS_NewStringCopyN(cx, v.c_str(), v.length());
        return str ? STRING_TO_JSVAL(str) : JSVAL_VOID;
    }
};

template<>
struct to_jsval<string> {
    static jsval convert(JSContext *cx, const string &v) {
        return v.val;
    }
};

template<>
struct to_jsval<value> {
    static jsval convert(JSContext *cx, const value &v) {
        return v.val;
    }
};

template<>
struct to_jsval<object> {
    static jsval convert(JSContext *cx, const object &v) {
        return v.val;
    }
};

template<>
struct to_jsval<array> {
    static jsval convert(JSContext *cx, const array &v) {
        return v.val;
    }
};

template<>
struct to_jsval<void> {
    static jsval convert(JSContext *cx) {
        return JSVAL_VOID;
    }
};

// Function pointer storage
struct FunctionStorage {
    static std::map<std::string, void*> functions;
};

// Argument extraction helper
template<typename... Args>
struct ArgExtractor;

template<>
struct ArgExtractor<> {
    static std::tuple<> extract(JSContext *cx, uintN argc, jsval *argv, uintN index) {
        return std::tuple<>();
    }
};

template<typename First, typename... Rest>
struct ArgExtractor<First, Rest...> {
    // Use decay_t to handle reference types like const std::string&
    using DecayedFirst = std::decay_t<First>;
    using DecayedRest = std::tuple<std::decay_t<Rest>...>;
    
    static std::tuple<DecayedFirst, std::decay_t<Rest>...> extract(JSContext *cx, uintN argc, jsval *argv, uintN index) {
        DecayedFirst first = (index < argc) ? from_jsval<DecayedFirst>::convert(cx, argv[index]) : DecayedFirst();
        auto rest = ArgExtractor<Rest...>::extract(cx, argc, argv, index + 1);
        return std::tuple_cat(std::make_tuple(first), rest);
    }
};

// Function caller helper - uses decayed types in tuple but calls function with original signature
template<typename Ret, typename... Args, typename Tuple, std::size_t... I>
Ret call_with_tuple(Ret (*func)(Args...), Tuple &args, std::index_sequence<I...>) {
    return func(std::get<I>(args)...);
}

} // namespace detail

// ============================================================================
// Native function wrapper registration
// ============================================================================

// Storage for function pointers (defined in cpp)
extern std::map<std::string, void*> g_function_storage;

// Template wrapper that creates a JSNative from a C++ function
// SpiderMonkey 1.8.5 uses the old-style JSNative signature
// We use argv[-2] to get the callee (function object) to look up the function name
template<typename Ret, typename... Args>
JSBool native_function_wrapper(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    // argv[-2] is the callee in SpiderMonkey 1.8.5
    jsval callee = argv[-2];
    if (!JSVAL_IS_OBJECT(callee)) {
        JS_ReportError(cx, "Could not get callee");
        return JS_FALSE;
    }
    
    JSObject *calleeObj = JSVAL_TO_OBJECT(callee);
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
    
    char *name = JS_EncodeString(cx, nameStr);
    if (!name) {
        JS_ReportError(cx, "Could not encode function name");
        return JS_FALSE;
    }
    
    auto it = g_function_storage.find(name);
    JS_free(cx, name);
    
    if (it == g_function_storage.end()) {
        JS_ReportError(cx, "Function not found in registry");
        return JS_FALSE;
    }
    
    auto func = reinterpret_cast<Ret (*)(Args...)>(it->second);
    
    try {
        auto args = detail::ArgExtractor<Args...>::extract(cx, argc, argv, 0);
        
        if constexpr (std::is_same_v<Ret, void>) {
            detail::call_with_tuple(func, args, std::index_sequence_for<Args...>{});
            *rval = JSVAL_VOID;
        } else {
            Ret result = detail::call_with_tuple(func, args, std::index_sequence_for<Args...>{});
            *rval = detail::to_jsval<Ret>::convert(cx, result);
        }
        return JS_TRUE;
    } catch (const std::exception &e) {
        JS_ReportError(cx, "%s", e.what());
        return JS_FALSE;
    }
}

// Register a native function
template<typename Ret, typename... Args>
void create_native_function(object obj, const char *name, Ret (*func)(Args...)) {
    // Store the function pointer
    g_function_storage[name] = reinterpret_cast<void*>(func);
    
    // Create the JS function
    JSFunction *jsfun = JS_DefineFunction(g_cx, obj.get_object_ptr(), name, 
        native_function_wrapper<Ret, Args...>, sizeof...(Args), 0);
    
    if (!jsfun) {
        throw exception(std::string("Failed to create native function: ") + name);
    }
}

// ============================================================================
// Native class support
// ============================================================================

namespace Impl {
    // Get raw JSContext from context
    inline JSContext *get_context(const context &ctx) {
        return ctx.get();
    }
}

// Forward declaration of native class template
template<typename T>
class native_object_base;

// Check if object is native wrapper
template<typename T>
bool is_native(const object &obj);

// Get native object from wrapper
template<typename T>
T &get_native(const object &obj);

// Create native object wrapper
template<typename T, typename... Args>
value create_native_object(const object &proto, Args&&... args);

// Load/register a class
template<typename T>
void load_class();

// ============================================================================
// Native class registry - stores JSClass definitions and private data
// ============================================================================

struct NativeClassInfo {
    JSClass *jsclass;
    void *constructor;
    // Add method/property info as needed
};

extern std::map<std::string, NativeClassInfo> g_class_registry;

// ============================================================================
// Base class for native-wrapped objects
// ============================================================================

template<typename Derived>
class native_object_base {
protected:
    object self_;
    
public:
    // Define base_type so derived classes can use : base_type(self) in constructors
    typedef native_object_base<Derived> base_type;
    
    native_object_base(const object &self) : self_(self) {}
    
    object &get_object() { return self_; }
    const object &get_object() const { return self_; }
    
    value get_property(const std::string &name) const {
        return self_.get_property(name);
    }
    
    void set_property(const std::string &name, const value &v) {
        self_.set_property(name, v);
    }
};

} // namespace flusspferd

// Include the class description macro support
#include "flusspferd_class_macros.hpp"

#endif // KINSLAYER_FLUSSPFERD_HPP


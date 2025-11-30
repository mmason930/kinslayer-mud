/**
 * flusspferd.hpp - SpiderMonkey 131 compatibility shim
 * 
 * This header provides a Flusspferd-compatible API using SpiderMonkey 131.
 * It allows existing code that was written for Flusspferd to work with modern SpiderMonkey.
 */

#ifndef KINSLAYER_FLUSSPFERD_HPP
#define KINSLAYER_FLUSSPFERD_HPP

#include <jsapi.h>
#include <js/Initialization.h>
#include <js/Conversions.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Object.h>
#include <js/Class.h>
#include <js/ValueArray.h>
#include <string>
#include <stdexcept>
#include <functional>
#include <map>
#include <vector>
#include <cassert>
#include <memory>

// ============================================================================
// SpiderMonkey 131 compatibility typedefs and macros
// These provide backward compatibility with SpiderMonkey 1.8.x API
// ============================================================================

// Type aliases for old names
typedef JS::Value jsval;
typedef bool JSBool;
typedef unsigned uintN;
typedef double jsdouble;

// Boolean constants
#ifndef JS_TRUE
#define JS_TRUE true
#endif
#ifndef JS_FALSE
#define JS_FALSE false
#endif

// Value conversion macros (SpiderMonkey 1.8.x -> 131)
#define JSVAL_IS_INT(v) ((v).isInt32())
#define JSVAL_TO_INT(v) ((v).toInt32())
#define INT_TO_JSVAL(i) (JS::Int32Value(i))
#define JSVAL_IS_BOOLEAN(v) ((v).isBoolean())
#define JSVAL_TO_BOOLEAN(v) ((v).toBoolean())
#define BOOLEAN_TO_JSVAL(b) (JS::BooleanValue(b))
#define JSVAL_IS_DOUBLE(v) ((v).isDouble())
#define JSVAL_TO_DOUBLE(v) ((v).toDouble())
#define DOUBLE_TO_JSVAL(d) (JS::DoubleValue(d))
#define JSVAL_IS_STRING(v) ((v).isString())
#define JSVAL_TO_STRING(v) ((v).toString())
#define STRING_TO_JSVAL(s) (JS::StringValue(s))
#define JSVAL_IS_OBJECT(v) ((v).isObject())
#define JSVAL_TO_OBJECT(v) (&(v).toObject())
#define OBJECT_TO_JSVAL(o) (JS::ObjectValue(*(o)))
#define JSVAL_IS_NULL(v) ((v).isNull())
#define JSVAL_IS_VOID(v) ((v).isUndefined())
#define JSVAL_VOID (JS::UndefinedValue())
#define JSVAL_NULL (JS::NullValue())

// Compatibility functions for old SpiderMonkey API
inline JSBool JS_ValueToBoolean(JSContext *cx, JS::Value v, JSBool *bp) {
    *bp = JS::ToBoolean(JS::HandleValue::fromMarkedLocation(&v));
    return true;
}

inline JSBool JS_ValueToNumber(JSContext *cx, JS::Value v, double *dp) {
    JS::RootedValue rv(cx, v);
    return JS::ToNumber(cx, rv, dp);
}

inline JSString* JS_ValueToString(JSContext *cx, JS::Value v) {
    JS::RootedValue rv(cx, v);
    return JS::ToString(cx, rv);
}

inline char* JS_EncodeString(JSContext *cx, JSString *str) {
    JS::RootedString rstr(cx, str);
    JS::UniqueChars chars = JS_EncodeStringToUTF8(cx, rstr);
    if (!chars) return nullptr;
    // Note: This leaks memory! Caller must free with JS_free
    return strdup(chars.get());
}

inline void JS_NewNumberValue(JSContext *cx, double d, JS::Value *vp) {
    *vp = JS::DoubleValue(d);
}

// JS_GetPrivate takes only the object in SpiderMonkey 131
// But old code passed (cx, obj) - we ignore the cx
inline void* JS_GetPrivate_compat(JSContext *cx, JSObject *obj) {
    JS::Value v = JS::GetReservedSlot(obj, 0);
    return v.isUndefined() ? nullptr : v.toPrivate();
}
#define JS_GetPrivate(cx, obj) JS_GetPrivate_compat(cx, obj)

// JS_GET_CLASS in old SpiderMonkey (note: do NOT define GET_CLASS as it conflicts with utils.h)
inline const JSClass* JS_GET_CLASS(JSContext *cx, JSObject *obj) {
    return JS::GetClass(obj);
}

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

// Global context - initialized by JSEnvironment
extern JSContext *g_cx;
extern JS::PersistentRootedObject *g_global;

// ============================================================================
// Wrapper functions to avoid template lookup issues
// In SpiderMonkey 131, private data uses reserved slots with PrivateValue
// ============================================================================

// Private data is stored in reserved slot 0 using PrivateValue
inline void* sm_get_private(JSObject *obj) {
    JS::Value v = JS::GetReservedSlot(obj, 0);
    return v.isUndefined() ? nullptr : v.toPrivate();
}

inline void sm_set_private(JSObject *obj, void *data) {
    JS::SetReservedSlot(obj, 0, JS::PrivateValue(data));
}

// Reserved slots for method/class names start at slot 1 (slot 0 is private data)
inline JS::Value sm_get_reserved_slot(JSObject *obj, uint32_t slot) {
    return JS::GetReservedSlot(obj, slot);
}

inline void sm_set_reserved_slot(JSObject *obj, uint32_t slot, const JS::Value &val) {
    JS::SetReservedSlot(obj, slot, val);
}

// ============================================================================
// Exception class
// ============================================================================
class exception : public std::runtime_error {
public:
    explicit exception(const std::string &msg) : std::runtime_error(msg) {}
    explicit exception(const char *msg) : std::runtime_error(msg) {}
};

// ============================================================================
// value class - wraps JS::Value
// ============================================================================
class value {
public:
    JS::Value val;
    
    value() : val(JS::UndefinedValue()) {}
    value(const JS::Value &v) : val(v) {}
    value(int i) : val(JS::Int32Value(i)) {}
    value(long l) : val(JS::DoubleValue(static_cast<double>(l))) {}
    value(unsigned long l) : val(JS::DoubleValue(static_cast<double>(l))) {}
    value(long long l) : val(JS::DoubleValue(static_cast<double>(l))) {}
    value(unsigned long long l) : val(JS::DoubleValue(static_cast<double>(l))) {}
    value(double d) : val(JS::DoubleValue(d)) {}
    value(bool b) : val(JS::BooleanValue(b)) {}
    value(const std::string &s);
    value(const char *s);
    value(JSObject *obj) : val(obj ? JS::ObjectValue(*obj) : JS::NullValue()) {}
    
    bool is_undefined() const { return val.isUndefined(); }
    bool is_null() const { return val.isNull(); }
    bool is_undefined_or_null() const { return val.isNullOrUndefined(); }
    bool is_boolean() const { return val.isBoolean(); }
    bool is_bool() const { return is_boolean(); }
    bool is_int() const { return val.isInt32(); }
    bool is_double() const { return val.isDouble(); }
    bool is_number() const { return val.isNumber(); }
    bool is_string() const { return val.isString(); }
    bool is_object() const { return val.isObject(); }
    bool is_function() const;
    
    int get_int() const { return val.toInt32(); }
    bool get_bool() const { return to_boolean(); }
    double get_double() const { return to_number(); }
    double to_number() const;
    bool to_boolean() const { return JS::ToBoolean(JS::HandleValue::fromMarkedLocation(&val)); }
    std::string to_std_string() const;
    string to_string() const;  // Forward declared, implemented after string class
    object to_object() const;
    object get_object() const;
    
    operator JS::Value() const { return val; }
    operator JS::HandleValue() const { return JS::HandleValue::fromMarkedLocation(&val); }
};

// ============================================================================
// string class - wraps JSString*
// ============================================================================
class string : public value {
public:
    string() : value() {}
    string(const JS::Value &v) : value(v) {}
    string(const std::string &s);
    string(const char *s);
    string(JSString *str);
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
};

// ============================================================================
// object class - wraps JSObject*
// ============================================================================
class object : public value {
public:
    object() : value(JS::NullValue()) {}
    object(const JS::Value &v) : value(v) {}
    object(JSObject *obj) : value(obj) {}
    
    bool is_null() const { return val.isNullOrUndefined(); }
    bool is_valid() const { return !is_null(); }
    bool is_array() const;
    
    JSObject *get_object_ptr() const { 
        return val.isObject() ? &val.toObject() : nullptr; 
    }
    
    // Property access
    value get_property(const std::string &name) const;
    value get_property(const char *name) const;
    void set_property(const std::string &name, const value &v);
    void set_property(const char *name, const value &v);
    bool has_property(const std::string &name) const;
    bool has_property(const char *name) const;
    void delete_property(const std::string &name);
    void delete_property(const char *name);
    
    // Method calls
    value call(const std::string &name);
    value call(const std::string &name, const value &arg);
    value call(const std::string &name, const value &arg1, const value &arg2);
    
    // Prototype and parent
    object prototype() const;
    object parent() const;
    
    // Iteration support
    property_iterator begin() const;
    property_iterator end() const;
};

// ============================================================================
// array class - wraps JSObject* (array)
// ============================================================================
class array : public object {
public:
    array() : object() {}
    array(const JS::Value &v) : object(v) {}
    array(JSObject *obj) : object(obj) {}
    array(const object &o) : object(o) {}
    
    uint32_t length() const;
    uint32_t size() const { return length(); }
    void set_length(uint32_t len);
    
    value get_element(uint32_t index) const;
    void set_element(uint32_t index, const value &v);
    void push(const value &v);
};

// ============================================================================
// root_value - GC-rooted value
// ============================================================================
class root_value {
private:
    std::unique_ptr<JS::PersistentRootedValue> rooted;
public:
    root_value();
    root_value(const value &v);
    ~root_value();
    
    root_value &operator=(const value &v);
    operator value() const;
    value get() const;
    
    bool is_undefined() const { return get().is_undefined(); }
    bool is_null() const { return get().is_null(); }
    bool is_object() const { return get().is_object(); }
};

// ============================================================================
// root_object - GC-rooted object
// ============================================================================
class root_object {
private:
    std::unique_ptr<JS::PersistentRootedObject> rooted;
public:
    root_object();
    root_object(const object &o);
    ~root_object();
    
    root_object &operator=(const object &o);
    operator object() const;
    object get() const;
    
    // Forward object methods
    value get_property(const std::string &name) const { return get().get_property(name); }
    void set_property(const std::string &name, const value &v) { /* need mutable access */ }
    bool has_property(const std::string &name) const { return get().has_property(name); }
    value call(const std::string &name) { return get().call(name); }
    value call(const std::string &name, const value &arg) { return get().call(name, arg); }
    bool is_null() const { return get().is_null(); }
    bool is_undefined() const { return get().is_null(); }
    object prototype() const { return get().prototype(); }
    object parent() const { return get().parent(); }
};

// ============================================================================
// context class - wraps JSContext*
// ============================================================================
class context {
private:
    JSContext *cx;
    bool owns_context;
    
public:
    context() : cx(nullptr), owns_context(false) {}
    context(JSContext *c) : cx(c), owns_context(false) {}
    
    static context create();
    
    bool is_valid() const { return cx != nullptr; }
    JSContext *get() const { return cx; }
    
    void destroy();
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
// call_context - used in native function calls
// ============================================================================
class call_context {
public:
    JSContext *cx;
    JS::CallArgs args;
    
    call_context(JSContext *c, const JS::CallArgs &a) : cx(c), args(a) {}
    
    unsigned arg_count() const { return args.length(); }
    value arg(unsigned i) const { return value(args.get(i)); }
    object this_object() const { return object(args.thisv().toObjectOrNull()); }
    void set_return(const value &v) { args.rval().set(v.val); }
};

// ============================================================================
// property_iterator - iterate over object properties
// ============================================================================
class property_iterator {
private:
    JSContext *cx;
    JS::PersistentRootedObject *obj;
    JS::PersistentRooted<JS::IdVector> *ids;
    size_t current_index;
    bool at_end;
    
public:
    property_iterator();
    property_iterator(JSContext *c, JSObject *o);
    property_iterator(const property_iterator &) = delete;
    property_iterator &operator=(const property_iterator &) = delete;
    property_iterator(property_iterator &&other) noexcept;
    property_iterator &operator=(property_iterator &&other) noexcept;
    ~property_iterator();
    
    property_iterator &operator++();
    bool operator!=(const property_iterator &other) const;
    bool operator==(const property_iterator &other) const;
    string operator*() const;
};

// ============================================================================
// Global functions
// ============================================================================

context current_context();
object global();

value evaluate(const std::string &code, const char *filename = "<eval>", int lineno = 1);
value evaluate(const char *code, const char *filename = "<eval>", int lineno = 1);

void gc();

object create_object();
array create_array();
array create_array(uint32_t length);

// ============================================================================
// Native class info storage
// ============================================================================
struct NativeClassInfo {
    const JSClass *jsclass;
    JSObject *prototype;
    std::map<std::string, std::function<bool(void*, JSContext*, unsigned, JS::Value*)>> methods;
    std::map<std::string, std::function<bool(void*, JSContext*, JS::MutableHandleValue)>> getters;
    std::map<std::string, std::function<bool(void*, JSContext*, JS::HandleValue)>> setters;
};

extern std::map<std::string, NativeClassInfo> g_class_registry;
extern std::map<std::string, void*> g_function_storage;

// Alias for backward compatibility with code using plural form
#define g_class_registries g_class_registry

// ============================================================================
// Native function creation
// ============================================================================

template<typename Func>
object create_native_function(const std::string &name, Func func);

// Overload that defines function on a specific object (Flusspferd compatible)
template<typename Func>
void create_native_function(object &target, const std::string &name, Func func);

// ============================================================================
// native_object_base - base class for C++ objects exposed to JS
// ============================================================================
template<typename Derived>
class native_object_base {
public:
    typedef native_object_base<Derived> base_type;
    
protected:
    call_context *current_call_context;
    object js_object_;  // The JavaScript object wrapper
    
public:
    native_object_base() : current_call_context(nullptr), js_object_() {}
    
    // Constructor that takes the JS object wrapper (for Flusspferd compatibility)
    explicit native_object_base(const object &self) 
        : current_call_context(nullptr), js_object_(self) {}
    
    virtual ~native_object_base() {}
    
    // Get the associated JavaScript object
    object &js_object() { return js_object_; }
    const object &js_object() const { return js_object_; }
    
    // Property access helpers (inline implementation for template)
    value get_property(const std::string &name) const {
        if (js_object_.is_null()) return value();
        return js_object_.get_property(name);
    }
    
    void set_property(const std::string &name, const value &v) {
        if (js_object_.is_null()) return;
        js_object_.set_property(name, v);
    }
    
    void set_call_context(call_context *ctx) { current_call_context = ctx; }
    call_context *get_call_context() const { return current_call_context; }
};

// ============================================================================
// Type conversion helpers
// ============================================================================
namespace detail {

// Convert C++ types to JS::Value
template<typename T>
struct to_jsval {
    static JS::Value convert(JSContext *cx, const T &v);
};

// Specializations
template<>
struct to_jsval<int> {
    static JS::Value convert(JSContext *cx, const int &v) {
        return JS::Int32Value(v);
    }
};

template<>
struct to_jsval<unsigned int> {
    static JS::Value convert(JSContext *cx, const unsigned int &v) {
        return JS::Int32Value(static_cast<int>(v));
    }
};

template<>
struct to_jsval<long> {
    static JS::Value convert(JSContext *cx, const long &v) {
        return JS::Int32Value(static_cast<int>(v));
    }
};

template<>
struct to_jsval<unsigned long> {
    static JS::Value convert(JSContext *cx, const unsigned long &v) {
        return JS::Int32Value(static_cast<int>(v));
    }
};

template<>
struct to_jsval<double> {
    static JS::Value convert(JSContext *cx, const double &v) {
        return JS::DoubleValue(v);
    }
};

template<>
struct to_jsval<float> {
    static JS::Value convert(JSContext *cx, const float &v) {
        return JS::DoubleValue(static_cast<double>(v));
    }
};

template<>
struct to_jsval<bool> {
    static JS::Value convert(JSContext *cx, const bool &v) {
        return JS::BooleanValue(v);
    }
};

template<>
struct to_jsval<std::string> {
    static JS::Value convert(JSContext *cx, const std::string &v);
};

template<>
struct to_jsval<const char*> {
    static JS::Value convert(JSContext *cx, const char* const &v);
};

template<>
struct to_jsval<value> {
    static JS::Value convert(JSContext *cx, const value &v) {
        return v.val;
    }
};

template<>
struct to_jsval<object> {
    static JS::Value convert(JSContext *cx, const object &v) {
        return v.val;
    }
};

template<>
struct to_jsval<array> {
    static JS::Value convert(JSContext *cx, const array &v) {
        return v.val;
    }
};

template<>
struct to_jsval<string> {
    static JS::Value convert(JSContext *cx, const string &v) {
        return v.val;
    }
};

// Convert JS::Value to C++ types
template<typename T>
struct from_jsval {
    static T convert(JSContext *cx, const JS::Value &v);
};

template<>
struct from_jsval<int> {
    static int convert(JSContext *cx, const JS::Value &v) {
        return v.isInt32() ? v.toInt32() : static_cast<int>(v.toNumber());
    }
};

template<>
struct from_jsval<unsigned int> {
    static unsigned int convert(JSContext *cx, const JS::Value &v) {
        return static_cast<unsigned int>(v.isInt32() ? v.toInt32() : v.toNumber());
    }
};

template<>
struct from_jsval<double> {
    static double convert(JSContext *cx, const JS::Value &v) {
        return v.toNumber();
    }
};

template<>
struct from_jsval<float> {
    static float convert(JSContext *cx, const JS::Value &v) {
        return static_cast<float>(v.toNumber());
    }
};

template<>
struct from_jsval<bool> {
    static bool convert(JSContext *cx, const JS::Value &v) {
        return JS::ToBoolean(JS::HandleValue::fromMarkedLocation(&v));
    }
};

template<>
struct from_jsval<std::string> {
    static std::string convert(JSContext *cx, const JS::Value &v);
};

template<>
struct from_jsval<string> {
    static string convert(JSContext *cx, const JS::Value &v) {
        return string(v);
    }
};

template<>
struct from_jsval<value> {
    static value convert(JSContext *cx, const JS::Value &v) {
        return value(v);
    }
};

template<>
struct from_jsval<object> {
    static object convert(JSContext *cx, const JS::Value &v) {
        return object(v);
    }
};

template<>
struct from_jsval<array> {
    static array convert(JSContext *cx, const JS::Value &v) {
        return array(v);
    }
};

template<>
struct from_jsval<JSObject*> {
    static JSObject* convert(JSContext *cx, const JS::Value &v) {
        return v.isObject() ? &v.toObject() : nullptr;
    }
};

} // namespace detail

// ============================================================================
// Argument extraction for native functions
// ============================================================================
template<typename... Args>
struct ArgExtractor;

template<>
struct ArgExtractor<> {
    static std::tuple<> extract(JSContext *cx, const JS::CallArgs &args, unsigned index) {
        return std::tuple<>();
    }
};

template<typename First, typename... Rest>
struct ArgExtractor<First, Rest...> {
    static std::tuple<std::decay_t<First>, std::decay_t<Rest>...> 
    extract(JSContext *cx, const JS::CallArgs &args, unsigned index) {
        std::decay_t<First> first{};
        if (index < args.length()) {
            first = detail::from_jsval<std::decay_t<First>>::convert(cx, args[index]);
        }
        auto rest = ArgExtractor<Rest...>::extract(cx, args, index + 1);
        return std::tuple_cat(std::make_tuple(first), rest);
    }
};

// ============================================================================
// Native function wrapper - converts C++ functions to SpiderMonkey natives
// ============================================================================
template<typename Ret, typename... Args, typename Tuple, std::size_t... Is>
Ret call_with_tuple_impl(Ret (*func)(Args...), Tuple &args, std::index_sequence<Is...>) {
    return func(std::get<Is>(args)...);
}

template<typename Ret, typename... Args, typename Tuple>
Ret call_with_tuple(Ret (*func)(Args...), Tuple &args) {
    return call_with_tuple_impl(func, args, std::index_sequence_for<Args...>{});
}

// Native function wrapper for void return
template<typename... Args>
bool native_function_wrapper_void(JSContext *cx, unsigned argc, JS::Value *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    // Get function pointer from reserved slot or global storage
    JSObject *callee = &args.callee();
    JS::Value funcPtrVal = sm_get_reserved_slot(callee, 0);
    
    if (!funcPtrVal.isDouble()) {
        JS_ReportErrorASCII(cx, "Invalid function pointer");
        return false;
    }
    
    auto funcPtr = reinterpret_cast<void(*)(Args...)>(
        static_cast<uintptr_t>(funcPtrVal.toDouble())
    );
    
    auto extracted = ArgExtractor<Args...>::extract(cx, args, 0);
    call_with_tuple(funcPtr, extracted);
    
    args.rval().setUndefined();
    return true;
}

// Native function wrapper for non-void return
template<typename Ret, typename... Args>
bool native_function_wrapper(JSContext *cx, unsigned argc, JS::Value *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    // Get function pointer from reserved slot
    JSObject *callee = &args.callee();
    JS::Value funcPtrVal = sm_get_reserved_slot(callee, 0);
    
    if (!funcPtrVal.isDouble()) {
        JS_ReportErrorASCII(cx, "Invalid function pointer");
        return false;
    }
    
    auto funcPtr = reinterpret_cast<Ret(*)(Args...)>(
        static_cast<uintptr_t>(funcPtrVal.toDouble())
    );
    
    auto extracted = ArgExtractor<Args...>::extract(cx, args, 0);
    Ret result = call_with_tuple(funcPtr, extracted);
    
    args.rval().set(detail::to_jsval<Ret>::convert(cx, result));
    return true;
}

// ============================================================================
// create_native_function implementations
// ============================================================================

// Implementation helper: store function pointer in a global map
extern std::map<std::string, void*> g_function_storage;

template<typename Ret, typename... Args>
object create_native_function(const std::string &name, Ret(*func)(Args...)) {
    if (!g_cx || !g_global) return object();
    
    // Store the function pointer
    g_function_storage[name] = reinterpret_cast<void*>(func);
    
    // Select the appropriate wrapper based on return type
    JSNative wrapper;
    if constexpr (std::is_void_v<Ret>) {
        wrapper = native_function_wrapper_void<Args...>;
    } else {
        wrapper = native_function_wrapper<Ret, Args...>;
    }
    
    // Create the function
    JSFunction *jsFunc = JS_NewFunction(g_cx, wrapper, sizeof...(Args), 0, name.c_str());
    if (!jsFunc) return object();
    
    JSObject *funcObj = JS_GetFunctionObject(jsFunc);
    
    // Store the function pointer in reserved slot
    sm_set_reserved_slot(funcObj, 0, JS::DoubleValue(static_cast<double>(
        reinterpret_cast<uintptr_t>(func)
    )));
    
    return object(funcObj);
}

// 3-argument version that defines the function on a target object
template<typename Ret, typename... Args>
void create_native_function(object &target, const std::string &name, Ret(*func)(Args...)) {
    object funcObj = create_native_function(name, func);
    if (!funcObj.is_null()) {
        target.set_property(name, funcObj);
    }
}

// ============================================================================
// Helper to get context from context object (for compatibility)
// ============================================================================
namespace Impl {
    inline JSContext *get_context(const context &ctx) {
        return ctx.get();
    }
}

// ============================================================================
// is_native and get_native - check if object is native and get the C++ object
// ============================================================================
template<typename T>
bool is_native(const object &obj);

template<typename T>
T &get_native(const object &obj);

template<typename T, typename... Args>
object create_native_object(Args&&... args);

// ============================================================================
// load_class - register a native class with the JS engine
// ============================================================================
template<typename T>
void load_class();

} // namespace flusspferd

// Include class registration macros
#include "flusspferd_class_macros.hpp"

#endif // KINSLAYER_FLUSSPFERD_HPP

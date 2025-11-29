/**
 * flusspferd_class_macros.hpp - Class description macro support
 * 
 * This header provides the FLUSSPFERD_CLASS_DESCRIPTION macro which allows
 * defining JavaScript-exposed classes with methods and properties.
 */

#ifndef KINSLAYER_FLUSSPFERD_CLASS_MACROS_HPP
#define KINSLAYER_FLUSSPFERD_CLASS_MACROS_HPP

#include <js/jsapi.h>
#include <vector>
#include <functional>
#include <cstring>
#include <type_traits>
#include <map>
#include <stdexcept>

namespace flusspferd {

// ============================================================================
// Forward declarations
// ============================================================================
class value;
class object;
class string;
class array;

// ============================================================================
// Method dispatcher function type
// A function that takes (native_ptr, cx, argc, argv) and returns jsval
// ============================================================================
using MethodDispatcher = std::function<jsval(void*, JSContext*, uintN, jsval*)>;
using PropertyGetter = std::function<jsval(void*, JSContext*)>;
using PropertySetter = std::function<void(void*, JSContext*, jsval)>;

// ============================================================================
// Per-class registry for methods and properties
// ============================================================================
struct ClassRegistry {
    std::map<std::string, MethodDispatcher> methods;
    std::map<std::string, PropertyGetter> getters;
    std::map<std::string, PropertySetter> setters;
};

// Global registry indexed by class name
extern std::map<std::string, ClassRegistry> g_class_registries;

// ============================================================================
// Template helpers for type conversion
// ============================================================================

namespace detail {

// Convert jsval to C++ type
template<typename T>
T js_to_cpp(JSContext *cx, jsval v);

template<>
inline int js_to_cpp<int>(JSContext *cx, jsval v) {
    if (JSVAL_IS_INT(v)) return JSVAL_TO_INT(v);
    jsdouble d;
    JS_ValueToNumber(cx, v, &d);
    return static_cast<int>(d);
}

template<>
inline bool js_to_cpp<bool>(JSContext *cx, jsval v) {
    JSBool b;
    JS_ValueToBoolean(cx, v, &b);
    return b == JS_TRUE;
}

template<>
inline double js_to_cpp<double>(JSContext *cx, jsval v) {
    jsdouble d;
    JS_ValueToNumber(cx, v, &d);
    return d;
}

template<>
inline float js_to_cpp<float>(JSContext *cx, jsval v) {
    return static_cast<float>(js_to_cpp<double>(cx, v));
}

template<>
inline std::string js_to_cpp<std::string>(JSContext *cx, jsval v) {
    JSString *str = JS_ValueToString(cx, v);
    if (!str) return "";
    char *cstr = JS_EncodeString(cx, str);
    if (!cstr) return "";
    std::string result(cstr);
    JS_free(cx, cstr);
    return result;
}

template<>
inline const char* js_to_cpp<const char*>(JSContext *cx, jsval v) {
    JSString *str = JS_ValueToString(cx, v);
    if (!str) return "";
    return JS_EncodeString(cx, str);
}

// Forward declare - implemented after string/value/object are defined
template<>
string js_to_cpp<string>(JSContext *cx, jsval v);

template<>
value js_to_cpp<value>(JSContext *cx, jsval v);

template<>
object js_to_cpp<object>(JSContext *cx, jsval v);

// Convert C++ type to jsval
template<typename T>
jsval cpp_to_js(JSContext *cx, const T &v);

template<>
inline jsval cpp_to_js<int>(JSContext *cx, const int &v) {
    return INT_TO_JSVAL(v);
}

template<>
inline jsval cpp_to_js<bool>(JSContext *cx, const bool &v) {
    return BOOLEAN_TO_JSVAL(v);
}

template<>
inline jsval cpp_to_js<double>(JSContext *cx, const double &v) {
    jsval rv;
    JS_NewNumberValue(cx, v, &rv);
    return rv;
}

template<>
inline jsval cpp_to_js<float>(JSContext *cx, const float &v) {
    return cpp_to_js<double>(cx, static_cast<double>(v));
}

template<>
inline jsval cpp_to_js<std::string>(JSContext *cx, const std::string &v) {
    JSString *str = JS_NewStringCopyN(cx, v.c_str(), v.length());
    return str ? STRING_TO_JSVAL(str) : JSVAL_VOID;
}

// Forward declare - implemented after string/value/object are defined
template<>
jsval cpp_to_js<string>(JSContext *cx, const string &v);

template<>
jsval cpp_to_js<value>(JSContext *cx, const value &v);

template<>
jsval cpp_to_js<object>(JSContext *cx, const object &v);

template<>
jsval cpp_to_js<array>(JSContext *cx, const array &v);

} // namespace detail

// ============================================================================
// Universal method wrapper - dispatches based on class/method name
// ============================================================================
JSBool universal_method_dispatch(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

// ============================================================================
// ClassTraits - stores class metadata
// ============================================================================
template<typename T>
struct ClassTraits {
    static JSClass jsclass;
    static const char *class_name;
    static bool registered;
    static JSObject *prototype;
    static JSObject *constructor_obj;
    
    static void ensure_registered();
    static void register_class() { ensure_registered(); }
    static void define_methods_on_prototype();
    
    static void add_method(const char *name, MethodDispatcher dispatcher);
    static void add_getter(const char *name, PropertyGetter getter);
    static void add_setter(const char *name, PropertySetter setter);
    
    // Property hooks
    static JSBool getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    static JSBool setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    
    // Constructor stub - objects are created from C++
    static JSBool construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    
    // Destructor
    static void finalize(JSContext *cx, JSObject *obj) {
        T *ptr = static_cast<T*>(JS_GetPrivate(cx, obj));
        if (ptr) {
            delete ptr;
            JS_SetPrivate(cx, obj, nullptr);
        }
    }
};

// Static member definitions
template<typename T>
const char *ClassTraits<T>::class_name = nullptr;

template<typename T>
bool ClassTraits<T>::registered = false;

template<typename T>
JSObject *ClassTraits<T>::prototype = nullptr;

template<typename T>
JSObject *ClassTraits<T>::constructor_obj = nullptr;

template<typename T>
JSClass ClassTraits<T>::jsclass = {
    nullptr,  // name - set during registration
    JSCLASS_HAS_PRIVATE,  // Simplified - methods defined directly on prototype
    JS_PropertyStub,
    JS_PropertyStub,
    ClassTraits<T>::getProperty,
    ClassTraits<T>::setProperty,
    JS_EnumerateStub,
    JS_ResolveStub,  // Use standard resolve - methods on prototype
    JS_ConvertStub,
    ClassTraits<T>::finalize,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

// ============================================================================
// Forward declarations needed for implementation
// ============================================================================
extern JSContext *g_cx;
extern JSObject *g_global;

// ============================================================================
// ClassTraits<T>::construct() - Constructor stub
// ============================================================================

template<typename T>
JSBool ClassTraits<T>::construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    // Native objects should be created from C++, not from JavaScript
    JS_ReportError(cx, "%s objects must be created from C++ code", class_name ? class_name : "Native");
    return JS_FALSE;
}

// ============================================================================
// ClassTraits<T>::ensure_registered() implementation
// ============================================================================

template<typename T>
void ClassTraits<T>::ensure_registered() {
    if (registered) return;
    registered = true;
    
    // Set the class name if not already set
    if (!jsclass.name && class_name) {
        // We need a persistent copy of the name
        jsclass.name = class_name;
    }
    
    if (!jsclass.name) {
        jsclass.name = "UnknownClass";
    }
    
    // Initialize the prototype object with a constructor
    // This makes the class available globally (e.g., JSCharacter.prototype)
    prototype = JS_InitClass(g_cx, g_global, 
        nullptr,   // parent proto
        &jsclass,  // JSClass
        construct, // constructor - makes class available on global
        0,         // nargs
        nullptr,   // JSPropertySpec - we'll add properties dynamically
        nullptr,   // JSFunctionSpec - we'll add methods dynamically
        nullptr,   // static props
        nullptr);  // static funcs
    
    if (!prototype) {
        fprintf(stderr, "Failed to initialize JS class: %s\n", jsclass.name);
        return;
    }
    
    // Store the constructor object for reference
    jsval ctor_val;
    if (JS_GetProperty(g_cx, g_global, jsclass.name, &ctor_val) && JSVAL_IS_OBJECT(ctor_val)) {
        constructor_obj = JSVAL_TO_OBJECT(ctor_val);
    }
    
    // Now define all registered methods on the prototype
    define_methods_on_prototype();
}

// ============================================================================
// ClassTraits<T>::define_methods_on_prototype() implementation
// ============================================================================

template<typename T>
void ClassTraits<T>::define_methods_on_prototype() {
    if (!prototype || !class_name) return;
    
    auto reg_it = g_class_registries.find(class_name);
    if (reg_it == g_class_registries.end()) return;
    
    // Define all methods on the prototype
    for (const auto &method_pair : reg_it->second.methods) {
        const std::string &method_name = method_pair.first;
        
        // Define the function on the prototype
        JSFunction *fun = JS_DefineFunction(g_cx, prototype, method_name.c_str(),
            universal_method_dispatch, 0, JSPROP_ENUMERATE);
        
        if (!fun) {
            fprintf(stderr, "Failed to define method %s on %s prototype\n", 
                    method_name.c_str(), class_name);
        }
    }
}

// ============================================================================
// ClassTraits<T>::add_method() implementation
// ============================================================================

template<typename T>
void ClassTraits<T>::add_method(const char *name, MethodDispatcher dispatcher) {
    if (!class_name) return;
    g_class_registries[class_name].methods[name] = dispatcher;
}

// ============================================================================
// ClassTraits<T>::add_getter() implementation
// ============================================================================

template<typename T>
void ClassTraits<T>::add_getter(const char *name, PropertyGetter getter) {
    if (!class_name) return;
    g_class_registries[class_name].getters[name] = getter;
}

// ============================================================================
// ClassTraits<T>::add_setter() implementation
// ============================================================================

template<typename T>
void ClassTraits<T>::add_setter(const char *name, PropertySetter setter) {
    if (!class_name) return;
    g_class_registries[class_name].setters[name] = setter;
}

// ============================================================================
// ClassTraits<T>::getProperty() implementation
// ============================================================================

template<typename T>
JSBool ClassTraits<T>::getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    if (!JSVAL_IS_STRING(id)) return JS_TRUE;
    
    JSString *str = JSVAL_TO_STRING(id);
    char *name = JS_EncodeString(cx, str);
    if (!name) return JS_TRUE;
    
    std::string prop_name(name);
    JS_free(cx, name);
    
    // Check if we have a getter for this property
    if (!class_name) return JS_TRUE;
    auto reg_it = g_class_registries.find(class_name);
    if (reg_it == g_class_registries.end()) return JS_TRUE;
    
    auto getter_it = reg_it->second.getters.find(prop_name);
    if (getter_it == reg_it->second.getters.end()) return JS_TRUE;
    
    T *native = static_cast<T*>(JS_GetPrivate(cx, obj));
    if (!native) return JS_TRUE;
    
    try {
        *vp = getter_it->second(native, cx);
    } catch (const std::exception &e) {
        JS_ReportError(cx, "Property getter error: %s", e.what());
        return JS_FALSE;
    }
    return JS_TRUE;
}

// ============================================================================
// ClassTraits<T>::setProperty() implementation
// ============================================================================

template<typename T>
JSBool ClassTraits<T>::setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    if (!JSVAL_IS_STRING(id)) return JS_TRUE;
    
    JSString *str = JSVAL_TO_STRING(id);
    char *name = JS_EncodeString(cx, str);
    if (!name) return JS_TRUE;
    
    std::string prop_name(name);
    JS_free(cx, name);
    
    // Check if we have a setter for this property
    if (!class_name) return JS_TRUE;
    auto reg_it = g_class_registries.find(class_name);
    if (reg_it == g_class_registries.end()) return JS_TRUE;
    
    auto setter_it = reg_it->second.setters.find(prop_name);
    if (setter_it == reg_it->second.setters.end()) return JS_TRUE;
    
    T *native = static_cast<T*>(JS_GetPrivate(cx, obj));
    if (!native) return JS_TRUE;
    
    try {
        setter_it->second(native, cx, *vp);
    } catch (const std::exception &e) {
        JS_ReportError(cx, "Property setter error: %s", e.what());
        return JS_FALSE;
    }
    return JS_TRUE;
}

// ============================================================================
// is_native implementation
// ============================================================================

template<typename T>
bool is_native(const object &obj) {
    if (obj.is_null()) return false;
    
    JSObject *jsobj = obj.get_object_ptr();
    if (!jsobj) return false;
    
    // Get the class of this object
    JSClass *cls = JS_GET_CLASS(g_cx, jsobj);
    if (!cls) return false;
    
    // Check if it matches our class
    if (!ClassTraits<T>::class_name) return false;
    if (std::strcmp(cls->name, ClassTraits<T>::class_name) != 0) return false;
    
    // Make sure there's valid private data
    void *priv = JS_GetPrivate(g_cx, jsobj);
    return priv != nullptr;
}

// ============================================================================
// get_native implementation
// ============================================================================

template<typename T>
T &get_native(const object &obj) {
    JSObject *jsobj = obj.get_object_ptr();
    if (!jsobj) {
        throw std::runtime_error("get_native: null object");
    }
    
    void *priv = JS_GetPrivate(g_cx, jsobj);
    if (!priv) {
        throw std::runtime_error("get_native: no private data");
    }
    
    return *static_cast<T*>(priv);
}

// ============================================================================
// create_native_object implementation
// ============================================================================

template<typename T, typename... Args>
value create_native_object(const object &proto, Args&&... args) {
    // Ensure the class is registered
    ClassTraits<T>::ensure_registered();
    
    // Create the JS object
    JSObject *jsobj = JS_NewObject(g_cx, &ClassTraits<T>::jsclass, 
        ClassTraits<T>::prototype, g_global);
    
    if (!jsobj) {
        throw std::runtime_error("Failed to create JS object");
    }
    
    // Create the native wrapper - pass the JS object as the self argument
    object self(jsobj);
    T *native = new T(self, std::forward<Args>(args)...);
    
    // Store the native pointer in the JS object's private slot
    JS_SetPrivate(g_cx, jsobj, native);
    
    return value(jsobj);
}

// ============================================================================
// load_class implementation
// ============================================================================

template<typename T>
void load_class() {
    ClassTraits<T>::register_class();
}

} // namespace flusspferd

// ============================================================================
// FLUSSPFERD_CLASS_DESCRIPTION macro
// ============================================================================

#define FLUSSPFERD_CLASS_DESCRIPTION(ClassName, ...) \
    class ClassName : public flusspferd::native_object_base<ClassName>

// Macro to register a class with the JS engine
#define FLUSSPFERD_REGISTER_CLASS(ClassName) \
    do { \
        flusspferd::ClassTraits<ClassName>::class_name = #ClassName; \
        flusspferd::load_class<ClassName>(); \
    } while(0)

// ============================================================================
// Method registration macros
// ============================================================================

// Register a method with 0 arguments returning a value
#define JS_REGISTER_METHOD_0(ClassName, jsName, cppMethod, RetType) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            return flusspferd::detail::cpp_to_js<RetType>(cx, self->cppMethod()); \
        })

// Register a void method with 0 arguments
#define JS_REGISTER_METHOD_0_VOID(ClassName, jsName, cppMethod) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            self->cppMethod(); \
            return JSVAL_VOID; \
        })

// Register a method with 1 argument returning a value
#define JS_REGISTER_METHOD_1(ClassName, jsName, cppMethod, RetType, Arg1Type) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            Arg1Type a1 = flusspferd::detail::js_to_cpp<Arg1Type>(cx, argc > 0 ? argv[0] : JSVAL_VOID); \
            return flusspferd::detail::cpp_to_js<RetType>(cx, self->cppMethod(a1)); \
        })

// Register a void method with 1 argument
#define JS_REGISTER_METHOD_1_VOID(ClassName, jsName, cppMethod, Arg1Type) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            Arg1Type a1 = flusspferd::detail::js_to_cpp<Arg1Type>(cx, argc > 0 ? argv[0] : JSVAL_VOID); \
            self->cppMethod(a1); \
            return JSVAL_VOID; \
        })

// Register a method with 2 arguments returning a value
#define JS_REGISTER_METHOD_2(ClassName, jsName, cppMethod, RetType, Arg1Type, Arg2Type) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            Arg1Type a1 = flusspferd::detail::js_to_cpp<Arg1Type>(cx, argc > 0 ? argv[0] : JSVAL_VOID); \
            Arg2Type a2 = flusspferd::detail::js_to_cpp<Arg2Type>(cx, argc > 1 ? argv[1] : JSVAL_VOID); \
            return flusspferd::detail::cpp_to_js<RetType>(cx, self->cppMethod(a1, a2)); \
        })

// Register a void method with 2 arguments
#define JS_REGISTER_METHOD_2_VOID(ClassName, jsName, cppMethod, Arg1Type, Arg2Type) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            Arg1Type a1 = flusspferd::detail::js_to_cpp<Arg1Type>(cx, argc > 0 ? argv[0] : JSVAL_VOID); \
            Arg2Type a2 = flusspferd::detail::js_to_cpp<Arg2Type>(cx, argc > 1 ? argv[1] : JSVAL_VOID); \
            self->cppMethod(a1, a2); \
            return JSVAL_VOID; \
        })

// Register a void method with 3 arguments
#define JS_REGISTER_METHOD_3_VOID(ClassName, jsName, cppMethod, Arg1Type, Arg2Type, Arg3Type) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            Arg1Type a1 = flusspferd::detail::js_to_cpp<Arg1Type>(cx, argc > 0 ? argv[0] : JSVAL_VOID); \
            Arg2Type a2 = flusspferd::detail::js_to_cpp<Arg2Type>(cx, argc > 1 ? argv[1] : JSVAL_VOID); \
            Arg3Type a3 = flusspferd::detail::js_to_cpp<Arg3Type>(cx, argc > 2 ? argv[2] : JSVAL_VOID); \
            self->cppMethod(a1, a2, a3); \
            return JSVAL_VOID; \
        })

// Register a void method with 4 arguments
#define JS_REGISTER_METHOD_4_VOID(ClassName, jsName, cppMethod, Arg1Type, Arg2Type, Arg3Type, Arg4Type) \
    flusspferd::ClassTraits<ClassName>::add_method(jsName, \
        [](void *native_ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            Arg1Type a1 = flusspferd::detail::js_to_cpp<Arg1Type>(cx, argc > 0 ? argv[0] : JSVAL_VOID); \
            Arg2Type a2 = flusspferd::detail::js_to_cpp<Arg2Type>(cx, argc > 1 ? argv[1] : JSVAL_VOID); \
            Arg3Type a3 = flusspferd::detail::js_to_cpp<Arg3Type>(cx, argc > 2 ? argv[2] : JSVAL_VOID); \
            Arg4Type a4 = flusspferd::detail::js_to_cpp<Arg4Type>(cx, argc > 3 ? argv[3] : JSVAL_VOID); \
            self->cppMethod(a1, a2, a3, a4); \
            return JSVAL_VOID; \
        })

// ============================================================================
// Property registration macros
// ============================================================================

// Register a getter-only property
#define JS_REGISTER_GETTER(ClassName, jsName, getterMethod, RetType) \
    flusspferd::ClassTraits<ClassName>::add_getter(jsName, \
        [](void *native_ptr, JSContext *cx) -> jsval { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return JSVAL_VOID; \
            return flusspferd::detail::cpp_to_js<RetType>(cx, self->getterMethod()); \
        })

// Register a setter property
#define JS_REGISTER_SETTER(ClassName, jsName, setterMethod, ArgType) \
    flusspferd::ClassTraits<ClassName>::add_setter(jsName, \
        [](void *native_ptr, JSContext *cx, jsval val) { \
            ClassName *self = static_cast<ClassName*>(native_ptr); \
            if (!self) return; \
            ArgType v = flusspferd::detail::js_to_cpp<ArgType>(cx, val); \
            self->setterMethod(v); \
        })

#endif // KINSLAYER_FLUSSPFERD_CLASS_MACROS_HPP

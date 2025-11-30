/**
 * flusspferd_class_macros.hpp - Class registration macros for SpiderMonkey 131
 */

#ifndef KINSLAYER_FLUSSPFERD_CLASS_MACROS_HPP
#define KINSLAYER_FLUSSPFERD_CLASS_MACROS_HPP

#include <jsapi.h>
#include <js/Object.h>
#include <js/Class.h>
#include <js/ValueArray.h>
#include <js/Id.h>
#include <stdexcept>
#include <string>
#include <map>
#include <functional>

namespace flusspferd {

// Forward declarations from flusspferd.hpp
extern JSContext *g_cx;
extern JS::PersistentRootedObject *g_global;
extern std::map<std::string, NativeClassInfo> g_class_registry;

// Forward declaration for method dispatcher
bool universal_method_dispatch(JSContext *cx, unsigned argc, JS::Value *vp);

// Note: Wrapper functions (sm_get_private, sm_set_private, etc.) are defined in flusspferd.hpp

// ============================================================================
// ClassTraits - type traits for native classes
// ============================================================================

template<typename T>
struct ClassTraits {
    static const char *class_name();
    static const char *constructor_name() { return class_name(); }
    
    // JSClass definition
    static JSClass jsclass;
    static const JSClassOps classOps;
    
    // Prototype object
    static JS::PersistentRootedObject *prototype;
    
    // Class operations
    static void ensure_registered();
    static bool constructor_stub(JSContext *cx, unsigned argc, JS::Value *vp);
    static void add_method(const std::string &name, 
                          std::function<bool(void*, JSContext*, unsigned, JS::Value*)> method);
    static void add_getter(const std::string &name,
                          std::function<bool(void*, JSContext*, JS::MutableHandleValue)> getter);
    static void add_setter(const std::string &name,
                          std::function<bool(void*, JSContext*, JS::HandleValue)> setter);
    
    // Property access hooks
    static bool getProperty(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp);
    static bool setProperty(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::HandleValue vp, 
                           JS::ObjectOpResult &result);
    
    // Define methods on prototype
    static void define_methods_on_prototype();
    
    // Destructor
    static void destructor(JS::GCContext *gcx, JSObject *obj);
};

// ============================================================================
// FLUSSPFERD_CLASS_DESCRIPTION macro
// This macro accepts the original Flusspferd format with 2 arguments:
// FLUSSPFERD_CLASS_DESCRIPTION(ClassName, (properties...))
// The properties are ignored as we handle binding separately.
// ============================================================================

#define FLUSSPFERD_CLASS_DESCRIPTION(className, properties)                          \
    class className;                                                                 \
    template<>                                                                       \
    inline const char *flusspferd::ClassTraits<className>::class_name() {           \
        return #className;                                                           \
    }                                                                                \
    template<>                                                                       \
    inline JS::PersistentRootedObject *flusspferd::ClassTraits<className>::prototype = nullptr; \
    template<>                                                                       \
    inline const JSClassOps flusspferd::ClassTraits<className>::classOps = {        \
        nullptr,                            /* addProperty */                        \
        nullptr,                            /* delProperty */                        \
        nullptr,                            /* enumerate */                          \
        nullptr,                            /* newEnumerate */                       \
        nullptr,                            /* resolve */                            \
        nullptr,                            /* mayResolve */                         \
        &flusspferd::ClassTraits<className>::destructor, /* finalize */             \
        nullptr,                            /* call */                               \
        nullptr,                            /* construct */                          \
        nullptr,                            /* trace */                              \
    };                                                                               \
    template<>                                                                       \
    inline JSClass flusspferd::ClassTraits<className>::jsclass = {                  \
        #className,                                                                  \
        JSCLASS_HAS_RESERVED_SLOTS(3),                                              \
        &flusspferd::ClassTraits<className>::classOps                               \
    };                                                                               \
    class className : public flusspferd::native_object_base<className>

// ============================================================================
// ClassTraits implementations (inline)
// ============================================================================

template<typename T>
void ClassTraits<T>::destructor(JS::GCContext *gcx, JSObject *obj) {
    T *native = static_cast<T*>(sm_get_private(obj));
    if (native) {
        delete native;
        sm_set_private(obj, nullptr);
    }
}

template<typename T>
bool ClassTraits<T>::constructor_stub(JSContext *cx, unsigned argc, JS::Value *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    // Check that we're called as a constructor
    if (!args.isConstructing()) {
        JS_ReportErrorASCII(cx, "%s must be called as a constructor", class_name());
        return false;
    }
    
    // Create the new object with our class
    JS::RootedObject newObj(cx, JS_NewObjectForConstructor(cx, &jsclass, args));
    if (!newObj) {
        return false;
    }
    
    // Note: actual native object creation happens in derived constructors
    // For now, just return the object
    args.rval().setObject(*newObj);
    return true;
}

template<typename T>
void ClassTraits<T>::add_method(const std::string &name, 
                                std::function<bool(void*, JSContext*, unsigned, JS::Value*)> method) {
    auto &info = g_class_registry[class_name()];
    info.methods[name] = method;
}

template<typename T>
void ClassTraits<T>::add_getter(const std::string &name,
                                std::function<bool(void*, JSContext*, JS::MutableHandleValue)> getter) {
    auto &info = g_class_registry[class_name()];
    info.getters[name] = getter;
}

template<typename T>
void ClassTraits<T>::add_setter(const std::string &name,
                                std::function<bool(void*, JSContext*, JS::HandleValue)> setter) {
    auto &info = g_class_registry[class_name()];
    info.setters[name] = setter;
}

template<typename T>
bool ClassTraits<T>::getProperty(JSContext *cx, JS::HandleObject obj, JS::HandleId id, 
                                  JS::MutableHandleValue vp) {
    // Get property name
    JS::RootedValue idVal(cx);
    if (!JS_IdToValue(cx, id, &idVal)) {
        return true;  // Let default handling take over
    }
    
    if (!idVal.isString()) {
        return true;  // Not a string property, let default handle it
    }
    
    JS::RootedString idStr(cx, idVal.toString());
    JS::UniqueChars propName = JS_EncodeStringToUTF8(cx, idStr);
    if (!propName) {
        return true;
    }
    
    // Get native object
    void *priv = sm_get_private(obj);
    if (!priv) {
        return true;  // No private data
    }
    
    // Look up getter
    auto &info = g_class_registry[class_name()];
    auto it = info.getters.find(propName.get());
    if (it != info.getters.end()) {
        return it->second(priv, cx, vp);
    }
    
    return true;  // Property not found in our getters, let default handle it
}

template<typename T>
bool ClassTraits<T>::setProperty(JSContext *cx, JS::HandleObject obj, JS::HandleId id,
                                  JS::HandleValue v, JS::ObjectOpResult &result) {
    // Get property name
    JS::RootedValue idVal(cx);
    if (!JS_IdToValue(cx, id, &idVal)) {
        return result.succeed();
    }
    
    if (!idVal.isString()) {
        return result.succeed();
    }
    
    JS::RootedString idStr(cx, idVal.toString());
    JS::UniqueChars propName = JS_EncodeStringToUTF8(cx, idStr);
    if (!propName) {
        return result.succeed();
    }
    
    // Get native object
    void *priv = sm_get_private(obj);
    if (!priv) {
        return result.succeed();
    }
    
    // Look up setter
    auto &info = g_class_registry[class_name()];
    auto it = info.setters.find(propName.get());
    if (it != info.setters.end()) {
        if (it->second(priv, cx, v)) {
            return result.succeed();
        }
        return false;
    }
    
    return result.succeed();  // Property not found in our setters
}

template<typename T>
void ClassTraits<T>::define_methods_on_prototype() {
    if (!g_cx || !prototype || !prototype->get()) return;
    
    auto &info = g_class_registry[class_name()];
    JS::RootedObject proto(g_cx, prototype->get());
    
    for (auto &pair : info.methods) {
        const std::string &methodName = pair.first;
        
        // Create a function object for this method
        JS::RootedObject funcObj(g_cx, JS_NewObject(g_cx, nullptr));
        if (!funcObj) continue;
        
        // Create the actual function
        JSFunction *func = JS_NewFunction(g_cx, universal_method_dispatch, 0, 0, methodName.c_str());
        if (!func) continue;
        
        JS::RootedObject funcObjReal(g_cx, JS_GetFunctionObject(func));
        
        // Store method name in reserved slot 0
        JSString *nameStr = JS_NewStringCopyUTF8Z(g_cx, JS::ConstUTF8CharsZ(methodName.c_str()));
        sm_set_reserved_slot(funcObjReal.get(), 0, JS::StringValue(nameStr));
        
        // Store class name in reserved slot 1
        JSString *classStr = JS_NewStringCopyUTF8Z(g_cx, JS::ConstUTF8CharsZ(class_name()));
        sm_set_reserved_slot(funcObjReal.get(), 1, JS::StringValue(classStr));
        
        // Define the method on the prototype
        JS::RootedValue funcVal(g_cx, JS::ObjectValue(*funcObjReal));
        JS_DefineProperty(g_cx, proto, methodName.c_str(), funcVal, JSPROP_ENUMERATE);
    }
}

template<typename T>
void ClassTraits<T>::ensure_registered() {
    static bool registered = false;
    if (registered) return;
    registered = true;
    
    if (!g_cx || !g_global) return;
    
    // Enter the global's realm
    JSAutoRealm ar(g_cx, g_global->get());
    
    // Initialize class info
    auto &info = g_class_registry[class_name()];
    info.jsclass = &jsclass;
    
    // Create prototype
    if (!prototype) {
        prototype = new JS::PersistentRootedObject(g_cx);
    }
    
    JS::RootedObject globalObj(g_cx, g_global->get());
    JS::RootedObject proto(g_cx);
    
    // Initialize class with constructor
    // JS_InitClass signature: cx, global, class, protoProto, name, constructor, nargs, ps, fs, static_ps, static_fs
    // In SpiderMonkey 131, use JS::NullHandleObject for null handle parameters
    proto = JS_InitClass(g_cx, globalObj, &jsclass, JS::NullHandleObject, class_name(), 
                        constructor_stub, 0, nullptr, nullptr, nullptr, nullptr);
    
    if (proto) {
        *prototype = proto;
        info.prototype = proto;
        
        // Define methods on prototype
        define_methods_on_prototype();
    }
}

// ============================================================================
// is_native - check if a JS object is a native object of type T
// ============================================================================

template<typename T>
bool is_native(const object &obj) {
    if (obj.is_null()) return false;
    
    JSObject *jsobj = obj.get_object_ptr();
    if (!jsobj) return false;
    
    const JSClass *cls = JS::GetClass(jsobj);
    if (!cls) return false;
    
    return cls == &ClassTraits<T>::jsclass;
}

// ============================================================================
// get_native - get the native C++ object from a JS object
// ============================================================================

template<typename T>
T &get_native(const object &obj) {
    if (!is_native<T>(obj)) {
        throw exception("Object is not of the expected native type");
    }
    
    JSObject *jsobj = obj.get_object_ptr();
    void *priv = sm_get_private(jsobj);
    if (!priv) {
        throw exception("Native object has no private data");
    }
    
    return *static_cast<T*>(priv);
}

// ============================================================================
// create_native_object - create a new native object
// ============================================================================

template<typename T, typename... Args>
object create_native_object(Args&&... args) {
    ClassTraits<T>::ensure_registered();
    
    if (!g_cx || !g_global) {
        throw exception("No JavaScript context");
    }
    
    // Enter the global's realm
    JSAutoRealm ar(g_cx, g_global->get());
    
    // Create the JS object
    JS::RootedObject proto(g_cx, ClassTraits<T>::prototype ? ClassTraits<T>::prototype->get() : nullptr);
    JS::RootedObject obj(g_cx, JS_NewObjectWithGivenProto(g_cx, &ClassTraits<T>::jsclass, proto));
    
    if (!obj) {
        throw exception("Failed to create native object");
    }
    
    // Create the native C++ object
    T *native = new T(std::forward<Args>(args)...);
    
    // Set the private data
    sm_set_private(obj, native);
    
    return object(obj);
}

// ============================================================================
// load_class - register a native class
// ============================================================================

template<typename T>
void load_class() {
    ClassTraits<T>::ensure_registered();
}

// ============================================================================
// Method registration macros
// ============================================================================

#define JS_REGISTER_METHOD_0(className, methodName)                                  \
    flusspferd::ClassTraits<className>::add_method(#methodName,                      \
        [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {         \
            JS::CallArgs args = JS::CallArgsFromVp(argc, vp);                        \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                auto result = self->methodName();                                    \
                args.rval().set(flusspferd::detail::to_jsval<decltype(result)>::convert(cx, result)); \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

#define JS_REGISTER_METHOD_0_VOID(className, methodName)                             \
    flusspferd::ClassTraits<className>::add_method(#methodName,                      \
        [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {         \
            JS::CallArgs args = JS::CallArgsFromVp(argc, vp);                        \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                self->methodName();                                                  \
                args.rval().setUndefined();                                          \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

#define JS_REGISTER_METHOD_1(className, methodName, arg1Type)                        \
    flusspferd::ClassTraits<className>::add_method(#methodName,                      \
        [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {         \
            JS::CallArgs args = JS::CallArgsFromVp(argc, vp);                        \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                arg1Type a1 = flusspferd::detail::from_jsval<arg1Type>::convert(cx, args.get(0)); \
                auto result = self->methodName(a1);                                  \
                args.rval().set(flusspferd::detail::to_jsval<decltype(result)>::convert(cx, result)); \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

#define JS_REGISTER_METHOD_1_VOID(className, methodName, arg1Type)                   \
    flusspferd::ClassTraits<className>::add_method(#methodName,                      \
        [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {         \
            JS::CallArgs args = JS::CallArgsFromVp(argc, vp);                        \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                arg1Type a1 = flusspferd::detail::from_jsval<arg1Type>::convert(cx, args.get(0)); \
                self->methodName(a1);                                                \
                args.rval().setUndefined();                                          \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

#define JS_REGISTER_METHOD_2(className, methodName, arg1Type, arg2Type)              \
    flusspferd::ClassTraits<className>::add_method(#methodName,                      \
        [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {         \
            JS::CallArgs args = JS::CallArgsFromVp(argc, vp);                        \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                arg1Type a1 = flusspferd::detail::from_jsval<arg1Type>::convert(cx, args.get(0)); \
                arg2Type a2 = flusspferd::detail::from_jsval<arg2Type>::convert(cx, args.get(1)); \
                auto result = self->methodName(a1, a2);                              \
                args.rval().set(flusspferd::detail::to_jsval<decltype(result)>::convert(cx, result)); \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

#define JS_REGISTER_METHOD_2_VOID(className, methodName, arg1Type, arg2Type)         \
    flusspferd::ClassTraits<className>::add_method(#methodName,                      \
        [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {         \
            JS::CallArgs args = JS::CallArgsFromVp(argc, vp);                        \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                arg1Type a1 = flusspferd::detail::from_jsval<arg1Type>::convert(cx, args.get(0)); \
                arg2Type a2 = flusspferd::detail::from_jsval<arg2Type>::convert(cx, args.get(1)); \
                self->methodName(a1, a2);                                            \
                args.rval().setUndefined();                                          \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

// Property registration macros
#define JS_REGISTER_GETTER(className, propName, cppGetter)                           \
    flusspferd::ClassTraits<className>::add_getter(#propName,                        \
        [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {            \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                auto result = self->cppGetter();                                     \
                vp.set(flusspferd::detail::to_jsval<decltype(result)>::convert(cx, result)); \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

#define JS_REGISTER_SETTER(className, propName, cppSetter, valueType)                \
    flusspferd::ClassTraits<className>::add_setter(#propName,                        \
        [](void *ptr, JSContext *cx, JS::HandleValue v) -> bool {                    \
            className *self = static_cast<className*>(ptr);                          \
            try {                                                                    \
                valueType val = flusspferd::detail::from_jsval<valueType>::convert(cx, v); \
                self->cppSetter(val);                                                \
                return true;                                                         \
            } catch (const std::exception &e) {                                      \
                JS_ReportErrorUTF8(cx, "%s", e.what());                              \
                return false;                                                        \
            }                                                                        \
        })

} // namespace flusspferd

#endif // KINSLAYER_FLUSSPFERD_CLASS_MACROS_HPP

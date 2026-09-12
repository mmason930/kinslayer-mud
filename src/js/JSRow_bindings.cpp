/**
 * JSRow_bindings.cpp - Method and property bindings for JSRow
 * Updated for SpiderMonkey 131 API.
 */

#include "../conf.h"
#include "JSRow.h"
#include "flusspferd.hpp"
#include "js_utils.h"

namespace {

inline JS::Value to_jsval(JSContext *cx, int v) { return JS::Int32Value(v); }
inline JS::Value to_jsval(JSContext *cx, bool v) { return JS::BooleanValue(v); }
inline JS::Value to_jsval(JSContext *cx, const flusspferd::string &v) { return v.val; }
inline JS::Value to_jsval(JSContext *cx, const flusspferd::value &v) { return v.val; }

inline int from_jsval_int(JSContext *cx, JS::HandleValue v) {
    if (v.isInt32()) return v.toInt32();
    double d; JS::ToNumber(cx, v, &d); return static_cast<int>(d);
}
inline flusspferd::string from_jsval_fstring(JSContext *cx, JS::HandleValue v) {
    return flusspferd::string(v.get());
}

} // anonymous namespace

void RegisterJSRowBindings() {
    using namespace flusspferd;
    
    // Methods - signature: bool(void*, JSContext*, unsigned, JS::Value*)
    g_class_registry["JSRow"].methods["get"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string propertyName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        args.rval().set(to_jsval(cx, self->get(propertyName)));
        return true;
    };
    
    g_class_registry["JSRow"].methods["getByIndex"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int index = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getByIndex(index)));
        return true;
    };
    
    g_class_registry["JSRow"].methods["isFieldNull"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string fieldName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        args.rval().set(to_jsval(cx, self->getIsFieldNull(fieldName)));
        return true;
    };
    
    g_class_registry["JSRow"].methods["isFieldNullByIndex"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int fieldIndex = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getIsFieldNullByIndex(fieldIndex)));
        return true;
    };
}

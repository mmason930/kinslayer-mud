/**
 * JSQuery_bindings.cpp - Method and property bindings for JSQuery
 * Updated for SpiderMonkey 131 API.
 */

#include "../conf.h"
#include "JSQuery.h"
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

void RegisterJSQueryBindings() {
    using namespace flusspferd;
    
    // Methods - signature: bool(void*, JSContext*, unsigned, JS::Value*)
    g_class_registry["JSQuery"].methods["skipRow"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->skipRow();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSQuery"].methods["getIndexByField"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string field = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        args.rval().set(to_jsval(cx, self->getIndexByField(field)));
        return true;
    };
    
    g_class_registry["JSQuery"].methods["getFieldByIndex"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int index = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getFieldByIndex(index)));
        return true;
    };
    
    g_class_registry["JSQuery"].methods["reverseRows"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->reverseRows();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSQuery"].methods["resetRowQueue"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->resetRowQueue();
        args.rval().setUndefined();
        return true;
    };
    
    // Property getters - signature: bool(void*, JSContext*, JS::MutableHandleValue)
    g_class_registry["JSQuery"].getters["numRows"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getnumRows())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSQuery"].getters["numFields"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getNumFields())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSQuery"].getters["hasNextRow"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (self) vp.set(to_jsval(cx, self->gethasNextRow())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSQuery"].getters["peekRow"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getpeekRow())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSQuery"].getters["getRow"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getRow())); else vp.setUndefined();
        return true;
    };
}

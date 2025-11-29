/**
 * JSQuery_bindings.cpp - Method and property bindings for JSQuery
 */

#include "../conf.h"
#include "JSQuery.h"
#include "JSRow.h"
#include "flusspferd.hpp"
#include "js_utils.h"

namespace {

inline jsval to_jsval(JSContext *cx, int v) { return INT_TO_JSVAL(v); }
inline jsval to_jsval(JSContext *cx, bool v) { return BOOLEAN_TO_JSVAL(v); }
inline jsval to_jsval(JSContext *cx, const flusspferd::string &v) { return v.val; }
inline jsval to_jsval(JSContext *cx, const flusspferd::value &v) { return v.val; }

inline int from_jsval_int(JSContext *cx, jsval v) {
    if (JSVAL_IS_INT(v)) return JSVAL_TO_INT(v);
    jsdouble d; JS_ValueToNumber(cx, v, &d); return static_cast<int>(d);
}
inline flusspferd::string from_jsval_fstring(JSContext *cx, jsval v) {
    return flusspferd::string(v);
}

} // anonymous namespace

void RegisterJSQueryBindings() {
    using namespace flusspferd;
    
    // Methods
    g_class_registries["JSQuery"].methods["skipRow"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) return JSVAL_VOID;
        self->skipRow();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSQuery"].methods["getIndexByField"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string field = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        return to_jsval(cx, self->getIndexByField(field));
    };
    
    g_class_registries["JSQuery"].methods["getFieldByIndex"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) return JSVAL_VOID;
        int index = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getFieldByIndex(index));
    };
    
    g_class_registries["JSQuery"].methods["reverseRows"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) return JSVAL_VOID;
        self->reverseRows();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSQuery"].methods["resetRowQueue"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        if (!self) return JSVAL_VOID;
        self->resetRowQueue();
        return JSVAL_VOID;
    };
    
    // Property getters
    g_class_registries["JSQuery"].getters["numRows"] = [](void *ptr, JSContext *cx) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        return self ? to_jsval(cx, self->getnumRows()) : JSVAL_VOID;
    };
    
    g_class_registries["JSQuery"].getters["numFields"] = [](void *ptr, JSContext *cx) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        return self ? to_jsval(cx, self->getNumFields()) : JSVAL_VOID;
    };
    
    g_class_registries["JSQuery"].getters["hasNextRow"] = [](void *ptr, JSContext *cx) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        return self ? to_jsval(cx, self->gethasNextRow()) : JSVAL_VOID;
    };
    
    g_class_registries["JSQuery"].getters["peekRow"] = [](void *ptr, JSContext *cx) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        return self ? to_jsval(cx, self->getpeekRow()) : JSVAL_VOID;
    };
    
    g_class_registries["JSQuery"].getters["getRow"] = [](void *ptr, JSContext *cx) -> jsval {
        JSQuery *self = static_cast<JSQuery*>(ptr);
        return self ? to_jsval(cx, self->getRow()) : JSVAL_VOID;
    };
}


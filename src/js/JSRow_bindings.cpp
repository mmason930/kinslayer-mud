/**
 * JSRow_bindings.cpp - Method and property bindings for JSRow
 */

#include "../conf.h"
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

void RegisterJSRowBindings() {
    using namespace flusspferd;
    
    // Methods
    g_class_registries["JSRow"].methods["get"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string propertyName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        return to_jsval(cx, self->get(propertyName));
    };
    
    g_class_registries["JSRow"].methods["getByIndex"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) return JSVAL_VOID;
        int index = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getByIndex(index));
    };
    
    g_class_registries["JSRow"].methods["isFieldNull"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string fieldName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        return to_jsval(cx, self->getIsFieldNull(fieldName));
    };
    
    g_class_registries["JSRow"].methods["isFieldNullByIndex"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRow *self = static_cast<JSRow*>(ptr);
        if (!self) return JSVAL_VOID;
        int fieldIndex = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getIsFieldNullByIndex(fieldIndex));
    };
}


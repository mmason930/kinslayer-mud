/**
 * JSObject_bindings.cpp - Method and property bindings for JSObject
 */

#include "../conf.h"
#include "JSObject.h"
#include "JSCharacter.h"
#include "JSRoom.h"
#include "flusspferd.hpp"
#include "js_utils.h"

namespace {

inline jsval to_jsval(JSContext *cx, int v) { return INT_TO_JSVAL(v); }
inline jsval to_jsval(JSContext *cx, bool v) { return BOOLEAN_TO_JSVAL(v); }
inline jsval to_jsval(JSContext *cx, double v) { jsval rv; JS_NewNumberValue(cx, v, &rv); return rv; }
inline jsval to_jsval(JSContext *cx, const flusspferd::string &v) { return v.val; }
inline jsval to_jsval(JSContext *cx, const flusspferd::value &v) { return v.val; }
inline jsval to_jsval(JSContext *cx, const flusspferd::object &v) { return v.val; }
inline jsval to_jsval(JSContext *cx, const flusspferd::array &v) { return v.val; }

inline int from_jsval_int(JSContext *cx, jsval v) {
    if (JSVAL_IS_INT(v)) return JSVAL_TO_INT(v);
    jsdouble d; JS_ValueToNumber(cx, v, &d); return static_cast<int>(d);
}
inline bool from_jsval_bool(JSContext *cx, jsval v) {
    JSBool b; JS_ValueToBoolean(cx, v, &b); return b == JS_TRUE;
}
inline flusspferd::string from_jsval_fstring(JSContext *cx, jsval v) {
    return flusspferd::string(v);
}
inline flusspferd::value from_jsval_value(JSContext *cx, jsval v) {
    return flusspferd::value(v);
}

inline JSRoom* get_js_room(JSContext *cx, jsval v) {
    if (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v)) return nullptr;
    JSObject *obj = JSVAL_TO_OBJECT(v);
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || strcmp(cls->name, "JSRoom") != 0) return nullptr;
    return static_cast<JSRoom*>(JS_GetPrivate(cx, obj));
}

inline JSCharacter* get_js_character(JSContext *cx, jsval v) {
    if (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v)) return nullptr;
    JSObject *obj = JSVAL_TO_OBJECT(v);
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || strcmp(cls->name, "JSCharacter") != 0) return nullptr;
    return static_cast<JSCharacter*>(JS_GetPrivate(cx, obj));
}

inline class JSObject* get_js_object(JSContext *cx, jsval v) {
    if (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v)) return nullptr;
    ::JSObject *obj = JSVAL_TO_OBJECT(v);
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || strcmp(cls->name, "JSObject") != 0) return nullptr;
    return static_cast<class JSObject*>(JS_GetPrivate(cx, obj));
}

} // anonymous namespace

void RegisterJSObjectBindings() {
    using namespace flusspferd;
    
    // Methods
    g_class_registries["JSObject"].methods["value"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int index = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->value(index));
    };
    
    g_class_registries["JSObject"].methods["canWear"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->can_wear(flag));
    };
    
    g_class_registries["JSObject"].methods["extraFlags"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->extra_flags(flag));
    };
    
    g_class_registries["JSObject"].methods["wearFlagged"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->wear_flagged(flag));
    };
    
    g_class_registries["JSObject"].methods["bitvector"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->bitvector(flag));
    };
    
    g_class_registries["JSObject"].methods["moveToRoom"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        JSRoom *dest = argc > 0 ? get_js_room(cx, argv[0]) : nullptr;
        if (dest) self->move_to_room(dest);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["moveToChar"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        JSCharacter *dest = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
        if (dest) self->move_to_char(dest);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["moveToObj"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        class JSObject *dest = argc > 0 ? get_js_object(cx, argv[0]) : nullptr;
        if (dest) self->move_to_obj(dest);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["loadObj"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int vnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->load_obj(vnum));
    };
    
    g_class_registries["JSObject"].methods["extract"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        self->extract();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["open"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        self->open();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["close"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        self->close();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["lock"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        self->lock();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["unlock"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        self->unlock();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["attach"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->attach(tVnum);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["detach"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int nr = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->detach(tVnum, nr);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["countJS"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, argv[0]) : flusspferd::value();
        return to_jsval(cx, self->countJS(tVnum));
    };
    
    g_class_registries["JSObject"].methods["getObjVal"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int x = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getObjVal(x));
    };
    
    g_class_registries["JSObject"].methods["setObjVal"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        int x = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int v = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->setObjVal(x, v);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["setRetoolName"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string newName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->setRetoolName(newName);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["setRetoolDesc"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string newName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->setRetoolDesc(newName);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["setRetoolSDesc"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string newName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->setRetoolSDesc(newName);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["setRetoolExDesc"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string newExDesc = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->setRetoolExDesc(newExDesc);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].methods["isRetooled"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) return JSVAL_VOID;
        return to_jsval(cx, self->isRetooled());
    };
    
    // Property getters
    g_class_registries["JSObject"].getters["id"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getID()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["name"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getName()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["namelist"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getNameList()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["ldesc"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getLongDescription()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["contents"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->contents()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["containedBy"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->contained_by()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["wornBy"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->worn_by()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["carriedBy"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->carried_by()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["satOnBy"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getSatOnBy()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["findHolder"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getFindHolder()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["inRoom"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getInRoom()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["weight"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->weight()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["cost"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->cost()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["type"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->type()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["timer"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->timer()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["vnum"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->vnum()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["max"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getMax()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isValid"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getIsValid()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isCorpse"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getIsCorpse()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["count"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getCount()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["hidden"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->getHidden()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["canOpen"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->canOpen()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["canLock"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->canLock()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isContainer"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->isContainer()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isClosed"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->isClosed()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isOpen"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->isOpen()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isLocked"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->isLocked()) : JSVAL_VOID;
    };
    
    g_class_registries["JSObject"].getters["isPickProof"] = [](void *ptr, JSContext *cx) -> jsval {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        return self ? to_jsval(cx, self->isPickProof()) : JSVAL_VOID;
    };
}


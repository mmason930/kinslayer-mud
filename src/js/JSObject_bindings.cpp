/**
 * JSObject_bindings.cpp - Method and property bindings for JSObject
 * Updated for SpiderMonkey 131 API.
 */

#include "../conf.h"
#include "JSObject.h"
#include "JSCharacter.h"
#include "JSRoom.h"
#include "flusspferd.hpp"
#include "js_utils.h"

namespace {

inline JS::Value to_jsval(JSContext *cx, int v) { return JS::Int32Value(v); }
inline JS::Value to_jsval(JSContext *cx, bool v) { return JS::BooleanValue(v); }
inline JS::Value to_jsval(JSContext *cx, double v) { return JS::DoubleValue(v); }
inline JS::Value to_jsval(JSContext *cx, const flusspferd::string &v) { return v.val; }
inline JS::Value to_jsval(JSContext *cx, const flusspferd::value &v) { return v.val; }
inline JS::Value to_jsval(JSContext *cx, const flusspferd::object &v) { return v.val; }
inline JS::Value to_jsval(JSContext *cx, const flusspferd::array &v) { return v.val; }

inline int from_jsval_int(JSContext *cx, JS::HandleValue v) {
    if (v.isInt32()) return v.toInt32();
    double d; JS::ToNumber(cx, v, &d); return static_cast<int>(d);
}
inline bool from_jsval_bool(JSContext *cx, JS::HandleValue v) {
    return JS::ToBoolean(v);
}
inline flusspferd::string from_jsval_fstring(JSContext *cx, JS::HandleValue v) {
    return flusspferd::string(v.get());
}
inline flusspferd::value from_jsval_value(JSContext *cx, JS::HandleValue v) {
    return flusspferd::value(v.get());
}

inline JSRoom* get_js_room(JSContext *cx, JS::HandleValue v) {
    if (!v.isObject()) return nullptr;
    ::JSObject *obj = &v.toObject();
    const JSClass *cls = JS::GetClass(obj);
    if (!cls || strcmp(cls->name, "JSRoom") != 0) return nullptr;
    return static_cast<JSRoom*>(flusspferd::sm_get_private(obj));
}

inline JSCharacter* get_js_character(JSContext *cx, JS::HandleValue v) {
    if (!v.isObject()) return nullptr;
    ::JSObject *obj = &v.toObject();
    const JSClass *cls = JS::GetClass(obj);
    if (!cls || strcmp(cls->name, "JSCharacter") != 0) return nullptr;
    return static_cast<JSCharacter*>(flusspferd::sm_get_private(obj));
}

inline class JSObject* get_js_object(JSContext *cx, JS::HandleValue v) {
    if (!v.isObject()) return nullptr;
    ::JSObject *obj = &v.toObject();
    const JSClass *cls = JS::GetClass(obj);
    if (!cls || strcmp(cls->name, "JSObject") != 0) return nullptr;
    return static_cast<class JSObject*>(flusspferd::sm_get_private(obj));
}

} // anonymous namespace

void RegisterJSObjectBindings() {
    using namespace flusspferd;
    
    // Methods - signature: bool(void*, JSContext*, unsigned, JS::Value*)
    g_class_registry["JSObject"].methods["value"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int index = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->value(index)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["canWear"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->can_wear(flag)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["extraFlags"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->extra_flags(flag)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["wearFlagged"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->wear_flagged(flag)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["bitvector"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->bitvector(flag)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["moveToRoom"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSRoom *dest = argc > 0 ? get_js_room(cx, args[0]) : nullptr;
        if (dest) self->move_to_room(dest);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["moveToChar"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSCharacter *dest = argc > 0 ? get_js_character(cx, args[0]) : nullptr;
        if (dest) self->move_to_char(dest);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["moveToObj"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        class JSObject *dest = argc > 0 ? get_js_object(cx, args[0]) : nullptr;
        if (dest) self->move_to_obj(dest);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["loadObj"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int vnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->load_obj(vnum)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["extract"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->extract();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["open"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->open();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["close"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->close();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["lock"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->lock();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["unlock"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->unlock();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["attach"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int tVnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->attach(tVnum);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["detach"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int tVnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int nr = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->detach(tVnum, nr);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["countJS"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, args[0]) : flusspferd::value();
        args.rval().set(to_jsval(cx, self->countJS(tVnum)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["getObjVal"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int x = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getObjVal(x)));
        return true;
    };
    
    g_class_registry["JSObject"].methods["setObjVal"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int x = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int v = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->setObjVal(x, v);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["setRetoolName"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string newName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->setRetoolName(newName);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["setRetoolDesc"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string newName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->setRetoolDesc(newName);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["setRetoolSDesc"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string newName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->setRetoolSDesc(newName);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["setRetoolExDesc"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string newExDesc = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->setRetoolExDesc(newExDesc);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].methods["isRetooled"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        args.rval().set(to_jsval(cx, self->isRetooled()));
        return true;
    };
    
    // Property getters - signature: bool(void*, JSContext*, JS::MutableHandleValue)
    g_class_registry["JSObject"].getters["id"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getID())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["name"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getName())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["namelist"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getNameList())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["ldesc"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLongDescription())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["contents"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->contents())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["containedBy"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->contained_by())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["wornBy"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->worn_by())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["carriedBy"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->carried_by())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["satOnBy"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getSatOnBy())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["findHolder"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getFindHolder())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["inRoom"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getInRoom())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["weight"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->weight())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["cost"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->cost())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["type"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->type())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["timer"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->timer())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["vnum"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->vnum())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["max"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMax())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isValid"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getIsValid())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isCorpse"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getIsCorpse())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["count"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getCount())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["hidden"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getHidden())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["canOpen"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->canOpen())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["canLock"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->canLock())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isContainer"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->isContainer())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isClosed"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->isClosed())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isOpen"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->isOpen())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isLocked"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->isLocked())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSObject"].getters["isPickProof"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->isPickProof())); else vp.setUndefined();
        return true;
    };

    // --- Missing getters (int return type) ---
    g_class_registry["JSObject"].getters["costPerDay"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->cost_per_day())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["offensive"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->offensive())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["parry"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->parry())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["dodge"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->dodge())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["absorb"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->absorb())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["clan"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->clan())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["decayType"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->decayType())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["decayTimer"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->decayTimer())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["decayTimerType"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->decayTimerType())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["scalpLevel"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getScalpLevel())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["scalpRace"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getScalpRace())); else vp.setUndefined();
        return true;
    };
    // --- Missing getters (bool return type) ---
    g_class_registry["JSObject"].getters["isScalp"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getIsScalp())); else vp.setUndefined();
        return true;
    };
    g_class_registry["JSObject"].getters["isPlayerScalp"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getIsPlayerScalp())); else vp.setUndefined();
        return true;
    };
    // --- Missing getters (flusspferd::string return type) ---
    g_class_registry["JSObject"].getters["extraDescription"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getExtraDescription())); else vp.setUndefined();
        return true;
    };
    // --- Missing getters (flusspferd::array return type) ---
    g_class_registry["JSObject"].getters["affects"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        class JSObject *self = static_cast<class JSObject*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getAffects())); else vp.setUndefined();
        return true;
    };
}

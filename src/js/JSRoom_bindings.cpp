/**
 * JSRoom_bindings.cpp - Method and property bindings for JSRoom
 * Updated for SpiderMonkey 131 API.
 */

#include "../conf.h"
#include "JSRoom.h"
#include "JSCharacter.h"
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
inline std::string from_jsval_string(JSContext *cx, JS::HandleValue v) {
    JSString *str = JS::ToString(cx, v);
    if (!str) return "";
    JS::RootedString rstr(cx, str);
    JS::UniqueChars cstr = JS_EncodeStringToUTF8(cx, rstr);
    if (!cstr) return "";
    return std::string(cstr.get());
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

} // anonymous namespace

void RegisterJSRoomBindings() {
    using namespace flusspferd;
    
    // Methods - signature: bool(void*, JSContext*, unsigned, JS::Value*)
    g_class_registry["JSRoom"].methods["echo"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->echo(msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["echoaround"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSCharacter *ch = argc > 0 ? get_js_character(cx, args[0]) : nullptr;
        std::string msg = argc > 1 ? from_jsval_string(cx, args[1]) : "";
        if (ch) self->echoaround(*ch, msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["loadObj"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int vnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->loadObj(vnum)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["loadMob"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int vnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->loadMob(vnum)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["zecho"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->zecho(msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["zreset"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->zreset();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["direction"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->direction(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorName"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorName(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorHidden"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorHidden(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorFlags"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getDoorFlags(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorPick"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorPick(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorKey"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorKey(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorDesc"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorDesc(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorExists"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorExists(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorIsLocked"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorIsLocked(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorIsClosed"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorIsClosed(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["doorIsRammable"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->doorIsRammable(dir)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["distanceTo"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSRoom *d = argc > 0 ? get_js_room(cx, args[0]) : nullptr;
        if (d) {
            args.rval().set(to_jsval(cx, self->distanceTo(d)));
        } else {
            args.rval().setInt32(-1);
        }
        return true;
    };
    
    g_class_registry["JSRoom"].methods["firstStep"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSRoom *d = argc > 0 ? get_js_room(cx, args[0]) : nullptr;
        if (d) {
            args.rval().set(to_jsval(cx, self->firstStep(d)));
        } else {
            args.rval().setInt32(-1);
        }
        return true;
    };
    
    g_class_registry["JSRoom"].methods["setDoorFlags"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int v = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->setDoorFlags(dir, v);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["attach"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int tVnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->attach(tVnum);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["detach"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int tVnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int nr = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->detach(tVnum, nr);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["countJS"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, args[0]) : flusspferd::value();
        args.rval().set(to_jsval(cx, self->countJS(tVnum)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["digTo"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        JSRoom *d = argc > 1 ? get_js_room(cx, args[1]) : nullptr;
        bool bothSides = argc > 2 ? from_jsval_bool(cx, args[2]) : false;
        bool temporary = argc > 3 ? from_jsval_bool(cx, args[3]) : false;
        if (d) self->digTo(dir, d, bothSides, temporary);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["killExit"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        bool bothSides = argc > 1 ? from_jsval_bool(cx, args[1]) : false;
        self->killExit(dir, bothSides);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["disableExit"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        bool bothSides = argc > 1 ? from_jsval_bool(cx, args[1]) : false;
        self->disableExit(dir, bothSides);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["enableExit"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dir = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        bool bothSides = argc > 1 ? from_jsval_bool(cx, args[1]) : false;
        self->enableExit(dir, bothSides);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["isFlagged"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->isFlagged(flag)));
        return true;
    };
    
    g_class_registry["JSRoom"].methods["pathToRoom"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSRoom *otherRoom = argc > 0 ? get_js_room(cx, args[0]) : nullptr;
        if (otherRoom) {
            args.rval().set(to_jsval(cx, self->pathToRoom(otherRoom)));
        } else {
            args.rval().setUndefined();
        }
        return true;
    };
    
    g_class_registry["JSRoom"].methods["roomFlagged"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->roomFlagged(flag)));
        return true;
    };
    
    // Method aliases for backward compatibility
    g_class_registry["JSRoom"].methods["getObjects"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) args.rval().set(to_jsval(cx, self->items())); else args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["getItems"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) args.rval().set(to_jsval(cx, self->items())); else args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].methods["getPeople"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) args.rval().set(to_jsval(cx, self->people())); else args.rval().setUndefined();
        return true;
    };
    
    // Property getters - signature: bool(void*, JSContext*, JS::MutableHandleValue)
    g_class_registry["JSRoom"].getters["name"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getName())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["description"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getDescription())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["neighbors"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->neighbors())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["people"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->people())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["vnum"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->vnum())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["sector"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->sector())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["items"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->items())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["dark"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->dark())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["zoneVnum"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->zoneVnum())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSRoom"].getters["zoneName"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (self) vp.set(to_jsval(cx, self->zoneName())); else vp.setUndefined();
        return true;
    };
}

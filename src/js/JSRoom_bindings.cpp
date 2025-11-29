/**
 * JSRoom_bindings.cpp - Method and property bindings for JSRoom
 */

#include "../conf.h"
#include "JSRoom.h"
#include "JSCharacter.h"
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
inline std::string from_jsval_string(JSContext *cx, jsval v) {
    JSString *str = JS_ValueToString(cx, v);
    if (!str) return "";
    char *cstr = JS_EncodeString(cx, str);
    if (!cstr) return "";
    std::string result(cstr);
    JS_free(cx, cstr);
    return result;
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

} // anonymous namespace

void RegisterJSRoomBindings() {
    using namespace flusspferd;
    
    // Methods
    g_class_registries["JSRoom"].methods["echo"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->echo(msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["echoaround"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        JSCharacter *ch = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
        std::string msg = argc > 1 ? from_jsval_string(cx, argv[1]) : "";
        if (ch) self->echoaround(*ch, msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["loadObj"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int vnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->loadObj(vnum));
    };
    
    g_class_registries["JSRoom"].methods["loadMob"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int vnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->loadMob(vnum));
    };
    
    g_class_registries["JSRoom"].methods["zecho"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->zecho(msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["zreset"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        self->zreset();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["direction"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->direction(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorName"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorName(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorHidden"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorHidden(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorFlags"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getDoorFlags(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorPick"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorPick(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorKey"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorKey(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorDesc"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorDesc(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorExists"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorExists(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorIsLocked"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorIsLocked(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorIsClosed"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorIsClosed(dir));
    };
    
    g_class_registries["JSRoom"].methods["doorIsRammable"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->doorIsRammable(dir));
    };
    
    g_class_registries["JSRoom"].methods["distanceTo"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        JSRoom *d = argc > 0 ? get_js_room(cx, argv[0]) : nullptr;
        if (d) return to_jsval(cx, self->distanceTo(d));
        return INT_TO_JSVAL(-1);
    };
    
    g_class_registries["JSRoom"].methods["firstStep"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        JSRoom *d = argc > 0 ? get_js_room(cx, argv[0]) : nullptr;
        if (d) return to_jsval(cx, self->firstStep(d));
        return INT_TO_JSVAL(-1);
    };
    
    g_class_registries["JSRoom"].methods["setDoorFlags"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int v = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->setDoorFlags(dir, v);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["attach"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->attach(tVnum);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["detach"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int nr = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->detach(tVnum, nr);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["countJS"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, argv[0]) : flusspferd::value();
        return to_jsval(cx, self->countJS(tVnum));
    };
    
    g_class_registries["JSRoom"].methods["digTo"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        JSRoom *d = argc > 1 ? get_js_room(cx, argv[1]) : nullptr;
        bool bothSides = argc > 2 ? from_jsval_bool(cx, argv[2]) : false;
        bool temporary = argc > 3 ? from_jsval_bool(cx, argv[3]) : false;
        if (d) self->digTo(dir, d, bothSides, temporary);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["killExit"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        bool bothSides = argc > 1 ? from_jsval_bool(cx, argv[1]) : false;
        self->killExit(dir, bothSides);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["disableExit"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        bool bothSides = argc > 1 ? from_jsval_bool(cx, argv[1]) : false;
        self->disableExit(dir, bothSides);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["enableExit"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int dir = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        bool bothSides = argc > 1 ? from_jsval_bool(cx, argv[1]) : false;
        self->enableExit(dir, bothSides);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["isFlagged"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->isFlagged(flag));
    };
    
    g_class_registries["JSRoom"].methods["pathToRoom"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        JSRoom *otherRoom = argc > 0 ? get_js_room(cx, argv[0]) : nullptr;
        if (otherRoom) return to_jsval(cx, self->pathToRoom(otherRoom));
        return JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["roomFlagged"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->roomFlagged(flag));
    };
    
    // Method aliases for backward compatibility
    g_class_registries["JSRoom"].methods["getObjects"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->items()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["getItems"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->items()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].methods["getPeople"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->people()) : JSVAL_VOID;
    };
    
    // Property getters
    g_class_registries["JSRoom"].getters["name"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->getName()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["description"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->getDescription()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["neighbors"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->neighbors()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["people"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->people()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["vnum"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->vnum()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["sector"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->sector()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["items"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->items()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["dark"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->dark()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["zoneVnum"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->zoneVnum()) : JSVAL_VOID;
    };
    
    g_class_registries["JSRoom"].getters["zoneName"] = [](void *ptr, JSContext *cx) -> jsval {
        JSRoom *self = static_cast<JSRoom*>(ptr);
        return self ? to_jsval(cx, self->zoneName()) : JSVAL_VOID;
    };
}


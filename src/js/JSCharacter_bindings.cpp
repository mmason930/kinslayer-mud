/**
 * JSCharacter_bindings.cpp - Method and property bindings for JSCharacter
 * 
 * This file registers all JSCharacter methods and properties with the JavaScript engine.
 * Updated for SpiderMonkey 131 API.
 */

#include "../conf.h"
#include "JSCharacter.h"
#include "JSRoom.h"
#include "JSObject.h"
#include "flusspferd.hpp"
#include "js_utils.h"

// Helper to convert return value to jsval (JS::Value)
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
inline double from_jsval_double(JSContext *cx, JS::HandleValue v) {
    double d; JS::ToNumber(cx, v, &d); return d;
}
inline float from_jsval_float(JSContext *cx, JS::HandleValue v) {
    return static_cast<float>(from_jsval_double(cx, v));
}
inline std::string from_jsval_string(JSContext *cx, JS::HandleValue v) {
    JSString *str = JS::ToString(cx, v);
    if (!str) return "";
    JS::RootedString rstr(cx, str);
    JS::UniqueChars cstr = JS_EncodeStringToUTF8(cx, rstr);
    if (!cstr) return "";
    return std::string(cstr.get());
}
inline const char* from_jsval_cstr(JSContext *cx, JS::HandleValue v, std::string &storage) {
    storage = from_jsval_string(cx, v);
    return storage.c_str();
}
inline flusspferd::string from_jsval_fstring(JSContext *cx, JS::HandleValue v) {
    return flusspferd::string(v.get());
}
inline flusspferd::value from_jsval_value(JSContext *cx, JS::HandleValue v) {
    return flusspferd::value(v.get());
}
inline flusspferd::object from_jsval_object(JSContext *cx, JS::HandleValue v) {
    return flusspferd::object(v.get());
}

// Get native JSCharacter* from JS::HandleValue
inline JSCharacter* get_js_character(JSContext *cx, JS::HandleValue v) {
    if (!v.isObject()) return nullptr;
    JSObject *obj = &v.toObject();
    const JSClass *cls = JS::GetClass(obj);
    if (!cls || strcmp(cls->name, "JSCharacter") != 0) return nullptr;
    return static_cast<JSCharacter*>(flusspferd::sm_get_private(obj));
}

// Get native JSRoom* from JS::HandleValue
inline JSRoom* get_js_room(JSContext *cx, JS::HandleValue v) {
    if (!v.isObject()) return nullptr;
    JSObject *obj = &v.toObject();
    const JSClass *cls = JS::GetClass(obj);
    if (!cls || strcmp(cls->name, "JSRoom") != 0) return nullptr;
    return static_cast<JSRoom*>(flusspferd::sm_get_private(obj));
}

// Get native JSObject* from JS::HandleValue
inline class JSObject* get_js_object(JSContext *cx, JS::HandleValue v) {
    if (!v.isObject()) return nullptr;
    ::JSObject *obj = &v.toObject();
    const JSClass *cls = JS::GetClass(obj);
    if (!cls || strcmp(cls->name, "JSObject") != 0) return nullptr;
    return static_cast<class JSObject*>(flusspferd::sm_get_private(obj));
}

} // anonymous namespace

// ============================================================================
// Registration function
// ============================================================================

void RegisterJSCharacterBindings() {
    using namespace flusspferd;
    
    // Register methods - signature: bool(void*, JSContext*, unsigned, JS::Value*)
    g_class_registry["JSCharacter"].methods["send"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        std::string storage;
        std::string msg = argc > 0 ? from_jsval_string(cx, args[0]) : "";
        self->send(msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["sendRaw"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        std::string msg = argc > 0 ? from_jsval_string(cx, args[0]) : "";
        self->sendRaw(msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["say"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        self->say(msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["damage"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int dmg = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        flusspferd::value attacker = argc > 1 ? from_jsval_value(cx, args[1]) : flusspferd::value();
        self->damage(dmg, attacker);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["eq"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int pos = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->eq(pos)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["getCharRoomVis"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        std::string storage;
        const char* arg = argc > 0 ? from_jsval_cstr(cx, args[0], storage) : "";
        args.rval().set(to_jsval(cx, self->js_get_char_room_vis(arg)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["getCharVis"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        std::string storage;
        const char* arg = argc > 0 ? from_jsval_cstr(cx, args[0], storage) : "";
        args.rval().set(to_jsval(cx, self->js_get_char_vis(arg)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["lag"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int pulses = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->lag(pulses);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["startTimer"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        float seconds = argc > 0 ? from_jsval_float(cx, args[0]) : 0;
        self->start_timer(seconds);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["comm"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        std::string storage;
        const char* cmd = argc > 0 ? from_jsval_cstr(cx, args[0], storage) : "";
        self->comm(cmd);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["loadObj"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int vnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->load_obj(vnum)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["extract"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        bool purgeItem = argc > 0 ? from_jsval_bool(cx, args[0]) : false;
        self->extract(purgeItem);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["moveToRoom"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSRoom *room = argc > 0 ? get_js_room(cx, args[0]) : nullptr;
        if (room) self->move_to_room(room);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["inClan"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int clan = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->in_clan(clan)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["qval"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string qName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        int nVal = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->qval(qName, nVal);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["quest"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::string qName = argc > 0 ? from_jsval_fstring(cx, args[0]) : flusspferd::string("");
        args.rval().set(to_jsval(cx, self->getQuest(qName)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["qp"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int cvnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getQP(cvnum)));
        return true;
    };
    
    // Color methods
    g_class_registry["JSCharacter"].methods["yellow"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getYellow(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["green"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getGreen(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["cyan"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getCyan(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["red"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getRed(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["magenta"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getMagenta(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["blue"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getBlue(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["bold"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getBold(level)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["normal"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int level = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getNormal(level)));
        return true;
    };
    
    // Additional methods
    g_class_registry["JSCharacter"].methods["mobFlagged"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int flag = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getMobFlagged(flag)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["getSkill"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int s = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getSkill(s)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["setSkill"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int s = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int v = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->setSkill(s, v);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["attach"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int tVnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->attach(tVnum);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["detach"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int tVnum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int nr = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        self->detach(tVnum, nr);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["countJS"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, args[0]) : flusspferd::value();
        args.rval().set(to_jsval(cx, self->countJS(tVnum)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["wantedBy"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int cNum = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->wantedBy(cNum)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["canSee"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        flusspferd::value t = argc > 0 ? from_jsval_value(cx, args[0]) : flusspferd::value();
        args.rval().set(to_jsval(cx, self->canSee(t)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["affect"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int effect = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        int duration = argc > 1 ? from_jsval_int(cx, args[1]) : 0;
        int skill = argc > 2 ? from_jsval_int(cx, args[2]) : 0;
        int modifier = argc > 3 ? from_jsval_int(cx, args[3]) : 0;
        self->affect(effect, duration, skill, modifier);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["unaffect"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int a = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->unaffect(a);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["affectedBy"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int a = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->affectedBy(a)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["warrantByClan"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int iClan = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->warrantByClan(iClan);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["pardonByClan"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int iClan = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        self->pardonByClan(iClan);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["getRank"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int iClan = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getRank(iClan)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["isCouncil"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        int iClan = argc > 0 ? from_jsval_int(cx, args[0]) : 0;
        args.rval().set(to_jsval(cx, self->getIsCouncil(iClan)));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["hasSource"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        args.rval().set(to_jsval(cx, self->getHasSource()));
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["resetSkills"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->resetSkills();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["resetTrades"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->resetTrades();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["resetSpells"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->resetSpells();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["tell"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSCharacter *vict = argc > 0 ? get_js_character(cx, args[0]) : nullptr;
        flusspferd::string msg = argc > 1 ? from_jsval_fstring(cx, args[1]) : flusspferd::string("");
        if (vict) self->tell(vict, msg);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["setFighting"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSCharacter *t = argc > 0 ? get_js_character(cx, args[0]) : nullptr;
        if (t) self->setFighting(t);
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["stopFighting"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->stopFighting();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["removeSource"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        self->removeSource();
        args.rval().setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["shouldBlockEngagementDueToNumberFighting"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSCharacter *victim = argc > 0 ? get_js_character(cx, args[0]) : nullptr;
        bool displayMessage = argc > 1 ? from_jsval_bool(cx, args[1]) : false;
        if (victim) {
            args.rval().set(to_jsval(cx, self->shouldBlockEngagementDueToNumberFighting(victim, displayMessage)));
        } else {
            args.rval().setBoolean(false);
        }
        return true;
    };
    
    g_class_registry["JSCharacter"].methods["canAggro"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) { args.rval().setUndefined(); return true; }
        JSCharacter *victim = argc > 0 ? get_js_character(cx, args[0]) : nullptr;
        if (victim) {
            args.rval().set(to_jsval(cx, self->canAggro(victim)));
        } else {
            args.rval().setBoolean(false);
        }
        return true;
    };
    
    // Register property getters - signature: bool(void*, JSContext*, JS::MutableHandleValue)
    g_class_registry["JSCharacter"].getters["name"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getName())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["title"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getTitle())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["sex"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getSex())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["class"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getClass())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["race"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getRace())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["level"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLevel())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["vnum"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getVnum())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["id"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getID())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["room"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->room())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["inventory"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->inventory())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["leader"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLeader())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["followers"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getFollowers())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["isValid"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getIsValid())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["namelist"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getNameList())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["description"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getDescription())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["longDesc"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLongDesc())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["hps"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getHps())); else vp.setUndefined();
        return true;
    };
    
    // Register property setters - signature: bool(void*, JSContext*, JS::HandleValue)
    g_class_registry["JSCharacter"].setters["hps"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setHps(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["position"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getPosition())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["position"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setPosition(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["gold"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getGold())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["gold"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setGold(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["fighting"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getFighting())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["weight"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getWeight())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["height"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getHeight())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["carryWeight"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getCarry_Weight())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["maxHit"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMax_Hps())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["maxHit"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMax_Hps(from_jsval_int(cx, val));
        return true;
    };
    
    // More property getters
    g_class_registry["JSCharacter"].getters["parry"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getParry())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["dodge"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getDodge())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["absorb"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getAbsorb())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["offense"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getOffense())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["weavePoints"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getWeave_Points())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["sps"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getSps())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["sps"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setSps(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["maxSpell"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMax_Sps())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["maxSpell"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMax_Sps(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["mvs"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMoves())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["mvs"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMoves(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["maxMove"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMax_Moves())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["maxMove"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMax_Moves(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["experience"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getExperience())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["experience"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setExperience(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["bankGold"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getBank_Gold())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["bankGold"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setBank_Gold(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["skillPractices"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getSkill_Practices())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["skillPractices"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setSkill_Practices(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["spellPractices"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getSpell_Practices())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["spellPractices"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setSpell_Practices(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["tradePractices"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getTrade_Practices())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].setters["tradePractices"] = [](void *ptr, JSContext *cx, JS::HandleValue val) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setTrade_Practices(from_jsval_int(cx, val));
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["mount"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMount())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["riddenBy"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getRiddenBy())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["target"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getTarget())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["marked"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMarked())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["hunting"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getHunting())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["wornWeight"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getWorn_Weight())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["arriveMessage"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getArriveMessage())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["leaveMessage"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLeaveMessage())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["shieldBlock"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getShieldBlock())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["maxCarryItems"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMaxCarryItems())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["maxCarryWeight"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMaxCarryWeight())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["expToLevel"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getExperienceToLevel())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["legend"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLegend())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["playingDays"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getPlayingDays())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["playingHours"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getPlayingHours())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["playerDeaths"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getPlayerDeaths())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["mobDeaths"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getMobDeaths())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["idle"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getIdle())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["linkless"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getLinkless())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["age"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getAge())); else vp.setUndefined();
        return true;
    };
    
    g_class_registry["JSCharacter"].getters["aliases"] = [](void *ptr, JSContext *cx, JS::MutableHandleValue vp) -> bool {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) vp.set(to_jsval(cx, self->getAliases())); else vp.setUndefined();
        return true;
    };
    
    // Method alias for backward compatibility
    g_class_registry["JSCharacter"].methods["getInventory"] = [](void *ptr, JSContext *cx, unsigned argc, JS::Value *vp) -> bool {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) args.rval().set(to_jsval(cx, self->inventory())); else args.rval().setUndefined();
        return true;
    };
}

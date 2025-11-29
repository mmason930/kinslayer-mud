/**
 * JSCharacter_bindings.cpp - Method and property bindings for JSCharacter
 * 
 * This file registers all JSCharacter methods and properties with the JavaScript engine.
 */

#include "../conf.h"
#include "JSCharacter.h"
#include "JSRoom.h"
#include "JSObject.h"
#include "flusspferd.hpp"
#include "js_utils.h"

// Helper to get native pointer with null check for methods (uses rval)
#define GET_NATIVE(cx, obj) \
    JSCharacter *self = static_cast<JSCharacter*>(JS_GetPrivate(cx, obj)); \
    if (!self) { *rval = JSVAL_VOID; return JS_TRUE; }

// Helper to get native pointer with null check for property getters/setters (uses vp)
#define GET_NATIVE_PROP(cx, obj) \
    JSCharacter *self = static_cast<JSCharacter*>(JS_GetPrivate(cx, obj)); \
    if (!self) { *vp = JSVAL_VOID; return JS_TRUE; }

// Helper to convert return value to jsval
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
inline double from_jsval_double(JSContext *cx, jsval v) {
    jsdouble d; JS_ValueToNumber(cx, v, &d); return d;
}
inline float from_jsval_float(JSContext *cx, jsval v) {
    return static_cast<float>(from_jsval_double(cx, v));
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
inline const char* from_jsval_cstr(JSContext *cx, jsval v) {
    JSString *str = JS_ValueToString(cx, v);
    if (!str) return "";
    return JS_EncodeString(cx, str);
}
inline flusspferd::string from_jsval_fstring(JSContext *cx, jsval v) {
    return flusspferd::string(v);
}
inline flusspferd::value from_jsval_value(JSContext *cx, jsval v) {
    return flusspferd::value(v);
}
inline flusspferd::object from_jsval_object(JSContext *cx, jsval v) {
    return flusspferd::object(v);
}

// Get native JSCharacter* from flusspferd::value
inline JSCharacter* get_js_character(JSContext *cx, jsval v) {
    if (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v)) return nullptr;
    JSObject *obj = JSVAL_TO_OBJECT(v);
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || strcmp(cls->name, "JSCharacter") != 0) return nullptr;
    return static_cast<JSCharacter*>(JS_GetPrivate(cx, obj));
}

// Get native JSRoom* from flusspferd::value
inline JSRoom* get_js_room(JSContext *cx, jsval v) {
    if (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v)) return nullptr;
    JSObject *obj = JSVAL_TO_OBJECT(v);
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || strcmp(cls->name, "JSRoom") != 0) return nullptr;
    return static_cast<JSRoom*>(JS_GetPrivate(cx, obj));
}

// Get native JSObject* from flusspferd::value
inline class JSObject* get_js_object(JSContext *cx, jsval v) {
    if (!JSVAL_IS_OBJECT(v) || JSVAL_IS_NULL(v)) return nullptr;
    JSObject *obj = JSVAL_TO_OBJECT(v);
    JSClass *cls = JS_GET_CLASS(cx, obj);
    if (!cls || strcmp(cls->name, "JSObject") != 0) return nullptr;
    return static_cast<class JSObject*>(JS_GetPrivate(cx, obj));
}

} // anonymous namespace

// ============================================================================
// Method wrappers
// ============================================================================

// void send(std::string message)
static JSBool JSCharacter_send(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    std::string msg = argc > 0 ? from_jsval_string(cx, argv[0]) : "";
    self->send(msg);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void sendRaw(std::string message)
static JSBool JSCharacter_sendRaw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    std::string msg = argc > 0 ? from_jsval_string(cx, argv[0]) : "";
    self->sendRaw(msg);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void say(flusspferd::string msg)
static JSBool JSCharacter_say(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
    self->say(msg);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void damage(int dmg, flusspferd::value attacker)
static JSBool JSCharacter_damage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int dmg = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    flusspferd::value attacker = argc > 1 ? from_jsval_value(cx, argv[1]) : flusspferd::value();
    self->damage(dmg, attacker);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// flusspferd::value eq(int pos)
static JSBool JSCharacter_eq(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int pos = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->eq(pos));
    return JS_TRUE;
}

// flusspferd::value js_get_char_room_vis(const char* arg)
static JSBool JSCharacter_getCharRoomVis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    const char* arg = argc > 0 ? from_jsval_cstr(cx, argv[0]) : "";
    *rval = to_jsval(cx, self->js_get_char_room_vis(arg));
    return JS_TRUE;
}

// flusspferd::value js_get_char_vis(const char* arg)
static JSBool JSCharacter_getCharVis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    const char* arg = argc > 0 ? from_jsval_cstr(cx, argv[0]) : "";
    *rval = to_jsval(cx, self->js_get_char_vis(arg));
    return JS_TRUE;
}

// void lag(int pulses)
static JSBool JSCharacter_lag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int pulses = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    self->lag(pulses);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void start_timer(float seconds)
static JSBool JSCharacter_startTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    float seconds = argc > 0 ? from_jsval_float(cx, argv[0]) : 0;
    self->start_timer(seconds);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void comm(const char* comm)
static JSBool JSCharacter_comm(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    const char* cmd = argc > 0 ? from_jsval_cstr(cx, argv[0]) : "";
    self->comm(cmd);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// flusspferd::value load_obj(const int vnum)
static JSBool JSCharacter_loadObj(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int vnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->load_obj(vnum));
    return JS_TRUE;
}

// void extract(bool purgeItem)
static JSBool JSCharacter_extract(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    bool purgeItem = argc > 0 ? from_jsval_bool(cx, argv[0]) : false;
    self->extract(purgeItem);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void move_to_room(JSRoom *r)
static JSBool JSCharacter_moveToRoom(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    JSRoom *room = argc > 0 ? get_js_room(cx, argv[0]) : nullptr;
    if (room) self->move_to_room(room);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// bool in_clan(const int clan)
static JSBool JSCharacter_inClan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int clan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->in_clan(clan));
    return JS_TRUE;
}

// void qval(flusspferd::string qName, const int nVal)
static JSBool JSCharacter_qval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    flusspferd::string qName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
    int nVal = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
    self->qval(qName, nVal);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// int getQuest(flusspferd::string qName)
static JSBool JSCharacter_quest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    flusspferd::string qName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
    *rval = to_jsval(cx, self->getQuest(qName));
    return JS_TRUE;
}

// int getQP(const int cvnum)
static JSBool JSCharacter_qp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int cvnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getQP(cvnum));
    return JS_TRUE;
}

// Color methods
static JSBool JSCharacter_yellow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getYellow(level));
    return JS_TRUE;
}

static JSBool JSCharacter_green(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getGreen(level));
    return JS_TRUE;
}

static JSBool JSCharacter_cyan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getCyan(level));
    return JS_TRUE;
}

static JSBool JSCharacter_red(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getRed(level));
    return JS_TRUE;
}

static JSBool JSCharacter_magenta(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getMagenta(level));
    return JS_TRUE;
}

static JSBool JSCharacter_blue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getBlue(level));
    return JS_TRUE;
}

static JSBool JSCharacter_bold(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getBold(level));
    return JS_TRUE;
}

static JSBool JSCharacter_normal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getNormal(level));
    return JS_TRUE;
}

// bool getMobFlagged(const int flag)
static JSBool JSCharacter_mobFlagged(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getMobFlagged(flag));
    return JS_TRUE;
}

// int getSkill(int s)
static JSBool JSCharacter_getSkill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int s = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getSkill(s));
    return JS_TRUE;
}

// void setSkill(int s, int v)
static JSBool JSCharacter_setSkill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int s = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    int v = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
    self->setSkill(s, v);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void attach(int tVnum)
static JSBool JSCharacter_attach(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    self->attach(tVnum);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void detach(int tVnum, int nr)
static JSBool JSCharacter_detach(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    int nr = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
    self->detach(tVnum, nr);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// int countJS(flusspferd::value tVnum)
static JSBool JSCharacter_countJS(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, argv[0]) : flusspferd::value();
    *rval = to_jsval(cx, self->countJS(tVnum));
    return JS_TRUE;
}

// bool wantedBy(int cNum)
static JSBool JSCharacter_wantedBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int cNum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->wantedBy(cNum));
    return JS_TRUE;
}

// bool canSee(flusspferd::value t)
static JSBool JSCharacter_canSee(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    flusspferd::value t = argc > 0 ? from_jsval_value(cx, argv[0]) : flusspferd::value();
    *rval = to_jsval(cx, self->canSee(t));
    return JS_TRUE;
}

// void affect(int effect, int duration, int skill, int modifier)
static JSBool JSCharacter_affect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int effect = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    int duration = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
    int skill = argc > 2 ? from_jsval_int(cx, argv[2]) : 0;
    int modifier = argc > 3 ? from_jsval_int(cx, argv[3]) : 0;
    self->affect(effect, duration, skill, modifier);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void unaffect(int a)
static JSBool JSCharacter_unaffect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int a = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    self->unaffect(a);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// bool affectedBy(int a)
static JSBool JSCharacter_affectedBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int a = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->affectedBy(a));
    return JS_TRUE;
}

// void warrantByClan(int iClan)
static JSBool JSCharacter_warrantByClan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    self->warrantByClan(iClan);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void pardonByClan(int iClan)
static JSBool JSCharacter_pardonByClan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    self->pardonByClan(iClan);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// int getRank(int iClan)
static JSBool JSCharacter_getRank(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getRank(iClan));
    return JS_TRUE;
}

// bool getIsCouncil(int iClan)
static JSBool JSCharacter_isCouncil(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
    *rval = to_jsval(cx, self->getIsCouncil(iClan));
    return JS_TRUE;
}

// bool getHasSource()
static JSBool JSCharacter_hasSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    *rval = to_jsval(cx, self->getHasSource());
    return JS_TRUE;
}

// void resetSkills()
static JSBool JSCharacter_resetSkills(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    self->resetSkills();
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void resetTrades()
static JSBool JSCharacter_resetTrades(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    self->resetTrades();
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void resetSpells()
static JSBool JSCharacter_resetSpells(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    self->resetSpells();
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void tell(JSCharacter *vict, flusspferd::string msg)
static JSBool JSCharacter_tell(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    JSCharacter *vict = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
    flusspferd::string msg = argc > 1 ? from_jsval_fstring(cx, argv[1]) : flusspferd::string("");
    if (vict) self->tell(vict, msg);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void setFighting(JSCharacter *t)
static JSBool JSCharacter_setFighting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    JSCharacter *t = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
    if (t) self->setFighting(t);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void stopFighting()
static JSBool JSCharacter_stopFighting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    self->stopFighting();
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// void removeSource()
static JSBool JSCharacter_removeSource(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    self->removeSource();
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// bool shouldBlockEngagementDueToNumberFighting(JSCharacter *victim, bool displayMessage)
static JSBool JSCharacter_shouldBlockEngagementDueToNumberFighting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    JSCharacter *victim = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
    bool displayMessage = argc > 1 ? from_jsval_bool(cx, argv[1]) : false;
    if (victim) {
        *rval = to_jsval(cx, self->shouldBlockEngagementDueToNumberFighting(victim, displayMessage));
    } else {
        *rval = BOOLEAN_TO_JSVAL(JS_FALSE);
    }
    return JS_TRUE;
}

// bool canAggro(JSCharacter *victim)
static JSBool JSCharacter_canAggro(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    GET_NATIVE(cx, obj);
    JSCharacter *victim = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
    if (victim) {
        *rval = to_jsval(cx, self->canAggro(victim));
    } else {
        *rval = BOOLEAN_TO_JSVAL(JS_FALSE);
    }
    return JS_TRUE;
}

// ============================================================================
// Property getters (read-only properties)
// ============================================================================

static JSBool JSCharacter_get_name(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getName());
    return JS_TRUE;
}

static JSBool JSCharacter_get_title(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getTitle());
    return JS_TRUE;
}

static JSBool JSCharacter_get_sex(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getSex());
    return JS_TRUE;
}

static JSBool JSCharacter_get_class(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getClass());
    return JS_TRUE;
}

static JSBool JSCharacter_get_race(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getRace());
    return JS_TRUE;
}

static JSBool JSCharacter_get_level(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getLevel());
    return JS_TRUE;
}

static JSBool JSCharacter_get_vnum(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getVnum());
    return JS_TRUE;
}

static JSBool JSCharacter_get_id(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getID());
    return JS_TRUE;
}

static JSBool JSCharacter_get_room(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->room());
    return JS_TRUE;
}

static JSBool JSCharacter_get_inventory(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->inventory());
    return JS_TRUE;
}

static JSBool JSCharacter_get_leader(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getLeader());
    return JS_TRUE;
}

static JSBool JSCharacter_get_followers(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getFollowers());
    return JS_TRUE;
}

static JSBool JSCharacter_get_isValid(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getIsValid());
    return JS_TRUE;
}

static JSBool JSCharacter_get_namelist(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getNameList());
    return JS_TRUE;
}

static JSBool JSCharacter_get_description(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getDescription());
    return JS_TRUE;
}

static JSBool JSCharacter_get_longDesc(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getLongDesc());
    return JS_TRUE;
}

// Read-write property getters/setters
static JSBool JSCharacter_get_hps(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getHps());
    return JS_TRUE;
}

static JSBool JSCharacter_set_hps(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    int val = from_jsval_int(cx, *vp);
    self->setHps(val);
    return JS_TRUE;
}

static JSBool JSCharacter_get_position(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getPosition());
    return JS_TRUE;
}

static JSBool JSCharacter_set_position(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    int val = from_jsval_int(cx, *vp);
    self->setPosition(val);
    return JS_TRUE;
}

static JSBool JSCharacter_get_gold(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getGold());
    return JS_TRUE;
}

static JSBool JSCharacter_set_gold(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    int val = from_jsval_int(cx, *vp);
    self->setGold(val);
    return JS_TRUE;
}

static JSBool JSCharacter_get_fighting(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getFighting());
    return JS_TRUE;
}

// More property getters for completeness
static JSBool JSCharacter_get_weight(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getWeight());
    return JS_TRUE;
}

static JSBool JSCharacter_get_height(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getHeight());
    return JS_TRUE;
}

static JSBool JSCharacter_get_carryWeight(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getCarry_Weight());
    return JS_TRUE;
}

static JSBool JSCharacter_get_maxHit(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    *vp = to_jsval(cx, self->getMax_Hps());
    return JS_TRUE;
}

static JSBool JSCharacter_set_maxHit(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    GET_NATIVE_PROP(cx, obj);
    int val = from_jsval_int(cx, *vp);
    self->setMax_Hps(val);
    return JS_TRUE;
}

// ============================================================================
// Registration function
// ============================================================================

void RegisterJSCharacterBindings() {
    using namespace flusspferd;
    
    // Register methods
    g_class_registries["JSCharacter"].methods["send"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        std::string msg = argc > 0 ? from_jsval_string(cx, argv[0]) : "";
        self->send(msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["sendRaw"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        std::string msg = argc > 0 ? from_jsval_string(cx, argv[0]) : "";
        self->sendRaw(msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["say"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string msg = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        self->say(msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["damage"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int dmg = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        flusspferd::value attacker = argc > 1 ? from_jsval_value(cx, argv[1]) : flusspferd::value();
        self->damage(dmg, attacker);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["eq"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int pos = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->eq(pos));
    };
    
    g_class_registries["JSCharacter"].methods["getCharRoomVis"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        const char* arg = argc > 0 ? from_jsval_cstr(cx, argv[0]) : "";
        return to_jsval(cx, self->js_get_char_room_vis(arg));
    };
    
    g_class_registries["JSCharacter"].methods["getCharVis"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        const char* arg = argc > 0 ? from_jsval_cstr(cx, argv[0]) : "";
        return to_jsval(cx, self->js_get_char_vis(arg));
    };
    
    g_class_registries["JSCharacter"].methods["lag"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int pulses = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->lag(pulses);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["startTimer"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        float seconds = argc > 0 ? from_jsval_float(cx, argv[0]) : 0;
        self->start_timer(seconds);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["comm"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        const char* cmd = argc > 0 ? from_jsval_cstr(cx, argv[0]) : "";
        self->comm(cmd);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["loadObj"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int vnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->load_obj(vnum));
    };
    
    g_class_registries["JSCharacter"].methods["extract"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        bool purgeItem = argc > 0 ? from_jsval_bool(cx, argv[0]) : false;
        self->extract(purgeItem);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["moveToRoom"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        JSRoom *room = argc > 0 ? get_js_room(cx, argv[0]) : nullptr;
        if (room) self->move_to_room(room);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["inClan"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int clan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->in_clan(clan));
    };
    
    g_class_registries["JSCharacter"].methods["qval"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string qName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        int nVal = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->qval(qName, nVal);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["quest"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::string qName = argc > 0 ? from_jsval_fstring(cx, argv[0]) : flusspferd::string("");
        return to_jsval(cx, self->getQuest(qName));
    };
    
    g_class_registries["JSCharacter"].methods["qp"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int cvnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getQP(cvnum));
    };
    
    // Color methods
    g_class_registries["JSCharacter"].methods["yellow"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getYellow(level));
    };
    
    g_class_registries["JSCharacter"].methods["green"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getGreen(level));
    };
    
    g_class_registries["JSCharacter"].methods["cyan"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getCyan(level));
    };
    
    g_class_registries["JSCharacter"].methods["red"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getRed(level));
    };
    
    g_class_registries["JSCharacter"].methods["magenta"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getMagenta(level));
    };
    
    g_class_registries["JSCharacter"].methods["blue"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getBlue(level));
    };
    
    g_class_registries["JSCharacter"].methods["bold"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getBold(level));
    };
    
    g_class_registries["JSCharacter"].methods["normal"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int level = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getNormal(level));
    };
    
    // Additional methods...
    g_class_registries["JSCharacter"].methods["mobFlagged"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int flag = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getMobFlagged(flag));
    };
    
    g_class_registries["JSCharacter"].methods["getSkill"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int s = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getSkill(s));
    };
    
    g_class_registries["JSCharacter"].methods["setSkill"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int s = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int v = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->setSkill(s, v);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["attach"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->attach(tVnum);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["detach"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int tVnum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int nr = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        self->detach(tVnum, nr);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["countJS"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::value tVnum = argc > 0 ? from_jsval_value(cx, argv[0]) : flusspferd::value();
        return to_jsval(cx, self->countJS(tVnum));
    };
    
    g_class_registries["JSCharacter"].methods["wantedBy"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int cNum = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->wantedBy(cNum));
    };
    
    g_class_registries["JSCharacter"].methods["canSee"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        flusspferd::value t = argc > 0 ? from_jsval_value(cx, argv[0]) : flusspferd::value();
        return to_jsval(cx, self->canSee(t));
    };
    
    g_class_registries["JSCharacter"].methods["affect"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int effect = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        int duration = argc > 1 ? from_jsval_int(cx, argv[1]) : 0;
        int skill = argc > 2 ? from_jsval_int(cx, argv[2]) : 0;
        int modifier = argc > 3 ? from_jsval_int(cx, argv[3]) : 0;
        self->affect(effect, duration, skill, modifier);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["unaffect"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int a = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->unaffect(a);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["affectedBy"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int a = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->affectedBy(a));
    };
    
    g_class_registries["JSCharacter"].methods["warrantByClan"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->warrantByClan(iClan);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["pardonByClan"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        self->pardonByClan(iClan);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["getRank"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getRank(iClan));
    };
    
    g_class_registries["JSCharacter"].methods["isCouncil"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        int iClan = argc > 0 ? from_jsval_int(cx, argv[0]) : 0;
        return to_jsval(cx, self->getIsCouncil(iClan));
    };
    
    g_class_registries["JSCharacter"].methods["hasSource"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        return to_jsval(cx, self->getHasSource());
    };
    
    g_class_registries["JSCharacter"].methods["resetSkills"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        self->resetSkills();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["resetTrades"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        self->resetTrades();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["resetSpells"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        self->resetSpells();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["tell"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        JSCharacter *vict = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
        flusspferd::string msg = argc > 1 ? from_jsval_fstring(cx, argv[1]) : flusspferd::string("");
        if (vict) self->tell(vict, msg);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["setFighting"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        JSCharacter *t = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
        if (t) self->setFighting(t);
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["stopFighting"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        self->stopFighting();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["removeSource"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        self->removeSource();
        return JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].methods["shouldBlockEngagementDueToNumberFighting"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        JSCharacter *victim = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
        bool displayMessage = argc > 1 ? from_jsval_bool(cx, argv[1]) : false;
        if (victim) {
            return to_jsval(cx, self->shouldBlockEngagementDueToNumberFighting(victim, displayMessage));
        }
        return BOOLEAN_TO_JSVAL(JS_FALSE);
    };
    
    g_class_registries["JSCharacter"].methods["canAggro"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (!self) return JSVAL_VOID;
        JSCharacter *victim = argc > 0 ? get_js_character(cx, argv[0]) : nullptr;
        if (victim) {
            return to_jsval(cx, self->canAggro(victim));
        }
        return BOOLEAN_TO_JSVAL(JS_FALSE);
    };
    
    // Register property getters
    g_class_registries["JSCharacter"].getters["name"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getName()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["title"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getTitle()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["sex"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getSex()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["class"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getClass()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["race"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getRace()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["level"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getLevel()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["vnum"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getVnum()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["id"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getID()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["room"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->room()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["inventory"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->inventory()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["leader"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getLeader()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["followers"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getFollowers()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["isValid"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getIsValid()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["namelist"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getNameList()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["description"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getDescription()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["longDesc"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getLongDesc()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["hps"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getHps()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["hps"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setHps(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["position"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getPosition()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["position"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setPosition(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["gold"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getGold()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["gold"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setGold(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["fighting"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getFighting()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["weight"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getWeight()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["height"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getHeight()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["carryWeight"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getCarry_Weight()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["maxHit"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMax_Hps()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["maxHit"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMax_Hps(from_jsval_int(cx, val));
    };
    
    // More property getters
    g_class_registries["JSCharacter"].getters["parry"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getParry()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["dodge"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getDodge()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["absorb"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getAbsorb()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["offense"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getOffense()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["weavePoints"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getWeave_Points()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["sps"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getSps()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["sps"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setSps(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["maxSpell"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMax_Sps()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["maxSpell"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMax_Sps(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["mvs"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMoves()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["mvs"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMoves(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["maxMove"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMax_Moves()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["maxMove"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setMax_Moves(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["experience"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getExperience()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["experience"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setExperience(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["bankGold"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getBank_Gold()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["bankGold"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setBank_Gold(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["skillPractices"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getSkill_Practices()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["skillPractices"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setSkill_Practices(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["spellPractices"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getSpell_Practices()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["spellPractices"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setSpell_Practices(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["tradePractices"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getTrade_Practices()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].setters["tradePractices"] = [](void *ptr, JSContext *cx, jsval val) {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        if (self) self->setTrade_Practices(from_jsval_int(cx, val));
    };
    
    g_class_registries["JSCharacter"].getters["mount"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMount()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["riddenBy"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getRiddenBy()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["target"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getTarget()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["marked"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMarked()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["hunting"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getHunting()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["wornWeight"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getWorn_Weight()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["arriveMessage"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getArriveMessage()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["leaveMessage"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getLeaveMessage()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["shieldBlock"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getShieldBlock()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["maxCarryItems"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMaxCarryItems()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["maxCarryWeight"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMaxCarryWeight()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["expToLevel"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getExperienceToLevel()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["legend"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getLegend()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["playingDays"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getPlayingDays()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["playingHours"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getPlayingHours()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["playerDeaths"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getPlayerDeaths()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["mobDeaths"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getMobDeaths()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["idle"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getIdle()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["linkless"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getLinkless()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["age"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getAge()) : JSVAL_VOID;
    };
    
    g_class_registries["JSCharacter"].getters["aliases"] = [](void *ptr, JSContext *cx) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->getAliases()) : JSVAL_VOID;
    };
    
    // Method aliases for backward compatibility
    g_class_registries["JSCharacter"].methods["getInventory"] = [](void *ptr, JSContext *cx, uintN argc, jsval *argv) -> jsval {
        JSCharacter *self = static_cast<JSCharacter*>(ptr);
        return self ? to_jsval(cx, self->inventory()) : JSVAL_VOID;
    };
}


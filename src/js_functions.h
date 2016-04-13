#ifndef KINSLAYER_JS_FUNCTIONS_H
#define KINSLAYER_JS_FUNCTIONS_H


class Character;
class JSTrigger;
class JSCharacter;
class JSObject;
class JSRoom;

#include "flusspferd.hpp"

bool is_allowed(JSBindable * b, Character * actor, JSTrigger * trig);
void js_list_scripts( JSBindable *owner, Character *ch );

/* Character Events */
void js_speech_triggers(Character * ch, const char* args);
bool js_command_triggers(Character * actor, const char* cmd, const char* args, bool aliasOverride);
bool js_enter_triggers(Room * r, Character * actor, int direction_leaving);
bool js_leave_triggers(Room * r, Character * actor, int direction_leaving);
void js_fight_triggers(Character * self, Character * victim);
void js_hitpercent_triggers(Character * ch, Character * victim);
void js_load_triggers(JSBindable *self);
void js_death_trigger(Character *self, Character *actor);
void js_bribe_trigger(Character *self, Character *actor, const int amt);
void js_enter_game_trigger(Character *self, Character *actor);
void js_tell_triggers(Character *actor, Character *target, const char *str);

/* Room Events */
void js_zone_reset(Room *room);
int js_dropped_in_room(Room *room, Object *obj, Character *actor);

/* Object Events */
int js_object_removed(Character *actor, Object *obj, const int pos);
int js_object_dropped(Character *actor, Object *obj);
int js_object_get(Character *actor, Object *obj);
int js_object_give(Character *actor, Object *obj, Character *receiver);
int js_object_wear(Character *actor, Object *obj, const int pos);

void js_random_triggers();
void js_time_triggers();
void js_extraction_scripts( JSBindable *self );


void JS_act( flusspferd::array args );
int JS_getTime();

void JS_fwrite( flusspferd::string fileName, flusspferd::string str );
flusspferd::string JS_fread( flusspferd::string fileName );
flusspferd::value JS_getRoom( int vnum );
flusspferd::value JS_getRoomByRnum( int rnum );
void JS_mudLog( int type, int lvl, flusspferd::string str );

int JS_getHour();
int JS_getDay();
int JS_getMonth();
int JS_getYear();

flusspferd::array JS_getListOfSkills();
int JS_getSkillVnum( flusspferd::string wName );
flusspferd::string JS_getSkillName( int wVnum );
flusspferd::string JS_getWeaveAttribute( int wVnum, flusspferd::string attribute );
flusspferd::string JS_getMobName( int vnum );
flusspferd::value JS_sendQuery( flusspferd::string qBuffer );
flusspferd::string JS_sqlEsc( flusspferd::string qBuffer );
flusspferd::string JS_sqlEscapeQuoteString( flusspferd::value str );

flusspferd::value getObjProto( int vnum );
flusspferd::value getObjProtoByRnum( int rnum );
flusspferd::value getMobProto( int vnum );
flusspferd::value getMobProtoByRnum( int rnum );

flusspferd::array getConnectedPlayers();
flusspferd::array JS_getCharacterList();
my_ulonglong JS_sqlInsertID();

bool JS_isZoneOpen( int vnum );
flusspferd::string JS_md5( flusspferd::string key );
flusspferd::string JS_clanNumToText( int iClanNum );
flusspferd::string JS_getClanRankName( int iClanNum, int iRankNum );
int JS_numberOfRooms();
bool JS_isClanSecret( int iClanVnum );
flusspferd::object JS_getGlobalObject();
flusspferd::string JS_getUserNameByUserId(int userId);
flusspferd::value JS_getUserIdByUserName(flusspferd::string userName);
void JS_saveTopLevelHolderItems(const std::string &holderType, const std::string &holderId, const flusspferd::array &objects);

//const char holderType, const std::string &holderId, const std::list<Object *> &contents)

int JS_getRankRequirement(int rankTo);

flusspferd::string JS_sqlEncodeQuoteDate(flusspferd::object dateTime);

void JS_sendToZone(int zoneNumber, flusspferd::string message);
void JS_setTimeout(unsigned int pulses, flusspferd::value callback, flusspferd::object arguments);
flusspferd::object JS_createDatetime(const DateTime &dateTime);

#endif

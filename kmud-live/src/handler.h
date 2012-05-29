/* ************************************************************************
*   File: handler.h                                     Part of CircleMUD *
*  Usage: header file: prototypes of handling and utility functions       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#ifndef HANDLER_H
#define HANDLER_H

/* handling the affected-structures */
void	affect_total(Character *ch);
void	affect_modify(Character *ch, byte loc, sbyte mod, long bitv, bool add
	                  );
void	affect_to_char(Character *ch, struct affected_type *af);
void	affect_remove(Character *ch, struct affected_type *af);
void	affect_from_char(Character *ch, int type, int bit);
bool	affected_by_spell(Character *ch, int type);
void	affect_join(Character *ch, struct affected_type *af,
                 bool add_dur, bool avg_dur, bool add_mod, bool avg_mod);

void	attach_weave(Character *ch, Character *victim, struct affected_type *af);
void	remove_weave(Character *ch, struct weave_data *weave, int loop);
void	add_affection_to_list(Character *ch, struct affect_type_not_saved *al);
void	remove_affection_list(Character *ch, struct affect_type_not_saved *al, int loop);

void	UpdateBootHigh( const int new_high, bool first=false );

// Races
int race_alias( Character *ch, char *alias );

/* Weight */
int CAN_GET_OBJ(Character *ch, Object *obj);
int CAN_CARRY_OBJ(Character *ch, Object *obj);

/* utility */

char *money_desc(int amount);
Object *create_money(int amount);
bool isname(const std::string &str, const std::string &namelist);
bool isname(const std::string &str, const char *namelist);
bool isname(const char *str, const std::string &namelist);
bool isname(const char *str, const char *namelist);
bool is_name(const char *str, const char *namelist);
char	*fname(char *namelist);
const int GetNumber(char **name);
const int GetNumber(std::string &name);

/* ******** objects *********** */
void	obj_to_char(Object *object, Character *ch);
void	obj_from_char(Object *object);

void	equip_char(Character *ch, Object *obj, int pos);
Object *unequip_char(Character *ch, int pos);

Object *get_obj_in_list(char *name, Object *listy);
Object *get_obj_in_list_num(int num, Object *listy);
Object *get_obj_in_eq_list(Character* Wearer, char* name);
Object *get_obj(char *name);
Object *get_obj_num(obj_rnum nr);
Object *get_obj_by_id(const boost::uuids::uuid &targetID);

void	obj_to_obj(Object *obj, Object *obj_to);
void	obj_from_obj(Object *obj);
void	object_list_new_owner(Object *listy, Character *ch);
void	move_obj_random(Object *obj, int bottom, int top, int inside_allowed);

/* ******* characters ********* */

Character *get_char_room(const char *name, room_rnum room);
Character *get_char_num(mob_rnum nr);
Character *get_char(const char *name);
Character *get_char_by_name(const char *name, int npc);

void	move_char_circle(Character *ch);
void	move_char_random(Character *ch, int bottom, int top, int inside_allowed);

/* find if character can see */
Character *get_char_room_vis(Character *ch, const char *name);
Character *get_player_vis(Character *ch, char *name, int inroom);
Character *get_char_vis(Character *ch, const char *name);
Object *get_obj_in_list_vis(Character *ch, char *name,
                            Object *listy);
Object *get_obj_vis(Character *ch, char *name);
Object *get_object_in_equip_vis(Character *ch,
                                char *arg, Object *equipment[], int *j);

int inches(int centimeters);
int feet(int centimeters);
int centimeters(int inches);
int str_display_sz(int sz, char *str);

/* find all dots */

int	find_all_dots(char *arg);

const int FIND_INDIV = 0;
const int FIND_ALL = 1;
const int FIND_ALLDOT = 2;

/* Generic Find */

int weapon_skill(Character *ch, int weapon_type);
int generic_find(char *arg, int bitvector, Character *ch,
                 Character **tar_ch, Object **tar_obj);

const int FIND_CHAR_ROOM = 1;
const int FIND_CHAR_WORLD = 2;
const int FIND_OBJ_INV = 4;
const int FIND_OBJ_ROOM = 8;
const int FIND_OBJ_WORLD = 16;
const int FIND_OBJ_EQUIP = 32;

/* prototypes from crash save system */
void	crashListRent(Character * ch, char *name);
void	crashsaveAll();

/* prototypes from fight.c */
void	set_fighting(Character *ch, Character *victim);
void	stop_fighting(Character *ch);
int		hit(Character *ch, Character *victim, int type);
int		damage(Character *ch, Character *victim, int dam, int attacktype, int BodyPart);
int		skill_message(int dam, Character *ch, Character *vict, int attacktype, int BodyPart);

byte     RaceByString( const std::string &str);
byte     ClassByString( const std::string &str);
byte     SexByString( const std::string &str);

unsigned long int CalcGold(unsigned long int total);
unsigned long int CalcSilver(unsigned long int total);
unsigned long int CalcCopper(unsigned long int total);

#endif


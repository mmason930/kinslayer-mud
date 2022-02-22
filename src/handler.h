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
void	affect_modify(Character *ch, sbyte loc, sbyte mod, long bitv, bool add);
void	affect_to_char(Character *ch, struct affected_type *af);
void	affect_remove(Character *ch, struct affected_type *af);
void	affect_from_char(Character *ch, int type, int bit);
bool	affected_by_spell(Character *ch, int type);
void	affect_join(Character *ch, struct affected_type *af, bool add_dur, bool avg_dur, bool add_mod, bool avg_mod);
void	add_affection_to_list(Character *ch, struct affect_type_not_saved *al);
void	remove_affection_list(Character *ch, struct affect_type_not_saved *al, int loop);
void	UpdateBootHigh( const int new_high, bool first=false );

// Races
int race_alias( Character *ch, char *alias );

/* utility */

bool isname(const std::string &str, const std::string &namelist);
bool isname(const std::string &str, const char *namelist);
bool isname(const char *str, const std::string &namelist);
bool isname(const char *str, const char *namelist);
bool is_name(const char *str, const char *namelist);
char *fname(const char *namelist);
int GetNumber(char **name);
int GetNumber(std::string &name);

/* ******** objects *********** */
void	obj_to_char(Object *object, Character *ch);
void	obj_from_char(Object *object);

void	equip_char(Character *ch, Object *obj, int pos);
Object *unequip_char(Character *ch, int pos);

void	obj_to_obj(Object *obj, Object *obj_to);
void	obj_from_obj(Object *obj);

/* ******* characters ********* */

Character *get_char_room(const char *name, room_rnum room);
Character *get_char_num(mob_rnum nr);
Character *get_char(const char *name);
Character *get_char_by_name(const char *name, int npc);

/* find if character can see */
Character *get_char_room_vis(Character *ch, const char *name);
Character *get_player_vis(Character *ch, char *name, int inroom);
Character *get_char_vis(Character *ch, const char *name);

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
int		hit(Character *ch, Character *victim, int type);
int		damage(Character *ch, Character *victim, int dam, int attacktype, int BodyPart);
int		skill_message(int dam, Character *ch, Character *vict, int attacktype, int BodyPart);

sbyte     RaceByString( const std::string &str);
sbyte     ClassByString( const std::string &str);
sbyte     SexByString( const std::string &str);

unsigned long int CalcGold(unsigned long int total);
unsigned long int CalcSilver(unsigned long int total);
unsigned long int CalcCopper(unsigned long int total);

#endif


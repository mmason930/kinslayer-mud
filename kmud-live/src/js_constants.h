//
// C++ Interface: js_constants
//
// Description: 
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifdef KINSLAYER_JAVASCRIPT


#ifndef KINSLAYER_JS_CONSTANTS_H
#define KINSLAYER_JS_CONSTANTS_H

namespace JS
{
    // trigger types
    const unsigned int RANDOM               = 0; // checked randomly
    const unsigned int INTERVAL             = 1; // checked every number of seconds
    const unsigned int COMMAND              = 2; // character types a command
    const unsigned int SPEECH               = 3; // char says a word/phrase
    const unsigned int ACT                  = 4; // word or phrase sent to Act
    const unsigned int ENTER                = 5; // a char enters a room
    const unsigned int LEAVE                 = 6; // a char exits the room
    const unsigned int LOAD                 = 7; // something is loaded
    const unsigned int TIME                 = 8; // checked on a certain mud hour

    const unsigned int CHARACTER_DEATH      = 9; // character dies
    const unsigned int CHARACTER_RECEIVE    = 10; // character is given obj
    const unsigned int CHARACTER_FIGHT      = 11;// each pulse while fighting
    const unsigned int CHARACTER_HITPERCENT = 12;// fighting and below some hp
    const unsigned int CHARACTER_GREET_SEEN = 13;// char enters room and is seen by another char

    const unsigned int MOB_BRIBE            = 14;// coins are given to mob
    const unsigned int MOB_MEMORY           = 15;// mob see's someone remembered

    const unsigned int OBJ_GET              = 16;// item is picked up
    const unsigned int OBJ_DROP             = 17;// character trys to drop obj
    const unsigned int OBJ_GIVE             = 18;// character trys to give obj
    const unsigned int OBJ_WEAR             = 19;// character trys to wear obj
    const unsigned int OBJ_REMOVE           = 20;// character trys to remove obj

    const unsigned int ZONE_RESET           = 21;// zone has been reset
    const unsigned int WORLD_DROP           = 22;//something dropped in room
	const unsigned int EXTRACTION			= 23;//something gets extracted from the game.

    const unsigned int CHARACTER_LOGIN		= 24;// char enters game.

	const unsigned int CHARACTER_TELL		= 25;// char receives a tell.

    const unsigned int NUMBER_FLAGS         = 26;

    const unsigned int ALLOW_OBJ_EQUIPPED    = 0; // obj must be in char's equip
    const unsigned int ALLOW_OBJ_INVENENTORY = 1; // obj must be in char's inven
    const unsigned int ALLOW_OBJ_ROOM        = 2; // obj must be in char's room
    const unsigned int ALLOW_SELF            = 3; // allow actors which are the holder of the script
    const unsigned int ALLOW_NOT_SELF        = 4; // allow actors are not the holder
    const unsigned int ALLOW_PLAYER          = 5; // allow actors to be a player
    const unsigned int ALLOW_MOB             = 6; // allow actor not be a player

    const unsigned int NUMBER_ALLOWS = 7;
	
	const unsigned int OPTION_ALIAS_OVERRIDE = 0; //This trigger will override character aliases.

	const unsigned int NUMBER_OPTIONS = 1;

    extern const char *js_trig_types[];
    extern const char *js_allow[];
	extern const char *js_options[];

	const int NUM_JS_ALLOWS		= (7);
	const int NUM_JS_TRIG_TYPES	= (26);
}
#endif
#endif


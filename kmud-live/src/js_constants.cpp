/*************************************************************
 *                                                           *
 *   js_constants.cpp -                                      *
 *                                                           *
 *   Author:                                                 *
 *   (C) July 20th, 2009                                     *
 *                                                           *
 *                                                           *
 *                                                           *
 *                                                           *
 *************************************************************/

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "sysdep.h"
#include "js.h"
#include "js_functions.h"
#include "js_constants.h"
#include "utils.h"
#include "olc.h"
#include <flusspferd.hpp>
#include <flusspferd/spidermonkey/context.hpp>
#include "js_trigger.h"
#include "screen.h"
#include "rooms/RoomSector.h"

flusspferd::object makeConstants()
{
	flusspferd::object o = flusspferd::create_object();

	o.set_property("RACE_HUMAN", flusspferd::value(RACE_HUMAN));
	o.set_property("RACE_TROLLOC", flusspferd::value(RACE_TROLLOC));
	o.set_property("RACE_SEANCHAN", flusspferd::value(RACE_SEANCHAN));
	o.set_property("RACE_AIEL", flusspferd::value(RACE_AIEL));
	o.set_property("RACE_ANIMAL", flusspferd::value(RACE_ANIMAL));
	o.set_property("RACE_OTHER", flusspferd::value(RACE_OTHER));

	o.set_property("CLASS_WARRIOR", flusspferd::value(CLASS_WARRIOR));
	o.set_property("CLASS_THIEF", flusspferd::value(CLASS_THIEF));
	o.set_property("CLASS_RANGER", flusspferd::value(CLASS_RANGER));
	o.set_property("CLASS_CHANNELER", flusspferd::value(CLASS_CHANNELER));
	o.set_property("CLASS_FADE", flusspferd::value(CLASS_FADE));
	o.set_property("CLASS_DREADLORD", flusspferd::value(CLASS_DREADLORD));
	o.set_property("CLASS_BLADEMASTER", flusspferd::value(CLASS_BLADEMASTER));
	o.set_property("CLASS_GREYMAN", flusspferd::value(CLASS_GREYMAN));
	o.set_property("CLASS_DRAGHKAR", flusspferd::value(CLASS_DRAGHKAR));
	o.set_property("CLASS_DREADGUARD", flusspferd::value(CLASS_DREADGUARD));
	o.set_property("CLASS_OGIER", flusspferd::value(CLASS_OGIER));

	o.set_property("CMP", flusspferd::value(CMP));
	o.set_property("NRM", flusspferd::value(NRM));
	o.set_property("BRF", flusspferd::value(BRF));

	o.set_property("TO_ROOM", flusspferd::value(TO_ROOM));
	o.set_property("TO_VICT", flusspferd::value(TO_VICT));
	o.set_property("TO_NOTVICT", flusspferd::value(TO_NOTVICT));
	o.set_property("TO_CHAR", flusspferd::value(TO_CHAR));
	o.set_property("TO_SLEEP", flusspferd::value(TO_SLEEP));

	o.set_property("PULSES_PER_SEC", flusspferd::value(PASSES_PER_SEC));
	o.set_property("NUM_WEARS", flusspferd::value(NUM_WEARS));

	o.set_property("MOB_SPEC", flusspferd::value(MOB_SPEC));
	o.set_property("MOB_SENTINEL", flusspferd::value(MOB_SENTINEL));
	o.set_property("MOB_SCAVENGER", flusspferd::value(MOB_SCAVENGER));
	o.set_property("MOB_ISNPC", flusspferd::value(MOB_ISNPC));
	o.set_property("MOB_AWARE", flusspferd::value(MOB_AWARE));
	o.set_property("MOB_STAY_ZONE", flusspferd::value(MOB_STAY_ZONE));
	o.set_property("MOB_WIMPY", flusspferd::value(MOB_WIMPY));
	o.set_property("MOB_MEMORY", flusspferd::value(MOB_MEMORY));
	o.set_property("MOB_HELPER", flusspferd::value(MOB_HELPER));
	o.set_property("MOB_NOSLEEP", flusspferd::value(MOB_NOSLEEP));
	o.set_property("MOB_NOBLIND", flusspferd::value(MOB_NOBLIND));
	o.set_property("MOB_MOUNT", flusspferd::value(MOB_MOUNT));
	o.set_property("MOB_TRACK", flusspferd::value(MOB_TRACK));
	o.set_property("MOB_BASH", flusspferd::value(MOB_BASH));
	o.set_property("MOB_AWARD", flusspferd::value(MOB_AWARD));
	o.set_property("MOB_SHADOW_MOUNT", flusspferd::value(MOB_SHADOW_MOUNT));
	o.set_property("MOB_NOFIGHT", flusspferd::value(MOB_NOFIGHT));
	o.set_property("MOB_INVIS", flusspferd::value(MOB_INVIS));
	o.set_property("MOB_GHOST", flusspferd::value(MOB_GHOST));
	o.set_property("MOB_TRAINER", flusspferd::value(MOB_TRAINER));
	o.set_property("MOB_INNKEEPER", flusspferd::value(MOB_INNKEEPER));
	o.set_property("MOB_BANKER", flusspferd::value(MOB_BANKER));
	o.set_property("MOB_DEALER", flusspferd::value(MOB_DEALER));
	o.set_property("MOB_SMOB", flusspferd::value(MOB_SMOB));
	o.set_property("MOB_TRADE_TRAINER", flusspferd::value(MOB_TRADE_TRAINER));
	o.set_property("MOB_NOTRACK", flusspferd::value(MOB_NOTRACK));
	o.set_property("MOB_NOGROUP", flusspferd::value(MOB_NOGROUP));
	o.set_property("MOB_OGIER_MOUNT", flusspferd::value(MOB_OGIER_MOUNT));


	o.set_property("SEX_NEUTRAL", flusspferd::value(SEX_NEUTRAL));
	o.set_property("SEX_MALE", flusspferd::value(SEX_MALE));
	o.set_property("SEX_FEMALE", flusspferd::value(SEX_FEMALE));

	o.set_property("POS_DEAD", flusspferd::value(POS_DEAD)); // dead //
	o.set_property("POS_MORTALLYW", flusspferd::value(POS_MORTALLYW)); // mortally wounded //
	o.set_property("POS_INCAP", flusspferd::value(POS_INCAP)); // incapacitated //
	o.set_property("POS_STUNNED", flusspferd::value(POS_STUNNED)); // stunned //
	o.set_property("POS_SLEEPING", flusspferd::value(POS_SLEEPING)); // sleeping //
	o.set_property("POS_RESTING", flusspferd::value(POS_RESTING)); // resting //
	o.set_property("POS_SITTING", flusspferd::value(POS_SITTING)); // sitting //
	o.set_property("POS_FIGHTING", flusspferd::value(POS_FIGHTING)); // fighting //
	o.set_property("POS_STANDING", flusspferd::value(POS_STANDING)); // standing //
	o.set_property("POS_FLYING", flusspferd::value(POS_FLYING)); // flying //

	o.set_property("CL_COMPLETE", flusspferd::value(CL_COMPLETE)); // color complete //
	o.set_property("CL_NORMAL", flusspferd::value(CL_NORMAL)); // color normal //
	o.set_property("CL_SPARSE", flusspferd::value(CL_SPARSE)); // color sparse //
	o.set_property("CL_OFF", flusspferd::value(CL_OFF)); // color off //

	o.set_property("AFF_BLIND", flusspferd::value(AFF_BLIND)); // o.set_property(R)); Char is blind
	o.set_property("AFF_INVISIBLE", flusspferd::value(AFF_INVISIBLE)); // Char is invisible
	o.set_property("AFF_WATERWALK", flusspferd::value(AFF_WATERWALK)); // Char can walk on water
	o.set_property("AFF_SANCTUARY", flusspferd::value(AFF_SANCTUARY)); // Char protected by sanct.
	o.set_property("AFF_GROUP", flusspferd::value(AFF_GROUP)); // o.set_property(R)); Char is grouped
	o.set_property("AFF_POISON", flusspferd::value(AFF_POISON)); // o.set_property(R)); Char is poisoned
	o.set_property("AFF_SLEEP", flusspferd::value(AFF_SLEEP)); // o.set_property(R)); Char magically asleep
	o.set_property("AFF_INCOGNITO", flusspferd::value(AFF_INCOGNITO)); // Room for future expansion
	o.set_property("AFF_SNEAK", flusspferd::value(AFF_SNEAK)); // Char can move quietly
	o.set_property("AFF_HIDE", flusspferd::value(AFF_HIDE)); // Char is hidden
	o.set_property("AFF_NOTICE", flusspferd::value(AFF_NOTICE)); // Char has notice on
	o.set_property("AFF_NOQUIT", flusspferd::value(AFF_NOQUIT)); // Char cannot quit
	o.set_property("AFF_PARANOIA", flusspferd::value(AFF_PARANOIA)); // Char is paranoid
	o.set_property("AFF_NIGHT_VISION", flusspferd::value(AFF_NIGHT_VISION)); // Char can see in the dark
	o.set_property("AFF_HASTE", flusspferd::value(AFF_HASTE)); // Char is affected by haste
	o.set_property("AFF_SHIELD", flusspferd::value(AFF_SHIELD)); // Char is blocked from channeling
	o.set_property("AFF_EFFUSION", flusspferd::value(AFF_EFFUSION)); // Char leaves unnoticed
	o.set_property("AFF_AGILITY", flusspferd::value(AFF_AGILITY)); // More Dodge Bonus
	o.set_property("AFF_STRENGTH", flusspferd::value(AFF_STRENGTH)); // Char has boosted strength
	o.set_property("AFF_UNUSED1", flusspferd::value(AFF_UNUSED1)); // Formerly Cover Tracks
	o.set_property("AFF_DIZZY", flusspferd::value(AFF_DIZZY)); // For Dizziness
	o.set_property("AFF_WET", flusspferd::value(AFF_WET)); // Char is wet, flusspferd::value(used amplifying weaves.
	o.set_property("AFF_CONFUSED", flusspferd::value(AFF_CONFUSED)); // Char is confused, flusspferd::value(used for amplifying weaves.
	o.set_property("AFF_FORCED_PEACE", flusspferd::value(AFF_FORCED_PEACE)); // Char cannot use violent commands.
	o.set_property("AFF_HIDDEN_INTENT", flusspferd::value(AFF_HIDDEN_INTENT)); // Removes *name* stars.
	o.set_property("AFF_DISGUISE", flusspferd::value(AFF_DISGUISE)); // Makes them look like a mob.
	o.set_property("AFF_INVERTED_POWER", flusspferd::value(AFF_INVERTED_POWER)); // Makes their spells invisible.
	o.set_property("AFF_ZING_SHIELD", flusspferd::value(AFF_ZING_SHIELD)); // Creates immunity to fire and zing weaves.
	o.set_property("AFF_INSANE", flusspferd::value(AFF_INSANE));
	o.set_property("AFF_SILENCE", flusspferd::value(AFF_SILENCE));
	o.set_property("AFF_CHILL", flusspferd::value(AFF_CHILL));
	o.set_property("AFF_FEAR", flusspferd::value(AFF_FEAR));
	o.set_property("AFF_PANIC", flusspferd::value(AFF_PANIC));
	o.set_property("AFF_REFLECT", flusspferd::value(AFF_REFLECT));
	o.set_property("AFF_SENSE_SHADOW", flusspferd::value(AFF_SENSE_SHADOW));
	o.set_property("AFF_SHIELD_BLOCK", flusspferd::value(AFF_SHIELD_BLOCK)); //Decreases the victim's parry bonus
	o.set_property("AFF_SEVERE_WOUND", flusspferd::value(AFF_SEVERE_WOUND)); //Decreases the victim's parry and dodge bonuses
	o.set_property("AFF_HAMSTRING", flusspferd::value(AFF_HAMSTRING)); //Decreases the victim's dodge bonus
	o.set_property("AFF_EAVESDROP", flusspferd::value(AFF_EAVESDROP));
	o.set_property("AFF_DEAF", flusspferd::value(AFF_DEAF));
	o.set_property("AFF_WEAKEN", flusspferd::value(AFF_WEAKEN));
	o.set_property("AFF_CRIPPLE", flusspferd::value(AFF_CRIPPLE));
	o.set_property("AFF_SLOW", flusspferd::value(AFF_SLOW));
	o.set_property("AFF_DECAY", flusspferd::value(AFF_DECAY));
	o.set_property("AFF_SLICE", flusspferd::value(AFF_SLICE));
	o.set_property("AFF_BURN", flusspferd::value(AFF_BURN));
	o.set_property("AFF_DAZE", flusspferd::value(AFF_DAZE));
	o.set_property("AFF_SHADOW_RAGE", flusspferd::value(AFF_SHADOW_RAGE));
	o.set_property("AFF_REGENERATION", flusspferd::value(AFF_REGENERATION));
	o.set_property("AFF_HEALING", flusspferd::value(AFF_HEALING));
	o.set_property("AFF_DARK_PLAGUE", flusspferd::value(AFF_DARK_PLAGUE));

	o.set_property("WEAR_LIGHT", flusspferd::value(WEAR_LIGHT));
	o.set_property("WEAR_HOLD", flusspferd::value(WEAR_HOLD));
	o.set_property("WEAR_NECK_1", flusspferd::value(WEAR_NECK_1));
	o.set_property("WEAR_NECK_2", flusspferd::value(WEAR_NECK_2));
	o.set_property("WEAR_EAR_1", flusspferd::value(WEAR_EAR_1));
	o.set_property("WEAR_EAR_2", flusspferd::value(WEAR_EAR_2));
	o.set_property("WEAR_HEAD", flusspferd::value(WEAR_HEAD));
	o.set_property("WEAR_ABOUT", flusspferd::value(WEAR_ABOUT));
	o.set_property("WEAR_BACK", flusspferd::value(WEAR_BACK));
	o.set_property("WEAR_BODY", flusspferd::value(WEAR_BODY));
	o.set_property("WEAR_SHOULDERS", flusspferd::value(WEAR_SHOULDERS));
	o.set_property("WEAR_ARMS", flusspferd::value(WEAR_ARMS));
	o.set_property("WEAR_WAIST", flusspferd::value(WEAR_WAIST));
	o.set_property("WEAR_WRIST_R", flusspferd::value(WEAR_WRIST_R));
	o.set_property("WEAR_WRIST_L", flusspferd::value(WEAR_WRIST_L));
	o.set_property("WEAR_HANDS", flusspferd::value(WEAR_HANDS));
	o.set_property("WEAR_FINGER_R", flusspferd::value(WEAR_FINGER_R));
	o.set_property("WEAR_FINGER_L", flusspferd::value(WEAR_FINGER_L));
	o.set_property("WEAR_WIELD", flusspferd::value(WEAR_WIELD));
	o.set_property("WEAR_SHIELD", flusspferd::value(WEAR_SHIELD));
	o.set_property("WEAR_LEGS", flusspferd::value(WEAR_LEGS));
	o.set_property("WEAR_FEET", flusspferd::value(WEAR_FEET));

	o.set_property("ROOM_DARK", flusspferd::value(ROOM_DARK));
	o.set_property("ROOM_NOMOB", flusspferd::value(ROOM_NOMOB));
	o.set_property("ROOM_INDOORS", flusspferd::value(ROOM_INDOORS));
	o.set_property("ROOM_PEACEFUL", flusspferd::value(ROOM_PEACEFUL));
	o.set_property("ROOM_SOUNDPROOF", flusspferd::value(ROOM_SOUNDPROOF));
	o.set_property("ROOM_NOTRACK", flusspferd::value(ROOM_NOTRACK));
	o.set_property("ROOM_NOMAGIC", flusspferd::value(ROOM_NOMAGIC));
	o.set_property("ROOM_TUNNEL", flusspferd::value(ROOM_TUNNEL));
	o.set_property("ROOM_PRIVATE", flusspferd::value(ROOM_PRIVATE));
	o.set_property("ROOM_GODROOM", flusspferd::value(ROOM_GODROOM));
	o.set_property("ROOM_HOUSE", flusspferd::value(ROOM_HOUSE));
	o.set_property("ROOM_VAULT", flusspferd::value(ROOM_VAULT));
	o.set_property("ROOM_ATRIUM", flusspferd::value(ROOM_ATRIUM));
	o.set_property("ROOM_OLC", flusspferd::value(ROOM_OLC));
	o.set_property("ROOM_BFS_MARK", flusspferd::value(ROOM_BFS_MARK));
	o.set_property("ROOM_NOPORT", flusspferd::value(ROOM_NOPORT));
	o.set_property("ROOM_POKER", flusspferd::value(ROOM_POKER));
	o.set_property("ROOM_NORENT", flusspferd::value(ROOM_NORENT));
	o.set_property("ROOM_NOSOURCE", flusspferd::value(ROOM_NOSOURCE));
	o.set_property("ROOM_NOHIDE", flusspferd::value(ROOM_NOHIDE));
	o.set_property("ROOM_LIT", flusspferd::value(ROOM_LIT));
	o.set_property("ROOM_MINING", flusspferd::value(ROOM_MINING));
	o.set_property("ROOM_FISHING", flusspferd::value(ROOM_FISHING));
	o.set_property("ROOM_SMITHING", flusspferd::value(ROOM_SMITHING));
	o.set_property("ROOM_TAILORING", flusspferd::value(ROOM_TAILORING));

	for (auto sectorIter = RoomSector::getStartIterator(); sectorIter != RoomSector::getEndIterator(); ++sectorIter)
		o.set_property((*sectorIter)->getStandardName(), (*sectorIter)->getValue());

	o.set_property("NORTH", flusspferd::value(NORTH));
	o.set_property("EAST", flusspferd::value(EAST));
	o.set_property("SOUTH", flusspferd::value(SOUTH));
	o.set_property("WEST", flusspferd::value(WEST));
	o.set_property("UP", flusspferd::value(UP));
	o.set_property("DOWN", flusspferd::value(DOWN));

	o.set_property("ITEM_LIGHT", flusspferd::value(1)); // Item is a light source
	o.set_property("ITEM_MONEY", flusspferd::value(2)); // Item is a money
	o.set_property("ITEM_WEAPON", flusspferd::value(3)); // Item is a weapon
	o.set_property("ITEM_ARMOR", flusspferd::value(4)); // Item is armor
	o.set_property("ITEM_POTION", flusspferd::value(5)); // Item is a potion
	o.set_property("ITEM_OTHER", flusspferd::value(6)); // Misc object
	o.set_property("ITEM_CONTAINER", flusspferd::value(7)); // Item is a container
	o.set_property("ITEM_NOTE", flusspferd::value(8)); // Item is note
	o.set_property("ITEM_DRINKCON", flusspferd::value(9)); // Item is a drink container
	o.set_property("ITEM_KEY", flusspferd::value(10)); // Item is a key
	o.set_property("ITEM_FOOD", flusspferd::value(11)); // Item is food
	o.set_property("ITEM_PEN", flusspferd::value(12)); // Item is a pen
	o.set_property("ITEM_BOAT", flusspferd::value(13)); // Item is a boat
	o.set_property("ITEM_FOUNTAIN", flusspferd::value(14)); // Item is a fountain
	o.set_property("ITEM_ANGREAL", flusspferd::value(15)); // Item is an angreal
	o.set_property("ITEM_CHAIR", flusspferd::value(16)); // Item is a chair
	o.set_property("ITEM_SPECIAL", flusspferd::value(17)); //Item has special abilities attached - typically for imms.
	o.set_property("ITEM_LAMPPOST", flusspferd::value(18)); //Item is a lamp post
	o.set_property("ITEM_SKIN", flusspferd::value(19)); //Item can be skinned from a corpse

	o.set_property("ITEM_WEAR_TAKE", flusspferd::value(1<<0)); // Item can be takes
	o.set_property("ITEM_WEAR_FINGER", flusspferd::value(1<<1)); // Can be worn on finger
	o.set_property("ITEM_WEAR_NECK", flusspferd::value(1<<2)); // Can be worn around neck
	o.set_property("ITEM_WEAR_BODY", flusspferd::value(1<<3)); // Can be worn on body
	o.set_property("ITEM_WEAR_HEAD", flusspferd::value(1<<4)); // Can be worn on head
	o.set_property("ITEM_WEAR_LEGS", flusspferd::value(1<<5)); // Can be worn on legs
	o.set_property("ITEM_WEAR_FEET", flusspferd::value(1<<6)); // Can be worn on feet
	o.set_property("ITEM_WEAR_HANDS", flusspferd::value(1<<7)); // Can be worn on hands
	o.set_property("ITEM_WEAR_ARMS", flusspferd::value(1<<8)); // Can be worn on arms
	o.set_property("ITEM_WEAR_SHIELD", flusspferd::value(1<<9)); // Can be used as a shield
	o.set_property("ITEM_WEAR_ABOUT", flusspferd::value(1<<10)); // Can be worn about body
	o.set_property("ITEM_WEAR_WAIST", flusspferd::value(1<<11)); // Can be worn around waist
	o.set_property("ITEM_WEAR_WRIST", flusspferd::value(1<<12)); // Can be worn on wrist
	o.set_property("ITEM_WEAR_WIELD", flusspferd::value(1<<13)); // Can be wielded
	o.set_property("ITEM_WEAR_HOLD", flusspferd::value(1<<14)); // Can be held
	o.set_property("ITEM_WEAR_BACK", flusspferd::value(1<<15)); // Can be worn on back
	o.set_property("ITEM_WEAR_SHOULDERS", flusspferd::value(1<<16)); // Can be worn on shoulders
	o.set_property("ITEM_WEAR_EAR", flusspferd::value(1<<17)); // Can be worn on ears

	o.set_property("ITEM_NORENT", flusspferd::value(ITEM_NORENT)); // Item cannot be rented
	o.set_property("ITEM_INVISIBLE", flusspferd::value(1)); // Item is invisible
	o.set_property("ITEM_NODROP", flusspferd::value(ITEM_NODROP)); // Item is cursed: can't drop
	o.set_property("ITEM_WARD", flusspferd::value(ITEM_WARD)); // Item is weapon warded
	o.set_property("ITEM_NOSELL", flusspferd::value(ITEM_NOSELL)); // Shopkeepers won't touch it
	o.set_property("ITEM_CHAIN", flusspferd::value(ITEM_CHAIN)); // Item hits anyone fighting you
	o.set_property("ITEM_POISON", flusspferd::value(ITEM_POISON)); // Item randomly poisons
	o.set_property("ITEM_CHEST", flusspferd::value(ITEM_CHEST)); // Item randomly poisons
	o.set_property("ITEM_TWO_HANDED", flusspferd::value(ITEM_TWO_HANDED)); // Item can be worn with two hands
	o.set_property("ITEM_DAGGER_SHEATH", flusspferd::value(ITEM_DAGGER_SHEATH)); // Item is a sheath for daggers
	o.set_property("ITEM_SWORD_SHEATH", flusspferd::value(ITEM_SWORD_SHEATH)); // Item is a sheath for swords
	o.set_property("ITEM_TEMP", flusspferd::value(ITEM_TEMP)); // Item is temporary and created by a weave
	o.set_property("ITEM_NO_SHOW", flusspferd::value(ITEM_NO_SHOW)); //Item does not show up in the website listing.
	o.set_property("ITEM_CLAN_EQUIPMENT", flusspferd::value(ITEM_CLAN_EQUIPMENT));//Item is a piece of clan equipment.
	o.set_property("ITEM_SPEAR_SHEATH", flusspferd::value(ITEM_SPEAR_SHEATH)); //Item can sheath spears
	o.set_property("ITEM_RARE", flusspferd::value(ITEM_RARE)); //Item is considered "rare"
	o.set_property("ITEM_TRADES", flusspferd::value(ITEM_TRADES)); //Item is part of the trade system.

	/* Object decay types */
	o.set_property("OBJECT_DECAY_GENERAL", flusspferd::value(OBJECT_DECAY_GENERAL));
	o.set_property("OBJECT_DECAY_CLOTH", flusspferd::value(OBJECT_DECAY_CLOTH));
	o.set_property("OBJECT_DECAY_WOOD", flusspferd::value(OBJECT_DECAY_WOOD));
	o.set_property("OBJECT_DECAY_METAL", flusspferd::value(OBJECT_DECAY_METAL));
	o.set_property("OBJECT_DECAY_PLANT", flusspferd::value(OBJECT_DECAY_PLANT));
	o.set_property("OBJECT_DECAY_FOOD", flusspferd::value(OBJECT_DECAY_FOOD));

	/* Object decay timer frequencies */
	o.set_property("OBJECT_DECAY_MINUTES", flusspferd::value(OBJECT_DECAY_MINUTES));
	o.set_property("OBJECT_DECAY_HOURS", flusspferd::value(OBJECT_DECAY_HOURS));
	o.set_property("OBJECT_DECAY_DAYS", flusspferd::value(OBJECT_DECAY_DAYS));
	o.set_property("OBJECT_DECAY_WEEKS", flusspferd::value(OBJECT_DECAY_WEEKS));
	o.set_property("OBJECT_DECAY_MONTHS", flusspferd::value(OBJECT_DECAY_MONTHS));
	o.set_property("OBJECT_DECAY_YEARS", flusspferd::value(OBJECT_DECAY_YEARS));


	o.set_property("WEAPON_LONG_BLADE", flusspferd::value(WEAPON_LONG_BLADE));
	o.set_property("WEAPON_SHORT_BLADE", flusspferd::value(WEAPON_SHORT_BLADE));
	o.set_property("WEAPON_CLUB", flusspferd::value(WEAPON_CLUB));
	o.set_property("WEAPON_STAFF", flusspferd::value(WEAPON_STAFF));
	o.set_property("WEAPON_SPEAR", flusspferd::value(WEAPON_SPEAR));
	o.set_property("WEAPON_AXE", flusspferd::value(WEAPON_AXE));
	o.set_property("WEAPON_CHAIN", flusspferd::value(WEAPON_CHAIN));
	o.set_property("WEAPON_BOW", flusspferd::value(WEAPON_BOW));
	o.set_property("WEAPON_LANCE", flusspferd::value(WEAPON_LANCE));

	o.set_property("LIQ_WATER", flusspferd::value(0));
	o.set_property("LIQ_BEER", flusspferd::value(1));
	o.set_property("LIQ_WINE", flusspferd::value(2));
	o.set_property("LIQ_ALE", flusspferd::value(3));
	o.set_property("LIQ_DARKALE", flusspferd::value(4));
	o.set_property("LIQ_WHISKY", flusspferd::value(5));
	o.set_property("LIQ_LEMONADE", flusspferd::value(6));
	o.set_property("LIQ_FIREBRT", flusspferd::value(7));
	o.set_property("LIQ_LOCALSPC", flusspferd::value(8));
	o.set_property("LIQ_SLIME", flusspferd::value(9));
	o.set_property("LIQ_MILK", flusspferd::value(10));
	o.set_property("LIQ_TEA", flusspferd::value(11));
	o.set_property("LIQ_COFFE", flusspferd::value(12));
	o.set_property("LIQ_BLOOD", flusspferd::value(13));
	o.set_property("LIQ_SALTWATER", flusspferd::value(14));
	o.set_property("LIQ_CLEARWATER", flusspferd::value(15));
	o.set_property("LIQ_OIL", flusspferd::value(16));

	o.set_property("MOOD_WIMPY", flusspferd::value(MOOD_WIMPY));
	o.set_property("MOOD_MILD", flusspferd::value(MOOD_MILD));
	o.set_property("MOOD_BRAVE", flusspferd::value(MOOD_BRAVE));
	o.set_property("MOOD_BERSERK", flusspferd::value(MOOD_BERSERK));


	o.set_property("CLAN_GAIDIN", flusspferd::value(1));
	o.set_property("CLAN_ANDORAN", flusspferd::value(2));
	o.set_property("CLAN_SHIENARAN", flusspferd::value(3));
	o.set_property("CLAN_MURANDIAN", flusspferd::value(4));
	o.set_property("CLAN_MERCHANT_GUILD", flusspferd::value(5));
	o.set_property("CLAN_KOBAL", flusspferd::value(7));
	o.set_property("CLAN_DHAVOL", flusspferd::value(8));
	o.set_property("CLAN_ALGHOL", flusspferd::value(9));
	o.set_property("CLAN_KNOMON", flusspferd::value(10));
	o.set_property("CLAN_GHOBHLIN", flusspferd::value(11));
	o.set_property("CLAN_CHOSEN", flusspferd::value(12));
	o.set_property("CLAN_WOLFBROTHER", flusspferd::value(13));
	o.set_property("CLAN_BLADEMASTERS", flusspferd::value(14));
	o.set_property("CLAN_BLACK_TOWER", flusspferd::value(15));
	o.set_property("CLAN_WHITE_TOWER", flusspferd::value(16));
	o.set_property("CLAN_GRAY_AJAH", flusspferd::value(17));
	o.set_property("CLAN_RED_AJAH", flusspferd::value(18));
	o.set_property("CLAN_YELLOW_AJAH", flusspferd::value(19));
	o.set_property("CLAN_BROWN_AJAH", flusspferd::value(20));
	o.set_property("CLAN_WHITE_AJAH", flusspferd::value(21));
	o.set_property("CLAN_GREEN_AJAH", flusspferd::value(22));
	o.set_property("CLAN_BLUE_AJAH", flusspferd::value(23));
	o.set_property("CLAN_BLACK_AJAH", flusspferd::value(24));
	o.set_property("CLAN_SOULLESS", flusspferd::value(CLAN_SOULLESS));
	o.set_property("CLAN_DREADGUARDS", flusspferd::value(CLAN_DREADGUARDS));
	o.set_property("CLAN_MYRDDRAAL", flusspferd::value(CLAN_MYRDDRAAL));
	o.set_property("CLAN_OGIER", flusspferd::value(CLAN_OGIER));
	o.set_property("CLANS_MAX", flusspferd::value(CLANS_MAX));

    o.set_property("VALUE_WEAPON_TYPE", flusspferd::value(0));       
    o.set_property("VALUE_IS_CORPSE", flusspferd::value(3));         
    o.set_property("VALUE_WEAPON_DAM_LOW", flusspferd::value(1));
    o.set_property("VALUE_WEAPON_DAM_HIGH", flusspferd::value(2));
    o.set_property("VALUE_WEAPON_ATTACK_TYPE", flusspferd::value(3));
    o.set_property("VALUE_WEAPON_BASH_RATING", flusspferd::value(4));
    o.set_property("VALUE_WEAPON_BACKSTAB_LOW", flusspferd::value(5));
    o.set_property("VALUE_WEAPON_BACKSTAB_HIGH", flusspferd::value(6));
    o.set_property("VALUE_WEAPON_BACKSTAB_RATING", flusspferd::value(7));
    o.set_property("VALUE_WEAPON_CHARGE_LOW", flusspferd::value(8));     
    o.set_property("VALUE_WEAPON_CHARGE_HIGH", flusspferd::value(9));    
    o.set_property("VALUE_WEAPON_CHARGE_RATING", flusspferd::value(10)); 
    o.set_property("VALUE_ARMOR_AC_APPLY", flusspferd::value(0));
    o.set_property("VALUE_POTION_SPELL_LEVEL", flusspferd::value(0));
    o.set_property("VALUE_POTION_SPELL_1", flusspferd::value(1));
    o.set_property("VALUE_POTION_SPELL_2", flusspferd::value(2));
    o.set_property("VALUE_POTION_SPELL_3", flusspferd::value(3));
    o.set_property("VALUE_CONTAINER_MAX", flusspferd::value(0));
    o.set_property("VALUE_CONTAINER_FLAGS", flusspferd::value(1));
    o.set_property("VALUE_CONTAINER_KEY", flusspferd::value(2));
    o.set_property("VALUE_CONTAINER_PICK_REQ", flusspferd::value(3));
    o.set_property("VALUE_LIGHT_HOURS", flusspferd::value(0));
    o.set_property("VALUE_LIGHT_MAX_REFILLED", flusspferd::value(1)); 
    o.set_property("VALUE_CAN_BE_REFILLED", flusspferd::value(2));
    o.set_property("VALUE_MONEY_VALUE", flusspferd::value(0));
    o.set_property("VALUE_LIQ_CONTAINER_MAX_DRINKS", flusspferd::value(0));
    o.set_property("VALUE_LIQ_CONTAINER_DRINKS", flusspferd::value(1));
    o.set_property("VALUE_LIQ_CONTAINER_LIQ_TYPE", flusspferd::value(2));
    o.set_property("VALUE_LIQ_CONTAINER_POISONED", flusspferd::value(3));
    o.set_property("VALUE_KEY_MAX_USES", flusspferd::value(0));
    o.set_property("VALUE_FOOD_HOURS_FILLED", flusspferd::value(0));
    o.set_property("VALUE_FOOD_POISONED", flusspferd::value(1));
    o.set_property("VALUE_FOUNTAIN_MAX_DRINKS", flusspferd::value(0));
    o.set_property("VALUE_FOUNTAIN_DRINKS", flusspferd::value(1));
    o.set_property("VALUE_FOUNTAIN_LIQ_TYPE", flusspferd::value(2));
    o.set_property("VALUE_FOUNTAIN_POISONED", flusspferd::value(3));
    o.set_property("VALUE_ANGREAL_COST", flusspferd::value(0));
    o.set_property("VALUE_ANGREAL_DAMAGE", flusspferd::value(1)); 
    o.set_property("VALUE_ANGREAL_NUMBER_CHARGES", flusspferd::value(2));
    o.set_property("CONTAINER_CLOSABLE", flusspferd::value(1 << 0));
    o.set_property("CONTAINER_CLOSED", flusspferd::value(1 << 1));
    o.set_property("CONTAINTER_LOCKED", flusspferd::value(1 << 2));

	o.set_property("ARENA_BLUE", flusspferd::value(ARENA_BLUE));
	o.set_property("ARENA_RED", flusspferd::value(ARENA_RED));
	o.set_property("ARENA_YELLOW", flusspferd::value(ARENA_YELLOW));
	o.set_property("ARENA_GREEN", flusspferd::value(ARENA_GREEN));

	o.set_property("APPLY_NONE", flusspferd::value(APPLY_NONE));
	o.set_property("APPLY_STR", flusspferd::value(APPLY_STR));
	o.set_property("APPLY_DEX", flusspferd::value(APPLY_DEX));
	o.set_property("APPLY_INT", flusspferd::value(APPLY_INT));
	o.set_property("APPLY_WIS", flusspferd::value(APPLY_WIS));
	o.set_property("APPLY_CON", flusspferd::value(APPLY_CON));
	o.set_property("APPLY_CHA", flusspferd::value(APPLY_CHA));
	o.set_property("APPLY_CLASS", flusspferd::value(APPLY_CLASS));
	o.set_property("APPLY_LEVEL", flusspferd::value(APPLY_LEVEL));
	o.set_property("APPLY_AGE", flusspferd::value(APPLY_AGE));
	o.set_property("APPLY_CHAR_WEIGHT", flusspferd::value(APPLY_CHAR_WEIGHT));
	o.set_property("APPLY_CHAR_HEIGHT", flusspferd::value(APPLY_CHAR_HEIGHT));
	o.set_property("APPLY_MANA", flusspferd::value(APPLY_MANA));
	o.set_property("APPLY_HIT", flusspferd::value(APPLY_HIT));
	o.set_property("APPLY_MOVE", flusspferd::value(APPLY_MOVE));
	o.set_property("APPLY_GOLD", flusspferd::value(APPLY_GOLD));
	o.set_property("APPLY_EXP", flusspferd::value(APPLY_EXP));
	o.set_property("APPLY_DB", flusspferd::value(APPLY_DB));
	o.set_property("APPLY_HITROLL", flusspferd::value(APPLY_HITROLL));
	o.set_property("APPLY_DAMROLL", flusspferd::value(APPLY_DAMROLL));
	o.set_property("APPLY_RACE", flusspferd::value(APPLY_RACE));

	return o;
}

namespace JS
{
	const char *js_trig_types[] = {
		"Random [All]",
		"Interval [All]",
		"Command [All]",
		"Speech [All]",
		"ACT() [All]",
		"Enter [All]",
		"Leave [All]",
		"Load [All]",
		"Time [All]",
		"Character Death [PC|M]",
		"Character Receive [PC|M]",
		"Character Fight [PC|M]",
		"Character HP Percent [PC|M]",
		"Character Greet (Seen) [PC|M]",
		"Mob Bribe [M]",
		"Mob Memory [M]",
		"Object Get [O]",
		"Object Drop [O]",
		"Object Give [O]",
		"Object Wear [O]",
		"Object Remove [O]",
		"Zone Reset [W]",
		"Dropped In Room [W]",
		"Extraction [C|O]",
		"Login [PC]",
		"Tell [PC|M]",
		"\n"
	};
    const char *js_allow[] = {
        "Equipped Objects",
        "Objects In Inventory",
        "Objects In Room",
        "Actor is self",
        "Actor is not self",
        "Players",
        "Mobs",
        "\n"
    };
	const char *js_options[] = {
		"Alias Override",
		"\n"
	};
}

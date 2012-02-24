//
// C++ Interface: JSObject
//
// Description:
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JSOBJECT_H
#define KINSLAYER_JSOBJECT_H

#ifdef KINSLAYER_JAVASCRIPT

#include <flusspferd.hpp>
#include <string>
#include <iostream>
#include "structs.h"
#include "utils.h"
#include "js_utils.h"

class JSCharacter;
class JSRoom;
class JSObject;

#include "JSCharacter.h"
#include "JSRoom.h"

using namespace std;

FLUSSPFERD_CLASS_DESCRIPTION(
    JSObject,
    (full_name, "JSObject")
    (constructor_name, "JSObject")
    (methods,
        ("value", bind, value)                            /* Values of the item (see list)                    */
        ("canWear", bind, can_wear)   /* Where you can wear it                            */
        ("extraFlags", bind, extra_flags)           /* If it hums, glows, etc.                          */
        ("bitvector", bind, bitvector)         /* To set chars bits                    */
		("moveToRoom", bind, move_to_room)
		("moveToChar", bind, move_to_char)
		("moveToObj", bind, move_to_obj)
		("loadObj", bind, load_obj)
		("extract", bind, extract)
		("open", bind, open)
		("close", bind, close)
		("lock", bind, lock)
		("unlock", bind, unlock)
		("attach", bind, attach)
		("detach", bind, detach)
		("countJS", bind, countJS)
		("getObjVal", bind, getObjVal)
		("setObjVal", bind, setObjVal)
		("setRetoolName", bind, setRetoolName)
		("setRetoolDesc", bind, setRetoolDesc)
		("setRetoolSDesc", bind, setRetoolSDesc)
		("setRetoolExDesc", bind, setRetoolExDesc)
		("isRetooled", bind, isRetooled)
		)
    (properties,
		("id", getter, getID)
		("max", getter, getMax)
		("isCorpse", getter, getIsCorpse)
		("hidden", getter, getHidden)
		("count", getter, getCount)
		("satOnBy", getter, getSatOnBy)
		("findHolder", getter, getFindHolder)
		("inRoom", getter, getInRoom)
        ("name", getter, getName)
		("namelist", getter, getNameList)
		("ldesc", getter, getLongDescription)
		("wornBy", getter, worn_by)
		("containedBy", getter, contained_by)
		("contents", getter, contents)
		("carriedBy", getter, carried_by)
		("weight", getter, weight)
        ("cost", getter, cost)
        ("costPerDay", getter, cost)
        ("type", getter, type)                        /* Type of item                                     */
        ("timer", getter, timer)                               /* Timer for object                                     */
        ("offensive", getter, offensive)                           /* OB addition                                      */
        ("parry", getter, parry)                               /* PB addition                                      */
        ("dodge", getter, dodge)                               /* DB addition                                      */
        ("absorb", getter, absorb)                              /* ABS addition                                     */
		("clan", getter, clan)									/* Object's clan affiliation (if any)				*/
		("vnum", getter, vnum)
		("canOpen", getter, canOpen)
		("canLock", getter, canLock)
		("isContainer", getter, isContainer)
		("isClosed", getter, isClosed)
		("isOpen", getter, isOpen)
		("isLocked", getter, isLocked)
		("isPickProof", getter, isPickProof)
		("decayType", getter, decayType)
		("decayTimer", getter, decayTimer)
		("decayTimerType", getter, decayTimerType)
		("isValid", getter, getIsValid)
		("scalpLevel", getter, getScalpLevel)
		("scalpRace", getter, getScalpRace)
		("isScalp", getter, getIsScalp)
		("isPlayerScalp", getter, getIsPlayerScalp)
		))
{
public:
    JSObject(flusspferd::object const &self, flusspferd::call_context& cc)
        : base_type(self)
    {
//        std::cout << "Creating JSObject: Real: -" << std::endl;
    }

    ~JSObject() {
//        std::cout << "Destroying JSObject" << std::endl;
    }

    JSObject(flusspferd::object const &self, Object* _real)
        : base_type(self)
    {
        real = _real;
//        std::cout << "Creating JSObject: Real: " << real << std::endl;
    }

	Object* toReal() { return real; }
	void setReal( Object *o ) { real = o; }
    int value(int index)
    {
        if (!real || index >= 11 || index < 0)
            return -1;
        else
            return real->obj_flags.value[index];
    }

    bool can_wear(int flag)
    {
		return ( real ? CAN_WEAR(real, flag) : false );
    }

    bool extra_flags(int flag)
    {
		return ( real ? OBJ_FLAGGED(real, flag) : false );
    }

    bool bitvector(int flag)
    {
		return ( real ? IS_SET_AR(real->obj_flags.bitvector, flag) : false );
    }
	void move_to_room(JSRoom *dest);
	void move_to_char(JSCharacter *dest);
	void move_to_obj(JSObject *dest);

	flusspferd::value load_obj( const int vnum );
	flusspferd::array contents();
	flusspferd::value contained_by();
	flusspferd::value worn_by();
	flusspferd::value getSatOnBy();
	flusspferd::value getFindHolder();
	flusspferd::value getInRoom();

	flusspferd::value carried_by();
	double weight()
	{
		return ( real ? real->Weight() : 0 );
	}
	flusspferd::string getNameList()
	{
		return flusspferd::string( (real ? ( real->getName() ) : "Invalid") );
	}
	flusspferd::string getLongDescription()
	{
		return flusspferd::string( (real ? ( real->retool_desc ? real->retool_desc : real->description ) : "Invalid") );
	}
    flusspferd::string getName()
    {
		return flusspferd::string( (real ? ( real->retool_sdesc ? real->retool_sdesc : real->short_description ) : "Invalid") );
    }

	int cost()			{ return (real ? real->obj_flags.cost : 0); }
	int cost_per_day()	{ return (real ? real->obj_flags.cost_per_day : 0); }
	int timer()			{ return (real ? real->obj_flags.timer : 0); }
	int offensive()		{ return (real ? real->obj_flags.offensive : 0); }
	int parry()			{ return (real ? real->obj_flags.parry : 0); }
	int dodge()			{ return (real ? real->obj_flags.dodge : 0); }
	int absorb()		{ return (real ? real->obj_flags.absorb : 0); }
	int type()			{ return (real ? real->obj_flags.type_flag : 0); }
	int clan()			{ return (real ? GET_OBJ_CLAN(real) : 0); }
	int vnum()			{ return (real ? real->getVnum() : 0); }
	int getMax()		{ return (real ? real->Max : 0); }
	int getHidden()		{ return (real ? real->hidden : 0 ); }
	int decayType()		{ return (real ? real->decayType : -1); }
	int decayTimer()	{ return (real ? real->decayTimer : -1); }
	int decayTimerType() { return (real ? real->decayTimerType : -1); }
	bool getIsValid()	{ return (real && !real->IsPurged()); }
	bool isRetooled()	{ return (real && real->IsRetooled()); }
	bool getIsCorpse();
	int getCount();
	flusspferd::string getID();

	void extract();

	bool canOpen();
	bool canLock();
	bool isContainer();
	bool isClosed();
	bool isOpen();
	bool isLocked();
	bool isPickProof();

	void open();
	void close();
	void lock();
	void unlock();
	void attach( int tVnum );
	void detach( int tVnum, int nr );
	int countJS( int tVnum );

	void setObjVal( int x, int v );
	int getObjVal( int x );

	void setRetoolName( flusspferd::string newName );
	void setRetoolDesc( flusspferd::string newName );
	void setRetoolSDesc( flusspferd::string newName );
	void setRetoolExDesc( flusspferd::string newExDesc );

	int getScalpLevel();
	int getScalpRace();
	bool getIsScalp();
	bool getIsPlayerScalp();

private:
    Object * real;
};

#endif
#endif

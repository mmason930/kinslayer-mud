//
// C++ Interface: JSRoom
//
// Description:
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JSROOM_H
#define KINSLAYER_JSROOM_H

#ifdef KINSLAYER_JAVASCRIPT

#include <flusspferd.hpp>

#include <string>
#include <iostream>
#include "comm.h"
#include "structs.h"
#include "db.h"
#include "js_utils.h"
#include "handler.h"
#include "zones.h"

class JSCharacter;
class JSRoom;
class JSObject;

void sendToZone(const char *messg, int zone_rnum);

using namespace flusspferd;

using namespace std;

FLUSSPFERD_CLASS_DESCRIPTION(
    JSRoom,
    (full_name, "JSRoom")
    (constructor_name, "JSRoom")
    (methods,
        ("echo", bind, echo)
		("echoaround", bind, echoaround)
		("loadObj", bind, load_obj)
		("loadMob", bind, load_mob)
		("zecho", bind, zecho)
		("zreset", bind, zreset)
		("direction", bind, direction)
		("doorName", bind, doorName)
		("doorHidden", bind, doorHidden)
		("doorFlags", bind, getDoorFlags)
		("doorPick", bind, doorPick)
		("doorKey", bind, doorKey)
		("doorDesc", bind, doorDesc)
		("doorExists", bind, doorExists)
		("doorIsLocked", bind, doorIsLocked)
		("doorIsClosed", bind, doorIsClosed)
		("doorIsRammable", bind, doorIsRammable)
		("distanceTo", bind, distanceTo)
		("firstStep", bind, firstStep)
		("setDoorFlags", bind, setDoorFlags)
		("attach", bind, attach)
		("detach", bind, detach)
		("countJS", bind, countJS)
		("digTo", bind, digTo)
		("killExit", bind, killExit)
		("disableExit", bind, disableExit)
		("enableExit", bind, enableExit)
		("isFlagged", bind, isFlagged)
		("pathToRoom", bind, pathToRoom)
		)
    (properties,
        ("name", getter, getName)
		("description", getter, getDescription)
		("neighbors", getter, neighbors)
		("people", getter, people)
		("vnum", getter, vnum)
		("sector", getter, sector)
		("items", getter, items)
		("dark", getter, dark)
		("zoneVnum", getter, zoneVnum)
		("zoneName", getter, zoneName)
	))
{
public:
    JSRoom(flusspferd::object const &self, flusspferd::call_context& cc)
        : base_type(self)
    {
//        std::cout << "Creating JSRoom: Real: -" << std::endl;
    }

    ~JSRoom() {
//       std::cout << "Destroying JSRoom" << std::endl;
    }

    JSRoom(flusspferd::object const &self, Room* _real)
        : base_type(self)
    {
        real = _real;
 //       std::cout << "Creating JSRoom: Real: " << real << std::endl;
    }

	Room* toReal() { return real; }
	void setReal( Room *r ) { real = r; }

	int vnum() { return (real ? real->vnum : (-1)); }
	int sector() { return (real ? real->sector_type : 0); }
	int zoneVnum() { return (real ? real->GetZone()->getVnum() : -1); }
	bool dark() { return (real ? real->IsDark() : false); }
	flusspferd::string zoneName() { return (real ? real->GetZone()->getName() : "undefined"); }

	flusspferd::value direction(int dir)
	{
		return lookupValue((real ? real->GetNeighbor(dir) : real));
	}
	flusspferd::array neighbors()
	{
		flusspferd::array a = flusspferd::create_array(NUM_OF_DIRS);
		for(unsigned int i = 0;i < NUM_OF_DIRS;++i)
		{
			if( real != 0 && real->dir_option[i] != 0 && real->dir_option[i]->to_room != 0 )
				a.set_element(i, lookupValue(real->dir_option[i]->to_room));
			else
				a.set_element(i, lookupValue( (Room*)0 ));
		}
		return a;
	}

	flusspferd::array people();

	flusspferd::array items();

	flusspferd::array pathToRoom(JSRoom *otherRoom);

    void echo(flusspferd::string message)
    {
		if( real ) {
	        sendToRoom(flusspferd::string::concat(message, "\r\n").c_str(), real);
		}
    }

	void echoaround(JSCharacter& ch, std::string message);

	void zecho(flusspferd::string message)
	{
		if( real ) {
			sendToZone(flusspferd::string::concat(message, "\r\n").c_str(), real->zone);
		}
	}

	void zreset()
	{
		if( real ) {
			real->GetZone()->Reset();
		}
	}
	flusspferd::value load_obj( const int vnum );
	flusspferd::value load_mob( const int vnum );
	flusspferd::string doorName( const int dir );
	int doorHidden( const int dir );
	int getDoorFlags( const int dir );
	void setDoorFlags( const int dir, const int v );
	int doorPick( const int dir );
	int doorKey( const int dir );
	flusspferd::string doorDesc( const int dir );
	bool doorExists( const int dir );
	bool doorIsLocked( const int dir );
	bool doorIsClosed( const int dir );
	bool doorIsRammable( const int dir );
	void attach( int tVnum );
	void detach( int tVnum, int nr );
	int countJS( int tVnum );
	int distanceTo( JSRoom *d );
	int firstStep( JSRoom *d);
	void digTo( int dir, JSRoom *d, bool bothSides, bool temporary );
	void killExit( int dir, bool bothSides );
	void enableExit( int dir, bool bothSides );
	void disableExit( int dir, bool bothSides );
	bool isFlagged( const int flag );

    flusspferd::string getName()
    {
		if( real ) {
	        return flusspferd::string(real->name);
		} else {
			return flusspferd::string("Invalid");
		}
    }
	flusspferd::string getDescription()
	{
		return flusspferd::string(real ? real->description : "Invalid");
	}
private:
    Room * real;
};


#endif
#endif

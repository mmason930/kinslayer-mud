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
		("loadObj", bind, loadObj)
		("loadMob", bind, loadMob)
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
		("roomFlagged", bind, roomFlagged)
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
	JSRoom(flusspferd::object const &self, flusspferd::call_context& cc);
	JSRoom(flusspferd::object const &self, Room* real);
	~JSRoom();

	Room* toReal();
	void setReal(Room *r);

	int vnum();
	int sector();
	int zoneVnum();
	bool dark();
	flusspferd::string zoneName();
	flusspferd::string doorName(const int dir);
	flusspferd::string getName();
	flusspferd::string getDescription();

	flusspferd::value direction(int dir);
	flusspferd::array neighbors();
	flusspferd::array people();
	flusspferd::array items();
	flusspferd::array pathToRoom(JSRoom *otherRoom);

	bool roomFlagged(const int flag);
	void echo(flusspferd::string message);
	void echoaround(JSCharacter& ch, std::string message);
	void zecho(flusspferd::string message);
	void zreset();

	flusspferd::value loadObj( const int vnum );
	flusspferd::value loadMob( const int vnum );


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
	int countJS( flusspferd::value tVnum );
	int distanceTo( JSRoom *d );
	int firstStep( JSRoom *d);
	void digTo( int dir, JSRoom *d, bool bothSides, bool temporary );
	void killExit( int dir, bool bothSides );
	void enableExit( int dir, bool bothSides );
	void disableExit( int dir, bool bothSides );
	bool isFlagged( const int flag );

private:
    Room * real;
};


#endif

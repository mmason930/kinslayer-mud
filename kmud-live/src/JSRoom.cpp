#include <flusspferd.hpp>

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "mobs.h"
#include "db.h"
#include "olc.h"
#include "constants.h"

#include "JSRoom.h"
#include "JSCharacter.h"
#include "JSObject.h"
#include "js_functions.h"
#include "StringUtil.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "rooms/Exit.h"

extern std::vector<Object*> obj_proto;

#include "js_interpreter.h"

void JSEnvironment::LoadJSRoom()
{
	load_class<JSRoom>();
}

void JSRoom::attach( int tVnum )
{
	if( !real ) return;
	real->attachJS( tVnum );

}
void JSRoom::detach( int tVnum, int nr )
{
	if( !real ) return;
	real->detachJS( tVnum, nr );
}

void JSRoom::digTo( int dir, JSRoom *d, bool bothSides, bool temporary )
{
	if( dir < 0 || dir >= NUM_OF_DIRS || !real || !d->toReal() ) return;

	if( real->dir_option[ dir ] != NULL )
		delete real->dir_option[ dir ];
	real->dir_option[ dir ] = new Exit();
	real->dir_option[ dir ]->setToRoom(d->toReal());

	if( bothSides && d->toReal()->dir_option[ rev_dir[ dir ] ] )
	{
		delete d->toReal()->dir_option[ rev_dir[ dir ] ];
		d->toReal()->dir_option[ rev_dir[ dir ] ] = new Exit();
		d->toReal()->dir_option[ rev_dir[ dir ] ]->setToRoom(real);
	}

}
void JSRoom::killExit( int dir, bool bothSides )
{
	if( dir < 0 || dir >= NUM_OF_DIRS ) return;

	if (bothSides && real->getNeighbor(dir))
	{
		if (real->getNeighbor(dir)->getNeighbor(rev_dir[dir]) == real)
		{
			delete real->getNeighbor(dir)->dir_option[rev_dir[dir]];
			real->getNeighbor(dir)->dir_option[rev_dir[dir]] = NULL;
		}
	}
	if( real->dir_option[ dir ] != NULL )
	{
		delete real->dir_option[ dir ];
		real->dir_option[ dir ] = NULL;
	}
}
void JSRoom::disableExit( int dir, bool bothSides )
{
	if (!real || !real->getNeighbor(dir))
		return;
	real->dir_option[ dir ]->disable();
	if (bothSides && real->getNeighbor(dir)->getNeighbor(rev_dir[dir]) == real)
		real->getNeighbor(dir)->dir_option[rev_dir[dir]]->disable();
}
void JSRoom::enableExit( int dir, bool bothSides )
{
	if (!real || !real->getNeighbor(dir))
		return;
	real->dir_option[ dir ]->enable();
	if (bothSides && real->getNeighbor(dir)->getNeighbor(rev_dir[dir]) == real)
		real->getNeighbor(dir)->dir_option[rev_dir[dir]]->enable();
}

int JSRoom::countJS( flusspferd::value tVnum )
{
	if( !real ) return 0;
	return real->countJS( tVnum.is_undefined() ? 0 : (int)tVnum.to_number() );
}
flusspferd::array JSRoom::people()
{
    Character* ch;
    flusspferd::array a = flusspferd::create_array();
	if( real ) {
	    for (ch = real->people; ch; ch = ch->next_in_room)
		{
		    a.call("push", lookupValue(ch) );
		}
	}
    return a;
}

flusspferd::array JSRoom::items()
{
    Object * o;
    flusspferd::array a = flusspferd::create_array();
	if( real ) {
	    for (o = real->contents; o; o = o->next_content)
	    {
		    a.call("push", lookupValue(o) );
		}
	}
    return a;
}

flusspferd::string JSRoom::doorName( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->getKeywords();
	return "";
}
int JSRoom::doorHidden( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->getHiddenLevel();
	return 0;
}
int JSRoom::getDoorFlags( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->getExitInfo();
	return 0;
}
void JSRoom::setDoorFlags( const int dir, const int v )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		real->dir_option[dir]->setExitInfo(v);
}
int JSRoom::doorPick( const int dir )
{
	if( real && dir >= 0 && dir <= NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->getPickRequirement();
	return 0;
}
int JSRoom::doorKey( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->getKey();
	return 0;
}
flusspferd::string JSRoom::doorDesc( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->getGeneralDescription();
	return "";
}
bool JSRoom::doorExists( const int dir )
{
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->isDoor());
}
bool JSRoom::doorIsLocked( const int dir )
{
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->isLocked());
}
bool JSRoom::doorIsClosed( const int dir )
{	
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->isClosed());
}
bool JSRoom::doorIsRammable( const int dir )
{
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->isRammable());
}
bool JSRoom::isFlagged( const int flag )
{
	return (real && (IS_SET(ROOM_FLAGS(real), (1<<flag))));
}
int JSRoom::distanceTo( JSRoom *d )
{
	if( real && d->toReal() )
		return real->getDistanceToRoom( d->toReal() );
	return -1;
}
int JSRoom::firstStep( JSRoom *d )
{
	if( real && d->toReal() )
		return real->findFirstStep( d->toReal() );
	return -1;
}
void JSRoom::echoaround(JSCharacter& ch, std::string message)
{
	if( real ) {
		for(Character *p = real->people;p;p = p->next_in_room) {
			if( p != ch.toReal() ) {
				p->send( StringUtil::vaEscape(message).c_str() );
			}
		}
	}
}

flusspferd::value JSRoom::loadObj( const int vnum )
{
	if( !real ) return lookupValue(0);

	int r_num;
	Object *obj;
	if ((r_num = real_object(vnum)) < 0)
	{
		return lookupValue(0);
	}

	if( obj_proto[r_num]->getType() == ITEM_SPECIAL )
	{
		lookupValue(0);
	}

	obj = read_object(r_num, REAL, true);
	sprintf(obj->creator, "Loaded by Javascript. JSRoom #%d.", this->real->getVnum());

	obj->MoveToRoom(real);
	if( !obj->IsPurged() ) {
		js_load_triggers(obj);
	}

	return lookupValue(obj);
}

flusspferd::value JSRoom::loadMob( const int vnum )
{
	if( !real ) return lookupValue(0);
	int r_num;
	Character *mob;
	if ((r_num = MobManager::GetManager().RealMobile(vnum)) < 0)
	{
		return lookupValue(0);
	}
	mob = new Character(r_num, REAL);
	mob->MoveToRoom(real);

	if( !mob->IsPurged() ) {
		js_load_triggers(mob);
	}
	return lookupValue(mob);
}

flusspferd::array JSRoom::pathToRoom( JSRoom *otherRoom )
{
	std::list<int> thePath;
	flusspferd::array path = flusspferd::create_array();

	if( real && otherRoom->toReal() )
	{
		thePath = real->pathToRoom( otherRoom->toReal() );
		for( std::list<int>::iterator i = thePath.begin(); i != thePath.end(); i++ )
			if( *i >= 0 )
				path.call("push", *i );
	}
	return path;
}

bool JSRoom::roomFlagged(const int flag)
{
	return ROOM_FLAGGED(real, flag);
}

Room* JSRoom::toReal()
{
	return real;
}

void JSRoom::setReal(Room *r)
{
	real = r;
}

int JSRoom::vnum()
{
	return (real ? real->getVnum() : (-1));
}

int JSRoom::sector()
{
	return (real ? real->getSector()->getValue() : 0);
}

int JSRoom::zoneVnum()
{
	return (real ? real->getZone()->getVnum() : -1);
}

bool JSRoom::dark()
{
	return (real ? real->isDark() : false);
}

flusspferd::string JSRoom::zoneName()
{
	return (real ? real->getZone()->getName() : "undefined");
}

flusspferd::value JSRoom::direction(int dir)
{
	return lookupValue((real ? real->getNeighbor(dir) : real));
}

flusspferd::array JSRoom::neighbors()
{
	flusspferd::array a = flusspferd::create_array(NUM_OF_DIRS);
	for (unsigned int i = 0; i < NUM_OF_DIRS; ++i)
	{
		if (real != 0 && real->dir_option[i] != 0 && real->dir_option[i]->getToRoom() != 0)
			a.set_element(i, lookupValue(real->dir_option[i]->getToRoom()));
		else
			a.set_element(i, lookupValue((Room*)0));
	}
	return a;
}

void JSRoom::echo(flusspferd::string message)
{
	if (real) {
		sendToRoom(flusspferd::string::concat(message, "\r\n").c_str(), real);
	}
}

void JSRoom::zecho(flusspferd::string message)
{
	if (real) {
		sendToZone(flusspferd::string::concat(message, "\r\n").c_str(), real->getZoneNumber());
	}
}

void JSRoom::zreset()
{
	if (real) {
		real->getZone()->Reset();
	}
}

flusspferd::string JSRoom::getName()
{
	if (real) {
		return flusspferd::string(real->getName());
	}
	else {
		return flusspferd::string("Invalid");
	}
}

flusspferd::string JSRoom::getDescription()
{
	return flusspferd::string(real ? real->description : "Invalid");
}

JSRoom::JSRoom(flusspferd::object const &self, flusspferd::call_context& cc) : base_type(self)
{
}

JSRoom::~JSRoom() {}

JSRoom::JSRoom(flusspferd::object const &self, Room *real) : base_type(self)
{
	this->real = real;
}
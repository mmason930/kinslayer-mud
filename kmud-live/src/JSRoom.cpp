

#ifdef KINSLAYER_JAVASCRIPT

#include "JSRoom.h"

#include "JSCharacter.h"
#include "JSObject.h"
#include "structs.h"
#include "mobs.h"
#include "db.h"
#include "olc.h"
#include "constants.h"

#include "js_functions.h"
#include "StringUtil.h"

extern std::list< Direction* > TemporaryDirections;
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
	real->dir_option[ dir ] = new Direction();
	real->dir_option[ dir ]->to_room = d->toReal();

	if( bothSides && d->toReal()->dir_option[ rev_dir[ dir ] ] )
	{
		delete d->toReal()->dir_option[ rev_dir[ dir ] ];
		d->toReal()->dir_option[ rev_dir[ dir ] ] = new Direction();
		d->toReal()->dir_option[ rev_dir[ dir ] ]->to_room = real;
	}

}
void JSRoom::killExit( int dir, bool bothSides )
{
	if( dir < 0 || dir >= NUM_OF_DIRS ) return;

	if( bothSides && real->GetNeighbor( dir ) )
	{
		if( real->GetNeighbor( dir )->GetNeighbor( rev_dir[ dir ] ) == real )
		{
			delete real->GetNeighbor( dir )->dir_option[ rev_dir[ dir ] ];
			real->GetNeighbor( dir )->dir_option[ rev_dir[ dir ] ] = NULL;
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
	if( !real || !real->GetNeighbor( dir ) )
		return;
	real->dir_option[ dir ]->Disable();
	if( bothSides && real->GetNeighbor( dir )->GetNeighbor( rev_dir[ dir ] ) == real )
		real->GetNeighbor( dir )->dir_option[ rev_dir[ dir ] ]->Disable();
}
void JSRoom::enableExit( int dir, bool bothSides )
{
	if( !real || !real->GetNeighbor( dir ) )
		return;
	real->dir_option[ dir ]->Enable();
	if( bothSides && real->GetNeighbor( dir )->GetNeighbor( rev_dir[ dir ] ) == real )
		real->GetNeighbor( dir )->dir_option[ rev_dir[ dir ] ]->Enable();
}

int JSRoom::countJS( int tVnum )
{
	if( !real ) return 0;
	return real->countJS( tVnum );
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
		return real->dir_option[dir]->keyword;
	return "";
}
int JSRoom::doorHidden( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->hidden;
	return 0;
}
int JSRoom::getDoorFlags( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->exit_info;
	return 0;
}
void JSRoom::setDoorFlags( const int dir, const int v )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		real->dir_option[dir]->exit_info = v;
}
int JSRoom::doorPick( const int dir )
{
	if( real && dir >= 0 && dir <= NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->PickReq;
	return 0;
}
int JSRoom::doorKey( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->key;
	return 0;
}
flusspferd::string JSRoom::doorDesc( const int dir )
{
	if( real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] )
		return real->dir_option[dir]->general_description;
	return "";
}
bool JSRoom::doorExists( const int dir )
{
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->IsDoor());
}
bool JSRoom::doorIsLocked( const int dir )
{
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->IsLocked());
}
bool JSRoom::doorIsClosed( const int dir )
{	
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->IsClosed());
}
bool JSRoom::doorIsRammable( const int dir )
{
	return (real && dir >= 0 && dir < NUM_OF_DIRS && real->dir_option[dir] && real->dir_option[dir]->IsRammable());
}
bool JSRoom::isFlagged( const int flag )
{
	return (real && (IS_SET(ROOM_FLAGS(real), (1<<flag))));
}
int JSRoom::distanceTo( JSRoom *d )
{
	if( real && d->toReal() )
		return real->DistanceToRoom( d->toReal() );
	return -1;
}
int JSRoom::firstStep( JSRoom *d )
{
	if( real && d->toReal() )
		return real->FindFirstStep( d->toReal() );
	return -1;
}
void JSRoom::echoaround(JSCharacter& ch, std::string message)
{
	if( real ) {
		for(Character *p = real->people;p;p = p->next_in_room) {
			if( p != ch.toReal() ) {
				p->Send( StringUtil::vaEscape(message).c_str() );
			}
		}
	}
}

flusspferd::value JSRoom::load_obj( const int vnum )
{
	if( !real ) return lookupValue(0);

	int r_num;
	Object *obj;
	if ((r_num = real_object(vnum)) < 0)
	{
		return lookupValue(0);
	}

	if( GET_OBJ_TYPE(obj_proto[r_num]) == ITEM_SPECIAL )
	{
		lookupValue(0);
	}

	obj = read_object(r_num, REAL, true);
	sprintf(obj->creator, "Loaded by Javascript");

	obj->MoveToRoom(real);
#ifdef KINSLAYER_JAVASCRIPT
	if( !obj->IsPurged() ) {
		js_load_triggers(obj);
	}
#endif

	return lookupValue(obj);
}
flusspferd::value JSRoom::load_mob( const int vnum )
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

#ifdef KINSLAYER_JAVASCRIPT
	if( !mob->IsPurged() ) {
		js_load_triggers(mob);
	}
#endif
	return lookupValue(mob);
}
flusspferd::array JSRoom::pathToRoom( JSRoom *otherRoom )
{
	std::list<int> thePath;
	flusspferd::array path = flusspferd::create_array();

	if( real && otherRoom->toReal() )
	{
		thePath = real->PathToRoom( otherRoom->toReal() );
		for( std::list<int>::iterator i = thePath.begin(); i != thePath.end(); i++ )
			if( *i >= 0 )
				path.call("push", *i );
	}
	return path;
}
#endif

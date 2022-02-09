#include "../conf.h"

#include "JSRoom.h"

#include "js_functions.h"
#include "js_interpreter.h"
#include "../rooms/Room.h"

void JSEnvironment::LoadJSObject()
{
	load_class<JSObject>();
}

extern std::vector<Object*> obj_proto;
void JSObject::attach( int tVnum )
{
	if( !real || real->IsProto() ) return;
	real->attachJS( tVnum );

}
void JSObject::detach( int tVnum, int nr )
{
	if( !real || real->IsProto() ) return;
	real->detachJS( tVnum, nr );
}
int JSObject::countJS( flusspferd::value tVnum )
{
	if( !real ) return 0;
	return real->countJS( tVnum.is_undefined() ? 0 : (int)tVnum.to_number() );
}
flusspferd::array JSObject::contents()
{
    vector<Object*> items;
    Object * o;
    flusspferd::array a = flusspferd::create_array();
	if( real ) {
	    for (o = real->contains; o; o = o->next_content)
		{
			a.call("push", lookupValue(o) );
		}
	}

    return a;
}
flusspferd::string JSObject::getID()
{
	return (real ? ToString(real->objID) : "");
}

flusspferd::value JSObject::contained_by()
{
	return lookupValue( (real ? real->in_obj : (0)) );
}

flusspferd::value JSObject::worn_by()
{
    return lookupValue( (real ? real->worn_by : (0)) );
}

flusspferd::value JSObject::carried_by()
{
    return lookupValue( (real ? real->carried_by : (0)) );
}
flusspferd::value JSObject::getSatOnBy()
{
	return lookupValue( (real ? real->SatOnBy : (0)) );
}
flusspferd::value JSObject::getFindHolder()
{
	return lookupValue( (real ? real->FindHolder() : (0)) );
}
flusspferd::value JSObject::getInRoom()
{
	return lookupValue( real ? real->in_room : (0) );
}
int JSObject::getCount()
{
	if( !real ) return 0;
	return ((int) ItemCount.size() > GET_OBJ_RNUM(real) ? ItemCount[GET_OBJ_RNUM(real)] : 1);
}
bool JSObject::getIsCorpse()
{
	if( !real || real->IsProto() ) return false;
	return IS_CORPSE(real);
}

flusspferd::value JSObject::load_obj( const int vnum )
{
	if( !real || real->IsProto() ) return lookupValue(0);

	int r_num;
	Object *obj;
	if ((r_num = real_object(vnum)) < 0)
	{
		return lookupValue(0);
	}

	if (obj_proto[r_num]->getType() == ITEM_SPECIAL)
	{
		lookupValue(0);
	}

	obj = read_object(r_num, REAL, true);
	Room *inRoom = this->real->getRoom();
	sprintf(obj->creator, "Loaded by Javascript. JSObject #%d. Room #%d.", this->real->getVnum(), (inRoom ? inRoom->getVnum() : -1));

	obj_to_obj(obj,real);

	if( !obj->IsPurged() ) {
		js_load_triggers(obj);
	}

	return lookupValue(obj);
}

void JSObject::extract()
{
	if( !real || real->IsProto() )
		return;
	real->Extract(true);
}

bool JSObject::canOpen()
{
	return (real ? real->CanOpen() : false);
}
bool JSObject::canLock()
{
	return (real ? real->CanLock() : false);
}
bool JSObject::isPickProof()
{
	return (real ? real->IsPickProof() : false);
}
bool JSObject::isContainer()
{
	return (real ? real->IsContainer() : false);
}
bool JSObject::isClosed()
{
	return (real ? real->IsClosed() : false);
}
bool JSObject::isOpen()
{
	return (real ? real->IsOpen() : false);
}
bool JSObject::isLocked()
{
	return (real ? real->IsLocked() : false);
}

void JSObject::open()
{
	if( real && !real->IsProto() ) {
		REMOVE_BIT(GET_OBJ_VAL(real, 1), CONT_CLOSED);
	}
}
void JSObject::close()
{
	if( real && !real->IsProto() ) {
		SET_BITK(GET_OBJ_VAL(real, 1), CONT_CLOSED);
	}
}
void JSObject::lock()
{
	if( real && !real->IsProto() ) {
		SET_BITK(GET_OBJ_VAL(real, 1), CONT_LOCKED);
	}
}
void JSObject::unlock()
{
	if( real && !real->IsProto() ) {
		REMOVE_BIT(GET_OBJ_VAL(real, 1), CONT_LOCKED);
	}
}


int JSObject::getObjVal( int x )
{
	return ( (real && !real->IsProto() && x >= 0 && x <= 11) ? GET_OBJ_VAL(real, x) : 0 );
}

void JSObject::setObjVal( int x, int v )
{
	if ( (real && !real->IsProto() && x >= 0 && x <= 11) )
		GET_OBJ_VAL(real, x) = v;
}

void JSObject::setRetoolName( flusspferd::string newName )
{
	if( !real || real->IsProto() )
		return;
	if( real->retool_name )
		delete[] real->retool_name;
	if( newName.empty() )
		real->retool_name = NULL;
	else
		real->retool_name = str_dup( newName.c_str() );
}

void JSObject::setRetoolDesc( flusspferd::string newName )
{
	if( !real || real->IsProto() )
		return;
	if( real->retool_desc )
		delete[] real->retool_desc;
	if( newName.empty() )
		real->retool_desc = NULL;
	else
		real->retool_desc = str_dup( newName.c_str() );
}

void JSObject::setRetoolSDesc( flusspferd::string newName )
{
	if( !real || real->IsProto() )
		return;
	if( real->retool_sdesc )
		delete[] real->retool_sdesc;
	if( newName.empty() )
		real->retool_sdesc = NULL;
	else
		real->retool_sdesc = str_dup( newName.c_str() );
}

void JSObject::setRetoolExDesc( flusspferd::string newExDesc )
{
	if( !real || real->IsProto() )
		return;
	if( real->retool_ex_desc )
		delete real->retool_ex_desc;
	if( newExDesc.empty() )
		real->retool_ex_desc = NULL;
	else {
		real->retool_ex_desc = new ExtraDescription();
		real->retool_ex_desc->keyword = NULL;
		real->retool_ex_desc->description = str_dup(newExDesc.c_str());
	}
}

int JSObject::getScalpLevel()
{
	if( !real || real->IsProto() || !real->scalp )
		return false;
	return real->scalp->level;
}
int JSObject::getScalpRace()
{
	if( !real || real->IsProto() || !real->scalp )
		return false;
	return real->scalp->race;
}
bool JSObject::getIsScalp()
{
	if( !real || real->IsProto() || !real->scalp )
		return false;
	return real->scalp->is_scalp;
}
bool JSObject::getIsPlayerScalp()
{
	if( !real || real->IsProto() || !real->scalp )
		return false;
	return real->scalp->player_scalp;
}

void JSObject::move_to_room(JSRoom *dest)
{
	if( !real || real->IsPurged() )
		return;
	if( real->carried_by )
		obj_from_char(real);
	if( real->worn_by )
		unequip_char(real->worn_by, real->worn_on);
	if( real->in_obj )
		obj_from_obj(real->in_obj);
	if( real->in_room )
		real->RemoveFromRoom();
	real->MoveToRoom(dest->toReal());
}

void JSObject::move_to_char(JSCharacter *dest)
{
	if( !real || real->IsPurged() )
		return;
	if( real->carried_by )
		obj_from_char(real);
	if( real->worn_by )
		unequip_char(real->worn_by, real->worn_on);
	if( real->in_obj )
		obj_from_obj(real->in_obj);
	if( real->in_room )
		real->RemoveFromRoom();
	obj_to_char(real,dest->toReal());
}

void JSObject::move_to_obj(JSObject *dest)
{
	if( !real || real->IsPurged() )
		return;
	if( real->carried_by )
		obj_from_char(real);
	if( real->worn_by )
		unequip_char(real->worn_by, real->worn_on);
	if( real->in_obj )
		obj_from_obj(real->in_obj);
	if( real->in_room )
		real->RemoveFromRoom();
	obj_to_obj(real,dest->toReal());
}

flusspferd::string JSObject::getExtraDescription()
{
	flusspferd::value v;
	if(!real || real->IsPurged())
		return v.to_string();
	
	ExtraDescription *extraDescription = real->GetExDesc();
	if(extraDescription)
		return flusspferd::string(extraDescription->description);

	return v.to_string();
}

flusspferd::array JSObject::getAffects()
{
	flusspferd::array a = flusspferd::create_array();
	if(!real || real->IsPurged())
		return a;
	
	for(int counter = 0; counter < MAX_OBJ_AFFECT; ++counter)
	{
		if(real->affected[counter].modifier)
		{
			flusspferd::object objectAffect = flusspferd::create_object();
			objectAffect.set_property("location", (int)real->affected[counter].location);
			objectAffect.set_property("modifier", (int)real->affected[counter].modifier);
			a.call("push", objectAffect);
		}
	}

	return a;
}

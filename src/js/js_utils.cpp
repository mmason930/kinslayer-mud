#include "../conf.h"

#include "../structs.h"

#include "JSObject.h"
#include "JSQuery.h"
#include "JSRow.h"

#include "../rooms/Room.h"

using namespace std;

std::unordered_map<void*, pair<std::string, flusspferd::value> > mapper;

/* Galnor - 11/13/2009 - Perform necessary maintenance for removal of a Javascript object. */
void deleteValue( JSBindable *addr )
{
	if( mapper.count( addr ) != 0 )
	{
		object o = mapper[addr].second.to_object();

		std::string pName = mapper[addr].first;
		global().delete_property( pName );//Delete from the JS global object.
		mapper.erase( addr );//Remove from our mapper.

		/* The Javascript variables(primarily, in yielded generators), which refer to this JSBindable
		instance, may still be alive, along with their binded wrapper class(JSObject,JSCharacter,JSRoom).
		We need to set the invalid pointer(real) to null to prevent instability from future references */

		//NOTE: It may be a good idea to use boost::weak_ptr to handle this in the future.
		if( is_native<JSCharacter>(o) )
		{
			get_native<JSCharacter>(o).setReal( 0 );
		}
		else if( is_native<JSObject>(o) )
		{
			get_native<JSObject>(o).setReal( 0 );
		}
		else if( is_native<JSRoom>(o) )
		{
			get_native<JSRoom>(o).setReal( 0 );
		}
	}
}

//Created by Narg: August 2009
template<typename wrapped_t, typename raw_t>
pair<std::string, flusspferd::value> findPair(raw_t * addr)
{
    if (!addr)//Address is invalid. We will return a null value.
        return make_pair("null", flusspferd::value());
	
    if (mapper.count(addr))
    {//Value is already registered. We'll return that.
        return mapper[addr];
    }

	//If we get to this point, we need to go ahead and create a new value. What we will do is attach the variable
	//to the flusspferd::global object so that it is both rooted and available to all scripts. We will also
	//need to obtain a unique variable name to assign to this. The value is then added to the mapper hash so
	//that it may be obtained at a later time by the codebase.
    std::string newValueName = getUniqueName();
    flusspferd::value newFlusspferdValue = flusspferd::create_native_object<wrapped_t>(object(), addr);
	flusspferd::object newFlusspferdObject = newFlusspferdValue.to_object();

	std::pair<std::string, flusspferd::value> pair = make_pair(newValueName, newFlusspferdValue);
	if( !flusspferd::is_native< JSRow   >( newFlusspferdObject ) &&
		!flusspferd::is_native< JSQuery >( newFlusspferdObject )
	  )
	{
		//However, we do not want to root flighty variables such as JSQuery or JSRows, because they are heavily
		//reliant upon being extracted after they are no longer in use. Since JSQuery wraps sql::Query, which is
		//a boost::shared_ptr wrapper of _sql::Query, we want these to get garbage collected normally, because
		//once they're done being used, nothing is ever going to refer to them again, which is not the case
		//with JSRoom, JSCharacter, JSObject, which may be referred to at any time by any script.
		global().set_property(newValueName, newFlusspferdValue);
		//The reason we're not adding these to the mapper is because JSCharacter, JSRoom, and JSObject all
		//Ensure that their entries get destroyed upon deletion. Until we add this same logic to JSRow
		//and JSQuery, adding them here is dangerous because addresses can be recycled, meaning incorrect
		//flusspferd::values would be returned.
		mapper[addr] = pair;
	}

	return pair;
}
/** Galnor - 11/06/2009 - Modified to utilize polymorphism **/
//Original version by Narg: 08/2009
std::string lookupName(JSBindable * b)
{
	Character *c;
	Object *o;
	Room *r;
	sqlJSQuery *query;
	sqlJSRow *row;
	if( (c = dynamic_cast<Character *>(b)) != 0 )
	    return findPair<JSCharacter>(c).first;
	else if( (o = dynamic_cast<Object *>(b)) != 0 )
	    return findPair<JSObject>(o).first;
	else if( (r = dynamic_cast<Room *>(b)) != 0 )
	    return findPair<JSRoom>(r).first;
	else if( (query = dynamic_cast<sqlJSQuery *>(b)) != 0 )
	    return findPair<JSQuery>(query).first;
	else if( (row = dynamic_cast<sqlJSRow *>(b)) != 0 )
	    return findPair<JSRow>(row).first;
	else
		return std::string("");
}
/** Galnor - 11/06/2009 - Modified to utilize polymorphism **/
//Original version by Narg: 08/2009
flusspferd::value lookupValue(JSBindable * b)
{
	Character *c;
	Object *o;
	Room *r;
	sqlJSQuery *query;
	sqlJSRow *row;
	if( (c = dynamic_cast<Character *>(b)) != 0 )
	    return findPair<JSCharacter>(c).second;
	else if( (o = dynamic_cast<Object *>(b)) != 0 )
	    return findPair<JSObject>(o).second;
	else if( (r = dynamic_cast<Room *>(b)) != 0 )
	    return findPair<JSRoom>(r).second;
	else if( (query = dynamic_cast<sqlJSQuery *>(b)) != 0 )
		return findPair<JSQuery>(query).second;
	else if( (row = dynamic_cast<sqlJSRow *>(b)) != 0 )
	    return findPair<JSRow>(row).second;
	return flusspferd::value();
}

//Created by Narg: August 2009
std::string getUniqueName()
{
    static unsigned long count = 0;
    count++;
    std::string s = "uvar";
    s = s + MiscUtil::convert<std::string>(count);
    return s;
}

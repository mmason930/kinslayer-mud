//
// C++ Interface: js_utils
//
// Description: 
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JS_UTILS_H
#define KINSLAYER_JS_UTILS_H
    
class Character;
class Room;
class Object;
namespace flusspferd
{
    class value;
}

#include <string>
   
/******************************************************************************************
 * All of the follow functions are basicly just overloads of the same function.
 * They take the pointer passed to them and look in a map to see if a JS wrapper object
 * has already been created for it. If it has, they return either it or a unique name 
 * that references it. If a wrapper doesn't exist, it creates one (of the correct type)
 * and returns it
 * If a null pointer is passed to any of these functions, a null flusspferd/js value
 * will be created and returned. Therefore, if all wrapper creation is done through these functions,
 * no invalid wrapper objects can be exposed to javascript.
 *******************************************************************************************/


flusspferd::value lookupValue(JSBindable * b);
std::string lookupName(JSBindable * b);
void deleteValue( JSBindable * addr );


/******

std::string lookupName(Character * ch);
std::string lookupName(Object * o);
std::string lookupName(Room * r);

flusspferd::value lookupValue(Character * ch);
flusspferd::value lookupValue(Object * o);
flusspferd::value lookupValue(Room * r);

******/

// This simply generates a name that is guarenteed to be unique within this javascript runtime.
std::string getUniqueName();


#endif

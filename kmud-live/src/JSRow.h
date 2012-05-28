//
// C++ Interface: JSRow
//
// Description:
//
//
// Author: Galnor (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JSROW_H
#define KINSLAYER_JSROW_H

#ifdef KINSLAYER_JAVASCRIPT

#include <flusspferd.hpp>

#include <string>
#include <iostream>
#include "comm.h"
#include "structs.h"
#include "db.h"
#include "js_utils.h"
#include "handler.h"

using namespace flusspferd;
using namespace std;

//Wrapper class
class sqlJSRow : public JSBindable
{
public:
	static int numberAllocated;
	static int numberDeallocated;
	sqlJSRow()
	{
//		std::cout << "Constructing JSRow..." << std::endl;
		MyRow = 0;
		++sqlJSRow::numberAllocated;
	}
	sqlJSRow( class sql::Row *base )
	{
//		std::cout << "Constructing JSRow..." << std::endl;
		MyRow = base;
		++sqlJSRow::numberAllocated;
	}
	~sqlJSRow()
	{
//		std::cout << "Destructing JSRow..." << std::endl;
		delete MyRow;
		++sqlJSRow::numberDeallocated;
	}
	class sql::Row *MyRow;
	class Room *InRoom() { return 0; }
	bool IsPurged() { return false; }
};

FLUSSPFERD_CLASS_DESCRIPTION(
    JSRow,
    (full_name, "JSRow")
    (constructor_name, "JSRow")
    (methods,
        ("get", bind, get)
		("getByIndex", bind, getByIndex)
		("isFieldNull", bind, getIsFieldNull)
		("isFieldNullByIndex", bind, getIsFieldNullByIndex)
		)
    (properties,
	))
{
public:
    JSRow(flusspferd::object const &self, flusspferd::call_context& cc)
        : base_type(self)
    {
    }

    ~JSRow() {
		delete real;
    }

    JSRow(flusspferd::object const &self, sqlJSRow* _real)
        : base_type(self)
    {
        real = _real;
    }

	sqlJSRow* toReal() { return real; }
	void setReal( sqlJSRow *r ) { real = r; }

	flusspferd::value get( flusspferd::string propertyName );
	flusspferd::value getByIndex( int index );
	bool getIsFieldNull(flusspferd::string fieldName);
	bool getIsFieldNullByIndex(int fieldIndex);


private:
    sqlJSRow *real;
};


#endif
#endif

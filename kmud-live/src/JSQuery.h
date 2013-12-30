//
// C++ Interface: JSQuery
//
// Description:
//
//
// Author: Galnor
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef KINSLAYER_JSQUERY_H
#define KINSLAYER_JSQUERY_H

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

//We need to make a wrapper class so that we can inherit JSBindable
class sqlJSQuery : public JSBindable
{
public:
	sqlJSQuery( sql::Query q )
	{
		MyQuery = q;
	}
	sql::Query MyQuery;
	class Room *InRoom() { return 0; }
	bool IsPurged() { return false; }
};

FLUSSPFERD_CLASS_DESCRIPTION(
    JSQuery,
    (full_name, "JSQuery")
    (constructor_name, "JSQuery")
    (methods,
        ("skipRow", bind, skipRow)
		("getIndexByField", bind, getIndexByField)
		("getFieldByIndex", bind, getFieldByIndex)
		("reverseRows", bind, reverseRows)
		("resetRowQueue", bind, resetRowQueue)
		)
    (properties,
        ("numRows", getter, getnumRows)
        ("numFields", getter, getNumFields)
		("hasNextRow", getter, gethasNextRow)
		("peekRow", getter, getpeekRow)
		("getRow", getter, getRow)
	))
{
public:
    JSQuery(flusspferd::object const &self, flusspferd::call_context& cc)
        : base_type(self)
    {
    }

    ~JSQuery() {
		delete real;
    }

    JSQuery(flusspferd::object const &self, sqlJSQuery *_real)
        : base_type(self)
    {
        real = _real;
    }

	sqlJSQuery *toReal() { return real; }
	void setReal( sqlJSQuery *r ) { real = r; }

	void skipRow();
	int getIndexByField( flusspferd::string field );
	flusspferd::string getFieldByIndex( int index );
	void reverseRows();
	void resetRowQueue();
	int getnumRows();
	int getNumFields();
	bool gethasNextRow();
	flusspferd::value getpeekRow();
	flusspferd::value getRow();

private:
    sqlJSQuery *real;
};


#endif

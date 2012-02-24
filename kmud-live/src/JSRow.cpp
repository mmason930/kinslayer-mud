#ifdef KINSLAYER_JAVASCRIPT

#include "JSQuery.h"
#include "JSRow.h"

#include "structs.h"
#include "mobs.h"
#include "db.h"

#include "js_functions.h"
#include "js_interpreter.h"

void JSEnvironment::LoadJSRow()
{
	load_class<JSRow>();
}
int sqlJSRow::numberAllocated = 0;
int sqlJSRow::numberDeallocated = 0;

flusspferd::string JSRow::get( flusspferd::string propertyName )
{
	return (*(real->MyRow))[ propertyName.to_string() ];
}
flusspferd::string JSRow::getByIndex( int index )
{
	return (*(real->MyRow))[ index ];
}

#endif

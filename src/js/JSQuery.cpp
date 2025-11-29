#include "JSQuery.h"
#include "JSRow.h"


#include "js_interpreter.h"

// Forward declaration of binding registration function
void RegisterJSQueryBindings();

void JSEnvironment::LoadJSQuery()
{
	flusspferd::ClassTraits<JSQuery>::class_name = "JSQuery";
	RegisterJSQueryBindings();  // Register methods and properties first
	load_class<JSQuery>();
}


void JSQuery::skipRow()
{
	real->MyQuery->skipRow();
}
int JSQuery::getIndexByField( flusspferd::string field )
{
	return real->MyQuery->getIndexByField( field.to_string() );
}
flusspferd::string JSQuery::getFieldByIndex( int index )
{
	return real->MyQuery->getFieldByIndex( index );
}
void JSQuery::reverseRows()
{
	return real->MyQuery->reverseRows();
}
void JSQuery::resetRowQueue()
{
	real->MyQuery->resetRowQueue();
}
int JSQuery::getnumRows()
{
	return real->MyQuery->numRows();
}
int JSQuery::getNumFields()
{
	return real->MyQuery->numFields();
}
bool JSQuery::gethasNextRow()
{
	return real->MyQuery->hasNextRow();
}

flusspferd::value JSQuery::getpeekRow()
{
	if( real->MyQuery->hasNextRow() )
		return lookupValue( new class sqlJSRow( new sql::Row(real->MyQuery->peekRow())) );
	return lookupValue( 0 );
}
flusspferd::value JSQuery::getRow()
{
	if( real->MyQuery->hasNextRow() )
		return lookupValue( new class sqlJSRow( new sql::Row(real->MyQuery->getRow())) );
	return lookupValue( 0 );
}

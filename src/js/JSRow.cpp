#include "JSRow.h"


#include "js_interpreter.h"

// Forward declaration of binding registration function
void RegisterJSRowBindings();

void JSEnvironment::LoadJSRow()
{
	flusspferd::ClassTraits<JSRow>::class_name = "JSRow";
	RegisterJSRowBindings();  // Register methods and properties first
	load_class<JSRow>();
}
int sqlJSRow::numberAllocated = 0;
int sqlJSRow::numberDeallocated = 0;

flusspferd::value JSRow::get( flusspferd::string propertyName )
{
	if((*(real->MyRow)).isFieldNull(propertyName.to_string()))
		return flusspferd::object();

	return flusspferd::string((*(real->MyRow))[ propertyName.to_string() ]);
}
flusspferd::value JSRow::getByIndex( int index )
{
	if((*(real->MyRow)).isFieldNull(index))
		return flusspferd::object();
	return flusspferd::string((*(real->MyRow))[ index ]);
}
bool JSRow::getIsFieldNull(flusspferd::string fieldName)
{
	return (*(real->MyRow)).isFieldNull(fieldName.to_string());
}
bool JSRow::getIsFieldNullByIndex(int fieldIndex)
{
	return (*(real->MyRow)).isFieldNull(fieldIndex);
}

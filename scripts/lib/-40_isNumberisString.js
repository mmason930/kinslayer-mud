/*** Galnor 12/24/2009 - Returns true if the argument holds numeric data regardless of its data type. ***/
function isNumber( x )
{
        return !isNaN( x );
}
var isString = function ( object ) {
	if ( getObjectClass(object) == "String" ) {
		return true;
	}
	return false;
}



//Alder
//March 2010
/**********************************************************
 * Returns intLength characters of str; returns null      *
 * if str is not a string                                 *
 * str: string to be altered                              *
 * intLength: number of characters of str to be displayed *
 **********************************************************/
function strAbbrev(str, intLength) {
	if ( getObjectClass(str) != "String" ) {
		return ( null );
	}
	var newStr = "";
	for ( var i = 0; i < intLength && i < str.length; i++ ) {
		newStr += str[i];
	}
	return ( newStr );
}

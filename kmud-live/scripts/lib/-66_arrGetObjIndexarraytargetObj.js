//Alder
//May 2010
/***********************************************************
 * array: array to be examined						       *
 * targetObj: object whose index is to be determined       *
 * Returns: index of targetObj in array (-1 if not found)  *
 ***********************************************************/
function arrGetObjIndex( array, targetObj ) {
	if ( getObjectClass(array) != "Array" && getObjectClass(array) != "String") {
		mudLog(3,101,"JScript Warning -66: Non-array type passed as array");
		return -1;
	}
	var index = -1;
	for ( var i = 0; i < array.length; i++ ) {
		if ( array[i] == targetObj ) {
			index = i;
			break;
		}
	}
	return index;
}


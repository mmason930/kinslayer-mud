//Alder
//May 2010
/*******************************************************************
 * fArr: first array                                               *
 * sArr: second array                                              *
 * Returns: array of elements common between fArr and sArr         *
 *******************************************************************/
function arrCompare( fArr, sArr ) {
	if ( getObjectClass(fArr) != "Array" || getObjectClass(sArr) != "Array" ) {
		mudLog(3,103,"JScript -110: non-array passed to arrCompare().");
		return;
	}
	var cArr = [];	// Holds common elements
	for each ( var element in fArr ) {
		if ( arrContains(sArr,element) ) {
			cArr.push(element);
		}
	}
	return ( cArr );
	// var iMatches = cArr.length;
	// if ( iMatches == fArr.length )
		// return 1;		// Second array contains every element of first array
	// else if ( iMatches )
		// return 0;		// Second array contains some elements of first array
	// else
		// return -1;		// Second array contains no elements of first array
}

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
		return null;
	}
	var cArr = [];	// Holds common elements
	for (var _autoKey in fArr) {
		var element = fArr[_autoKey];
		if ( arrContains(sArr,element) ) {
			cArr.push(element);
		}
	}
	return ( cArr );
}


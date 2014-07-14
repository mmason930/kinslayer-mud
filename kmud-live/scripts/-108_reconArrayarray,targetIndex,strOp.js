//Alder March 2010    
	/*****************************************************
	 * array: array to be operated on                    *
	 * targetElement: index to inject or remove element  *
	 * strOp: "INJECT", "REMOVE"                         *
	 * Returns reconstructed array                       *
	 * EX: reconArray( array,3, "remove" )               *
	 *****************************************************/
function reconArray (array, targetIndex, strOp) {
	var reconArray = [];//Array of elements excluding entry to be removed
	if ( array.length > 1 ) {
		var marker = targetIndex;//Element index in array to be removed
		/**SAVE ELEMENTS BEFORE AND AFTER TARGET ELEMENT**/
		for ( var i = 0; i < marker; i++ ) {
			reconArray.push(array[i]);
		}
		if ( strn_cmp(strOp,"inject",3) == true ) {
			reconArray.push("<INJECTED ELEMENT>");//Create a blank element at the desired location within the array
			for ( var k = marker; k < array.length; k++ ) {
				reconArray.push(array[k]);
			}
		}
		else if ( strn_cmp(strOp,"remove",3) == true ) {
			for ( var i = (marker + 1); i < array.length; i++ ) {		
				reconArray.push(array[i]);
			}
		}
	}
	else  {
		if ( strn_cmp(strOp,"remove",3) == true )
			array.pop();
		else if ( strn_cmp(strOp,"inject",3) == true ) {
			reconArray.push("");
			reconArray.push(array[0]);
		}
	}
	return ( reconArray );
}

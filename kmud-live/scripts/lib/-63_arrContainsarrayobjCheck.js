//Alder
//April 2010
/********************************************************
 * array: array whose elements are being examined       *
 * objCheck: object being compared to array's elements  *
 * Returns: true if array contains objCheck, else false *
 ********************************************************/
	function arrContains ( array, objCheck ) {
		for each ( var thing in array ) {
			if ( thing == objCheck ) {
				return true;
			}
		}
		return false;
	}

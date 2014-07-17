/*********************************************************
 *                                                       *
 * strlenDisplay()                                       *
 * Arguments:                                            *
 * # str    - The string to be analyzed                  *
 * Returns: The display length of the string             *
 *                                                       *
 * ~~~By: Galnor 01/11/2010                              *
 *                                                       *
 *********************************************************/
 function strlenDisplay( str )
 {
	var iCount = 0;
	for(var i = 0;i < str.length;) {
		if( str.charCodeAt(i) == 27 ) {
			while (i < str.length && str[i++] != 'm' ) {}
			continue;
		}
		++i; ++iCount;
	}
	return iCount;
 }
 

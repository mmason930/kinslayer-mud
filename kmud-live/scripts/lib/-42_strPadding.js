/*********************************************************
 *                                                       *
 * strPadding()                                          *
 * Arguments:                                            *
 * # str    - The string to be manipulated               *
 * # cType  - The character used to fill the space       *
 * # iWidth - The maximum width of the string            *
 * # sDir   - "left" or "right"                          *
 * Returns: The altered string                           *
 *                                                       *
 * ~~~By: Galnor 01/11/2010                              *
 *                                                       *
 *********************************************************/
 function strPadding( str, cType, iWidth, sDir )
 {
	if( cType.length < 1 ) return str;
	if( sDir == "left" )
		var iDir = 0;
	else if( sDir == "right" )
		var iDir = 1;
	else {
		mudLog(3, 102, "strPadding() called with invalid sDir: " + sDir + ".");
		return str;
	}
	while( (strlenDisplay( str )+cType.length) < iWidth )
	{
		if( iDir == 0 )
			str = cType + str;
		else
			str += cType;
	}
	return str;
 }

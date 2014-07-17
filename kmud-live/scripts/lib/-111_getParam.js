/**************************************************************************
 *                                                                        *
 * getParam()                                                             *
 * Arguments: - sName : The parameter name to look for.                   *
 *            - vArgs : The argument array thought to contain the param   *
 *            - iStartingIndex : The index to begin searching.            *
 * Purpose: Given a string such as: vnum mob --name=tree --race=human     *
 *          you can use parameters to allow input in an unspecified order.*
 *			getParam will search your string to see if such an input      *
 *			parameter exists, and if so, it will return its value.        *
 *                                                                        *
 * ~~~ By: Galnor 05/10/2010                                              *
 *                                                                        *
 **************************************************************************/
function getParam( sName, vArgs, iStartingIndex )
{
	for(var i = iStartingIndex; i < vArgs.length;++i)
	{
		var sParam = vArgs[ i ];
		if( sParam.length < 2 || sParam[ 0 ] != '-' || sParam[ 1 ] != '-' )
			continue;
		sParam = sParam.substr( 2 );
		var vParamParts = sParam.split("=");
		if( vParamParts.length != 2 )
			continue;
		if( vParamParts[ 0 ] == sName )
			return vParamParts[ 1 ];
	}
	return "";
}
function isParam( text ) {
	if( !text || (text.length > 1 && text[0] == '-' && text[1] == '-') )
		return true;
	return false;
}

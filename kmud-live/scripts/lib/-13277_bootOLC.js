//Boot OLC
var vOLC = [];
/**************************************************************************
 *                                                                        *
 * bootOLC()                                                              *
 * Arguments: <None>                                                      *
 * Purpose: Responsible for initiating the OLC vector.                    *
 *                                                                        *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
function bootOLC()
{
	vOLC=[];
	if( typeof bootPeditOLC != "undefined" )
		bootPeditOLC();
	if( typeof bootQueditOLC != "undefined" )
		bootQueditOLC();
	if( typeof bootAcheditOLC != "undefined" )
		bootAcheditOLC();
	if( typeof bootGateditOLC != "undefined" )
		bootGateditOLC();
	if( typeof bootReceditOLC != "undefined" )
		bootReceditOLC();
	if( typeof bootHelpeditOLC != "undefined" )
		bootHelpeditOLC();
}
setTimeout( 1, bootOLC );


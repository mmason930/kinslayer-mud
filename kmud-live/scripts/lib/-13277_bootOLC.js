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
	mudLog(constants.BRF, 100, "Booting Pedit.");
	if( typeof bootPeditOLC != "undefined" )
		bootPeditOLC();
	mudLog(constants.BRF, 100, "Booting Quedit.");
	if( typeof bootQueditOLC != "undefined" )
		bootQueditOLC();
	mudLog(constants.BRF, 100, "Booting Achedit.");
	if( typeof bootAcheditOLC != "undefined" )
		bootAcheditOLC();
	mudLog(constants.BRF, 100, "Booting Gatedit.");
	if( typeof bootGateditOLC != "undefined" )
		bootGateditOLC();
	mudLog(constants.BRF, 100, "Booting Recedit.");
	if( typeof bootReceditOLC != "undefined" )
		bootReceditOLC();
	mudLog(constants.BRF, 100, "Booting Helpedit.");
	if( typeof bootHelpeditOLC != "undefined" )
		bootHelpeditOLC();
}
setTimeout( 1, bootOLC );


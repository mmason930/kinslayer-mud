/**************************************************************************
 *                                                                        *
 * runTimeouts()                                                          *
 * Arguments: - pulses: the number of pulses until the callback runs      *
 *            - handle: The callback function                             *
 *            - vArgs : an array of arguments passed to the handle        *
 * Purpose: Adds a callback to the event vector to be run at a later time *
 * Important: DO NOT EDIT THIS UNLESS YOU _REALLY_ KNOW WHAT              *
 *            YOU ARE DOING!                                              *
 *                                                                        *
 * ~~~ By: Galnor 01/10/2010                                              *
 *                                                                        *
 **************************************************************************/
 
if( eCallbackVector == undefined ) {
	var eCallbackVector = [];
}
if(typeof setTimeout == "undefined")
{
	function setTimeout( pulses, handle, vArgs )
	{
		var callback = new Object();
		callback.handle = handle;
		callback.args = vArgs;
		callback.pulses = pulses;
		eCallbackVector.push( callback );
	}
}

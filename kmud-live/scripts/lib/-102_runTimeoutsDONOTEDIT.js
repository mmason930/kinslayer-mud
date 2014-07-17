/*******************************************************************
 *                                                                 *
 * runTimeouts()                                                   *
 * Arguments: none                                                 *
 * Returns: 1, if it succeeds                                      *
 * Purpose: Runs all callbacks when their time has expired.        *
 * Important: DO __NOT__ EDIT THIS UNLESS YOU _REALLY_ KNOW WHAT   *
 *            YOU ARE DOING!                                       *
 *                                                                 *
 * ~~~ By: Galnor 01/10/2010                                       *
 *                                                                 *
 *******************************************************************/
if( eCallbackVector == undefined ) {
	var eCallbackVector = [];
}
if(typeof runTimeouts == "undefined")
{
	function runTimeouts()
	{
		for( var i = 0;i < eCallbackVector.length;++i ) {
			if( --eCallbackVector[i].pulses <= 0 ) {
				var f = eCallbackVector[i].handle;
				var vArgs = eCallbackVector[i].args;
				eCallbackVector.splice(i,1);
				f( vArgs );
				--i;
			}
		}
		return 1;
	}
}

/**************************************************************************
 *                                                                        *
 * JSRoom.echoToNeighbors                                                 *
 * Arguments: - sMessage: The message to be sent to people in the room.   *
 *               This can be either a function, in which chase it will    *
 *               take in a single argument, JSCharacter, which will return*
 *               the message to be sent to the player, or it can be a     *
 *               string.                                                  *
 *            - iDepth: The radius to which the message reverberates.     *
 *            - fPredicate: A function that takes in a JSCharacter        *
 *               argument. Returns true if the msg is to be sent, and     *
 *               false if not.        ( OPTIONAL )                        *
 * Purpose: Sends a message to neighboring rooms up to  specified depth   *
 *                                                                        *
 * ~~~ By: Galnor 03/20/2010                                              *
 *                                                                        *
 **************************************************************************/
 JSRoom.prototype.echoToNeighbors = function(sMessage, iDepth, fPredicate)
 {
	var fMessage = undefined;
	if( getObjectClass(sMessage) == "Function" )
		fMessage = sMessage;
	var roomsSentTo = [ this ];
	var roomsRemaining = [];
	
	var neighbors = this.neighbors;
	for(var i = 0;i < neighbors.length;++i) {
		if( neighbors[ i ] == undefined ) continue;
		var obj = new Object();
		obj.depth = 1;
		obj.room = neighbors[ i ];
		roomsRemaining.push( obj );
	}
//	mudLog(constants.BRF, 100, "Echo with depth of " + iDepth + " from room #" + this.vnum);
	while( roomsRemaining.length > 0 ) {
		var rObj = roomsRemaining[ 0 ];
		var iDepthInThisRoom = rObj.depth;
		var thisRoom = rObj.room;
		roomsSentTo.push( thisRoom );
		roomsRemaining.splice( 0, 1 );
		
		//mudLog(constants.BRF, 100, "Trying room #" + thisRoom.vnum);
		
		var people = thisRoom.people;
		for(var i = 0; i < people.length;++i) {
			if( fPredicate == undefined || fPredicate( people[i] ) )
			{
				if( fMessage != undefined )
					sMessage = fMessage( people[i] );
				people[ i ].send( sMessage );
			}
		}
		/*** Queue up the neighboring rooms ***/
		if( iDepthInThisRoom < iDepth )
		{
			var neighbors = thisRoom.neighbors;
			for(var i = 0;i < neighbors.length;++i) {
				if( neighbors[ i ] == undefined ) continue;
				//Look to see if we've already touched this room.
				var touched = false;
				for(var r = 0;r < roomsSentTo.length;++r) {
					if( roomsSentTo[ r ] == neighbors[ i ] )
					{
						touched = true;
						break;
					}
				}
				if( touched == false ) {
					var obj = new Object();
					obj.depth = iDepthInThisRoom + 1;
					obj.room = neighbors[ i ];
					roomsRemaining.push( obj );
				}
			}
		}
	}
//	mudLog(constants.BRF, 100, "Sent to " + roomsSentTo.length + " rooms.");
 }
 

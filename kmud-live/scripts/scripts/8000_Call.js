var script8000 = function(self, actor, here, args, extra) {
	/**************************************************************************
	 *                                                                        *
	 * global command: CALL                                                   *
	 *  This makes use of the gatekeepers table in the database, which is     *
	 *  loaded on bootup. Players may call gates either in the main room,     *
	 *  or from the outside.                                                  *
	 *                                                                        *
	 * ~~~ By: Galnor 06/07/2010                                              *
	 *                                                                        *
	 **************************************************************************/
	 if(actor.room.vnum == 2305) {
	 
		return;
	 }
	_block;
	function attemptGateCall()
	{//
		var iRoomVnum = here.vnum;
		var vGateKeepers = getGateKeeperVector();
		var index;
		var gateKeeperObject;
		for(var index = 0;index < vGateKeepers.length;++index) {
			gateKeeperObject = vGateKeepers[ index ];
			if( gateKeeperObject.gateKeeperRoomVnum == iRoomVnum || gateKeeperObject.otherRoomVnum == iRoomVnum )
				break;
		}
		if( index == vGateKeepers.length )
			return false;//No match found.
		if( gateKeeperObject.gateKeeperRoomVnum == undefined || gateKeeperObject.otherRoomVnum == undefined )
			return false;//Invalid. Both rooms must be set.
		//Now we'll need to find the exit that links the two rooms together.
		var dir;
		var nrOfDirections = 6;
		var mainGateRoom = getRoom(gateKeeperObject.gateKeeperRoomVnum);
		if( !mainGateRoom )
			return false;//Even though the vnum is set, the room doesn't actually exist.
		for(var dir = 0;dir < nrOfDirections;++dir) {
			var neighbor = mainGateRoom.direction( dir );
			if( !neighbor )
				continue;
			if( mainGateRoom.vnum == gateKeeperObject.gateKeeperRoomVnum && neighbor.vnum == gateKeeperObject.otherRoomVnum )
				break;
			if( mainGateRoom.vnum == gateKeeperObject.otherRoomVnum && neighbor.vnum == gateKeeperObject.gateKeeperRoomVnum )
				break;
		}
		if( dir == nrOfDirections || mainGateRoom.doorExists( dir ) == false )
			return false;
		actor.send("You call for the gate to be opened.");
		act("$n calls for the gate to be opened.", false, actor, null, null, constants.TO_ROOM);
		var gateKeeper = getMobAtRoom( gateKeeperObject.gateKeeperRoomVnum, gateKeeperObject.gateKeeperVnum );
		if( !gateKeeper )
			return true;//The gatekeeper wasn't found in the correct room.
		if( gateKeeper.race != actor.race )
			return true;//Must be same race.
		gateKeeper.comm("unlock " + mainGateRoom.doorName(dir) );
		gateKeeper.comm("open " + mainGateRoom.doorName(dir) );

		global.galnor.send("Dir Before: " + dir);
		var performCloseGate = function( vArgs ) {
			global.galnor.send("Test from callback.");
			global.galnor.send("Dir After: " + dir);
			var gateKeeper = vArgs[ 0 ];
			gateKeeper.comm("close " + gateKeeper.room.doorName(dir) );
			gateKeeper.comm("lock " + gateKeeper.room.doorName(dir) );
		}

		performCloseGate([gateKeeper]);
		setTimeout(gateKeeperObject.pulsesToWaitForClose ? gateKeeperObject.pulsesToWaitForClose : 1, performCloseGate, [ gateKeeper ]);
		return true;
	}
	if( !attemptGateCall() ) {
		actor.send("Call what?");
	}
}
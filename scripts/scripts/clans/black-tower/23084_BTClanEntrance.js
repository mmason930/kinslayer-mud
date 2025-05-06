var script23084 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	_block;
	if( vArgs.length < 2 || str_cmp(vArgs[1],"rope") ) {
		actor.send("Pull what?");
		return;
	}
	actor.send("You pull down on the rope.");
	act("$n pulls down on the rope.", true, actor, null, null, constants.TO_ROOM);
	wait 4;
	here.echo("A chiming can be heard on the other side of the doors to the north.");
	if( actor.inClan(15) ) {
		var thisRoom = here;
		var thatRoom = thisRoom.direction( constants.NORTH );
		
		thisRoom.echo("The expansive doors open wide.");
		thisRoom.openDoor( constants.NORTH,true );
		thatRoom.echo("The " + thatRoom.doorName( constants.SOUTH ) + " is opened from the other side.");
		wait 4;
		thisRoom.echo("The doors close and a bar can be heard scraping across them on the other side.");
		thatRoom.echo("The " + thatRoom.doorName( constants.SOUTH ) + " is closed from the other side.");
		thisRoom.closeDoor( constants.NORTH,true );
		thisRoom.lockDoor( constants.NORTH,true );
	}
	else {
		actor.send("You have no business in there!");
	}
}
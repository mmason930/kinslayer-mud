var script23501 = function(self, actor, here, args, extra) {
	var otherRoom = here.direction(constants.SOUTH);
	wait 2
	here.echo("An iron bar is lifted from the doors and they swing open soundlessly.");
	otherRoom.echo( "The " + otherRoom.doorName(constants.NORTH) + " is opened from the other side." );
	here.openDoor(constants.SOUTH,true);
	wait 3
	here.echo("The doors swing shut and the bar is replaced, locking them from intruders.");
	otherRoom.echo( "The " + otherRoom.doorName(constants.NORTH) + " is closed from the other side." );
	here.closeDoor(constants.SOUTH,true);
	here.lockDoor(constants.SOUTH,true);
}
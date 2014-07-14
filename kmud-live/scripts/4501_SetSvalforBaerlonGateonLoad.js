var script4501 = function(self, actor, here, args, extra)
{
	setSval(here, 4512, "BAERLON_EAST_GATE_OPEN", false);
self.room.closeDoor(constants.DIRECTION_EAST);
self.room.lockDoor(constants.DIRECTION_EAST);
self.room.echo("test");

};


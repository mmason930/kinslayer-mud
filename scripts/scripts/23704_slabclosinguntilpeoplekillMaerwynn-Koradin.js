var script23704 = function(self, actor, here, args, extra) {
	//Slab automatically closes behind players as they enter Maerwynn smob area.
	//Koradin - December 2009
	if (extra.direction == constants.EAST)
	{
	waitpulse 1;
	if (self.doorIsClosed(constants.EAST))
	{
	return;
	}
	var eastroom = self.direction(constants.EAST);
	self.echo("The slab rolls into place behind " + actor.name + ", blocking the way east.");
	eastroom.echo("The slab rolls back into place behind you, blocking the way west!");
	self.closeDoor(constants.EAST);
	eastroom.closeDoor(constants.WEST);
	}
}
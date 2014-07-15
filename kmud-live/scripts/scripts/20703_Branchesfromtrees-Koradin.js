var script20703 = function(self, actor, here, args, extra) {
	//Tree branches
	//Koradin - May 2010
	if (random(1,10) == 1)
		return;
	if (random(1,4) == 1)
		var obj = 20970;
	else
		var obj = 1800;
	self.room.echo("A branch snaps off the tree and falls to the ground.");
	self.room.loadObj(obj);
}
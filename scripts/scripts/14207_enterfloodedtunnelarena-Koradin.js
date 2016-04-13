var script14207 = function(self, actor, here, args, extra) {
	if (dirToText(extra.direction).toLowerCase() != "down" || getSval(getRoom(14350),14206,"flooded") != 1) {
		//here.echo(getSval(getRoom(14350),14206,"flooded"));
		_noblock;
		return;
	}
	_block;
	actor.send("The tunnel below is flooded! You can't survive down there!");
	return;
}
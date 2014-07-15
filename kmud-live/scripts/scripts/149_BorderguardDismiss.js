var script149 = function(self, actor, here, args, extra) {
	_block;
	self.comm("nod " + actor.name);
	wait 2;
	self.comm("emote opens the irondoor.");
	wait 1;
	actor.send("You exit to the east.");
	act("$n exits to the east.", false, actor, null, null, constants.TO_ROOM);
	actor.moveToRoom(getRoom(10391));
	actor.comm("look");
}
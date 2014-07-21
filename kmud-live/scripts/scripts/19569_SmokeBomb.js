var script19569 = function(self, actor, here, args, extra) {
	here.echo("Test");
	waitpulse 4;
	actor.send("You vanish in a cloud of smoke!");
	actor.moveToRoom(getRoom(1700));
	actor.comm("look");
	here.echo("A cloud of smoke fills the room!");
	self.extract();
};//////
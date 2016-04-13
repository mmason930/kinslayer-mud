var script21431 = function(self, actor, here, args, extra) {
	_block;
	actor.send("You peer over the edge, looking down into the ravine.");
	act("$n peers off the edge of the log, into the ravine.", true, actor, null, null, constants.TO_ROOM);
	wait 2;
	actor.send("You close your eyes and take a flying leap off the log!");
	act("$n closes $s eyes and takes a flying leap off the log!");
	actor.moveToRoom( getRoom(21453) );
	actor.comm("look");
	actor.send("You land hard on your legs, and fall to your knees!");
	actor.send("Ouch!");
	actor.damage(10 + random(0,5));
	act("$n falls in from above, landing hard on $s knees!", false, actor, null, null, constants.TO_ROOM);
}
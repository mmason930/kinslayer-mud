var script23201 = function(self, actor, here, args, extra) {
	_block;
	var arg_list = args.split(" ");
	if( arg_list.length <= 1 ) {
	    actor.send("Steer, yes, but which way?!");
	    return;
	}
	var dir = dirFromText(arg_list[1]);
	if( dir < 0 || dir > 3) {
	    actor.send("That's an invalid direction.");
	    return;
	}
	if( self.shipDirection == dir ) {
	    actor.send("The ship is already facing that direction.");
	    return;
	}
	actor.send("You begin steering...");
	here.echoaround(actor, actor.name + " begins to turn the steering wheel.");
	actor.startTimer(1);
	var success = runTimer(actor);
	if( success ) {
	    //Timer ran to completion.
	    self.shipDirection=dir;
	    self.sendToOutdoors("The ship turns to the " + dirToText(dir) + ".");
	    self.sendToIndoors("The ship sways as it slowly turns.");
	}
	else
	    actor.send("You are interrupted as you attempt to steer the ship!");
}
var script13202 = function(self, actor, here, args, extra) {
	if( extra.direction == constants.WEST ) {
		_block;
		actor.send("You begin climbing your way to the west...");
		here.echoaround(actor, capFirstLetter(actor.name) + " begins climbing " + actor.hisHer() + " way to the west.");
		actor.startTimer(3);
		var result = runTimer(actor);
		
		if( result == true )
		{//25% chance of slipping back in
		    if( random(1,100) <= 25 ) {
			    //Fail.
				actor.send("You fall back down into the sink hole.");
				here.echoaround(actor, capFirstLetter(actor.name) + " falls back down into the sink hole.");
				actor.moveToRoom(here);
				actor.comm("look");
			}
			else {
			    //Success.
				actor.send("You manage to pull through.");
				here.echoaround(actor, capFirstLetter(actor.name) + " manages to pull " + actor.himHer() + "self to the west.");
				actor.moveToRoom( here.direction( constants.WEST ) );
				actor.comm("look");
				actor.room.echoaround(actor, capFirstLetter(actor.name) + " climbs over from the east.");
			}
		}
	}
}

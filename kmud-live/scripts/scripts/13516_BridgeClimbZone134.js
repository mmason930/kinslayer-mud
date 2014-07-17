var script13516 = function(self, actor, here, args, extra) {
	var dir = extra.direction
	if( dir == constants.WEST || dir == constants.EAST ) {
	        _block
	        actor.send("You leap from your boat and try to climb up the bridge!...");
	        here.echoaround(actor, capFirstLetter(actor.name) + " jumps from " + his_her(actor.sex) + " boat and tries to climb up the bridge!");
	        actor.startTimer(3)
	        var result = runTimer(actor);
	        if( result == true )
	        {//25% chance of slipping back in
	            if( random(1,100) <= 25 ) {
	                    //Fail.
	                        actor.send("You slip and fall back down into your boat!");
	                        here.echoaround(actor, capFirstLetter(actor.name) + " looses" + his_her(actor.sex) + " footing and falls back down into the boat.");
	                        actor.moveToRoom(here);
	                        actor.comm("look");
	                }
	                else {
	                    //Success.
	                        actor.send("You manage to climb up and onto the bridge!");
	                        here.echoaround(actor, capFirstLetter(actor.name) + " manages to climb up and onto the bridge.");
	                        actor.moveToRoom( here.direction( dir ) );
	                        actor.comm("look");
	                        actor.room.echoaround(actor, capFirstLetter(actor.name) + " climbs up and out of " + his_her(actor.sex) + "boat.");
	                }
	        }
	}
}
var script10633 = function(self, actor, here, args, extra) {
	_block;
	if (actor.inClan(7) && !actor.affectedBy(constants.AFF_NOQUIT)) {
	                waitpulse 4;
	                actor.send("You open the door to the wine cellar and crawl inside.");
	                act("$n opens a rotting wooden door and leaves north.", true, actor, null, null, constants.TO_ROOM);
	                act("The winecellardoor swings shut behind $n.", true, actor, null, null, constants.TO_ROOM);
	                actor.moveToRoom( getRoom(20875) );
	                waitpulse 14;
	                actor.comm("look");
	}
	else {
	
	        actor.send("The door is locked from the inside.");
	}
}
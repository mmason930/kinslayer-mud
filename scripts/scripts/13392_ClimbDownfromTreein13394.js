var script13392 = function(self, actor, here, args, extra) {
	_block;
	actor.send("You begin climbing down the tree.");
	actor.startTimer(5);
	var success = runTimer(actor);
	if ( success ) {
		actor.send("\nYou reach the base of the tree and make a short leap to the ground.");
		actor.moveToRoom( getRoom(13344) );
		actor.comm("look");
	}
	else {
		waitpulse 1;
		actor.send("\nYou lose your grip on the tree and plummet to the forest floor!");
		actor.moveToRoom( getRoom(13344) );
		actor.comm("look");
		actor.damage(5);
		actor.send("\nOuch! That hurt!");
	}
}
var script526 = function(self, actor, here, args, extra) {
	_block;
	if( !self.isPlaying( actor ) ) {
		actor.send("You aren't even playing!");
	}
	else if( !self.isMode("SNOOZE") && !self.isMode("SIGNUP") ) {
		actor.send("You can't leave while the game is in progress.");
	}
	else {
		actor.send("You leave the game.");
		act("$n has left the game.", false, actor, null, null, constants.TO_ROOM);
		self.removePlayer( actor );
	}
}
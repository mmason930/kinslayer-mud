var script524 = function(self, actor, here, args, extra)
{
	_block;
if( !self.isPlaying( actor ) ) {
	if( self.isMode("SNOOZE") ) {
		actor.send("You are the first player.");
	}
	else if( self.isMode("SIGNUP") ) {
	}
	else {
		actor.send("This game is already in progress.");
		return;
	}
	self.addPlayer( actor );
	actor.send("You join the game.");
	act("$n has joined the game.", false, actor, null, null, constants.TO_ROOM);
	
	if( self.isMode("SNOOZE") ) {
		self.beginCountdown();
	}
}
else
	actor.send("You are already signed up.");

};


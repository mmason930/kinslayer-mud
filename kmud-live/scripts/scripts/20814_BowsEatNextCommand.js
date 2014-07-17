var script20814 = function(self, actor, here, args, extra) {
	// Fogel, January 2011 - Delays the command that the shooter used to cancel their shoot timer.
	var charArgs = args;
	_block;
	actor.lag(9);
	setTimeout(9, argsTimeout, charArgs);
	function argsTimeout(args) {
		actor.comm(args);
	}
}
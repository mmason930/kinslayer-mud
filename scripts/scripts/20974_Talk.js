var script20974 = function(self, actor, here, args, extra) {
	// This command is here to inform new players how to communicate. They often try to
	// use the "talk" command, which isn't valid, so we'll prompt them if they try to do so.
	if(actor.level <= 5) {
		_block;
		getCharCols(actor);
		actor.send("To talk, try using the " + cyn + "SAY" + nrm + " command.");
	}
}
var script20974 = function(self, actor, here, args, extra) {
	if(actor.level <= 5) {
		_block;
		getCharCols(actor);
		actor.send("To talk, try using the " + cyn + "SAY" + nrm + " command.");
	}
}
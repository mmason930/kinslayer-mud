var script21906 = function(self, actor, here, args, extra) {
	// Exit Clan Room Script for Blademasters
	// Fogel - June 2010
	_block;
	var vArgs = getArgList(args);
	if(!actor.inClan(14)) {
		return;
	}
	if(actor.position != constants.POS_STANDING){
		actor.send("You're not in the correct position.");
		return;
	}
	actor.send("You exit through the brush.");
	echoaround(actor, actor.name + " disappears into the brush.");
	actor.moveToRoom(getRoom(1253));
	getRoom(1253).echoaround(actor,actor.name + " appears from the brush.");
	actor.comm("look");
	return;
}
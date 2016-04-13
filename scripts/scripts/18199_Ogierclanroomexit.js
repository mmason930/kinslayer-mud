var script18199 = function(self, actor, here, args, extra) {
	// Exit Clan Room Script for Ogier
	var vArgs = getArgList(args);
	if(!actor.inClan(30)) {
		return 1;
	}
	if(actor.position != constants.POS_STANDING){
		actor.send("You're not in the correct position.");
		return 0;
	}
	if(strn_cmp(vArgs[1],"stump",3)) {
		actor.send("You leave the stump.");
		echoaround(actor, actor.name + " leaves the stump.");
		actor.moveToRoom(getRoom(18194));
		getRoom(18194).echoaround(actor,actor.name + " comes from inside the stump.");
		actor.comm("look");
		return 0;
	}
	return 1;
}
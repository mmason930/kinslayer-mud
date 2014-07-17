var script18194 = function(self, actor, here, args, extra) {
	// Enter Clan Room Script for Ogier
	// Rhollor Jan 2010
	var vArgs = getArgList(args);
	if(!actor.inClan(30)) {
		return 1;
	}
	if(actor.position != constants.POS_STANDING){
		actor.send("You're not in the correct position.");
		return 0;
	}
	if(strn_cmp(vArgs[1],"stump",4)) {
		if(actor.affectedBy(constants.AFF_NOQUIT)) {
			actor.send("You must collect yourself first.");
		}
		else {
			actor.send("You walk into the fukn stump.");
			echoaround(actor, actor.name + " enters the stump.");
			actor.moveToRoom(getRoom(18199));
			getRoom(18199).echoaround(actor,actor.name + " walks in.");
			actor.comm("look");
		}
		return 0;
	}
	return 1;
}
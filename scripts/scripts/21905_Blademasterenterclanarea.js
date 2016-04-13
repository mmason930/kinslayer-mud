var script21905 = function(self, actor, here, args, extra) {
	// Enter Clan Room Script for Blademasters
	// June 2010 - Fogel
	var vArgs = getArgList(args);
	if(!actor.inClan(14)) {
		return 1;
	}
	if(actor.position != constants.POS_STANDING){
		actor.send("You're not in the correct position.");
		return 0;
	}
	if(strn_cmp(vArgs[1],"brush",3)) {
		if(actor.affectedBy(constants.AFF_NOQUIT)) {
			actor.send("You must collect yourself first.");
		}
		else {
			actor.send("You make your way through the brush.");
			echoaround(actor, actor.name + " disappears into the brush.");
			actor.moveToRoom(getRoom(21905));
			getRoom(21905).echoaround(actor,actor.name + " pops out of the brush.");
			actor.comm("look");
		}
		return 0;
	}
	return 1;
}
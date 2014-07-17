var script8105 = function(self, actor, here, args, extra) {
	// Enter Clan Room Script for Soulless
	// Rhollor Jan 2010
	var vArgs = getArgList(args);
	if(!actor.inClan(25)) {
		return 1;
	}
	if(actor.position != constants.POS_STANDING){
		actor.send("You're not in the correct position.");
		return 0;
	}
	if(strn_cmp(vArgs[1],"crevice",4)) {
		if(actor.affectedBy(constants.AFF_NOQUIT)) {
			actor.send("You must collect yourself first.");
		}
		else {
			actor.send("You slip through the crevice.");
			echoaround(actor, actor.name + " seems to disappear.");
			actor.moveToRoom(getRoom(8174));
			getRoom(8174).echoaround(actor,actor.name + " suddenly appears.");
			actor.comm("look");
		}
		return 0;
	}
	return 1;
}
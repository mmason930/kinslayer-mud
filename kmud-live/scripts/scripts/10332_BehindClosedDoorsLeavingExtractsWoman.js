var script10332 = function(self, actor, here, args, extra) {
	//Alder
	//Leave trigger
	var qName = "Behind Closed Doors";
	if ( actor.quest(qName) > 2 && actor.quest(qName) < 4 ) {
		var nella = actor.getCharRoomVis("Nella");
		if ( nella ) {
			nella.moveToRoom(getRoom(501));
			nella.extract();
			waitpulse 1;
			getCharCols(actor);
			actor.send(bld+cyn+"You leave the woman behind. Return to Zind with the information you've learned."+nrm);
		}
	}
}
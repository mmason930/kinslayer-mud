var script4522 = function(self, actor, here, args, extra) {
	//Alder
	//Nella: Leaving Purges Her
		
		var qName = "Where Ends Meet";
		if ( Quest.getByName(qName).hasCompleted(actor) && !Quest.getByName("Branching Out").hasBegun(actor) ) {
			var nella = actor.getCharRoomVis("nella");
			if ( nella != undefined ) {
				nella.moveToRoom(getRoom(501));
				nella.extract();
			}
		}		
}
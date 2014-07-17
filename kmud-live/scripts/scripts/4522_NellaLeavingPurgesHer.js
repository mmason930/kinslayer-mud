var script4522 = function(self, actor, here, args, extra) {
	//Alder
	//Nella: Leaving Purges Her
		
		var qName = "Where Ends Meet";
		if ( actor.quest(qName) == -1 && actor.quest("Branching Out") == 0 ) {
			var nella = actor.getCharRoomVis("nella");
			if ( nella != undefined ) {
				nella.moveToRoom(getRoom(501));
				nella.extract();
			}
		}		
}
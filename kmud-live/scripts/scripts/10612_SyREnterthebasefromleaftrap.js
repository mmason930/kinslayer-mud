var script10612 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var targetStr = vArgs[1];
	if ( targetStr == "leaftrap" && actor.quest('Search and Rescue') > 0 ) {
	     _block;
	     getCharCols(actor);
	     actor.send(bld+cyn+"You dig under the leaves and discover a trap door!"+nrm);
	     wait 1;
	     actor.send("You open the leaftrap.");
	     waitpulse 4;
	     echoaround(actor, actor.name + " opens the leaftrap and leaves.");
	     actor.moveToRoom(getRoom(20865));
	     getRoom(20865).echoaround(actor, actor.name + " enters from the up.");
	     actor.comm("look");
	for (var _autoKey in actor.followers ) {
		var follower = actor.followers [_autoKey];
	        if( follower.room != here )
	        {
	                continue;
	        }
	        else
	        {
	                act( "$n follows $N through the leaftrap.", true, follower, null, actor, constants.TO_NOTVICT );
	                act( "You follow $N through the leaftrap.", true, follower, null, actor, constants.TO_CHAR );
	                follower.moveToRoom(getRoom(20865));
	                echoaround( follower, capFirstLetter(follower.name) + " enters the leaftrap." );
	                follower.comm( "look" );
	        }
	 }
	}
}
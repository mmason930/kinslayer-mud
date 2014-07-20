var script10250 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	_block;
	if( actor.position != constants.POS_STANDING )
	{
	        actor.send("You're not in the correct position.");
	        return;
	}
	if( strn_cmp(vArgs[1],"out",2) )
	{
	        actor.send("You crawl out of the mines.");
	        echoaround(actor, actor.name + " crawls out through the mine's entrance.");
	        actor.moveToRoom(getRoom(10249));
	        getRoom(10249).echoaround(actor, actor.name + " crawls out of the mines.");
	        actor.comm("look");
	for (var _autoKey in actor.followers ) {
		var follower = actor.followers [_autoKey];
	        if( follower.position != constants.POS_STANDING )
	        {
	                continue;
	        }
	        if( follower.room != here )
	        {
	                continue;
	        }
	        else
	        {
	                act( "$n follows $N out through the mine's entrance.", true, follower, null, actor, constants.TO_NOTVICT );
	                act( "You follow $N out through the mine's entrance.", true, follower, null, actor, constants.TO_CHAR );
	                follower.moveToRoom(getRoom(10249));
	                echoaround( follower, capFirstLetter(follower.name) + " crawls out of the mines." );
	                follower.comm( "look" );
	        }
	 }
	}
}
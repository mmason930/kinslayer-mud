var script10249 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	_block;
	if( actor.position != constants.POS_STANDING )
	{
	        actor.send("You're not in the correct position.");
	        return;
	}
	if( strn_cmp(vArgs[1],"entrance",4) )
	{
	        actor.send("You crawl through the mine's entrance.");
	        echoaround(actor, actor.name + " crawls into the mine's entrance.");
	        actor.moveToRoom(getRoom(10250));
	        getRoom(10250).echoaround(actor, actor.name + " crawls in from outside.");
	        actor.comm("look");
	for each( var follower in actor.followers ) {
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
	                act( "$n follows $N through the mine's entrance.", true, follower, null, actor, constants.TO_NOTVICT );
	                act( "You follow $N through the mine's entrance.", true, follower, null, actor, constants.TO_CHAR );
	                follower.moveToRoom(getRoom(10250));
	                echoaround( follower, capFirstLetter(follower.name) + " crawls in from outside." );
	                follower.comm( "look" );
	        }
	 }
	}
}
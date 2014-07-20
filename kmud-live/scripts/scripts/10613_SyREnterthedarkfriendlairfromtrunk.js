var script10613 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var cmd = vArgs[0];
	var correctCommand = ( strn_cmp(cmd,"enter",2) || strn_cmp(cmd,"search",2) || strn_cmp(cmd,"open",2) );
	if ( correctCommand ) {
		var targetStr = vArgs[1];
		if ( actor.quest('Search and Rescue') > 0 && targetStr == "trunk" && !actor.questTaskProgress('Search and Rescue')[4].comp ) {
			 _block;
			 getCharCols(actor);
			 actor.send(bld+cyn+"Breaking away the bark reveals a way inside the trunk!"+nrm);
			 wait 1;
			 actor.send("You enter the hollow ironwood.");
			 waitpulse 4;
			 echoaround(actor, actor.name + " enters the ironwood trunk and leaves.");
			 actor.moveToRoom(getRoom(1474));
			 getRoom(1474).echoaround(actor, actor.name + " enters from the up.");
			 actor.comm("look");
			 for (var _autoKey in actor.followers ) {
			 	var follower = actor.followers[_autoKey];
				if( follower.room != here )
				{
						continue;
				}
				else
				{
						act( "$n follows $N through the ironwood trunk.", true, follower, null, actor, constants.TO_NOTVICT );
						act( "You follow $N through the ironwood trunk.", true, follower, null, actor, constants.TO_CHAR );
						follower.moveToRoom(getRoom(1474));
						echoaround( follower, capFirstLetter(follower.name) + " enters the ironwood trunk." );
						follower.comm( "look" );
				}
		 }
		}
	}
}
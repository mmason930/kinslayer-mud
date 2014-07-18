var script210 = function(self, actor, here, args, extra) {
	//	June 2010 - Fogel
	//	Allows a human to "whistle" or a fade/dreadlord to "summon", calling his/her horse
	var vArgs = getArgList(args);
	var mount = getSval( actor, 207, "horse" );
	if(mount && mount.isValid==false) {//If a mount dies, the variable persists, but is set to invalid. We need to delete it.
		setSval(actor, 207, "horse", undefined);
		mount=undefined;
	}
	if( !mount || actor.getSkillLevel( "Ride" ) < 5 )
	{
		_noblock;
		return;
	}
	if( strn_cmp(vArgs[0], "whistle", 3) )
	{
		_block;
		waitpulse 1;
		echoaround(actor, actor.name + " raises " + actor.hisHer() + " fingers to " + actor.hisHer() + " lips and whistles loudly.");
		actor.send("You raise your fingers to your lips and whistle loudly.");
		waitpulse 3;
		if( mount.fighting )
			mount.comm("flee");
		var path = mount.room.pathToRoom( actor.room );
		for each( var dir in path )
		{
			if( mount.fighting || mount.room.doorIsClosed(dir) )
				break;
			waitpulse 2;
			mount.comm( dirToText(dir) );
		}
	}
	else if( strn_cmp(vArgs[0], "summon", 3 ) && actor.class == constants.CLASS_FADE || actor.class == constants.CLASS_DREADLORD )
	{
		_block;
		actor.startTimer(10);
		var success = runTimer(actor);
		if (success)
		{
			var cost = ( actor.class == constants.CLASS_FADE ) ? 30 : 25;
			var resource = (actor.class == constants.CLASS_FADE) ? actor.shadowPoints : actor.sps
			if( resource < cost )
			{
				actor.send("You don't have the energy to do that!");
				return;
			}
			if( actor.class == constants.CLASS_FADE )
			{
				actor.send("You beckon towards the shadows and your mount emerges from their depths, roaring angrily.");
				echoaround(actor, actor.name + " beckons towards the shadows and " + mount.name + " emerges, roaring in anger!");
				echoaround(mount, mount.name + " is suddenly pulled into the shadows and disappears!");
				actor.shadowPoints -= 30;
			}
			else if( actor.class == constants.CLASS_DREADLORD )
			{
				actor.send("You raise your arms to the sky and a massive flame erupts, out of which your mount steps!");
				echoaround(actor, actor.name + " raises " + actor.hisHer() + " towards the sky and a blazing inferno erupts, out of which " + mount.name + " emerges!");
				echoaround(mount, mount.name + " is suddenly engulfed in flames, disappearing completely!");
			}
			mount.moveToRoom(actor.room);
		}
		else
		{
		   actor.send("Cancelled.");
		}
	}
}
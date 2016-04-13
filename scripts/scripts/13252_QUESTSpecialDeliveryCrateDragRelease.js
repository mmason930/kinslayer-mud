var script13252 = function(self, actor, here, args, extra) {
	if( !Quest.getByName( "Special Delivery" ).hasBegun(actor) )
		return;
	var vArgs = getArgList( args );
	if( vArgs.length >= 2 && !str_cmp(vArgs[1], "crate") )
	{
		_block;
		var lastDragger = getSval(self, 13252, "draggedBy");
		if( !str_cmp(vArgs[0], "drag") && !getSval(self, 13252, "noDrag") )
		{/*** DRAGGING ***/
			if( lastDragger != undefined && lastDragger.isValid )
			{
				if( lastDragger == actor )
					actor.send("You are already dragging the crate.");
				else
					act("$N is already dragging the crate.", false, ch, null, lastDragger, constants.TO_CHAR);
				return;
			}
			
			actor.send("You begin dragging " + self.name + ".");
			act("$n begins dragging $P.", true, actor, null,  self, constants.TO_ROOM);
			setSval(self, 13252, "draggedBy", actor);
			wait 1;
			actor.questSend("To stop dragging the crate, simply type 'release crate'");
		}
		else if( !str_cmp(vArgs[0], "release") )
		{/*** RELEASE ***/
			if( lastDragger != actor )
			{
				actor.send("But you aren't even dragging it!");
				return;
			}
			actor.send("You release the crate.");
			act("$n releases the crate.", true, actor, null, null, constants.TO_ROOM);
			setSval(self, 13252, "draggedBy", null);
		}
	}
	
}
var script23262 = function(self, actor, here, args, extra) {
	if ( actor.name == getSval(self, 23259, "attached") )
	{
	    _block;
	    var dir = extra.direction;
	    var start_room = here;
	    actor.startTimer(2);
	    var success = runTimer(actor);
	    if ( success ) //Actor moves with crate and exit and entrance is displayed to others.
	    {
	        actor.send("\nYou drag " + self.name + " along the floor as you move " + dirToText(dir) + ".\n");
	        here.echoaround(actor, actor.name + " drags " + self.name + " along the floor as " + actor.heShe() + " leaves " + dirToText(dir) + ".\n");
	        var rev_dir = revDir(dir); //Numerical value of arriving direction
	        var dt = ("the " + dirToText(rev_dir) );           //Text version of arriving direction
	        if ( rev_dir == constants.UP )
	            var dt = "above";             //Message changes to 'above' since direction of start room is UP
	        else if ( rev_dir == constants.DOWN )
	            var dt = "below";             //Message changes to 'below' since direction of start room is DOWN
	        actor.moveToRoom( here.direction(dir) );
	        self.moveToRoom(actor.room);
	        actor.comm("look");
	        actor.room.echoaround(actor, actor.name + " enters from " + dt + " dragging " + self.name + ".\n");
			return;
	    }
	    else          //Actor's movement is interrupted and sits down.
	    {
	        actor.send("\nYou lose your grip on " + self.name + " and stumble.\n");
	        actor.room.echoaround(actor, actor.name + "'s hands slip from " + self.name + ", causing " + actor.himHer() + " to stumble.\n");
	        actor.comm("sit");
			setSval(actor, 23259, "attached", undefined);
	        setSval(self, 23259, "attached", undefined);                 /*Disassociate actor and crate*/
			return;
	    }
	}
}

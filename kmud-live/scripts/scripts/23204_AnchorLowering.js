var script23204 = function(self, actor, here, args, extra) {
	var ship = getMyShip(here);
	if( args == "lower anchor" )
	{
	    _block;
	    if( ship.anchored )
	    {
	        actor.send("The ship's anchor is already lowered.");
	        return;
	    }
	    actor.send("You begin lowering the ship's anchor.");
	    here.echoaround(actor, actor.name + " begins lowering the ship's anchor.");
	    actor.startTimer(1);
	    var success = runTimer(actor);
	    
	    if( success ) {
	        ship.sendToShip("The ship comes to a grinding halt.");
	        ship.anchored=true;
	    }
	    else {
	        //...
	    }
	    return;
	}
	else if( args == "raise anchor" )
	{
	    _block;
	    if( !ship.anchored )
	    {
	        actor.send("The ship's anchor is already raised.");
	        return;
	    }
	    actor.send("You begin pulling the anchor by its chain.");
	    here.echoaround(actor, actor.name + " begins pulling the anchor by its chain.");
	    actor.startTimer(1);
	    var success = runTimer(actor);
	    
	    if( success ) {
	        ship.sendToShip("The ship slowly begins to drift forward.");
	        ship.anchored=false;
	    }
	    else {
	        //...
	    }
	    return;
	}
	else
	    _noblock;//Should never get here, but...
}
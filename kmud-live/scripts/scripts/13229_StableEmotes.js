var script13229 = function(self, actor, here, args, extra) {
	var rand = random(1,4);
	if( rand == 1 )
	    here.echo("A horse whinnies from within it's stall.");
	else if( rand == 2 )
	    here.echo("A young stablehand walks by you, shoveling manure.");
	else if( rand == 3 )
	{
	    target = getRandomPersonInRoom(here);
	    if( target != self )
	    {
	        here.echoaround(target, "A horse nudges " + target.name + ", probably looking for food.");
	        target.send("A horse nudges you with it's head, looking for food.");
	    }
	}
}
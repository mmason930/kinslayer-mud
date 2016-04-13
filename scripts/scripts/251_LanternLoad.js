var script251 = function(self, actor, here, args, extra) {
	nrOfLanterns = getSval(self,  251, "lanterns for " + actor.id);
	if( !nrOfLanterns )
	    nrOfLanterns = 0;
	if( nrOfLanterns < 1 ) {
	    setSval( self,  251,  "lanterns for " + actor.id, nrOfLanterns + 1 );
	    actor.loadObj( 2042 );
	    here.echoaround(actor, "An elderly librarian gets a lantern from under the desk and hands it to " + actor.name + ".");
	actor.send("The elderly librarian gets a lantern from under the desk and hands it to you.");
	}
	else
	    self.say("You already received a lantern. Don't be greedy!");
}
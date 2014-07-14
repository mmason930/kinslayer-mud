var script13204 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList( args );
var aliases = getArgList( self.namelist );
var target = actor.getCharRoomVis(vArgs[1]);
if( target != self )
	return;
if ( getSval(actor, 13203, "Story") != 0 )
	return;
setSval(actor, 13203, "Story", 1);//This is what we will check periodically to see if parts of the interaction can be done
waitpulse 6;
self.say("What are you looking at? Don't judge me just because I'm dressed like this!");
waitpulse 8;
self.comm("emote scoffs and pushes a potato around his plate with a fork.");
waitpulse 6;
self.say("Buy me a mug of ale and I'll tell you my story.");

};


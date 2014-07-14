var script10653 = function(self, actor, here, args, extra)
{
	///Jenkin 
//October 2010
//Quest: An Artifact of a Lost Age 
var qName = 'An Artifact of a Lost Age';
if ( actor.questInProgress(qName) ) {
_block;
                waitpulse 4;
                actor.send("You wedge yourself into the entrance and crawl inside.");
                act("$n wedges himself into the entrance and leaves down.", true, actor, null, null, constants.TO_ROOM);
                actor.moveToRoom( getRoom(20877) );
                waitpulse 14;
                actor.comm("look");
}
else {
_block;
        actor.send("The way is shut.");
}

};


var script18666 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
_block;
if( vArgs.length >= 2 && vArgs[1] == "ladder" ) {
	actor.send("You begin your attempt to climb the ladder...");
	actor.startTimer(3);
	var success = runTimer(actor);
	if( !success )
		return;//Timer itself has failed.
	if( random(1,100) >= 80 ) {
		actor.send("You reach up and grab hold of the rotting ladder only to fall back as a piece breaks off!");
		here.echoaround(actor, actor.name + " reaches up and grabs hold of the rotting ladder only to fall back as a piece breaks off!");
		actor.damage(random(1,20));
	}
	else {
		actor.send("");
		here.echoaround(actor, actor.name + " ");
		actor.moveToRoom(getRoom(18662));
		actor.comm("look");
	}
}
else
	actor.send("Climb what?!");

};


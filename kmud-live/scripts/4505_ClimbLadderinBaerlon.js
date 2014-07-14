var script4505 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args);
if(strn_cmp(vArgs[1], "ladder",2) && actor.position != constants.POSITION_FIGHTING) {
	var dest = here;
	if(here.vnum == 4501)
		dest = getRoom(4502);
	else if(here.vnum == 4584)
		dest = getRoom(4589);
	var timerLength = 21 - (actor.strength + actor.dexterity)/2;
	actor.send("You try to climb the ladder.");
	echoaround(actor, actor.name + " starts to climb up the ladder.");
	actor.startTimer(timerLength);
	var complete = runTimer(actor);
	if(complete) {
		actor.send("\nYou climb up the ladder.");
		echoaround(actor, actor.name + " climbs up the ladder.");
		actor.moveToRoom(dest);
		echoaround(actor, actor.name + " climbs up from the ladder.");
		actor.comm("look");
	}
}
else
	actor.send("Climb what?!");

};


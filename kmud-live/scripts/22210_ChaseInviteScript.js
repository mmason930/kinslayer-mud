var script22210 = function(self, actor, here, args, extra)
{
	_block;
var doorDir = constants.SOUTH;
if( actor.name != "Chase" ) {
	actor.send("This isn't your house!");
	return;
}
var vArgs = getArgList(args);
if( vArgs.length < 3 || (str_cmp(vArgs[2], "inside") && str_cmp(vArgs[2], "inside.")) ) {
	actor.send("Invite who inside?");
	return;
}
var target = getCharInListVis(actor, vArgs[1], here.direction(doorDir).people);
if( !target ) {
	actor.send("That person doesn't seem to be at the door.");
	return;
}
if( target.vnum != -1 ) {
	actor.send("They don't seem to want to come inside...");
	return;
}
wait 1;
target.send("You open the door to the " + dirToText(revDir(doorDir)) + " and enter the house.");
act("$n opens the door to the " + dirToText(revDir(doorDir)) + " and enters the house.", true, target, null, null, constants.TO_ROOM);
target.moveToRoom(here);
target.comm("look");
act("$n enters through the door to the " + dirToText(doorDir) + ".", false, target, null, null, constants.TO_ROOM);

};


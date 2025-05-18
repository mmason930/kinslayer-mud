var script22377 = function(self, actor, here, args, extra) {
	_block;
	 if( actor.inClan(constants.CLAN_BLUE_AJAH) == false ) {
	        actor.send("It is not your place to be dismissing people from here.");
	        return;
	}
	var vArgs = getArgList(args);
	if( vArgs.length < 2) {
	        actor.send("Dismiss who?");
	        return;
	}
	var target = getCharInListVis(actor, vArgs[1], here.people);
	if( !target ) {
	        actor.send("That person doesn't seem to be here.");
	        return;
	}
	if( target.vnum != -1 ) {
	        actor.send("They don't seem to be able to leave...");
	        return;
	}
	wait 1;
	target.send("You open the bluedoor to the east and leave the Blue Chambers.");
	act("$n opens the bluedoor to the east and leaves the Blue chambers.", true, target, null, null, constants.TO_ROOM);
	target.moveToRoom(getRoom(22352));
	target.comm("look");
	act("$n leaves through the bluedoor to the east.", false, target, null, null, constants.TO_ROOM);
}
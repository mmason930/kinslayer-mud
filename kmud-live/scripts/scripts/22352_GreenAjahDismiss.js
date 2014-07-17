var script22352 = function(self, actor, here, args, extra) {
	_block;
	 if( actor.inClan(constants.CLAN_RED_AJAH) == false ) {
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
	target.send("You open the greendoor to the west and leave the Green Lounge.");
	act("$n opens the greendoor to the west and leaves the Green Lounge.", true, target, null, null, constants.TO_ROOM);
	target.moveToRoom(getRoom(22352));
	target.comm("look");
	act("$n leaves through the greendoor to the west.", false, target, null, null, constants.TO_ROOM);
}
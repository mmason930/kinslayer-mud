var script22359 = function(self, actor, here, args, extra) {
	_block;
	 if( actor.inClan(constants.CLAN_GRAY_AJAH) == false ) {
	        actor.send("It is not your place to be inviting people inside here.");
	        return;
	}
	var vArgs = getArgList(args);
	if( vArgs.length < 3 || (str_cmp(vArgs[2], "inside") && str_cmp(vArgs[2], "inside.")) ) {
	        actor.send("Invite who inside?");
	        return;
	}
	var target = getCharInListVis(actor, vArgs[1], here.direction(constants.NORTH).people);
	if( !target ) {
	        actor.send("That person doesn't seem to be at the door.");
	        return;
	}
	if( target.vnum != -1 ) {
	        actor.send("They don't seem to want to come inside...");
	        return;
	}
	wait 1;
	target.send("You open the graydoor to the south and enter a small foyer.");
	act("$n opens the graydoor to the north and enters a small foyer.", true, target, null, null, constants.TO_ROOM);
	target.moveToRoom(here);
	target.comm("look");
	act("$n enters through the graydoor to the north.", false, target, null, null, constants.TO_ROOM);
}
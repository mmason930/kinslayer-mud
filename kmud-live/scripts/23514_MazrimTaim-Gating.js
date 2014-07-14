var script23514 = function(self, actor, here, args, extra)
{
	if( !actor.inClan( 15 ) ) return;
var vArgs = getArgList(args);
if( vArgs.length < 2 || str_cmp(vArgs[1],"me") ) return;
var iDestinationRoomVnum = -1;
if( vArgs.length >= 4 && !str_cmp(vArgs[2],"fal") && (!str_cmp(vArgs[3],"dara") || !str_cmp(vArgs[3],"dara.")) ) {
	iDestinationRoomVnum = 1799;
}
waitpulse constants.PULSES_PER_SEC * 2;
if( iDestinationRoomVnum == -1 ) {
	//No valid destination provided.
	getCharCols(actor);
	self.say("I can provide you a gate to the following location(s): " + cyn + "Fal Dara" + nrm);
}
else {
	//Valid locaiton. Port the bastard.
	self.comm("nod");
	waitpulse constants.PULSES_PER_SEC*2;
	act("$n stretches $s arms outward, preparing to channel.", true, self, null, null, constants.TO_ROOM);
	wait 2;
	act("A gate forms before you. $N ushers you through.", false, actor, null, self, constants.TO_CHAR);
	act("A gate forms before $N. You usher $M through.", false, self, null, actor, constants.TO_CHAR);
	act("A gate forms before $n. $N ushers $m through.", false, actor, null, self, constants.TO_NOTVICT);
	actor.moveToRoom(getRoom(iDestinationRoomVnum));
	act("$n steps through a gate in the air, which vanishes shortly after.", false, actor, null, null, constants.TO_ROOM);
	actor.comm("look");
	actor.send("\nThe gate vanishes behind you.");
}

};


var script8156 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args.toLowerCase());
	if( vArgs[1] == "barreddoor" ) {
		wait 1;
		act("The barreddoor to the west opens just long enough to let $n out.", false, actor, null, null, constants.TO_ROOM);
		actor.send("The barreddoor to the west opens and you leave.");
		actor.moveToRoom( getRoom(8153) );
		actor.comm("look");
	}
}
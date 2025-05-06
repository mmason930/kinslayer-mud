var script22823 = function(self, actor, here, args, extra) {

	let vArgs = getArgList(args);

	if( vArgs.length >= 3 ) {
		if( !str_cmp(vArgs[0], "enter") &&
			!str_cmp(vArgs[1], "second") &&
			(!str_cmp(vArgs[2], "arch") || !str_cmp(vArgs[2], "arch."))
		) {
			_block;
			act("As you step through the second arch, the Light consumes you, and one whispered thought passes through your mind...The way back will come but once. Be steadfast.", false, actor, null, null, constants.TO_CHAR);
			act("As " + actor.name + " steps through the second arch, the Light consumes her.", false, actor, null, null, constants.TO_ROOM);
			let rand = random(1,1);
			if( rand === 1 ) {
				actor.moveToRoom(getRoom(22847));
				wait 8;
				actor.comm("gotwo");
			}
		}
	}
}
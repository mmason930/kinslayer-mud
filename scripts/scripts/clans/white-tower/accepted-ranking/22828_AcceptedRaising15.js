var script22828 = function(self, actor, here, args, extra) {
	const vArgs = getArgList(args);
	if( !str_cmp(vArgs[0], "enter") &&
		!str_cmp(vArgs[1], "third") &&
		(!str_cmp(vArgs[2], "arch") || !str_cmp(vArgs[2], "arch."))
	) {
		_block;
		act("As you step through the third arch, the Light consumes you, and one whispered thought passes through your mind...The way back will come but once. Be steadfast.", false, actor, null, null, constants.TO_CHAR); 
		act("As " + actor.name + " steps through the third arch, the Light consumes her.", false, actor, null, null, constants.TO_ROOM);
		
		let rand = random(1,1);
		if( rand === 1 ) {
			actor.moveToRoom(getRoom(22857));
			wait 8;
			actor.comm("gothree");
		}
	}
}
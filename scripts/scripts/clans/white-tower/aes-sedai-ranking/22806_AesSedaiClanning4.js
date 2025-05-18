var script22806 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( vArgs.length >= 2 && !str_cmp(vArgs[1],"portal") ) {
		_block;
		var targetRoom = getRoom(22893);
		targetRoom.loadObj(10600);
		act("$n steps in to the ter'angreal and disappears.", false, actor, null, null, constants.TO_ROOM);
		actor.send("You step in to the ter'angreal.");
		actor.moveToRoom(targetRoom);
		wait 1;
		actor.comm("look");
		wait 2;
		actor.comm("inroom");
	}
}
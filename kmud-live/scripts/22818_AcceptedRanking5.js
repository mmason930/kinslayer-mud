var script22818 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args.toLowerCase());
if( vArgs[0] == "enter" && vArgs[1] == "first" && (vArgs[2] == "arch" || vArgs[2] == "arch.") ) 
{
	act("As you step through the first arch, the Light consumes you, and one whispered thought passes through your mind...The way back will come but once. Be steadfast.", false, actor, null, null, constants.TO_CHAR); 
	act("As " + actor.name + " steps through the first arch, the Light consumes her.", false, actor, null, null, constants.TO_ROOM);
	_block;
	
	var rand = random(1,2);
	if( rand == 1 ) {
		actor.moveToRoom(getRoom(22837));
		wait 8;
		_block;
		actor.comm("go");
	}
	if( rand == 2 ) {
		actor.moveToRoom(getRoom(22838));
		wait 8;
		_block;
		actor.comm("go");
	}
}

};


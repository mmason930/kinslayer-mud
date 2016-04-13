var script22835 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args.toLowerCase());
	if( vArgs[0] == "enter" && (vArgs[1] == "portal" || vArgs[1] == "portal.") ) {
	   here.echo("You step into the portal, the sound of sizzling skin filling your ears.");
	actor.moveToRoom(getRoom(10575));
	wait 1;
	_block;
	actor.comm("phasethree");
	}
}
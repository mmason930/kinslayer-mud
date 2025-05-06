var script22835 = function(self, actor, here, args, extra) {
	_block;
	let vArgs = getArgList(args);
	if( !str_cmp(vArgs[0], "enter") &&
		(!str_cmp(vArgs[1], "portal") || !str_cmp(vArgs[1], "portal."))
	) {
		here.echo("You step into the portal, the sound of sizzling skin filling your ears.");
		actor.moveToRoom(getRoom(10575));
		wait 1;
		_block;
		actor.comm("phasethree");
	}
}
var script22821 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args.toLowerCase());
	if( vArgs[0] == "enter" && (vArgs[1] == "portal" || vArgs[1] == "portal.") ) {
	   here.echo("You leave your parents to their fate, and step into the searing light.");
	actor.moveToRoom(getRoom(10575));
	wait 1;
	_block;
	actor.comm("phasethree");
	}
}
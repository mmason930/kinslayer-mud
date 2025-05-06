var script22821 = function(self, actor, here, args, extra) {
	const vArgs = getArgList();

	if( !str_cmp(vArgs[0], "enter") &&
		(!str_cmp(vArgs[1], "portal") || !str_cmp(vArgs[1], "portal."))
	) {
		_block;
		here.echo("You leave your parents to their fate, and step into the searing light.");
		actor.moveToRoom(getRoom(10575));
		wait 1;

		actor.comm("phasethree");
	}
}
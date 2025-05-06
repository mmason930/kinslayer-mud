var script22831 = function(self, actor, here, args, extra) {

	let vArgs = getArgList(args);
	if( !str_cmp(vArgs[0], "enter") &&
		(!str_cmp(vArgs[1], "portal") || !str_cmp(vArgs[1], "portal."))
	) {
		_block;
		here.echo("As you pass through the portal, the hacking sound of metal tearing through skin and bone as well as the bubbling gurgle of one choking on blood fills your ears.");
		actor.moveToRoom(getRoom(10575));
		actor.comm("release");
		wait 1;

		actor.comm("partthree");
	}
}
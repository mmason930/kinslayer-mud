var script22826 = function(self, actor, here, args, extra) {
	let vArgs = getArgList(args);

	if( !str_cmp(vArgs[0], "enter") &&
		(!str_cmp(vArgs[1], "portal") || !str_cmp(vArgs[1], "portal."))
	) {
		_block;
		here.echo("As you turn and pass through the portal, you hear the sickening sound of a body being tossed into water, Masuri's haunting scream chasing you as the Light tears you apart.");
		actor.moveToRoom(getRoom(10575));
		actor.comm("release");

		wait 1;
		actor.comm("stagethree");
	}
}
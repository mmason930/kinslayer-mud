var script22826 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args.toLowerCase());
if( vArgs[0] == "enter" && (vArgs[1] == "portal" || vArgs[1] == "portal.") ) {
   here.echo("As you turn and pass through the portal, you hear the sickening sound of a body being tossed into water, Masuri's haunting scream chasing you as the Light tears you apart.");
actor.moveToRoom(getRoom(10575));
actor.comm("release");
wait 1;
_block;
actor.comm("stagethree");
}

};


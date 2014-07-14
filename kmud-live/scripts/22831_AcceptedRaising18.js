var script22831 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args.toLowerCase());
if( vArgs[0] == "enter" && (vArgs[1] == "portal" || vArgs[1] == "portal.") ) {
   here.echo("As you pass through the portal, the hacking sound of metal tearing through skin and bone as well as the bubbling gurgle of one choking on blood fills your ears.");
actor.moveToRoom(getRoom(10575));
actor.comm("release");
wait 1;
_block;
actor.comm("partthree");
}

};


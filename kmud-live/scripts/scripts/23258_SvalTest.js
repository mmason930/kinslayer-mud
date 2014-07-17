var script23258 = function(self, actor, here, args, extra) {
	here.echo("My sval:" + getSval(self, 23259, "attached"));
	here.echo("Actor sval:" + getSval(actor, 23259, "attached"));
	var arg_list = args.split(" ");
	if (arg_list[1])
	{
	    setSval(self, 23259, "attached", undefined);
	    setSval(actor, 23259, "attached", undefined);
	}
	here.echo("UP:" + constants.UP);
	here.echo("DOWN:" +constants.DOWN);
	here.echo("NORTH:" +constants.NORTH);
	here.echo("SOUTH:" +constants.SOUTH);
	here.echo("EAST:" +constants.EAST);
	here.echo("WEST:" +constants.WEST);
}
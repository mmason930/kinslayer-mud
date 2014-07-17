var script23701 = function(self, actor, here, args, extra) {
	//Spectre guard in room 23701. Blocks entrance to Maerwyn smob
	//Koradin - December 2009
	var arg_array = getArgList(args);
	if (arg_array.length < 2)
	{
	_block;
	actor.send("What gate do you wish to knock on?");
	return;
	}
	if (arg_array[1] != "irongate")
	{
	_block;
	actor.send("There is no '" + arg_array[1] + "' here.");
	return;
	}
	if (getSval(self, 23700, "knocked") > 0)
		return;
	if (self.vnum == 23702)
	{
	setSval(self, 23700, "knocked", 1);
	wait 2;
	here.loadMob(23701);
	var spectre = self.getCharRoomVis("spectre");
	here.echo("The air shimmers as a spectre fades into existence!");
	wait 2;
	spectre.say("No one may enter the manor!");
	self.extract();
	}
	else
	{
	_block;
	actor.send("As you near the irongate, the spectre attacks!");
	echoaround(actor, "As " + actor.name + " nears the irongate, the spectre attacks!");
	self.comm("kill " + actor.name);
	}
}
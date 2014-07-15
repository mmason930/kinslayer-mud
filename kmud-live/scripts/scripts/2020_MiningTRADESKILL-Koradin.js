var script2020 = function(self, actor, here, args, extra) {
	//Main Mining Trade Skill script.
	//Koradin - Converted from DG in January 2010
	_block;
	if (actor.position != constants.POS_STANDING)
	{
		actor.send("You're not in the right position to do that right now!");
		return;
	}
	else if (actor.level < 5)
	{
		actor.send("You're much too inexperienced to start developing the black lung, kid!");
		return;
	}
	else if (actor.mvs < 30)
	{
		actor.send("You're too tired to start swinging that heavy pick around!");
		return;
	}
	here.echo("Trigger activated");
}
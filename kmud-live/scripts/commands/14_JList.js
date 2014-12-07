var script14 = function(self, actor, here, args, extra) {

	if(actor.level < 100)
		return;

	_block;
	here.echo("Test");

	var vArgs = getArgList(args);

	if(vArgs.length < 2)
	{
		actor.send("Please enter the ID of the trigger for which you would like to find attachments.");
		return;
	}

	var vnumString = vArgs[1];
	if(!isNumber(vnumString))
	{
		actor.send("The trigger ID that you enter must be numeric.");
		return;
	}

	var vnum = Number(parseInt(vnumString));

	if(vnum < 0)
	{
		actor.send("The trigger ID that you enter must be zero or greater.");
		return;
	}

	here.echo("Vnum: " + vnum);
};
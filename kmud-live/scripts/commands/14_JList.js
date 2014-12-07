var script14 = function(self, actor, here, args, extra) {

	if(actor.level < 100)
		return;

	_block;

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

	for(var roomIndex = 0;;++roomIndex)
	{
		var room = getRoom(roomIndex);

		if(room == null)
			break;

		if(room.countJS(vnum) > 0)
		{
			here.echo("Room #" + room.vnum);
		}
	}
};
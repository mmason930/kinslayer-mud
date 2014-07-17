var script23705 = function(self, actor, here, args, extra) {
	//This script will block entry into Maerwynn's suite when the apparition is there.
	//Koradin - December 2009
	var arg_array = getArgList(args);
	if (arg_array.length < 2)
	{
	actor.send("Open what?");
	_block;
	return;
	}
	if (arg_array[1] != "door")
	{
	actor.send("There's no '" + arg_array[1] + "' here.");
	_block;
	return;
	}
	var apparition = actor.getCharRoomVis("apparition");
	if (!apparition)
	{
	_noblock;
	return;
	}
	else
	{
		if (getSval(actor, 23705, "maerdoor_sval") == 0)
		{
		setSval(actor, 23705, "maerdoor_sval", 1);
		actor.send("As you move to open the door, " + apparition.name + " starts cackling with insanity!");
		_block;
		echoaround(actor, "As " + actor.name + " moves to open the door, a smirking apparition starts cackling with insanity!");
		waitpulse 2;
		apparition.say("Do you really think I'll let you pass so easily?");
		}
		else
		{
		actor.send("The apparition won't let you through! Maybe you should try to ask him nicely to open the door.");
		_block;
		return;
		}
	return;
	}
}
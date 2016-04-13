var script20902 = function(self, actor, here, args, extra) {
	//For quest 'Snakes and Berries'
	//Koradin - January 2010
	var arg_array = getArgList(args);
	var quest_name = 'Snakes and Berries';
	if (arg_array.length < 2)
	{
		act("Pick what?", true, actor, null, null, constants.TO_CHAR);
		_block;
		return;
	}
	else
	{
		if (actor.hasItem(21431,true,true,true))
		{
			actor.send("You already have some berries!");
			_block;
			return;
		}
		_block;
		act("You reach over and pick some berries off of the branches.", true, actor, null, null, constants.TO_CHAR);
		act("$n reaches over and picks some berries off the branches of a nearby bush.", true, actor, null, null, constants.TO_ROOM);
		actor.send("You gathered a handful of wild berries!");
		actor.loadObj(21431);
		actor.updateJournalTask(quest_name,0);
	}
}
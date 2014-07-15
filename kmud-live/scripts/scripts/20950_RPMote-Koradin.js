var script20950 = function(self, actor, here, args, extra) {
	//RPMOTE command (RP)
	//Koradin - March 2010
	_block;
	var arg_array = getArgList(args);
	if (arg_array.length == 1)
	{
		actor.send("What would you like to roleplay to the room?");
		return;
	}
	else
	{
		var text = arg_array.splice(1);
		text = text.join(" ");
		if (text.length > 160)
		{
			actor.send("Your roleplay could not be displayed because the line was too long!");
			return;
		}
		var people = actor.room.people;
		for each(var person in people)
		{
			if (person.race == actor.race)
			{
				person.send(strColFormat(capFirstLetter(text),80) + "  [" + actor.name + "]");
			}	
			else
			{
				act("$n makes some strange gestures.", true, actor, null, person, constants.TO_VICT);
			}
		}
	}
}
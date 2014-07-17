var script20971 = function(self, actor, here, args, extra) {
	// Fire and Shelter heals
	// Koradin - March 2010
	var human = constants.RACE_HUMAN;
	var troll = constants.RACE_TROLLOC;
	var room = self.room;
	if (self.vnum == 20971) //human campfire
	{
		var race = human;
		var hps = 1;
		var mvs = 1;
		var sps = 1;
	}
	else if (self.vnum == 20972) //troll campfire
	{
		var race = troll;
		var hps = 1;
		var mvs = random(1,2);
		var sps = 1;
	}
	else if (self.vnum == 20974) //human bonfire
	{
		var race = human;
		var hps = random(1,2);
		var mvs = random(1,2);
		var sps = random(1,2);
	}
	else if (self.vnum == 20975) //troll bonfire
	{
		var race = troll;
		var hps = random(1,2);
		var mvs = random(1,3);
		var sps = random(1,2);
	}
	else if (self.vnum == 20977) //human shack
	{
		var race = human;
		var hps = random(1,3);
		var mvs = random(1,3);
		var sps = random(1,3);
	}
	else if (self.vnum == 20978) //troll hut
	{
		var race = troll;
		var hps = random(1,3);
		var mvs = random(1,4);
		var sps = random(1,3);
	}
	else if (self.vnum == 20979) //human tower
	{
		var race = human;
		var hps = random(1,4);
		var mvs = random(1,4);
		var sps = random(1,4);
	}
	else if (self.vnum == 20980) //troll tower
	{
		var race = troll;
		var hps = random(1,4);
		var mvs = random(1,5);
		var sps = random(1,4);
	}
	wait random(1,5);
	for each (var person in room.people)
	{
		if (person.race == race)
		{
			person.hps += hps;
			person.mvs += mvs;
			if (person.sps)
				person.sps += sps;
			if (person.mount)
			{
				person.mount.hps += hps;
				person.mount.mvs += mvs;
			}
		}
	}
	if (random(1,4) == 1)
	{
		wait random(1,5);
		for each (var person in room.people)
		{
			if (person.race == race && !person.isBashed && person.position != constants.POS_FIGHTING)
			{
				person.hps += hps;
				person.mvs += mvs;
				if (person.sps)
					person.sps += sps;
				if (person.mount)
				{
					person.mount.hps += hps;
					person.mount.mvs += mvs;
				}
			}
		}
	}
}
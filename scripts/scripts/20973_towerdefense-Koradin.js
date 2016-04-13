var script20973 = function(self, actor, here, args, extra) {
	//tower defense
	//Koradin - March 2010
	wait random(1,7);
	if (self.vnum == 20979)
		var race = constants.RACE_HUMAN;
	else if (self.vnum == 20980)
		var race = constants.RACE_TROLLOC;
	var first_ring = here.neighbors;
	var area_of_effect = [];
	for (var _autoKey in first_ring) {
		var room = first_ring[_autoKey];
		if (room)
		{
			if (room.sector != constants.SECT_INSIDE && room.sector != constants.SECT_CITY)
				area_of_effect.push(room);
			for (var _autoKey in room.neighbors) {
				var room2 = room.neighbors[_autoKey];
				if (room2)
				{
					if (room2.sector != constants.SECT_INSIDE && room2.sector != constants.SECT_CITY)
						area_of_effect.push(room2);
					for (var _autoKey in room2.neighbors) {
						var room3 = room2.neighbors[_autoKey];
						if (room3)
						{
							if (room3.sector != constants.SECT_INSIDE && room3.sector != constants.SECT_CITY)
								area_of_effect.push(room3);
							for (var _autoKey in room3.neighbors) {
								var room4 = room3.neighbors[_autoKey];
								if (room4)
								{
									if (room4.sector != constants.SECT_INSIDE && room4.sector != constants.SECT_CITY)
										area_of_effect.push(room4);
								}
							}
						}
					}
				}
			}
		}
	}
	var indices = [];
	var final_array = [];
	var targets = [];
	var x = 0;
	while (x < 10)
	{
		var index = 0;
		while (index != -1)
		{
			indices.push(index);
			index = area_of_effect.indexOf(area_of_effect[index], index + 1);
		}
		final_array.push(area_of_effect[indices[0]]);
		indices.reverse();
		for (var _autoKey in indices) {
			var dex = indices[_autoKey];
			area_of_effect.splice(dex,1);
		}
		indices = [];
		x += 1;
	}
	for (var _autoKey in final_array) {
		var area = final_array[_autoKey];
		if( !area ) continue;
		for (var _autoKey in area.people) {
			var person = area.people[_autoKey];
			if (person.race != race && person.vnum == -1 && person.level < 100 && person.level > 14)
			{
				targets.push(person);
			}
		}
	}
	var target = targets[random(0,targets.length - 1)];
	if (!target)
		return;
	var dmg_message = [];
	function damageMessage(dmg)
	{
		if (dmg < 5)
			dmg_message.push(" lightly.");
		else if (dmg < 10)
			dmg_message.push(".");
		else if (dmg < 15)
			dmg_message.push(" hard.");
		else if (dmg < 20)
			dmg_message.push(" very hard.");
	}
	var dmg = random(1,19);
	damageMessage(dmg);
	//here.echo("Firing on " + target.name + " in room " + target.room.name + " vnum " + target.room.vnum + ".");
	act("An archer fires an arrow from atop the tower!",true,null,self,null,constants.TO_ROOM);
	wait 1;
	if (target && target.room)
		var enter_message = dirToText(target.room.firstStep(self.room));
	if (!enter_message)
		enter_message = "";
	else if (enter_message == "up")
		enter_message = " flies in from above and";
	else if (enter_message == "down")
		enter_message = " flies in from below and";
	else
		enter_message = " flies in from the " + enter_message + " and";
	if (random(1,3) == 3) {
	act(target.red(constants.CL_SPARSE) + "An arrow" + enter_message + " hits you" + dmg_message + target.normal(constants.CL_SPARSE), true, target, null, null, constants.TO_CHAR);
	act("An arrow" + enter_message + " hits $n" + dmg_message, true, target, self, null, constants.TO_ROOM);
	target.damage(dmg);
	}
	else {
		act("An arrow" + enter_message + " narrowly misses you.", true, target, null, null, constants.TO_CHAR);
		act("An arrow" + enter_message + " narrowly misses $n.", true, target, self, null, constants.TO_ROOM);
	}
	dmg_message = [];
}
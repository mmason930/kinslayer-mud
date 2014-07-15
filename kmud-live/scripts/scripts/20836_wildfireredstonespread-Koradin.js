var script20836 = function(self, actor, here, args, extra) {
	//Wildfire from the red stone
	//Koradin - May 2010
	var total_time = 13;
	while (total_time > 0)
	{
		var room = self.room;
		if (!room)
			return;
		var items = [];
		for each (var item in room.items)
		{
			if (item.vnum == 9023)
				item.extract();
			else if (item.vnum == 9022 && item != self)
				item.extract();
	if (item.vnum == 9025)
	items.push(item);
			else if (!room.isFlagged(constants.ROOM_VAULT) && item != self && item.type != constants.ITEM_CONTAINER && item.vnum != 9024 && random(1,48) == 1 && item.canWear(constants.ITEM_WEAR_TAKE) && room.vnum != 19304 && room.vnum != 2032)
				items.push(item);
		}
		for each(var it in items)
		{
			room.echo("The blazing fire consumes " + it.name + ", leaving nothing but charred remains.");
			it.extract();
			room.loadObj(9024);
		}
		var chars = [];
		for each (var person in room.people)
		{
			if (!person.mobFlagged(constants.MOB_INVIS) && person.level < 100 && random(1,24) == 1)
				chars.push(person);	
		}
		for each(var per in chars)
		{
			per.send("You step too close to the fire and it flares up, burning you!");
			per.send("Ouch!");
			act("The blazing wildfire burns $n!",true,per,null,null,constants.TO_ROOM);
			if (!per.affectedBy(constants.AFF_BURN))
				per.affect(constants.AFF_BURN,1,0);
			per.damage(random(10,15));
		}
		var rooms = [];
		for each (var rm in room.neighbors)
		{
			if (rm)
			{
				if ( Math.floor(rm.vnum / 100) == Math.floor(room.vnum / 100) || random(1,100) == 1 )
				{
					var spread_chance = random(1,100);
					var char_exists = null;
					for each(var im in rm.items)
					{
						if (im.vnum == self.vnum)
							spread_chance += 100;
						if (im.vnum == 9023)
						{
							spread_chance += 3;
							char_exists = im;
						}
					}
					rooms.push([rm,spread_chance,char_exists]);
				}
			}
		}
		for each (var new_room in rooms)
		{
			if (new_room[1] < 5)
			{
				var from_dir = dirToText(revDir(room.firstStep(new_room[0])));
				if (from_dir == "up")
					var from = "above";
				else if (from_dir == "down")
					var from = "below";
				else
					var from = "the " + from_dir;
				if (room.doorIsClosed(room.firstStep(new_room[0])) && room.doorHidden(room.firstStep(new_room[0])) < 1)
					here.echo("The wildfire flares up, flames licking against the " + room.doorName(room.firstStep(new_room[0])) + "!");
				else if (room.doorHidden(room.firstStep(new_room[0])) > 0)
				{
					here.echo("The wildfire grows larger, the breeze fanning the flames!");
				}
				else
				{
					here.echo("The wildfire rages on, sweeping "+dirToText(room.firstStep(new_room[0]))+"!");
					new_room[0].echo("A blazing wildfire sweeps in from "+from+"!");
					new_room[0].loadObj(9022);
					if (new_room[2] != null)
						new_room[2].extract();
				}
				//here.echo("Chance to spread "+dirToText(room.firstStep(new_room[0]))+" is " + new_room[1]);
			}
		}
		if (random(1,50) == 1)
		{
			room.echo("The fire dies away as quickly as it began, leaving nothing but charred landscape.");
			for each (var prs in room.people)
			{
				if (prs.affectedBy(constants.AFF_BURN))
				{
					prs.unaffect(constants.AFF_BURN)
					prs.send("You finally manage to put out the flames before they consume you.");
					act("The flames die off of $n.",true,prs,null,null,constants.TO_ROOM);
				}
			}
			room.loadObj(9023);
			self.extract();
			return;
		}
		wait 4;
		total_time -= 4;
	}
	
}
var script20842 = function(self, actor, here, args, extra)
{
	//clever thief run
//Koradin - April 2010
var dest_check = 10000;
while (dest_check > 0)
{
	var destination = getRoom(random(100,25000));
	if (destination)
	{
		if (self.room.firstStep(destination) > -1 && self.room.firstStep(destination) < 6)
		{
			var dest = destination;
			break;
		}
	}
dest_check -= 1;
}
var meanie_msg = [];
meanie_msg.push("You'll never catch me!");
meanie_msg.push("Suckers!");
meanie_msg.push("Catch me if you can!");
meanie_msg.push("Later fools!");
meanie_msg.push("You're too slow!");
meanie_msg.push("Try to keep up, fatty!");
meanie_msg.push("Hah! My grandmother runs faster than you!");
var time = 12;
while (time > 0)
{
	if (!self)
		return;
	else if (self.fighting && random(1,3) == 1)
		self.comm("flee");
	if (random(1,2) == 1)
	{
		var char_check = 10;
		while (char_check > 0)
		{
			var actor_check = getRandomPersonInRoom(self.room);
			if (actor_check != self && self.canSee(actor_check))
			{
				var actor = actor_check;
				break;
			}
			char_check -= 1;
		}
		if (actor)
		{
			if (actor.level > 9)
			{
				var inventory = [];
				var eq = [];
				var rand_gold = random(5,100);
				for each (var item in actor.inventory)
				{
					if (actor.vnum == -1)
					{
						if (item.type != constants.ITEM_WEAPON && item.type != constants.ITEM_CONTAINER)
							inventory.push(item);
					}
					else
						inventory.push(item);
				}
				for (i = 0; i < (constants.NUM_WEARS); i++)
				{
					if (actor.eq(i))
					{
						if (actor.vnum == -1)
						{
							if (actor.eq(i).type != constants.ITEM_WEAPON && actor.eq(i).type != constants.ITEM_CONTAINER)
								eq.push(actor.eq(i));
						}
						else
							eq.push(actor.eq(i));
					}
				}
				if (inventory.length > 0)
				{
					var stolen = inventory[random(0,inventory.length - 1)];
					stolen.moveToChar(self);
				}
				else if (actor.gold > rand_gold)
				{
					actor.gold -= rand_gold;
					self.gold += rand_gold;
					var stolen = rand_gold;
				}
				else
				{
					if (eq.length > 0)
					{
						var stolen = eq[random(0,eq.length - 1)];
						stolen.moveToChar(self);
					}
				}
				if (stolen)
				{
					self.say("Thanks for the donation, " + actor.name + "!");
					self.comm("wear all");
				}
			}
		}
	}
	//self.say(dirToText(self.room.firstStep(dest)));
	var current_room = self.room;
	self.comm(dirToText(current_room.firstStep(dest)));
	if (self.room == current_room);
	{
		self.comm("open " + current_room.doorName(current_room.firstStep(dest)));
		self.comm("call");
		self.comm("crawl out");
	}
	self.comm("hide");
	wait 1;
	if (random(1,15) == 1)
		self.comm("yell " + meanie_msg[random(0,meanie_msg.length - 1)]);
	wait 1;
	time -= 1;
}

};


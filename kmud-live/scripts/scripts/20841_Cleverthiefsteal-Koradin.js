var script20841 = function(self, actor, here, args, extra) {
	//clever thief steal
	//Koradin - April 2010
	for (i = 0; i < here.people.length; i++)
	{
		if (here.people[i] != self && self.canSee(here.people[i]))
		{
			actor = here.people[i];
			break;
		}
	}
	if (!actor)
		return;
	var inventory = [];
	var eq = [];
	for each (var item in actor.inventory)
	{
		if (actor.vnum == -1)
		{
			if (item.type != constants.ITEM_WEAPON && item.type != constants.ITEM_CONTAINER && item.type != constants.ITEM_KEY)
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
	else
	{
		if (eq.length > 0)
		{
			var stolen = eq[random(0,eq.length - 1)];
			stolen.moveToChar(self);
		}
		else
		{
			var rand_gold = random(10,5000);
			if (actor.gold > rand_gold)
			{
				actor.gold -= rand_gold;
				self.gold += rand_gold;
				var stolen = rand_gold;
			}
			else
			{
				self.say("Wow... you've got absolutely nothing of value. Cheap skate!");
				self.comm("emote runs off to bother someone else.");
				self.moveToRoom(getRoom(20800));
				return;
			}
		}
	}
	
}
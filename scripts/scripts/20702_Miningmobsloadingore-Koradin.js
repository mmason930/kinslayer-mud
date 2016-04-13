var script20702 = function(self, actor, here, args, extra) {
	//Mining mobs loading ore on death
	//Koradin - May 2010
	if (self.vnum == 20840)
	{
		var number = random(1,2);
		var message = capFirstLetter(self.name)+"'s insides spill out onto the ground, revealing some ore!";
	}
	else if (self.vnum == 6110)
	{
		var number = random(1,4);
	var message = capFirstLetter(self.name) + " cries out, revealing his cache of ore!";
	}
	else if (self.vnum == 4204)
	{
		var number = random(1,3);
	var message = "As the giant scorpion falls, more scorpions burst from her midsection!";
		var mobs = random(2,5);
		while (mobs > 0)
		{
			self.room.loadMob(4203);
			mobs -= 1;
		}
	}
	self.room.echo(message);
	for (i = 0; i < number; i++)
	{
		var ore = random(4584,4599);
		self.loadObj(ore);
	}
}
var script20871 = function(self, actor, here, args, extra) {
	var pack = self.eq(constants.WEAR_BACK);
	if (!pack)
		return;
	for (var _autoKey in self.room.people) {
		var person = self.room.people[_autoKey];
		if (person.vnum == -1)
			return;
	}
	if (pack.contents.length > 15) {
		for (var _autoKey in pack.contents) {
			var item = pack.contents[_autoKey];
			item.extract();
		}
		return;
	}
	if (random(1,25) == 1 && pack.contents.length > 0)
	{
		wait random(1,5);
		var rand = random(1,3);
		if (rand==1)
			self.comm("yell Care for "+pack.contents[random(0,pack.contents.length-1)].name+"? Come see all my master's work at "+self.room.name+"!");
		else if (rand==2)
			self.comm("yell Check out "+pack.contents[random(0,pack.contents.length-1)].name+" and more at "+self.room.name+"!");
		else if (rand==3)
			self.comm("yell "+capFirstLetter(pack.contents[random(0,pack.contents.length-1)].name)+" is on sale for a limited time at "+self.room.name+"!");
		wait 60;
	}
	
	
}
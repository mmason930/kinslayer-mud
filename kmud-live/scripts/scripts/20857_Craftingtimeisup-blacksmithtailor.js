var script20857 = function(self, actor, here, args, extra) {
	var bootTime = self.getPval("smithingTime");
	var smither = self.getPval("smither");
	var namelist = self.namelist.split(" ");
	if (namelist[namelist.length-1] == "tailor")
		var place = "workshop";
	else if (namelist[namelist.length-1] == "blacksmith")
		var place = "forge";
	for each (var room in self.room.neighbors) {
		if (room)
			var newRoom = room;
	}
	if (!bootTime || !smither)
		return;
	bootTime = parseInt(bootTime);
	if (time() < bootTime)
		return;
	for each (var person in self.room.people)
	{
		if (person.name == smither)
			actor = person;
	}
	if (actor) {
		self.say("Looks like you've run out of time!");
		actor.send("The door opens just wide enough for " + self.name + " to throw you out!");
		actor.moveToRoom(newRoom);
		actor.comm("look");
		actor.send("You go sprawling into the dirt outside the "+place+"!");
		act("The door to the "+place+" opens, and $n comes sprawling out into the dirt.",true,actor,null,null,constants.TO_ROOM);
		actor.damage(random(1,3),self);
		actor.position = constants.POS_SITTING;
	}
	self.deletePval("smithingTime",true);
	self.deletePval("smither",true);
	
	
}
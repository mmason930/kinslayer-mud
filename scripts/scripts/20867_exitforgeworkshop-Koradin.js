var script20867 = function(self, actor, here, args, extra) {
	if (self.isFlagged(constants.ROOM_SMITHING))
		var type = "forge";
	else if (self.isFlagged(constants.ROOM_TAILORING))
		var type = "workshop";
	_block;
	actor.send("Ok.");
	wait 1;
	actor.send("The "+type+" door is opened just wide enough for you to leave.");
	act("$n leaves the "+type+", closing the door behind $m.",true,actor,null,null,constants.TO_ROOM);
	for (var _autoKey in actor.room.neighbors) {
		var room = actor.room.neighbors[_autoKey];
		if (room)
			var newRoom = room;
	}
	actor.moveToRoom(newRoom);
	actor.comm("look");
	act("The "+type+" door opens, and $n steps out.",true,actor,null,null,constants.TO_ROOM);
	waitpulse 2;
	for (var _autoKey in newRoom.people) {
		var per = newRoom.people[_autoKey];
		for (var _autoKey in per.namelist.split(" ")) {
			var pName = per.namelist.split(" ")[_autoKey];
			if (pName == "dude") {
				per.say("Welcome back, "+actor.name+"! The "+type+" is still yours. Just knock on the door to be let in.");
				return;
			}
		}
	}
}
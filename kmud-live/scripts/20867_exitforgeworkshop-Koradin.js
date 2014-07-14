var script20867 = function(self, actor, here, args, extra)
{
	if (self.isFlagged(constants.ROOM_SMITHING))
	var type = "forge";
else if (self.isFlagged(constants.ROOM_TAILORING))
	var type = "workshop";
_block;
actor.send("Ok.");
wait 1;
actor.send("The "+type+" door is opened just wide enough for you to leave.");
act("$n leaves the "+type+", closing the door behind $m.",true,actor,null,null,constants.TO_ROOM);
for each (var room in actor.room.neighbors) {
	if (room)
		var newRoom = room;
}
actor.moveToRoom(newRoom);
actor.comm("look");
act("The "+type+" door opens, and $n steps out.",true,actor,null,null,constants.TO_ROOM);
waitpulse 2;
for each (var per in newRoom.people) {
	for each (var pName in per.namelist.split(" ")) {
		if (pName == "dude") {
			per.say("Welcome back, "+actor.name+"! The "+type+" is still yours. Just knock on the door to be let in.");
			return;
		}
	}
}

};


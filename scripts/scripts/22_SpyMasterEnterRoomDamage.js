var script22 = function(self, actor, here, args, extra) {
	waitpulse 1
	actor.damage(20);
	act("As $n enters the room, $e trips on a snare.", false, actor, null, null, constants.TO_ROOM);
	actor.send("You trip on a snare as you enter the room.");
	actor.send("Ouch!");
}
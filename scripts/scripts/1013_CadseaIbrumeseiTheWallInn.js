var script1013 = function(self, actor, here, args, extra) {
	waitpulse 15;
	if (actor.room.vnum == 7099){
	if (random (1,5) == 1){
		self.comm("tell " + actor.name + " Get out of my kitchen! You'll ruin my cooking!");
		}
	else
		if (actor.race == constants.RACE_HUMAN){
		self.comm("tell " + actor.name + " Hello, help yourself to the scones!");
	}
	}
}
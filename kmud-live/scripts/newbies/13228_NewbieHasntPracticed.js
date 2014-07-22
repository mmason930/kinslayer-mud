var script13228 = function(self, actor, here, args, extra) {

	if(actor.level == 1 && actor.skillPractices >= 6)
	{
		var directionMap = {
			13233: constants.SOUTH,
			13247: constants.WEST,
			13338: constants.EAST
		}

		var directionRequired = directionMap[here.vnum];

		if(directionRequired == null)
			mudLog(constants.BRF, Math.MAX(actor.invis, 100), "Player `" + actor.name + "` entered room not found in map. Newbie Not Practiced Script.");

		if(directionRequired != extra.direction)
			return;

		getCharCols(actor);
		actor.send(bld + red + "You are leaving the main area of the village without fully training your skills." + nrm);
		actor.send(bld + red + "Return " + bld + cyn + dirToText(revDir(extra.direction)).toUpperCase() + bld + red + " to practice your skills." + nrm);
		actor.send(bld + red + "If you have trouble finding the trainers, be sure to " + bld + cyn + "READ MAP" + nrm + ".");
	}
};
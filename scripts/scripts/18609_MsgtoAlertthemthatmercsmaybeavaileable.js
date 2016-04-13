var script18609 = function(self, actor, here, args, extra) {
	const ylw = actor.yellow(constants.CL_OFF);
	const bld = actor.bold(constants.CL_OFF);
	const nrm = actor.normal(constants.CL_OFF);
	const HUMAN = constants.RACE_HUMAN;
	const TROLLOC = constants.RACE_TROLLOC;
	const TROLLOC_DEPOT_VNUM = 8081; //The vnum of the room for trollocs
	const HUMAN_DEPOT_VNUM = 388; //The vnum of the room for humans
	waitpulse 1;
	if((actor.race == HUMAN && actor.room.vnum == HUMAN_DEPOT_VNUM) || (actor.race == TROLLOC && actor.room.vnum == TROLLOC_DEPOT_VNUM)) {
		actor.send(bld+ ylw + "Type 'list' to view available mercenaries." + nrm);
		return;
	}
}
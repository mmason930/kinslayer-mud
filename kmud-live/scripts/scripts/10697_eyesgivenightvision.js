var script10697 = function(self, actor, here, args, extra) {
	if ( actor.race == constants.RACE_HUMAN && !actor.affectedBy(constants.AFF_NIGHT_VISION) ) {
	    actor.affect(constants.AFF_NIGHT_VISION,14,0,0);
	    waitpulse 1
	    actor.send("Your vision sharpens, piercing even the murkiest shadow.")
	}
}
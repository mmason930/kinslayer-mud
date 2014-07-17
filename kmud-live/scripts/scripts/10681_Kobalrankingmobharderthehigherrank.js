var script10681 = function(self, actor, here, args, extra) {
	var rank = actor.getRank(7);
	if ( !actor.inClan(7) ) { 
	     return;
	}
	else if (rank >= 3 && !self.affectedBy(constants.AFF_AGILITY) ) {
	    self.affect(constants.AGILITY,10,0,0);
	}
	else if (rank >= 4 && !self.affectedBy(constants.AFF_STRENGTH) ) {
	    self.affect(constants.STRENGTH,10,0,0);
	}
	else if (rank >= 5 && !self.affectedBy(constants.AFF_SANCTUARY) ) {
	    self.affect(constants.SANCTUARY,10,0,0);
	}
}
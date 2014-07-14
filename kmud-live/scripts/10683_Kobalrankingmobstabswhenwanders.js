var script10683 = function(self, actor, here, args, extra)
{
	var victim = getRandomPersonInRoom(here);
if( victim && victim != self && self.canSee(victim) && victim.inClan(7) ) {
     if (victim.hps >= 200) {
     self.comm("backstab " + victim.name);
     }
     else {
     self.comm("kill " + victim.name);
     }
}

};


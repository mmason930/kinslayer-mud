var script10684 = function(self, actor, here, args, extra) {
	waitpulse 3;
	if( actor.inClan(7) ) {
	     if (actor.hps >= 200) {
	     waitpulse 3;
	     self.comm("backstab " + actor.name);
	     }
	     else {
	waitpulse 35;
	     self.comm("kill " + actor.name);
	     }
	}
}
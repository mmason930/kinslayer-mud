var script10693 = function(self, actor, here, args, extra) {
	if ( actor.level <= 5 ) {
	    waitpulse 5;
	    self.comm("say I bet you could bring me some deer pelts, " + actor.name + ".");
	}
}
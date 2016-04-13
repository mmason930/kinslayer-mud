var script10606 = function(self, actor, here, args, extra) {
	if ( Quest.getByName('Search and Rescue').hasBegun(actor) && !actor.questTaskProgress('Search and Rescue')[6].comp && self.leader != actor ) {
	     waitpulse 7;
	     self.comm("say Save me!");
	     waitpulse 7
	     self.comm("fol " + actor.name);
	}
}
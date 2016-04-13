var script13207 = function(self, actor, here, args, extra) {
	waitpulse 1;
	if ( Quest.getByName('Shear Labor').hasBegun(actor) ) {
		actor.questSend("Use these shears to -trim- the -hedges-.");
	}
}
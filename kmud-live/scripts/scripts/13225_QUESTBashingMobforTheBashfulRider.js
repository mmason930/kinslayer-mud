var script13225 = function(self, actor, here, args, extra) {
	if ( !Quest.getByName('The Bashful Rider').hasBegun(actor) ) {
		return;
	}
	if ( self.isBashed == true ) {
		waitpulse 1;
		actor.updateJournalTask('The Bashful Rider',0);
	}
}
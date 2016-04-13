var script21418 = function(self, actor, here, args, extra) {
	if ( self.vnum == 4566 ) {
		waitpulse 1;
		actor.updateJournalTask('Crafting Supplies',1);
	}
	else if ( self.vnum == 21418 ) {
		waitpulse 1;
		actor.updateJournalTask('Crafting Supplies', 0);
	}
}
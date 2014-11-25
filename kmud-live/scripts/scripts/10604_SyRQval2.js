var script10604 = function(self, actor, here, args, extra) {
	if ( Quest.getByName('Search and Rescue').hasBegun(actor) && !actor.questTaskProgress('Search and Rescue')[2].comp ) {
		wait 1;
		getCharCols(actor);
		actor.send(bld+cyn+"You have discovered Terrion's base!"+nrm);
		wait 2;
		actor.updateJournalTask('Search and Rescue', 1);
	}
}
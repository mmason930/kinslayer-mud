var script10604 = function(self, actor, here, args, extra) {
	if ( actor.quest('Search and Rescue') > 0 && !actor.questTaskProgress('Search and Rescue')[2].comp ) {
	     wait 1
	     getCharCols(actor);
	     actor.send(bld+cyn+"You have discovered Terrion's base!"+nrm);
	     wait 2
	     actor.updateJournalTask('Search and Rescue', 1);
	     }
}
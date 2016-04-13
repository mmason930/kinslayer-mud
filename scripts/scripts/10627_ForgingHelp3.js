var script10627 = function(self, actor, here, args, extra) {
	var qName = 'Forging Help'
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[3].comp && self.vnum == 4561 ) {
	     getCharCols(actor);
	     actor.send(bld+cyn+"You carefully select a handful of explosive powder to fill a small pouch."+nrm);
	     waitpulse 5
	     actor.updateJournalTask(qName, 2);
	}
}
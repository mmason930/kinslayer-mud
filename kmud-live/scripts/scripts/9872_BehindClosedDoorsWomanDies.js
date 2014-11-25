var script9872 = function(self, actor, here, args, extra) {
	//Alder
	//Behind Closed Doors: Woman Dies
		var qName = "Behind Closed Doors";
		if ( Quest.getByName(qName).hasBegun(actor) ) {
			function f(arg) {
				var actor = arg[0];
				if ( Quest.getByName(qName).tasks[1].isInProgress(actor) ) {
					actor.updateJournalTask(qName,0);
				}
				actor.qval(qName,4);
				actor.qval("NELLA_DEAD",1);
			}
			setTimeout(1,f,[actor]);
		}
}
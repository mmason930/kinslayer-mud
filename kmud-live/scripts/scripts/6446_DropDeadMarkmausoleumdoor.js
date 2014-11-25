var script6446 = function(self, actor, here, args, extra) {
		var qName = "Drop Dead";
		var taskData = actor.questTaskProgress(qName);
		if ( Quest.getByName(qName).tasks[0].isInProgress(actor) && taskData[2].comp == true ) {
			_block;
			actor.qval(qName,2);
			actor.send("You grab a stone from the ground and scratch a small 'x' on the mausoleumdoor.");
			echoaround(actor, actor.name+" picks up a stone from the ground and scratches something on the mausoleumdoor.");
			waitpulse 10;
			actor.updateJournalTask(qName,2);
			waitpulse 7;
			getCharCols(actor);
			actor.send(bld+cyn+"You have finished the job. Either return to Faladin to receive an extra reward or return straight to Zind."+nrm);
		}
			
}
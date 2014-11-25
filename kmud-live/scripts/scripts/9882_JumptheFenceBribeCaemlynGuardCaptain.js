var script9882 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	//Jump the Fence: Bribe Caemlyn Guard Captain
	var qName = "Jump the Fence";
	if ( Quest.getByName(qName).tasks[0].isInProgress(actor) && !actor.questTaskProgress(qName)[1].comp ) {
		waitpulse 7;
		actor.send(capFirstLetter(self.name)+" nods to you coyly.");
		waitpulse 7;
		actor.updateJournalTask(qName,0);
	}
}
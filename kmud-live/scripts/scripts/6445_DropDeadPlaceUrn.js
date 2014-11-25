var script6445 = function(self, actor, here, args, extra) {
	var qName = "Drop Dead";
	var quest = Quest.getByName(qName);
	if (Quest.getByName(qName).tasks[0].isInProgress(actor)) {
		var name = "a small porcelain urn";
		if ( actor.checkQuestItem(name,1,quest) == true ) {
			_block;
			actor.send("You place "+name+" near one of the tombs.");
			actor.remQuestItem(name,-1,quest);
			waitpulse 7;
			actor.updateJournalTask(qName,1);
		}
	}
}
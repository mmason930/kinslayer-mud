var script6445 = function(self, actor, here, args, extra) {
		var qName = "Drop Dead";
		if ( actor.quest(qName) == 1 ) {
			var name = "a small porcelain urn";
			if ( actor.checkQuestItem(name,1,qName) == true ) {
				_block;
				actor.send("You place "+name+" near one of the tombs.");
				actor.remQuestItem(name,-1,qName);
				waitpulse 7;
				actor.updateJournalTask(qName,1);
			}
		}
}
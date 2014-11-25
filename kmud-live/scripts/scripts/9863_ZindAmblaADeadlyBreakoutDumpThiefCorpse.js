var script9863 = function(self, actor, here, args, extra) {
	var qName = "A Deadly Breakout";
	var quest = Quest.getByName(qName);
	if ( Quest.getByName(qName).tasks[0].isInProgress(actor) && actor.checkQuestItem("the corpse of an imprisoned thief", 1, quest) ) {
		_block;
		actor.send("You lean forward and release the corpse, letting it fall over the edge of the well into the darkness.");
		self.echoaround(actor,actor.name + " leans forward and releases a bloody corpse, letting it fall over the edge of the well into the darkness.");
		actor.remQuestItem("the corpse of an imprisoned thief",-1,quest);
		waitpulse 10;
		actor.updateJournalTask(qName,1);
		actor.qval(qName,2);
	}
}
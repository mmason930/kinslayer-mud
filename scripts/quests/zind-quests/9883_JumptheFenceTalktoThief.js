var script9883 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	//Jump the Fence: Talk to Thief
	var qName = "Jump the Fence";
	var quest = Quest.getByName();
	if ( Quest.getByName(qName).tasks[0].isInProgress(actor) && !actor.questTaskProgress(qName)[2].comp ) {
		_block;
		waitpulse 10;
		actor.send(capFirstLetter(self.name)+" cautiously approaches you.");
		waitpulse 7;
		actor.send(capFirstLetter(self.name)+" whispers 'I gotta get rid of this stuff!'");
		waitpulse 10;
		actor.send(capFirstLetter(self.name)+" procures a large bundle from the folds of his cloak.");
		waitpulse 14;
		actor.send(capFirstLetter(self.name)+" whispers 'Here, take these to the fences.")
		waitpulse 10;
		actor.addQuestItem("a delicate ceremonial dagger",1,quest);
		actor.addQuestItem("a small clay sculpture",1,quest);
		actor.addQuestItem("a bolt of golden silk",1,quest);
		waitpulse 15;
		actor.updateJournalTask(qName,1);
	}
}
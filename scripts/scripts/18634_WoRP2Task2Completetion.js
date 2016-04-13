var script18634 = function(self, actor, here, args, extra) {
	//WoR_P2 Speak to Kharsk
	//Rhollor April 2010
	var qName = "Confront the Merchant";
	if(Quest.getByName(qName).hasBegun(actor)) {
		var quest = getQuestByName(qName);
		waitpulse 1;
		wait 1;
		self.comm("say I can see that you have a desire to learn and understand what you have stumbled upon.");
		waitpulse 14;
		self.comm("say If you truly wish to learn about the Way of Ruin, you must complete tasks to prove yourself to Us.");
		waitpulse 7;
		actor.updateJournalTask(qName,1);
		//actor.send("You have unlocked quest(s) from: " + bld + cyn + self.name + nrm + ".");
	}
}
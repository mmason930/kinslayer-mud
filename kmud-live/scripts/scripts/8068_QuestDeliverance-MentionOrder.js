var script8068 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Deliverance
	/** Speech: 'order' **/
	var qName = "Deliverance";
	if ( actor.quest(qName) && !actor.questTaskProgress(qName)[1].comp ) {
		waitpulse 7;
		self.say("Yes, I have written the order and need only to have it delivered to my southern command.");
		waitpulse 20;
		self.say("It seems you are regarded as competent for this mission "+actor.name+", so you shall deliver the order to the fade posted in Four Kings.");
		actor.addQuestItem("Murash's order of attack on Caemlyn",1,qName);
		waitpulse 24;
		self.say("Go now, and make haste.");
		actor.updateJournalTask(qName,0);
	}
}
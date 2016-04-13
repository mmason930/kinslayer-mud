var script165 = function(self, actor, here, args, extra) {
	//Alder
	//June 2011
	//Sifting for Clues: give envelope to Avin
	var qName = "Sifting for Clues";
	var quest = Quest.getByName(qName);
	if ( actor.quest(qName) && actor.questTaskProgress(qName)[1] && actor.questTaskProgress(qName)[2] ) {
		actor.send("You give a wax sealed envelope to "+self.name+".");
		actor.remQuestItem("a wax sealed envelope",1,quest);
		here.echoaround(actor, actor.name+" gives a wax sealed envelope to "+self.name+".");
		waitpulse 1;
		self.say("Hmm, what's this.");
		self.comm("emote takes a moment to open the envelope and read the contents.");
		waitpulse 21;
		self.say("Thomdramyn huh? I haven't heard of that bastard in ages. A good man that one.");
		waitpulse 16;
		self.say("I guess he figured I'd be his best bet in interpreting this map. At any rate the place you're looking for is in Aridhol, just northeast of here.");
		waitpulse 22;
		self.say("Just follow the Caemlyn road east from Baerlon and look for a hill. If you reach the lake, you've gone too far.");
		waitpulse 23;
		self.say("When you find the hill, climb it and cross the old bridge and follow the overgrown trail until you come across a ruined courtyard.");
		waitpulse 21;
		self.say("Take this, you should be able to figure out what to do with it once you're in the courtyard. Be careful out there, Shadar Logoth is close to that area.");
		actor.addQuestItem("a conspicuous looking stone block",quest);
		actor.updateJournalTask(qName,1);
	}
}
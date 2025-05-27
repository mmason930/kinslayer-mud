let script164 = function(self, actor, here, args, extra) {
	//Alder
	//June 2011
	//Sifting for Clues: speak with The Katana's Sheath weaponsmith
	let qName = "Sifting for Clues";
	let quest = Quest.getByName(qName);
	if ( actor.quest(qName) && !actor.questTaskProgress(qName)[1] ) {
		waitpulse 1;
		self.comm("emote stops what she's doing in looks up.");
		waitpulse 10;
		self.say("Well noone's asked me about that in a long time.");
		waitpulse 14;
		self.say("Let me guess, you've been talking to Thomdramyn. Well just be sure you take anything he says with grain of salt...");
		waitpulse 21;
		self.say("He has a penchant for folklore and legend, but don't tell him it's nothing but that.");
		waitpulse 21;
		self.comm("emote glances at the door to her shop before continuing.");
		waitpulse 15;
		self.say("If he's got you doing what I think he is, take this letter to a guard in Baerlon called Avin. You can usually find him around the southern gate.");
		actor.addQuestItem("a wax sealed envelope",1,quest);
		waitpulse 25;
		self.say("And when you see Thomdramyn again, tell him I haven't forgotten any of it. Good luck.");
		actor.updateJournalTask(qName,0);
	}
}
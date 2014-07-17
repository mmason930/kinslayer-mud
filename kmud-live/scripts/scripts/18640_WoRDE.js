var script18640 = function(self, actor, here, args, extra) {
	//WoR:DE Find Scout
	//Rhollor April 2010
	var qName = "WoR: A Dead End";
	if(actor.quest(qName) > 0 && here.vnum == 1090) {
		waitpulse 1;
		actor.updateJournalTask(qName,0);
	}
	if(actor.quest(qName) > 0 && here.vnum == 2098 && actor.questTaskProgress(qName)[3].quotient != 1 && actor.questTaskProgress(qName)[2].quotient == 1 ) {
		here.loadObj(2098);
		waitpulse 1;
		actor.send(cyn + bld + "You notice a human skull; you can only guess it is the Questioner you have been searching for.");
		actor.send("You should return the skull to Kharsk, and inform him of the end the Questioner met." +nrm);
		waitpulse 1;
		actor.updateJournalTask(qName,2);
	}
}
var script18688 = function(self, actor, here, args, extra) {
	//What's the Catch?
	//Rhollor April 2010
	var qName = "What's the Catch?";
	if(Quest.getByName(qName).hasBegun(actor)) {
		waitpulse 1;
		if(self.vnum == 20900)
			actor.updateJournalTask(qName,0);
		else if(self.vnum == 20901)
			actor.updateJournalTask(qName,1);
		else if(self.vnum == 20902)
			actor.updateJournalTask(qName,2);
	}
}
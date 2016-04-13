var script18638 = function(self, actor, here, args, extra) {
	//WoR_IM - Find the Miner
	//Rhollor April 2010
	waitpulse 1;
	var qName = "WoR: It's Mine!";
	if(Quest.getByName(qName).hasBegun(actor) && self.room.vnum == 20843) {
		waitpulse 4;
		self.comm("say No wait! It's mine! Don't take it!");
		waitpulse 3;
		actor.updateJournalTask(qName,0);
	}
}
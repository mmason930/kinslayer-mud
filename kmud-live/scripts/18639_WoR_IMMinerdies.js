var script18639 = function(self, actor, here, args, extra)
{
	//WoR_IM - Kill the Miner
//Rhollor April 2010
var qName = "WoR: It's Mine!";
if(actor.quest(qName) > 0 && self.room.vnum == 20843 && actor.questTaskProgress(qName)[2].quotient != 1) {
	var vnum = 20835 + random(0,2);
	self.loadObj(vnum);
	self.comm("say But it was mine...");
	actor.updateJournalTask(qName,1);
}

};


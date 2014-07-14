var script18636 = function(self, actor, here, args, extra)
{
	//WoR_TaBE
//Rhollor April 2010
var qName = "WoR: Turn a Blind Eye";
if(actor.quest(qName) > 0) {
	if(isName("cannot", args)) {
		self.comm("say That is too bad " + actor.name + ".");
		self.comm("kill " + actor.name);
	}
	actor.updateJournalTask(qName,0);
	//actor.send(yel+bld+ "Journal Updated -- Quest: 'WoR: Turn a Blind Eye' --" + nrm+cyn+bld + " Completed" +nrm);
	//actor.comm("save");
	actor.qval(self.vnum+'_BLOCK_QUESTS',1);
	questComplete(self,actor,getQuestsByMob(self.vnum),2,0);
}

};


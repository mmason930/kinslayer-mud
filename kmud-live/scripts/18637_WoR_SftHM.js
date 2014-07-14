var script18637 = function(self, actor, here, args, extra)
{
	//WoR_SftHM Task Complete
//Rhollor April 2010
var qName = "WoR: Straight from the Horse's Mouth";
if(self.hps <= (self.maxHit/4) && actor.quest(qName) > 0) {
	waitpulse 1;
	self.comm("say FUCK! Please don't kill me!");
	waitpulse 1;
	actor.updateJournalTask(qName, 0);
	waitpulse 6;
	self.comm("say I swear I'll keep my mouth shut!");
	self.comm("flee");
	self.comm("flee");
}

};


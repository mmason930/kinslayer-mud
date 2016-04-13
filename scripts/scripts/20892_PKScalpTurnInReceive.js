var script20892 = function(self, actor, here, args, extra) {
	var item = extra.obj;
	var itemName = item.namelist.split(" ")[0];
	var afterWait = function()
	{
	if (actor && item) {
		if (self.race != actor.race)
		{
			self.say("I don't trade with your kind!");
			self.comm("drop "+itemName);
			return;
		}
		if (!item.isPlayerScalp || item.scalpRace == actor.race) {
			self.say("I can't accept that, "+actor.name+".");
			self.comm("give "+itemName+" "+actor.name);
			self.comm("drop "+itemName);
			return;
		}
	}
	
	var reward = Math.floor(item.scalpLevel * 200);
	var message = "Great work, "+actor.name+"!";
	self.say(message);
	actor.send(capFirstLetter(self.name) +" gives you" +actor.numCopperToText(reward,true) + ".");
	act(capFirstLetter(self.name) + " hands a few coins to $n.",true,actor,null,null,constants.TO_ROOM);
	actor.gold += reward;
	item.extract();
	mudLog(3, 102, actor.name+" rewarded "+actor.numCopperToText(reward,false)+" for PK scalp: "+item.name);
	}
	setTimeout(1,afterWait,[actor,self,item]);
}
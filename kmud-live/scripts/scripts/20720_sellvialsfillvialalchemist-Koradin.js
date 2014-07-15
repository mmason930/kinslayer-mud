var script20720 = function(self, actor, here, args, extra) {
	//tailor receive shop
	//Koradin - June 2010
	var item = extra.obj;
	var afterWait = function()
	{
	if (self.race != actor.race)
	{
		self.say("I don't trade with your kind!");
		self.comm("drop "+item.namelist.split(" ")[0]);
		return;
	}
	if (item.vnum < 2700 || item.vnum > 2799)
	{
		if (item.vnum != 2030)
		{
			self.say("Sorry, I don't deal in these.");
			self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
			if (self.hasItem(item,true,true,true))
				self.comm("drop "+item.namelist.split(" ")[0]);
			return;
		}
		else if (item.vnum == 2030 && actor.gold < 5000)
		{
			self.say("I don't do freebies. Come back when you have the coin.");
			self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
			if (self.hasItem(item,true,true,true))
				self.comm("drop "+item.namelist.split(" ")[0]);
			return;
		}
		else if (item.vnum == 2030 && item.name == "a small glass vial (full)")
		{
			self.say("This vial is already full!");
			self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
			if (self.hasItem(item,true,true,true))
				self.comm("drop "+item.namelist.split(" ")[0]);
			return;
		}
		else
		{
			//do vial fill stuff
			actor.gold -= 5000;
			act(capFirstLetter(self.name) + " fills the vial from one of $s beakers.",true,self,null,null,constants.TO_ROOM);
			self.say("That should do nicely.");
			item.setRetoolSDesc(item.name+ " (full)");
			item.setRetoolName(item.namelist+ " vialisnowfull");
			self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
			if (self.hasItem(item,true,true,true))
				self.comm("drop "+item.namelist.split(" ")[0]);
			return;
		}
	}
	var reward = item.cost;
	self.say("Thank you for your business!");
	actor.send(capFirstLetter(self.name) +" gives you" +actor.numCopperToText(reward,true) + ".");
	act(capFirstLetter(self.name) + " hands a few coins to $n.",true,actor,null,null,constants.TO_ROOM);
	actor.gold += reward;
	item.extract();
	}
	setTimeout(1,afterWait,[actor,self,item]);
}
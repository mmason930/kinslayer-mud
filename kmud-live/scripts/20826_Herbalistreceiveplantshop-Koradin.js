var script20826 = function(self, actor, here, args, extra)
{
	//Herbalist shop transactions
//Koradin - May 2010
var item = extra.obj;
var afterWait = function()
{
if (self.race != actor.race)
{
	self.say("I don't trade with your kind!");
	self.comm("drop "+item.namelist.split(" ")[0]);
	return;
}
if (item.vnum == 20969)
{
	if (actor.gold < 1000)
	{
		self.say("Sorry, I charge 10 silver to identify plants, and you don't have it!");
		self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
		if (self.hasItem(item,true,true,true))
			self.comm("drop "+item.namelist.split(" ")[0]);
		return;
	}
	actor.gold -= 1000;
	actor.room.echo(capFirstLetter(self.name) +" quickly studies the plant.");
	var rand = random(20985,20998);
	if (random(1,15) == 1)
	{
		self.say("Sorry, it's too withered to identify!  I'll only charge you half price.");
		actor.send(capFirstLetter(self.name) +" gently takes your coins.");
		actor.gold += 500;
		item.extract();
		return;
	}
	self.say("Well, it's clearly "+getObjProto(rand).name+"!");
	actor.loadObj(rand);
	item.extract();
	actor.send(capFirstLetter(self.name) +" hands the plant back to you, taking your coins.");
	return;
}
if (item.vnum < 20985 || item.vnum > 20998)
{
	self.say("Sorry, I don't deal in these.");
	self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
	if (self.hasItem(item,true,true,true))
		self.comm("drop "+item.namelist.split(" ")[0]);
	return;
}
var decay = (100 - getDecayProgress(item));
var message2 = "and it's in great condition!";
if (decay < 1)
	decay = 1;
if (decay < 86)
	message2 = "and it's in good condition!";
else if (decay < 66)
	message2 = "and it's in fair condition...";
else if (decay < 36)
	message2 = "but it's in poor condition...";
else if (decay < 21)
	message2 = "but it's in bad condition!";
else if (decay < 11)
	message2 = "but it's in awful condition!";
var reward = Math.floor(item.cost * (decay / 100) );
var message = "It will do just fine, "+message2;
if (getSval(self,20825,"herb tries") == 1)
{
	var newHerbVnum = random(20985, 20998);
	message = "Thanks, that's the last of these that I need.";
	setSval(self,20825,"new herb", newHerbVnum);
	setSval(self,20825,"herb tries",random(2,6));
}
var special_item_vnum = getSval(self,20825,"new herb");
if (item.vnum == special_item_vnum)
{
	if (item.vnum != 20989)
		reward += (item.cost*2);
	message = "Ah, fantastic! I've been needing one of these, "+message2;
	setSval(self,20825,"herb tries", getSval(self,20825,"herb tries") - 1);
}
if (item.vnum == 20989)
{
	message = "Oh, my!  I haven't found one of these in years, "+message2;
	reward *= 12;
}
self.say(message);
actor.send(capFirstLetter(self.name) +" gives you" +actor.numCopperToText(reward,true) + ".");
act(capFirstLetter(self.name) + " hands a few coins to $n.",true,actor,null,null,constants.TO_ROOM);
actor.gold += reward;
item.extract();
}
setTimeout(1,afterWait,[actor,self,item]);

};


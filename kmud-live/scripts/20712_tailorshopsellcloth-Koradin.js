var script20712 = function(self, actor, here, args, extra)
{
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
if (item.vnum < 4900 || item.vnum > 4939)
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
	message2 = "and it's in fair condition.";
else if (decay < 36)
	message2 = "but it's in poor condition.";
else if (decay < 21)
	message2 = "but it's in bad condition!";
else if (decay < 11)
	message2 = "but it's in awful condition!";
var reward = Math.floor(item.cost * (decay / 100) );
var message = "Thank you! This is what I needed... "+message2;
if (item.vnum == 4905)
{
	if (self.race == constants.RACE_TROLLOC)
		message = "Yum yum!";
	else
		message = "Oh good, a treat for my dog!";
	reward *= 6;
}
self.say(message);
actor.send(capFirstLetter(self.name) +" gives you" +actor.numCopperToText(reward,true) + ".");
act(capFirstLetter(self.name) + " hands a few coins to $n.",true,actor,null,null,constants.TO_ROOM);
actor.gold += reward;
item.extract();
}
setTimeout(1,afterWait,[actor,self,item]);

};


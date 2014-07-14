var script20874 = function(self, actor, here, args, extra)
{
	var item = extra.obj;
var afterWait = function()
{
if (self.race != actor.race)
{
	self.say("I don't trade with your kind!");
	self.comm("drop "+item.namelist.split(" ")[0]);
	return;
}
if (arrContains(self.namelist.split(" "), "blacksmith")) {
	var tradeType = "Smithing";
	var trader = "blacksmith";
	var place = "forge";
}
else if (arrContains(self.namelist.split(" "), "tailor")) {
	var tradeType = "Tailoring";
	var trader = "tailor";
	var place = "workshop";
}
if (actor) {
	if (trader == "blacksmith") {
		if (item.vnum < 2600 || item.vnum == 2609 || item.vnum > 2649 && item.vnum < 4536 || item.vnum > 4535 && item.vnum < 4583 && !isName("bar",item.name) && !isName("brick",item.name) || item.vnum > 4582 && item.vnum < 11300 || item.vnum > 13099 && item.vnum < 30000) {
			self.say("I can't accept that, "+actor.name+".");
			self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
			self.comm("drop "+item.namelist.split(" ")[0]);
			return;
		}
	}
	if (trader == "tailor") {
		if (item.vnum < 4940 || item.vnum > 4982 && item.vnum < 31080 || item.vnum > 32537) {
			self.say("I can't accept that, "+actor.name+".");
			self.comm("give "+item.namelist.split(" ")[0]+" "+actor.name);
			self.comm("drop "+item.namelist.split(" ")[0]);
			return;
		}
	}
}
var decay = (100 - getDecayProgress(item));
var message2 = "and it's in great condition!";
if (decay < 1 || !decay)
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
var reward = Math.floor(item.cost * (decay / 100) * getCostMultiplier(item.name) * 0.90 );
if (item.vnum > 4535 && item.vnum < 4583 && (isName("bar",item.name) || isName("brick",item.name)))
	reward /= 2;
var message = "This will do just fine, "+message2;
self.say(message);
actor.send(capFirstLetter(self.name) +" gives you" +actor.numCopperToText(reward,true) + ".");
act(capFirstLetter(self.name) + " hands a few coins to $n.",true,actor,null,null,constants.TO_ROOM);
actor.gold += reward;
item.extract();
}
setTimeout(1,afterWait,[actor,self,item]);




};


var script20708 = function(self, actor, here, args, extra)
{
	//Ore Merchant receive
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
var acceptedItems = [4567,4568,4569,4584,4583,20835,20836,20837,4585,4586,4587,4588,4589,4590,4591,4592,4593,4594,4595,4596,4597,4598,4599,4603,4525,4562];
var accepted = false;
for (i=0;i<acceptedItems.length;i++)
{
	if (item.vnum == acceptedItems[i])
		var accepted = true;
}
if (accepted == false)
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
if (item.vnum == 4569)
var message = "Wow! This thing is huge, "+message2;
self.say(message);
actor.send(capFirstLetter(self.name) +" gives you" +actor.numCopperToText(reward,true) + ".");
act(capFirstLetter(self.name) + " hands a few coins to $n.",true,actor,null,null,constants.TO_ROOM);
actor.gold += reward;
item.extract();
}
setTimeout(1,afterWait,[actor,self,item]);

};


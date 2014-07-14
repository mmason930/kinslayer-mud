var script20714 = function(self, actor, here, args, extra)
{
	_block;
if (actor.race != self.race)
{
self.say("I don't trade with your kind!");
return;
}
var arg_array = getArgList(args);
if (strn_cmp(arg_array[0],"list",2))
{
actor.comm("say list");
return;
}
var item = parseInt(arg_array[1]);
if (!self.getPval("orelist"))
{
self.say("Sorry, I don't have anything for sale right now. Try back later!");
return;
}
var itemList = self.getPval("orelist").split("~");
if (!item || item < 1 || item > 10)
{
actor.send("What do you want to buy?");
return;
}
var arrayPosition = item-1;
item = getObjProto(parseInt(itemList[arrayPosition]));
var cost = item.cost*11;
if (actor.gold < cost)
{
self.comm("tell " +actor.name + " Sorry, you don't have enough money to buy that.");
return;
}
actor.gold -= cost;
actor.send("You buy "+item.name+".");
self.say("An excellent decision, "+actor.name+"! You won't be disappointed.");
self.loadObj(item.vnum);
self.comm("give " +item.namelist.split(" ")[0]+ " " + actor.name);
self.comm("drop " +item.namelist.split(" ")[0]);
itemList.splice(arrayPosition,1);
var newPval = itemList.join("~");
self.deletePval("orelist",true);
self.setPval("orelist",newPval,true);

};


var script20716 = function(self, actor, here, args, extra) {
	_block;
	if (actor.race != self.race)
	{
		self.say("I don't trade with your kind!");
		return;
	}
	var arg_array = getArgList(args);
	if (strn_cmp(arg_array[0],"list",2))
	{
		actor.say("list");
		return;
	}
	var itemNumber = parseInt(arg_array[1]) - 1;
	if (!self.getPval("herblist"))
	{
		self.say("Sorry, I don't have anything for sale right now. Try back later!");
		return;
	}
	var itemList = self.getPval("herblist").split("~");
	if (itemNumber < 0 || itemNumber >= itemList.length)
	{
		actor.send("What do you want to buy?");
		return;
	}
	var item = getObjProto(parseInt(itemList[itemNumber]));
	var cost = item.cost * 23;
	if (actor.gold < cost)
	{
		self.tell(actor, "Sorry, you don't have enough money to buy that.");
		return;
	}
	actor.gold -= cost;
	actor.send("You buy "+item.name+".");
	self.say("An excellent decision, "+actor.name+"! You won't be disappointed.");
	self.loadObj(item.vnum);
	self.comm("give " +item.namelist.split(" ")[0]+ " " + actor.name);
	self.comm("drop " +item.namelist.split(" ")[0]);
	itemList.splice(itemNumber,1);
	var newPval = itemList.join("~");
	self.deletePval("herblist",true);
	self.setPval("herblist",newPval,true);
	
}
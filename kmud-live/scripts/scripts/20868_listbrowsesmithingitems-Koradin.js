var script20868 = function(self, actor, here, args, extra) {
	_block;
	if (self.race != actor.race)
		return;
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
	var pack = self.eq(constants.WEAR_BACK);
	if (!pack || pack.contents.length == 0) {
		self.say("Sorry "+actor.name+", but the "+trader+" doesn't have anything for sale right now.");
		return;
	}
	var itemList = pack.contents;
	actor.send(" ");
	self.comm("nod");
	act(capFirstLetter(self.name)+" shows a small slip of paper to $n.",true, actor,null,null,constants.TO_ROOM)
	actor.send(capFirstLetter(self.name) + " shows you a list of items currently for sale.");
	actor.send("_____________________________________________________________________________");
	actor.send(" ");
	getCharCols(actor);
	for (i=0; i<itemList.length; i++) {
		var curItem = itemList[i];
		var money = Math.floor(curItem.cost*getCostMultiplier(curItem.name));
		money = money.toString().split("");
		var moneyline = "";
		var copper2 = "";
		var copper1 = "";
		var silver2 = "";
		var silver1 = "";
		if (money.length > 4)
		{
			if (money.length == 5)
				moneyline += "  ";
			else if (money.length == 6)
				moneyline += " ";
			copper2 = money.pop();
			copper1 = money.pop();
			silver2 = money.pop();
			silver1 = money.pop();
			moneyline += money.join("")+bld+yel+" gold"+nrm+", "+silver1+silver2+cyn+bld+" silver"+nrm+", "+copper1+copper2+yel+" copper"+nrm;
		}
		else if (money.length > 2)
		{
			copper2 = money.pop();
			copper1 = money.pop();
			if (money.length == 2)
			{
				silver2 = money.pop();
				silver1 = money.pop();
			}
			else
				silver2 = " "+money.pop();
			moneyline += "          "+silver1+silver2+cyn+bld+" silver"+nrm+", "+copper1+copper2+yel+" copper"+nrm;
		}
		else
		{
			if (money.length == 2)
			{
				copper2 = money.pop();
				copper1 = money.pop();
			}
			else
				copper2 = " "+money.pop();
			moneyline += "                     "+copper1+copper2+yel+" copper"+nrm;
		}
		var space = "";
		for (j=0;j<40-curItem.name.length;j++) {
			space+=" ";
		}
		if (i<9)
			space += " ";
		actor.send(cyn+(i+1)+nrm+") "+curItem.name+space+moneyline);
	}
	actor.send("_____________________________________________________________________________");
	actor.send(" ");
	actor.send("["+cyn+"#"+nrm+"] Purchase Item      ["+cyn+"C"+nrm+"] Cancel");
	actor.send(" ");
	actor.attach(20869);
	
	
}
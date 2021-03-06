var script20827 = function(self, actor, here, args, extra) {
	getCharCols(actor);
	wait 1;
	if (self.race == actor.race)
	{
	self.say("I'll buy any herbs from you! Just hand them over.");
	wait 1;
	getCharCols(actor);
	self.say("I'll also identify a plant for you, but it doesn't come cheap!");
	self.say("I charge 10 "+cyn+"silver"+nrm+" coins for the service.");
	self.say("Hand me the plant, and I'll show you what it is.");
	}
	else
	{
	self.say("I don't trade with your kind!");
	return;
	}
	wait 2;
	if (getArgList(args)[0] == "list")
	{
	if (!self.getPval("herblist"))
	{
	self.say("...and I'm afraid I don't have anything for sale right now. Try back later!");
	return;
	}
	var list = self.getPval("herblist").split("~");
	self.say ("I also have the following items for sale, if you're interested...");
	waitpulse 4;
	getCharCols(actor);
	if (actor.room != self.room)
	return;
	actor.send(capFirstLetter(self.name) + " shows you a small list of items.");
	act("$n shows $N a small piece of paper.",true,self,null,actor,constants.TO_NOTVICT);
	actor.send("_____________________________________________________________________________");
	actor.send(" ");
	for (i=1;i<list.length+1;i++)
	{
	var money = getObjProto(parseInt(list[i-1])).cost*23;
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
	for (j=0;j<40-getObjProto(parseInt(list[i-1])).name.length;j++){space+=" ";}
	actor.send(cyn+i+nrm+") "+getObjProto(parseInt(list[i-1])).name+space+moneyline);
	}
	actor.send("_____________________________________________________________________________");
	actor.send(" ");
	actor.send(bld+"To buy an item, type " + cyn + bld + "buy #" + nrm + ".");
	}
}
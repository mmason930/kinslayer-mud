var script20722 = function(self, actor, here, args, extra) {
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
	//BUY RECIPES
	getCharCols(actor);
	if (strn_cmp(arg_array[1],"recipes",6)) {
		var availableRecipes = [];
		var skill = actor.getSkill(getSkillVnum("Mixing"));
		var recipes = getRecipeContainer("M");
		for (i = 0; i < recipes.length; i++) {
			if (skill >= recipes[i].skillRequired && recipes[i].skillRequired > 0 && !actor.isRecipeUnlocked(recipes[i]) && recipes[i].price > 0 && recipes[i].skillRequired < 81)
				availableRecipes.push(recipes[i]);
			if (availableRecipes.length > 14)	
				break;
		}
		if (availableRecipes.length == 0) {
			self.say("You've already purchased all the recipes that are currently available to you, "+actor.name+".");
			return;
		}
		actor.send("At your skill level, you can currently purchase the following recipes:");
		actor.send("______________________________________________________________________________");
		actor.send(" ");
		for (j = 0; j < availableRecipes.length; j++) {
			var money = availableRecipes[j].price;
			if (money < 10000)
				money = 10000;
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
			for (k=0;k<(44-availableRecipes[j].name.length);k++){
				space+=" ";
			}
			if (j<9)
				space += " ";
			actor.send(cyn+(j+1)+nrm+") "+availableRecipes[j].name+space+moneyline);
		}
		actor.send("______________________________________________________________________________");
		actor.send(" ");
		actor.send("["+cyn+"#"+nrm+"] Purchase Recipe      ["+cyn+"C"+nrm+"] Cancel");
		setSval(actor,20862,"buyRecipe","Mixing");
		actor.attach(20862);
		return;
	}
	//BUY # (VIAL)
	var item = parseInt(arg_array[1]);
	if (!self.getPval("mixlist"))
	{
	self.say("Sorry, I don't have anything for sale right now. Try back later!");
	return;
	}
	var itemList = self.getPval("mixlist").split("~");
	if (!item || item < 1 || item > 10)
	{
	actor.send("Type 'buy #' to purchase a vial or 'buy recipes' to view the recipes you can purchase.");
	return;
	}
	var arrayPosition = item-1;
	item = getObjProto(parseInt(itemList[arrayPosition]));
	var cost = item.cost*25;
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
	self.deletePval("mixlist",true);
	self.setPval("mixlist",newPval,true);
	
}
var script20873 = function(self, actor, here, args, extra) {
	getCharCols(actor);
	_block;
	var arg_array = getArgList(args);
	if (!arg_array[1] || arg_array[2]) {
		actor.send("Buy what?");
		return;
	}
	if (arrContains(self.namelist.split(" "), "blacksmith"))
		var tradeType = "Smithing";
	else if (arrContains(self.namelist.split(" "), "tailor"))
		var tradeType = "Tailoring";
	var tradeChar = tradeType.charAt(0);
	if (strn_cmp(arg_array[1],"recipes",6)) {
		var availableRecipes = [];
		var skill = actor.getSkill(getSkillVnum(tradeType));
		if (skill > 80)
			skill = 80;
		var recipes = getRecipeContainer(tradeChar);
		for (i = 1; i < recipes.length; i++) {
			var recCheck = recipes[i].vnum+1;
			var pushOk = false;
			//actor.send(arrContains("light",getRecipeByVnum(recCheck, tradeChar).name.split(" ")));
			if (getRecipeByVnum(recCheck, tradeChar) && (parseInt(recCheck.toString().charAt(recCheck.length-1)) != 0 || arrContains("velvet",getRecipeByVnum(recCheck, tradeChar).name.split(" ")))) {
				//actor.send("boom");
				if (actor.isRecipeUnlocked(getRecipeByVnum(recCheck, tradeChar)) || skill < getRecipeByVnum(recCheck, tradeChar).skillRequired || arrContains("velvet",getRecipeByVnum(recCheck, tradeChar).name.split(" "))) {			
					pushOk = true;
				}
			}
			if (pushOk == true && skill >= recipes[i].skillRequired && recipes[i].skillRequired > 0 && !actor.isRecipeUnlocked(recipes[i]) && recipes[i].price > 0 && recipes[i].skillRequired < 81) {
				//here.echo("pushing "+recipes[i].name);
				availableRecipes.push(recipes[i]);
			}
		}
		for (j = 0; j < availableRecipes.length; j++) {
			if (availableRecipes[j+1]) {
				var rec1 = availableRecipes[j].name.split(" ");
				var rec2 = availableRecipes[j+1].name.split(" ");
				if (rec1[rec1.length-2] == "leather" && (rec1[rec1.length-3] == "quilted" || rec1[rec1.length-3] == "light" || rec1[rec1.length-3] == "padded" || rec1[rec1.length-3] == "studded") )
					var placeHolder1 = 3;
				else
					var placeHolder1 = 2;
				if (rec2[rec2.length-2] == "leather" && (rec2[rec2.length-3] == "quilted" || rec2[rec2.length-3] == "light" || rec2[rec2.length-3] == "padded" || rec2[rec2.length-3] == "studded") )
					var placeHolder2 = 3;
				else
					var placeHolder2 = 2;
				var type1 = [1,rec1[rec1.length-placeHolder1]];
				var type2 = [1,rec2[rec2.length-placeHolder2]];
				if (rec1[rec1.length-1] == "projectile" || rec1[rec1.length-2] == "short")
						type1 = [1,rec1[rec1.length-3]];
				if (rec2[rec2.length-1] == "projectile" || rec2[rec2.length-2] == "short")
						type2 = [1,rec2[rec2.length-3]];
				if (rec1[1].split("-")[1])
						type1 = [rec1[1],rec1[2]];
				if (rec2[1].split("-")[1])
						type2 = [rec2[1],rec2[2]];
				//here.echo(availableRecipes[j].name + " " + type1 + " " + type2);
				if (rec1[rec1.length-1] == rec2[rec2.length-1] && type1[0] == type2[0] && (type1[1] != type2[1] && tradeType == "Smithing")) {
					//here.echo(availableRecipes[j].name + " is getting cut! RecOne: "+type1+" RecTwo: "+type2);
					availableRecipes.splice(j, 1);
				}
			}
		}
		if (availableRecipes.length == 0) {
			self.say("You've already purchased all the recipes that are currently available to you, "+actor.name+".");
			return;
		}
		actor.send("At your skill level, you can currently purchase the following recipes:");
		actor.send("______________________________________________________________________________");
		actor.send(" ");
		for (j = 0; j < availableRecipes.length; j++) {
			var money = availableRecipes[j].price / 5;
			if (money < 2000)
				money = 2000;
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
		setSval(actor,20862,"buyRecipe",tradeType);
		actor.attach(20862);
		return;
	}
	else
		actor.send("Buy what?");
	
	
	
}
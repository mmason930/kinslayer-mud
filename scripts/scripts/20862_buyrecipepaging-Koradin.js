var script20862 = function(self, actor, here, args, extra) {
	var recType = getSval(actor,20862,"buyRecipe");
	var recLetter = clone(recType);
	recLetter = recLetter.charAt(0);
	var tradeChar = recLetter;
	var tradeType = recType;
	var recipes = getRecipeContainer(recLetter);
	var skill = self.getSkill(getSkillVnum(recType));
	var availableRecipes = [];
	_block;
	if (recLetter == "M") {
		for (i = 0; i < recipes.length; i++) {
			if (skill >= recipes[i].skillRequired && recipes[i].skillRequired > 0 && !actor.isRecipeUnlocked(recipes[i]) && recipes[i].price > 0 && recipes[i].skillRequired < 81)
				availableRecipes.push(recipes[i]);
			if (availableRecipes.length > 14)
				break;
		}
	}
	else if (recLetter == "S" || recLetter == "T") {
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
	}
	if (availableRecipes.length == 0) {
		actor.detach(20862);
		return;
	}
	getCharCols(actor);
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"cancel",1)) {
		actor.send("You cancel the transaction.");
		actor.detach(20862);
		return;
	}
	else if (vArgs[0] > 0 && vArgs[0] <= availableRecipes.length) {
		var recipe = availableRecipes[(vArgs[0]-1)];
		if (actor.gold < (recipe.price/5)) {
			actor.send(red+"You can't afford to learn that recipe!"+nrm);
			actor.send(" ");
			actor.detach(20862);
			actor.comm("buy recipes");
			return;
		}
		else {
			actor.gold -= (recipe.price/5);
			actor.send("You purchase the recipe for "+recipe.name+", tucking it into your recipe book.");
			act("$n purchases a sheet of parchment, stuffing it into $s recipe book.",true,actor,null,null,constants.TO_ROOM);
			actor.send(" ");
			actor.unlockRecipe(recipe.vnum, recLetter);
			actor.detach(20862);
			return;
		}
	}
	else {
		actor.send("That is not a valid choice.");
		actor.send("Choose another option, or type "+cyn+"C"+nrm+" to cancel the transaction.");
		return;
	}
	
}
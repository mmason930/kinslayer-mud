var script20839 = function(self, actor, here, args, extra) {
	//RECIPES paging script
	//Koradin - May 2010
	var cmd = getArgList(args)[0];
	actor.send("");
	if (strn_cmp(cmd,"alias",1) || strn_cmp(cmd,"close",1))
	{
		actor.detach(20839);
		actor.send("You close the recipe book with a snap, stirring up a cloud of dust.");
		act("$n snaps $s recipe book closed, stirring up a cloud of dust.",true,actor,null,null,constants.TO_ROOM);
		if (strn_cmp(cmd,"close",1))
			_block;
		else
			_noblock;
		setSval(actor,20840,"recipes",0);
		return;
	}
	else if (strn_cmp(cmd,"table",1))
	{
		_block;
		act("$n flips through the pages of $s recipe book...",true,actor,null,null,constants.TO_ROOM);
		actor.detach(20839);
		actor.send("");
		actor.comm("recipes");
		return;
	}
	_block;
	getCharCols(actor);
	var sval = getSval(actor,20840,"recipes");
	var mixRecipes = actor.getRecipes(constants.RECIPE_TYPE_MIXING);
	var smithRecipes = actor.getRecipes(constants.RECIPE_TYPE_SMITHING);
	var tailorRecipes = actor.getRecipes(constants.RECIPE_TYPE_TAILORING)
	var allRecipes = mixRecipes.concat(smithRecipes).concat(tailorRecipes);
	var spacer = " ";
	// for (var _autoKey in allRecipes) { 	var pVnum = allRecipes[_autoKey];
	// { }
		// var potion = getPotionByVnum(pVnum);
		// var potion_type = potion.name.split(" ");
		// if (potion_type.pop() == "vial")
			// mixRecipes.push(potion);
		// else if (potion_type.pop() == "meal")
			// smithRecipes.push(potion);
	// }
	if (sval == 1)
	{
		if (cmd == 1)
		{
			if (smithRecipes.length < 1)
			{
				actor.send(red+"You do not know any smithing recipes yet!"+nrm);
				actor.send("");
				actor.detach(20839);
				actor.comm("recipes");
				return;
			}
			actor.send(bld+"Smithing Recipes"+nrm);
			actor.send("____________________________________________________________________________");
			actor.send("");
			for (i = 0; i < smithRecipes.length; i++)
				if (getRecipeByVnum(smithRecipes[i].vnum,constants.RECIPE_TYPE_SMITHING))
					actor.send(grn+(i+1)+nrm+") "+getRecipeByVnum(smithRecipes[i].vnum,constants.RECIPE_TYPE_SMITHING).name );
			actor.send("");
			actor.send("----------------------------------------------------------------------------");
			actor.send("["+grn+"#"+nrm+"] Make a Selection ("+grn+"1-"+smithRecipes.length+nrm+")    ["+grn+"T"+nrm+"] Table of Contents    ["+grn+"C"+nrm+"] Close Recipe Book");
			setSval(actor,20840,"recipes",2);
		}
		else if (cmd == 2)
		{
			if (mixRecipes.length < 1)
			{
				actor.send(red+"You do not know any mixing recipes yet!"+nrm);
				actor.send("");
				actor.detach(20839);
				actor.comm("recipes");
				return;
			}
			actor.send(bld+"Mixing Recipes"+nrm);
			actor.send("____________________________________________________________________________");
			actor.send("");
			for (i = 0; i < mixRecipes.length; i++)
			{
				if (getRecipeByVnum(mixRecipes[i].vnum,constants.RECIPE_TYPE_MIXING))
					actor.send(grn+(i+1)+nrm+") "+getRecipeByVnum(mixRecipes[i].vnum,constants.RECIPE_TYPE_MIXING).name );
			}
			actor.send("");
			actor.send("----------------------------------------------------------------------------");
			actor.send("["+grn+"#"+nrm+"] Make a Selection ("+grn+"1-"+mixRecipes.length+nrm+")    ["+grn+"T"+nrm+"] Table of Contents    ["+grn+"C"+nrm+"] Close Recipe Book");
			setSval(actor,20840,"recipes",3);
		}
		else if (cmd == 3)
		{
			if (tailorRecipes.length < 1)
			{
				actor.send(red+"You do not know any tailoring recipes yet!"+nrm);
				actor.send("");
				actor.detach(20839);
				actor.comm("recipes");
				return;
			}
			actor.send(bld+"Tailoring Recipes"+nrm);
			actor.send("____________________________________________________________________________");
			actor.send("");
			for (i = 0; i < tailorRecipes.length; i++)
			{
				if (getRecipeByVnum(tailorRecipes[i].vnum,constants.RECIPE_TYPE_TAILORING))
					actor.send(grn+(i+1)+nrm+") "+getRecipeByVnum(tailorRecipes[i].vnum,constants.RECIPE_TYPE_TAILORING).name );
			}
			actor.send("");
			actor.send("----------------------------------------------------------------------------");
			actor.send("["+grn+"#"+nrm+"] Make a Selection ("+grn+"1-"+tailorRecipes.length+nrm+")    ["+grn+"T"+nrm+"] Table of Contents    ["+grn+"C"+nrm+"] Close Recipe Book");
			setSval(actor,20840,"recipes",4);
		}
		else if (cmd == 4)
		{
			if (allRecipes.length < 1)
			{
				actor.send("You do not know any recipes yet!");
				actor.send("");
				actor.detach(20839);
				actor.comm("recipes");
				return;
			}
			actor.send(bld+"All Recipes"+nrm);
			actor.send("____________________________________________________________________________");
			actor.send("");
			for (i = 0; i < allRecipes.length; i++)
			{
				if (getRecipeByVnum(allRecipes[i].vnum,allRecipes[i].type))
					actor.send(grn+(i+1)+nrm+") "+getRecipeByVnum(allRecipes[i].vnum,allRecipes[i].type).name );
			}
			actor.send("");
			actor.send("----------------------------------------------------------------------------");
			actor.send("["+grn+"#"+nrm+"] Make a Selection ("+grn+"1-"+allRecipes.length+nrm+")    ["+grn+"T"+nrm+"] Table of Contents    ["+grn+"C"+nrm+"] Close Recipe Book");
			setSval(actor,20840,"recipes",5);
		}
		else
		{
			actor.send("That is not a valid command!");
			actor.send("Choose another command, or type "+grn+"C"+nrm+" to close your recipe book.");
			actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
		}
	}
	//Smithing
	else if (sval == 2)
	{
		for (i = 0; i < smithRecipes.length; i++)
		{
			if (cmd == i+1)
			{ 
				var vRecipe = getRecipeByVnum(smithRecipes[i].vnum, smithRecipes[i].type);
				var vIng = vRecipe.ingredients;
				actor.send(bld+"Recipe For: "+nrm+cyn+vRecipe.name+nrm);
				actor.send("____________________________________________________________________________");
				actor.send("");
				for (j = 0; j < vIng.length; j++)
				{
					var pounds = vIng[j].quantity;
					var vName = getObjProto(vIng[j].vnum).name.split(" ");
					vName = vName[4] + " " + vName[5];
					actor.send(strPadding(""," ",10,"left")+strPadding(vName," ",30,"right") + " ("+cyn+pounds +" lbs"+nrm+")");
				}
				actor.send("");
				actor.send("----------------------------------------------------------------------------");
				actor.send("["+grn+"B"+nrm+"] Back to Smithing Recipes    ["+grn+"T"+nrm+"] Table of Contents   ["+grn+"C"+nrm+"] Close Recipe Book");
				setSval(actor,20840,"recipes",6);
				return;
			}
		}
		actor.send("That is not a valid command!");
		actor.send("Choose another command, or type "+grn+"C"+nrm+" to close your recipe book.");
		actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
	}
	//Mixing
	else if (sval == 3)
	{
		for (i = 0; i < mixRecipes.length; i++)
		{
			if (cmd == i+1)
			{
				var vRecipe = getRecipeByVnum(mixRecipes[i].vnum, mixRecipes[i].type);
				var skillLevel = vRecipe.skillRequired;
				var colorLevel = nrm;
				//here.echo(skillLevel);
				if (skillLevel < 11) {
					skillLevel = "Awful";
					colorLevel = bld;
				}
				else if (skillLevel < 21) {
					skillLevel = "Bad";
					colorLevel = blu;
				}
				else if (skillLevel < 41) {
					skillLevel = "Poor";
					colorLevel = blu;
				}
				else if (skillLevel < 56) {
					colorLevel = grn;
					skillLevel = "Average";
				}
				else if (skillLevel < 71) {
					colorLevel = yel;
					skillLevel = "Fair";
				}
				else if (skillLevel < 81) {
					colorLevel = yel;
					skillLevel = "Good";
				}
				else if (skillLevel < 86) {
					colorLevel = mag;
					skillLevel = "Very Good";
				}
				else if (skillLevel < 99) {
					colorLevel = mag;
					skillLevel = "Superb";
				}
				else {
					colorLevel = red;
					skillLevel = "Artisan";
				}
				var vIng = vRecipe.ingredients;
				actor.send(bld+"Recipe For: "+nrm+colorLevel+vRecipe.name+nrm+strPadding(bld+"Skill: "+nrm," ",18,"left")+colorLevel+skillLevel+nrm);
				actor.send("____________________________________________________________________________");
				actor.send("");
				for (j = 0; j < vIng.length; j++)
				{
					actor.send(strPadding(""," ",10,"left")+strPadding(getObjProto(vIng[j].vnum).name," ",40,"right") + " ("+colorLevel+vIng[j].quantity+nrm+")");
				}
				actor.send("");
				actor.send("----------------------------------------------------------------------------");
				setSval(actor,20840,"recipes",7);
				actor.send("["+grn+"B"+nrm+"] Back to Mixing Recipes    ["+grn+"T"+nrm+"] Table of Contents   ["+grn+"C"+nrm+"] Close Recipe Book");
				return;
			}
		}
		actor.send("That is not a valid command!");
		actor.send("Choose another command, or type "+grn+"C"+nrm+" to close your recipe book.");
		actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
	}
	//Tailoring
	else if (sval == 4)
	{
		for (i = 0; i < tailorRecipes.length; i++)
		{
			if (cmd == i+1)
			{
				var vRecipe = getRecipeByVnum(tailorRecipes[i].vnum, tailorRecipes[i].type);
				var vIng = vRecipe.ingredients;
				actor.send(bld+"Recipe For: "+nrm+cyn+vRecipe.name+nrm);
				actor.send("____________________________________________________________________________");
				actor.send("");
				for (j = 0; j < vIng.length; j++)
				{
					var pounds = vIng[j].quantity;
					var vName = getObjProto(vIng[j].vnum).name.split(" ");
					if (!arrContains(vName, "spool")) {
						vName = vName[4] + " " + vName[5];
						actor.send(strPadding(""," ",10,"left")+strPadding(vName," ",40,"right") + " ("+cyn+pounds +" lbs"+nrm+")");
					}
					else
						actor.send(strPadding(""," ",10,"left")+strPadding(getObjProto(vIng[j].vnum).name," ",40,"right") + " ("+cyn+vIng[j].quantity+nrm+")");
				}
				actor.send("");
				actor.send("----------------------------------------------------------------------------");
				actor.send("["+grn+"B"+nrm+"] Back to Tailoring Recipes    ["+grn+"T"+nrm+"] Table of Contents   ["+grn+"C"+nrm+"] Close Recipe Book");
				setSval(actor,20840,"recipes",8);
				return;
			}
		}
		actor.send("That is not a valid command!");
		actor.send("Choose another command, or type "+grn+"C"+nrm+" to close your recipe book.");
		actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
	}
	//All
	else if (sval == 5)
	{
		for (i = 0; i < allRecipes.length; i++)
		{
			if (cmd == i+1)
			{
				var vRecipe = getRecipeByVnum(allRecipes[i].vnum, allRecipes[i].type);
				var vIng = vRecipe.ingredients;
				actor.send(bld+"Recipe For: "+nrm+cyn+vRecipe.name+nrm);
				actor.send("____________________________________________________________________________");
				actor.send("");
				for (j = 0; j < vIng.length; j++)
				{
					actor.send(strPadding(""," ",10,"left")+strPadding(getObjProto(vIng[j].vnum).name," ",40,"right") + " ("+cyn+vIng[j].quantity+nrm+")");
				}
				actor.send("");
				actor.send("----------------------------------------------------------------------------");
				actor.send("["+grn+"B"+nrm+"] Back to All Recipes    ["+grn+"T"+nrm+"] Table of Contents   ["+grn+"C"+nrm+"] Close Recipe Book");
				setSval(actor,20840,"recipes",9);
				return;
			}
		}
		actor.send("That is not a valid command!");
		actor.send("Choose another command, or type "+grn+"C"+nrm+" to close your recipe book.");
		actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
	}
	else if (sval > 5)
	{
		if (strn_cmp(cmd,"back",1))
		{
			if (sval == 6)
				var command = 1;
			else if (sval == 7)
				var command = 2;
			else if (sval == 8)
				var command = 3;
			else if (sval == 9)
				var command = 4;
			setSval(actor,20840,"recipes",1)
			actor.send(" ");
			actor.detach(20839);
			actor.attach(20839);
			actor.comm(command);
		}
		else
		{
			actor.send("That is not a valid command!");
			actor.send("Choose another command, or type "+grn+"C"+nrm+" to close your recipe book.");
			actor.send(red+"Make sure you do not have an alias that conflicts with your command."+nrm);
		}
		
	}
	
}
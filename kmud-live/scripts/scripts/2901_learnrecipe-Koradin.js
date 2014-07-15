var script2901 = function(self, actor, here, args, extra) {
	var argArray = getArgList(args);
	getCharCols(actor);
	if (!argArray[1])
	{
		actor.send("Try learning the "+cyn+"recipe"+nrm+".");
		_block;
		return;
	}
	if (self != getObjInListVis(actor,argArray[1],actor.inventory))
	{
		_noblock;
		return;
	}
	var recipeVnum = parseInt(self.getPval("recVnum"));
	var recType = self.getPval("recType");
	if (recType == "M")
		var rArray = global.vMixingRecipes;
	else if (recType == "S")
		var rArray = global.vSmithingRecipes;
	else if (recType == "T")
		var rArray = global.vTailoringRecipes;
	for (i=0;i<rArray.length;i++)
	{
		if (rArray[i].vnum == recipeVnum)
		{
			var recipe = rArray[i];
			break;
		}
	}
	_block;
	for each (var rec in actor.getRecipes(recType))
	{
		if (recipe.vnum == rec.vnum)
		{
		actor.send("You already know this recipe!");
		return;
		}
	}
	actor.send("You read the recipe intently, then stuff it into your recipe book.");
	act("$n studies a sheet of worn parchment, then stuffs it into $s recipe book.",true,actor,null,null,constants.TO_ROOM);
	actor.unlockRecipe(recipe.vnum, recType);
	self.moveToRoom(getRoom(20899));
	self.extract();
}
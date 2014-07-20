var script3183 = function(self, actor, here, args, extra) {
	_block;
	var container = self;												// Forge
	var recipeType = constants.RECIPE_TYPE_SMITHING;					// Smithing
	actor.startTimer(2);
	var success = runTimer(actor);
	if ( success ) {
		var recipe = getBestMatchingRecipe( container, recipeType );
	}
	else {
		actor.send("The smithing process is interrupted and nothing is created.");
		return;
	}
	if ( recipe && actor.isRecipeUnlocked(recipe) ) {
		act("\nWithin $p, something can be seen taking shape amidst the red hot embers as its previous contents are incinerated.", true, null, container, null, constants.TO_ROOM);
		// Incinerate forge contents
		for (var _autoKey in container.contents) {
			var item = container.contents[_autoKey];
			item.extract();
		}
		// Load results into the forge
		for (var _autoKey in recipe.results) {
			var result = recipe.results[_autoKey];
			var i = 0;
			// Load correct quantity
			while ( i < result.quantity ) {
				var itemName = container.loadObj(result.vnum).name;
				act(itemName+" is ready for you inside $p.", true, actor, container, null, constants.TO_CHAR);
				act(itemName+" is ready for $n inside $p.", true, actor, container, null, constants.TO_ROOM);
				i++;
			}
		}
		return;
	}
	else {
		actor.send("\nWhat exactly are you trying to smith?");
		return
	}
}
var script13276 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList( args );
	if( vArgs.length < 2 || vArgs.length > 2) {
		actor.send("Mix what?");
		return;
	}
	function failMix( actor,container,potion )
	{
		if (potion)
		{
			if (potion.failureMessageToActor)
				var message = potion.failureMessageToActor;
			else
				var message = "The contents don't mix too well...";
			if (potion.failureMessageToRoom)
				var message2 = potion.failureMessageToRoom;
			else
				var message2 = "$n smells the contents of the vial and almost gags, shaking $s head.";
		}
		else
		{
			var message = "The contents don't mix too well...";
			var message2 = "$n smells the contents of the vial and almost gags, shaking $s head.";		
		}
		actor.send(message);
		act(message2,true,actor,null,null,constants.TO_ROOM);
		for (var _autoKey in contents) {
			var item = contents[_autoKey];
			item.extract();
		}
		container.loadObj(20981);
	}
	var container = actor.hasItem(2030,true,true,false);
	if( !container ) {
		actor.send("You need a small glass vial to mix your ingredients in.");
		return;
	}
	if( !isName( vArgs[1], container.namelist ) ) {
		actor.send("You can only mix a vial.");
		return;
	}
	var contents = container.contents;
	if( contents.length == 0 ) {
		actor.send( capFirstLetter(container.name) + " has nothing inside for you to mix.");
		return;
	}
	if (!arrContains(container.namelist.split(" "), "vialisnowfull")) {
		actor.send("The vial must be full before you can mix it.");
		return;
	}
	for (i = 0; i < contents.length; i++)
	{
		if( !isName("herb",contents[ i ].namelist) ) {
			actor.send(capFirstLetter(contents[i].name) + " cannot be mixed. Remove it from the vial.");
			return;
		}
	}
	/*** Mix is being performed on a valid container. Let us now determine the outcome. ***/
	actor.send("You begin mixing the contents of the vial...");
	act("$n begins mixing the contents of a glass vial...", true, actor, container, null, constants.TO_ROOM);
	var potion = getBestMatchingRecipe( container, constants.RECIPE_TYPE_MIXING );
	if( potion ) {
		actor.startTimer( potion.ingredients.length + potion.results.length + 1 );
		var s = runTimer(actor);
		if( !s )
			return;
		var actorHasRecipe = false;
		for (i = 0; i < actor.getRecipes('M').length; i++)
		{
			if (actor.getRecipes('M')[i].vnum == potion.vnum)
			{
				actorHasRecipe = true;
				break;
			}
		}
		if (actorHasRecipe == false)
		{
			//here.echo("actor recipes: "+actor.getRecipes('M')[i].vnum+" potion: "+potion.vnum);
			failMix(actor,container,potion);
			return;
		}
		for(var i = 0;i < potion.ingredients.length;++i) {
			//Remove all of the required ingredients.
			var vItems = container.getItems( potion.ingredients[i].vnum );
			for(var n = 0;n < vItems.length;++n) {
				vItems[ n ].extract();
			}
		}
		if( actor.getSkill( getSkillVnum("Mixing") ) >= potion.skillRequired ) {
			if (potion.successMessageToActor)
				var message = potion.successMessageToActor;
			else
				var message = "You finish mixing the vial, smiling happily.";
			if (potion.successMessageToRoom)
				var message2 = potion.successMessageToRoom;
			else
				var message2 = "$n finishes mixing the vial, smiling happily.";
			actor.send(message);
			act(message2,true,actor,null,null,constants.TO_ROOM);
			for(var i = 0;i < potion.results.length;++i) {
				for(var n = 0;n < potion.results[i].quantity;++n) {
					actor.loadObj( potion.results[i].vnum );
					var new_obj = actor.inventory[0];
					new_obj.setPval("made_by", actor.name, true);
				}
			}
			container.extract();
			awardPracs(actor,"Mixing","success",potion);
		}
		else
		{
			failMix(actor,container,potion);
			awardPracs(actor,"Mixing","fail",potion);
		}
	}
	else
	{
		actor.startTimer( random(4,8) );
		var s = runTimer(actor);
		if( !s )
			return;
		act("$n finishes mixing.", true, actor, container, null, constants.TO_ROOM);
		failMix(actor,container);
	}
	
}
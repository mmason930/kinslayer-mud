var script2546 = function(self, actor, here, args, extra) {
	if(TempleManager.isSpiderSlabStuck() || actor.level >= 100)
		return;
	
	var numberOfSpiders = here.getMobs(TempleManager.spiderMobVnum);
	var numberOfNonSpiders = here.people.length - numberOfSpiders.length;
	
	if(random(1, numberOfNonSpiders * 5) <= numberOfNonSpiders)
	{
		here.echo("A deep rumbling resonates from the east as a slab closes the chamber shut!");
		here.echo("A large clump of web drops from the ceiling, blocking the slab!");
		
		var roomVnumToFirstStepMap = {};
		
		var echoMessageFunction = function(targetCharacter)
		{
			var firstStep = roomVnumToFirstStepMap[ targetCharacter.room.vnum ];
			
			if(firstStep === undefined)
			{
				firstStep = targetCharacter.room.firstStep(here);
				roomVnumToFirstStepMap[ targetCharacter.room.vnum ] = firstStep;
			}
			
			return "The ground shakes violently as a deep rumbling noise resonates from the " + dirToText(firstStep) + ".";
		}
		
		here.echoToNeighbors(echoMessageFunction, 5);
		
		TempleManager.markSpiderSlabAsStuck();
		here.closeDoor(constants.EAST);
		here.direction(constants.EAST).closeDoor(constants.WEST);
		
		setTimeout(60, function() {
			
			here.echo("The spider web blocking the slab disintigrates.");
			TempleManager.unmarkSpiderSlabAsStuck();
		});
	}
}
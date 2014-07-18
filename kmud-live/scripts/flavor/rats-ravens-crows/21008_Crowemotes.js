var script21008 = function(self, actor, here, args, extra) {
	var rand = random(1,4);
	if( rand == 1) {
		act("A black feathered crow flutters its wings.", true, self, null, null, constants.TO_ROOM);
	}
	else {
		var potentialTargets = here.getCharacters(function(character) {

			return character.race == constants.RACE_HUMAN && self.canSee(character);
		});

		var target = potentialTargets.length == 0 ? null : potentialTargets[random(0, potentialTargets.length - 1)];

		if(target == null)
			return;

		if( rand == 2) {
			act("A black feathered crow stares at you with beady black eyes, waiting silently.", true, self, null, null, constants.TO_VICT);
			act("A black feathered crow stares at " + target.name + ", waiting silently.", true, self, null, null, constants.TO_ROOM);
		}
		else if( rand == 3) {
			act("A black feathered crow looks straight at you and caws three times.", true, self, null, null, constants.TO_VICT);
			act("A black feathered crow looks straight at " + target.name + " and caws three times.", true, self, null, null, constants.TO_ROOM);
		}
		else if(rand == 4) {
			act("A black feathered crow swoops down and pecks you! Ouch!", true, self, null, null, constants.TO_VICT);
			act("A black feathered crow swoops down and pecks $N.", true, self, null, target, constants.TO_NOTVICT);
		}
	}
};
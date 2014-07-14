var script21009 = function(self, actor, here, args, extra)
{
	var rand = random(1,3);
if( rand == 1) {
	act("A sleek black-feathered raven flutters its wings.", true, self, null, null, constants.TO_ROOM);
}
else {
	target = getRandomPersonInRoom(here);
	if(target.invis > self.level) {
		return;
	}
	if( rand == 2) {
		act("A sleek black-feathered raven stares at you with beady black eyes, almost as if waiting.", true, self, null, null, constants.TO_VICT);
		act("A sleek black-feathered raven stares at " + target.name + ", almost as if waiting.", true, self, null, null, constants.TO_ROOM);
	}
	else if( rand == 3) {
		act("A sleek black-feathered raven looks straight at you and caws three times.", true, self, null, null, constants.TO_VICT);
		act("A sleek black-feathered raven looks straight at " + target.name + " and caws three times.", true, self, null, null, constants.TO_ROOM);
	}
}

};


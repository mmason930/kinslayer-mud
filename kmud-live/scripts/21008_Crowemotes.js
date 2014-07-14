var script21008 = function(self, actor, here, args, extra)
{
	var rand = random(1,3);
if( rand == 1) {
	act("A black feathered crow flutters its wings.", true, self, null, null, constants.TO_ROOM);
}
else {
	target = getRandomPersonInRoom(here);
	if(target.invis > self.level) {
		return;
	}
	if( rand == 2) {
		act("A black feathered crow stares at you with beady black eyes, almost as if waiting.", true, self, null, null, constants.TO_VICT);
		act("A black feathered crow stares at " + target.name + ", almost as if waiting.", true, self, null, null, constants.TO_ROOM);
	}
	else if( rand == 3) {
		act("A black feathered crow looks straight at you and caws three times.", true, self, null, null, constants.TO_VICT);
		act("A black feathered crow looks straight at " + target.name + " and caws three times.", true, self, null, null, constants.TO_ROOM);
	}
}

};


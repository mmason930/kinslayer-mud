var script704 = function(self, actor, here, args, extra) {
	var ch = self;
	var room = here;
	
	var doBlock = false;
	// If any players are running a bash timer on the MOB, throw up a shield block
	for (var _autoKey in room.people) {
		var person = room.people[_autoKey];
		if (person.fighting == ch &&
			person.target == ch
		) {
			doBlock = true;
		}
	}
	
	if (doBlock) {
		ch.comm("block");
	}
}
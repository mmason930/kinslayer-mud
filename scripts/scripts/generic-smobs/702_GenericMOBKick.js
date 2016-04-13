var script702 = function(self, actor, here, args, extra) {
	var ch = self;
	var room = here;
	var vict = null

	if(self.isBashed || self.position < constants.POS_FIGHTING)
		return;
	
	// If the mob's kick isn't on cooldown, kick an opposing player. Prioiritiz
	var kickCooldown = getSval(ch, 4001, "kickCooldown");
	if (!kickCooldown) {
		wait random(1, 5);
		for (var _autoKey in room.people) {
			var person = room.people[_autoKey];
			if (person.fighting == ch &&
				person.target == ch
			) {
				vict = person;
			}
		}
		if (!vict) {
			vict = actor;
		}
		namelist = vict.namelist.split(" ");
		setTimeout(random(1, 5), kickVictim, [namelist[0]]);
	}
	
	function kickVictim(vArgs) {

		if(!self.isBashed && self.position >= constants.POS_FIGHTING)
			ch.comm("kick " + vArgs[0]);
	}
}
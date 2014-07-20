var script703 = function(self, actor, here, args, extra) {
	var ch = self;
	var room = here;
	
	// Precise Strike the target with the lowest hp currently fighting the MOB
	var vict = null;
	var victHit = 1000;
	for (var _autoKey in room.people) {
		var person = room.people[_autoKey];
		if (person.fighting == ch &&
			person.hps < victHit) 
		{
				vict = person;
				victHit = person.hps;
		}
	}
	if (!vict) {
		vict = actor;
	}
	var namelist = vict.namelist.split(" ");
	setTimeout(random(1, 5), strikeVictim, [namelist[0]]);
	
	function strikeVictim(vArgs) {
		ch.comm("strike " + vArgs[0]);
	}
}
var script14225 = function(self, actor, here, args, extra) {
	wait random(1,10);
	if (!self || !self.room)
		return;
	if (self.room.zoneVnum != global.arenaZoneVnum)
		return;
	if (self.race == constants.RACE_HUMAN) {
		var noAgro = constants.ARENA_BLUE;
	}
	else if (self.race == constants.RACE_TROLLOC) {
		var noAgro = constants.ARENA_RED;
	}
	else if (self.race == constants.RACE_AIEL) {
		var noAgro = constants.ARENA_YELLOW;
	}
	else if (self.race == constants.RACE_SEANCHAN) {
		var noAgro = constants.ARENA_GREEN;
	}
	var aggArr = [];
	var assistArr = [];
	for each (var player in self.room.people) {
		if (player != self && !player.mobFlagged(constants.MOB_MOUNT)) {
			if (self.mobFlagged(constants.MOB_SMOB))
				aggArr.push(player);
			else if (global.arenaFreeForAll == true) {
				if (player.race == self.race && player.fighting) {
					if (player.vnum == -1)
						var aName = player.name;
					else
						var aName = player.namelist.split(" ")[0];
					assistArr.push(aName);
				}
				if (player.race != self.race)
					aggArr.push(player);
			}
			else {
				if (player.arenaTeam == noAgro && player.fighting) {
					if (player.vnum == -1)
						var aName = player.name;
					else
						var aName = player.namelist.split(" ")[0];
					assistArr.push(aName);
				}
				if ( (player.vnum == -1 && player.arenaTeam != noAgro) || (player.vnum > 0 && player.race != self.race) )
					aggArr.push(player);
			}
		}
	}
	if (assistArr.length == 1) {
		self.comm("assist "+assistArr[0]);
		return;
	}
	else if (assistArr.length > 1) {
		self.comm("assist "+assistArr[ random(0, assistArr.length-1) ]);
		return;
	}
	if (aggArr.length == 1)
		var victim = aggArr[0];
	else
		var victim = aggArr[random(0,aggArr.length-1)];
	if (!victim)
		return;
	if (victim.vnum == -1)
		var alias = victim.name;
	else
		var alias = victim.namelist.split(" ")[0];
	self.comm("k "+alias);
}
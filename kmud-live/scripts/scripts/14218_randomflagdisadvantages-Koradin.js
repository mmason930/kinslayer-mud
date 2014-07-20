var script14218 = function(self, actor, here, args, extra) {
	actor = self.carriedBy;
	if (actor) {
	actor.sps = 0;
	actor.shadowPoints = 0;
	}
	if (self.vnum == 14200)
		var team = constants.ARENA_BLUE;
	else if (self.vnum == 14201)
		var team = constants.ARENA_RED;
	else if (self.vnum == 14203)
		var team = constants.ARENA_YELLOW;
	else if (self.vnum == 14204)
		var team = constants.ARENA_GREEN;
	for (var _autoKey in global.arenaPlayers) {
		var player = global.arenaPlayers[_autoKey];
		if (player.arenaTeam == team)
			return;
	}
	if (self.carriedBy) {
		self.carriedBy.detach(14215);
		self.carriedBy.sps = (Math.floor(self.carriedBy.maxSpell*.6));
		self.carriedBy.shadowPoints = (Math.floor(self.carriedBy.maxShadow*.6));
		var room = self.carriedBy.room;
	}
	else {
		var room = self.room;
	}
	room.echo(capFirstLetter(self.name)+ " suddenly winks out of existence!");
	self.moveToRoom(getRoom(20899));
	self.extract();
	
}
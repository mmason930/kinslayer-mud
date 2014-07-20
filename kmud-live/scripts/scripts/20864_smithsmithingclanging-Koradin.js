var script20864 = function(self, actor, here, args, extra) {
	for (var _autoKey in self.neighbors) {
		var room = self.neighbors[_autoKey];
		if (room) {
			for (var _autoKey in room.items) {
				var item = room.items[_autoKey];
				if (getSval(item,20863,"isLit") > 0) {
					waitpulse 5;
					self.echo("The clanging of a blacksmith's hammer on metal can be heard to the "+dirToText(self.firstStep(room))+".");
					return;
				}
			}
		}
	}
}
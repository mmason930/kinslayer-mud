var script20864 = function(self, actor, here, args, extra) {
	for each (var room in self.neighbors) {
		if (room) {
			for each (var item in room.items) {
				if (getSval(item,20863,"isLit") > 0) {
					waitpulse 5;
					self.echo("The clanging of a blacksmith's hammer on metal can be heard to the "+dirToText(self.firstStep(room))+".");
					return;
				}
			}
		}
	}
}
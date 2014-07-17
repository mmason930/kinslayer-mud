var script14232 = function(self, actor, here, args, extra) {
	wait random(1,5);
	for each (var thing in self.items) {
		if (thing.vnum == 14205)
			return;
	}
	self.loadObj(14205);
}
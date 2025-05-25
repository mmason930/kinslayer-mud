var script14232 = function(self, actor, here, args, extra) {
	wait random(1,5);
	for (var _autoKey in self.items) {
		var thing = self.items[_autoKey];
		if (thing.vnum == 14205)
			return;
	}
	self.loadObj(14205);
}
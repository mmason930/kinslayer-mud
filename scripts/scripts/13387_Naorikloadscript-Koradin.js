var script13387 = function(self, actor, here, args, extra) {
	self.comm("rem shoufa");
	for (var _autoKey in self.inventory) {
		var item = self.inventory[_autoKey];
		item.extract();
	}
	self.loadObj(43);
	self.comm("wear hood");
}
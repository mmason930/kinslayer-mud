var script354 = function(self, actor, here, args, extra) {
	if(extra.obj.vnum == 303) {
		extra.obj.extract();
		var obj = self.loadObj(352);
		self.comm("give bra " + actor.name);
	}
}
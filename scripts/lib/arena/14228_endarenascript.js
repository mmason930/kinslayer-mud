var script14228 = function(self, actor, here, args, extra) {
	var timer = getSval(getRoom(500).items[0], 14200, "stall");
	if (!timer || (time()-timer) > 5) {
		actor.detach(14228);
		return;
	}
	else
		_block;
}
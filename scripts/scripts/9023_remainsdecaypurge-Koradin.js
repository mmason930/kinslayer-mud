var script9023 = function(self, actor, here, args, extra) {
	//remains purge/decay from wildfire
	//Koradin - May 2010
	var room = self.room;
	if (!room)
		return;
	for (var _autoKey in room.items) {
		var item = room.items[_autoKey];
		if (item.vnum == self.vnum && item != self)
			item.extract();
	}
}
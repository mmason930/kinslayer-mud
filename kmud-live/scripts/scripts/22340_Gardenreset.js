var script22340 = function(self, actor, here, args, extra) {
	var rooms = [getRoom(22339),getRoom(22340),getRoom(22341),getRoom(22342)];
	for (var _autoKey in rooms) {
		var room = rooms[_autoKey];
		room.echo("Weeds start to sprout again, covering the garden...");
		setSval(room, 22339, "raked", 0);
	}
	var dRooms = [getRoom(22310),getRoom(22311)];
	for (var _autoKey in dRooms) {
		var room = dRooms[_autoKey];
		room.echo("A thin layer of dust settles over the shelves...");
		setSval(room, 22339, "dusted", 0);
	}
	var kRooms = [getRoom(22318)];
	for (var _autoKey in kRooms) {
		var room = kRooms[_autoKey];
		room.echo("More dishes are brought in, cluttering the sinks...");
		setSval(room, 22339, "washed", 0);
		for (var _autoKey in room.items) {
			var thing = room.items[_autoKey];
			if (thing.vnum == 22318)
				return;
		}
		room.loadObj(22318);
	}
	
}
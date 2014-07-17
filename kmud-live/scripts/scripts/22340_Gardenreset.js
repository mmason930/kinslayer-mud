var script22340 = function(self, actor, here, args, extra) {
	var rooms = [getRoom(22339),getRoom(22340),getRoom(22341),getRoom(22342)];
	for each (var room in rooms) {
		room.echo("Weeds start to sprout again, covering the garden...");
		setSval(room, 22339, "raked", 0);
	}
	var dRooms = [getRoom(22310),getRoom(22311)];
	for each (var room in dRooms) {
		room.echo("A thin layer of dust settles over the shelves...");
		setSval(room, 22339, "dusted", 0);
	}
	var kRooms = [getRoom(22318)];
	for each (var room in kRooms) {
		room.echo("More dishes are brought in, cluttering the sinks...");
		setSval(room, 22339, "washed", 0);
		for each (var thing in room.items) {
			if (thing.vnum == 22318)
				return;
		}
		room.loadObj(22318);
	}
	
}
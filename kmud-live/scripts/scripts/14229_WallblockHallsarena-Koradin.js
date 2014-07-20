var script14229 = function(self, actor, here, args, extra) {
	waitpulse random(1,60);
	var rand = random(1,3);
	if (global.arenaCurrentMap.currentBlockedRooms) {
	if (global.arenaCurrentMap.currentBlockedRooms.length > 0) {
	for (var _autoKey in global.arenaCurrentMap.currentBlockedRooms) {
		var room = global.arenaCurrentMap.currentBlockedRooms[_autoKey];
		if (room) {
			for (var _autoKey in room.neighbors) {
				var neighbor = room.neighbors[_autoKey];
				if (neighbor)
					neighbor.echo("A slight shimmer in the air to the "+dirToText( neighbor.firstStep(room) )+" plays tricks on your eyes...");
			}
			room.detach(14230);
		}
	}
	}
	}
	global.arenaCurrentMap.currentBlockedRooms = [];
	if (global.arenaStage != constants.ARENA_PLAYING)
		return;
	var roomArr = [14602,14603,14604,14608,14609,14610,14614,14615,14616,14620,14621,14622,14624,14625,14626,14627,14628,14629,14630,14631,14632,14633,14634,14635];
	while (rand > 0) {
		var newRoom = getRoom(roomArr[ random(0,roomArr.length-1) ]);
		global.arenaCurrentMap.currentBlockedRooms.push(newRoom);
		newRoom.attach(14230);
		for (var _autoKey in newRoom.neighbors) {
			var neighbor = newRoom.neighbors[_autoKey];
			if (neighbor)
				neighbor.echo("A slight shimmer in the air to the "+dirToText( neighbor.firstStep(newRoom) )+" plays tricks on your eyes...");
		}
		//sendKoradin(rand);
		rand -= 1;
	}
}
var script14220 = function(self, actor, here, args, extra) {
	if (global.arenaStage != constants.ARENA_PLAYING || global.arenaZoneVnum != 330)
		return;
	function tornadoRoom(rVnum) {
		var room = getRoom(rVnum);
		room.echo("A gust of wind suddenly sweeps into the valley, scattering everyone!");
		var roomArr = [];
		for (i=global.arenaCurrentMap.min;i<global.arenaCurrentMap.max+1;i++) {
			if (!arrContains(global.arenaCurrentMap.exclusions, i))
				roomArr.push(i);
		}
		for (var _autoKey in room.people) {
			var player = room.people[_autoKey];
			player.send("You are picked up by the wind and thrown out of the area!");
			act("$n is picked up by the wind and tossed through the air!",true,player,null,null,constants.TO_ROOM);
			player.moveToRoom(getRoom(roomArr[random(0,roomArr.length-1)]));
			player.comm("look");
			act("$n flies into the room, landing with a thud.",true,player,null,null,constants.TO_ROOM);
		}
	}
	var affectRooms = [];
	for (i=global.arenaCurrentMap.min;i<global.arenaCurrentMap.max+1;i++) {
		if (!arrContains(global.arenaCurrentMap.exclusions, i))
			affectRooms.push(i);
	}
	var timer = 6;
	while (timer > 0) {
		tornadoRoom(affectRooms[random(0,affectRooms.length-1)]);
		wait 6;
		timer -= 6;
	}
}
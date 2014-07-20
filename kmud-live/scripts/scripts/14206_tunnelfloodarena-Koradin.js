var script14206 = function(self, actor, here, args, extra) {
	var timeToFlood = getSval(getRoom(14350),14206,"river");
	// for (var _autoKey in getConnectedPlayers()) { 	var player = getConnectedPlayers()[_autoKey];
		// if (player.name == "Koradin")
			// player.send("Time till next flood: "+(timeToFlood-time()));
	// }
	if (time() < timeToFlood)
		return;
	if (getSval(self,14206,"flooded") == 1) {
		self.echo("The waters recede...");
		getRoom(14356).echo("You notice the water in the tunnel below you receding...");
		getRoom(14343).echo("You notice the water in the tunnel below you receding...");
		var timer = time() + random(180,360);
		setSval(self,14206,"river",timer);
		setSval(self,14206,"flooded",0);
		return;
	}
	if (getSval(self,14206,"running") == 1)
		return;
	var roomArr = [14344,14345,14346,14347,14348,14351,14352,14353,14354,14355];
	var farRoomArr = [14357,14358,14359,14360,14361,14362,14363,14342,14341,14349,14340,14339,14338,14337];
	var msg1 = "A low rumbling can be heard overhead...";
	var msg2 = "The rumbling grows louder as cracks spread across the rocky ceiling...";
	var msg3 = "Suddenly the river breaks through overhead, burying you in rock and rushing water!";
	var msg4 = "The rushing water from the river above sweeps you away!";
	var altMsg1 = "A low rumbling can be heard down the tunnel...";
	var altMsg2 = "The rumbling grows louder, bits of rock falling from the ceiling...";
	var altMsg3 = "An enormous crash followed by the sound of rushing water echoes down the tunnel...";
	var altMsg41 = "Suddenly a wall of water rushes in from the ";
	var altMsg42 = ", sweeping you away!";
	setSval(self,14206,"running",1);
	self.echo(msg1);
	for (var _autoKey in roomArr) {
		var roomVnum = roomArr[_autoKey];
		getRoom(roomVnum).echo(altMsg1);
	}
	wait 6;
	self.echo(msg2);
	for (var _autoKey in roomArr) {
		var roomVnum = roomArr[_autoKey];
		getRoom(roomVnum).echo(altMsg2);
	}
	wait 6;
	self.echo(msg3);
	for (var _autoKey in roomArr) {
		var roomVnum = roomArr[_autoKey];
		getRoom(roomVnum).echo(altMsg3);
	}
	if (self.vnum == 14350) {
		for (var _autoKey in self.people) {
			var person = self.people[_autoKey];
			act("You watch in horror as $n is crushed by the falling rock and water.",true,person,null,null,constants.TO_ROOM);
			person.damage(10000);
		}
	}
	wait 6;
	setSval(self,14206,"running",0);
	self.echo(msg4);
	for (var _autoKey in roomArr) {
		var roomVnum = roomArr[_autoKey];
		getRoom(roomVnum).echo(altMsg41+dirToText(getRoom(roomVnum).firstStep(getRoom(14350)))+altMsg42);
		for (var _autoKey in getRoom(roomVnum).people) {
			var pers = getRoom(roomVnum).people[_autoKey];
			act("$n is swept away by the force of the water!",true,pers,null,null,constants.TO_ROOM);
			pers.damage(10000);
		}
	}
	for (var _autoKey in self.people) {
		var per = self.people[_autoKey];
		act("$n is swept away by the force of the water!",true,per,null,null,constants.TO_ROOM);
		per.damage(10000);
	}
	if (self.vnum == 14350) {
		var timer = time() + random(180,360);
		setSval(self,14206,"river",timer);
		setSval(self,14206,"flooded",1);
	}
	getRoom(14356).echo("Water rushes in to fill the tunnel below, gurgling up around your feet.");
	getRoom(14343).echo("Water rushes in to fill the tunnel below, gurgling up around your feet.");
	for (var _autoKey in farRoomArr) {
		var roomVnum = farRoomArr[_autoKey];
		getRoom(roomVnum).echo("The sound of rushing water can be heard farther down the tunnel...");
	}
}
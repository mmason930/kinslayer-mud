var script33212 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	getCharCols(actor);
	if (str_cmp(vArgs[1],"siege") && str_cmp(vArgs[1],"team") && str_cmp(vArgs[1],"member"))
		return;
	for (var _autoKey in self.room.people) {
		var per = self.room.people[_autoKey];
		if (per.name == "a member of the siege team")
			var mob = per;
	}
	if (!mob)
		return;
	_block;
	act("$n issues an order.",true,actor,null,null,constants.TO_ROOM);
	if (actor.id != getSval(self,33212,"owner")) {
		mob.say("You're not the boss of us!");
		return;
	}
	var goalRoom = getRoom(getSval(self,33212,"goal"));
	var manor = getManor(goalRoom);
	if (!vArgs[2]) {
		mob.say("Order us to do what?!");
		mob.comm("tell "+actor.name+" You can order us to MARCH or DESTROY.");
		return;
	}
	else if (strn_cmp(vArgs[2],"march",3)) {
		if (strn_cmp(vArgs[3],"east",1)) {
			var dir = constants.EAST;
		}
		else if (strn_cmp(vArgs[3],"south",1)) {
			var dir = constants.SOUTH;
		}
		else if (strn_cmp(vArgs[3],"north",1)) {
			var dir = constants.NORTH;
		}
		else if (strn_cmp(vArgs[3],"west",1)) {
			var dir = constants.WEST;
		}
		else if (strn_cmp(vArgs[3],"up",1)) {
			var dir = constants.UP;
		}
		else if (strn_cmp(vArgs[3],"down",1)) {
			var dir = constants.DOWN;
		}
		else {
			mob.say("Order us to march where?!");
			return;
		}
		if (getManor(goalRoom).isUnderSiege) {
			mob.say("Move now?! We're sieging the gate right now!");
			return;
		}
		if (self.room == goalRoom) {
			mob.say("But we're already here!");
			if (!manor.gateIsBroken)
				mob.say("Order us to DESTROY the GATE!");
			return;
		}
		if (!self.room.direction(dir)) {
			mob.say("We can't go that way!");
			return;
		}
		if (self.room.distanceTo(goalRoom) < self.room.direction(dir).distanceTo(goalRoom)) {
			mob.say("That way will lead us away from our orders!");
			return;
		}
		mob.say("Right away!");
		var newRoom = self.room.direction(dir);
		act("The siege team rolls the cannon "+dirToText(dir)+".",true,null,self,null,constants.TO_ROOM);
		for (var _autoKey in self.room.people) {
			var person = self.room.people[_autoKey];
			if (person.name == "a member of the siege team")
				person.moveToRoom(newRoom);
		}
		self.moveToRoom(newRoom);
		act("A siege team has arrived, slowly rolling an iron cannon between them.",true,null,self,null,constants.TO_ROOM);
	}
	else if (strn_cmp(vArgs[2],"destroy",3)) {
		if (self.room != goalRoom) {
			mob.say("We haven't reached our target yet!");
			mob.say("Order us to MARCH first!");
			return;
		}
		if (manor.gateIsBroken || !manor.hasGate) {
			mob.say("We don't see anything to destroy!");
			act("The siege team rolls the cannon away, heading back to camp.", true, null, self, null,constants.TO_ROOM);
			for (var _autoKey in self.room.people) {
				var person = self.room.people[_autoKey];
				if (person.name == "a member of the siege team")
					person.moveToRoom(getRoom(20899));
			}
			self.moveToRoom(getRoom(20899));
			setSval(actor,33214,"hired",0);
			return;
		}
		if (manor.isUnderSiege) {
			mob.say("We're giving it all we've got!");
			return;
		}
		mob.say("As you command, "+actor.name+"!");
		// for (var _autoKey in getConnectedPlayers()) { 	var perp = getConnectedPlayers()[_autoKey];
		//	if (perp.id == manor.ownerUserId || arrContains(manor.allowedUsers, perp.id)) {
		//		perp.affect(constants.AFF_NOQUIT,5,0,0);
		//	}
		// }
		ctfEcho("The "+manor.name+" near "+manor.area+" is under siege!");
		manor.isUnderSiege = true;
		var tries = random(3,5);
		while (tries > 0) {
			act("The siege team loads the cannon and lights the fuse!",true,null,self,null,constants.TO_ROOM);
			wait 2;
			self.room.zecho("BOOM! An explosion rips through the area, leaving your ears ringing!");
			if (tries == 1) {
				ctfEcho("The walls of the "+manor.name+" near "+manor.area+" have been breached!");
				self.room.echo("Timbers burst as the gate falls inward with a CRASH!");
				self.room.unlockDoor(self.room.firstStep(getRoom(manor.insideGateRoomVnum)), true);
				self.room.openDoor(self.room.firstStep(getRoom(manor.insideGateRoomVnum)), true);
				manor.gateIsBroken = true;
				manor.isUnderSiege = false;
				setSval(self.room,33212,"gate",(time()+1440));
				wait 2;
				act("The siege team rolls the cannon away, heading back to camp.", true, null, self,null,constants.TO_ROOM);
				for (var _autoKey in self.room.people) {
					var person = self.room.people[_autoKey];
					if (person.name == "a member of the siege team")
						person.moveToRoom(getRoom(20899));
				}
				setSval(actor,33214,"hired",0);
				self.moveToRoom(getRoom(20899));
			}
			else {
				self.room.echo("The cannonball rips through the gate, sending splinters everywhere!");
			}
			tries -= 1;
			wait 12;
		}
	}
	else {
		mob.say("Order us to do what?!");
		mob.comm("tell "+actor.name+" You can order us to MARCH or DESTROY.");
		return;
	}
	
	
}
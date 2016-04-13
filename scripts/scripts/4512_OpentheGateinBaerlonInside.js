var script4512 = function(self, actor, here, args, extra) {
	/* Gate open in Baerlon */
	/* Overhauled by Rhollor 6/5/10 */
	if(actor.race != constants.RACE_HUMAN) {
		waitpulse 2;
		if(actor.room == self.room) {
			actor.send("Arrows rain down on you from the wall above!");
			echoaround(actor, "Arrows rain down onto " + actor.name + " from the wall above!");
			actor.damage(random(1,60));
		}
	}
	else {
		var command = "";
		if(args.indexOf("close") == -1 && args.indexOf("lock") == -1) {
			command = "open";
		}
		else if(args.indexOf("open") == -1 && args.indexOf("lock") == -1) {
			command = "close";
		}
		else
			command = "lock";
		actor.send(command);
		if(args.indexOf("gate") != -1) {
			if(command == "lock") {
				if(self.room.doorIsLocked(constants.EAST)) {
					waitpulse 4;
					self.comm("say The gates are already locked.");
				}
				else if(self.room.doorIsClosed(constants.EAST)) { 
					waitpulse 4;
					self.comm("emote nods and finds the key.");
					self.room.echo(self.name + " locks the ironbound gates.");
					self.room.lockDoor(constants.EAST);
					setSval(here, 4512, "BAERLON_EAST_GATE_OPEN", false);
				}
				else {
					waitpulse 4;
					self.comm("say The gates are open though...");
				}
				return;
			}
			if(!getSval(here,4512, "BAERLON_EAST_GATE_OPEN") && self.room.doorIsClosed(constants.EAST)) {
				//Gate is closed
				if(command == "close") {
					waitpulse 4;
					self.comm("say The gate is already closed");
				}
				if(command == "open") {
					waitpulse 4;
					self.comm("emote nods and orders " + self.hisHer() + " men to open the gates.");
					waitpulse 10;
					self.room.echo("The guards begin opening the gates.");
					waitpulse 15;
					self.room.echo("The gates hit the town's walls with a dull thud.");
					self.room.unlockDoor(constants.EAST);
					self.room.openDoor(constants.EAST);
					setSval(here, 4512, "BAERLON_EAST_GATE_OPEN", true);
				}
			}
			else {
				//Gate is open
				if(command == "open") {
					waitpulse 4;
					self.comm("say The gate is already open.");
				}
				if(command == "close") {
					if(getSval(here,4512, "BAERLON_EAST_GATE_OPEN") && !self.room.doorIsClosed(constants.EAST)) {
						waitpulse 4;
						self.comm("emote nods and orders " + self.hisHer() + " men to close the gates.");
						waitpulse 10;
						self.room.echo("The gates creak as the guards begin to close them.");
						waitpulse 15;
						self.room.echo("The gates close with a thud.");
						self.room.closeDoor(constants.EAST);
						self.room.lockDoor(constants.EAST);
						setSval(here, 4512, "BAERLON_EAST_GATE_OPEN", false);
					}
				}
			}
		}
	}
}
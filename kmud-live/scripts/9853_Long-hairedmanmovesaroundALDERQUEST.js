var script9853 = function(self, actor, here, args, extra)
{
		if ( self.vnum == 9850 ) {
		// for each ( var person in getConnectedPlayers() ) {
			// if ( person.name == "Alder" ) {
				// var room = person.room;
				// break;
			// }
		// }
		// room.echo(self.room.vnum);
		var destination = getRoom(getSval(self,9850,"DESTINATION"));
		var D = getRoom(getSval(self,9850,"POINT_D"));
		var C = getRoom(getSval(self,9850,"POINT_C"));
		var B = getRoom(getSval(self,9850,"POINT_B"));
		var A = getRoom(getSval(self,9850,"POINT_A"));
		function afterWait(arg) {
			var person = arg[0];
			getCharCols(person,constants.CL_OFF);
			person.send(bld+"A long-haired man roughly brushes past you without saying a word."+nrm);
			person.send("");
			person.send(bld+"You turn to let him know but he is nowhere to be seen. As you turn back around you realize that you are holding a small piece of paper."+nrm);
			person.loadObj(9850);
			person.comm("Save");
		}
		for ( var i = 0; i < 8 && i <= self.room.distanceTo(destination); i++ ) {
			if ( self.room == destination ) {
				if ( self.room == A ) {
					setSval(self,9850,"DESTINATION",getSval(self,9850,"POINT_B"));
				}
				else if ( self.room == B ) {
					setSval(self,9850,"DESTINATION",getSval(self,9850,"POINT_C"));
				}
				else if ( self.room == C ) {
					setSval(self,9850,"DESTINATION",getSval(self,9850,"POINT_D"));
				}
				else if ( self.room == D ) {
					setSval(self,9850,"DESTINATION",getSval(self,9850,"POINT_A"));
				}
			}
			if ( self.room.doorExists(self.room.firstStep(destination)) == true ) {
				self.comm("pick "+self.room.doorName(self.room.firstStep(destination)));
				self.comm("open "+self.room.doorName(self.room.firstStep(destination)));
			}
			self.comm(dirToText(self.room.firstStep(destination)));
			for ( var j = 0; j < self.room.people.length; j++ ) {
				var person = self.room.people[j];
				if ( person.vnum == -1 && person.quest("CHECK_Just Think...") == 0 && person.race == constants.RACE_HUMAN ) {
					person.qval("CHECK_Just Think...",1);
					setTimeout(5,afterWait,[person]);
					break;
				}
			}
		}
	}
	// if ( global.alder ) global.alder.send(self.room.name+" - ["+self.room.vnum+"]");

};


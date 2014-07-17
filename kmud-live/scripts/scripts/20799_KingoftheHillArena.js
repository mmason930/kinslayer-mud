var script20799 = function(self, actor, here, args, extra) {
	//King of the Hill
	var timer = 60;
	while (timer > 0) {
		var mainBlue = false;
		var mainRed = false;
		var mainYellow = false;
		var mainGreen = false;
		var outsideBlue = false;
		var outsideRed = false;
		var outsideYellow = false;
		var outsideGreen = false;
		var hillRoom = self.room;
		var rooms = hillRoom.neighbors;
		for each (var player in hillRoom.people) {
			if (player.vnum == -1 && !player.fighting) {
				var acuTime = getSval(player,20799,"khTime");
				if (!acuTime)
					acuTime = 0;
				setSval(player,20799,"khTime",acuTime+10);
				if (player.arenaTeam == constants.ARENA_BLUE)
					mainBlue = true;
				if (player.arenaTeam == constants.ARENA_RED)
					mainRed = true;
				if (player.arenaTeam == constants.ARENA_YELLOW)
					mainYellow = true;
				if (player.arenaTeam == constants.ARENA_GREEN)
					mainGreen = true;
			}
		}
		if (mainBlue == true)
			global.arenaCurrentGame.blueHillTimer += 10;
		if (mainRed == true)
			global.arenaCurrentGame.redHillTimer += 10;
		if (mainYellow == true)
			global.arenaCurrentGame.yellowHillTimer += 10;
		if (mainGreen == true)
			global.arenaCurrentGame.greenHillTimer += 10;
		for each (var neighbor in rooms) {
			if (neighbor) {
				for each (var person in neighbor.people) {
					if (person.vnum == -1 && !person.fighting) {
						var acuTime = getSval(person,20799,"khTime");
						if (!acuTime)
							acuTime = 0;
						setSval(person,20799,"khTime",acuTime+2);
						if (person.arenaTeam == constants.ARENA_BLUE)
							outsideBlue = true;
						if (person.arenaTeam == constants.ARENA_RED)
							outsideRed = true;
						if (person.arenaTeam == constants.ARENA_YELLOW)
							outsideYellow = true;
						if (person.arenaTeam == constants.ARENA_GREEN)
							outsideGreen = true;
					}
				}
			}
		}
		if (outsideBlue == true)
			global.arenaCurrentGame.blueHillTimer += 2;
		if (outsideRed == true)
			global.arenaCurrentGame.redHillTimer += 2;
		if (outsideYellow == true)
			global.arenaCurrentGame.yellowHillTimer += 2;
		if (outsideGreen == true)
			global.arenaCurrentGame.greenHillTimer += 2;
		/**winning the match**/
		if (global.arenaFreeForAll == true) { //FFA mode on
			for each (var player in global.arenaPlayers) {
				var pTime = getSval(player,20799,"khTime");
				pTime = Math.floor(pTime/10);
				var goal = global.arenaCurrentGame.limit*60;
				if (pTime >= goal) {//Player has met the goal, wins
					arenaEcho("Time's up! "+player.name+" has been crowned King of the Hill!");
					for each (var person in global.arenaPlayers) {
						person.send(" ");
						if (person != player) {
							addArenaPoint(person, "soloLosses");
							var hillTime = (getSval(person,20799,"khTime") ? getSval(person,20799,"khTime") : 0);
							addArenaPoint(person, "hillTime", hillTime);
						}
					}
					addArenaPoint(player, "soloWins");
					addArenaPoint(player, "hillTime", getSval(player,20799,"khTime"));
					resetArena();
					return;
				}
			}
		}
		else { //Regular, check team times
			if ( (global.arenaCurrentGame.blueHillTimer/10) >= (global.arenaCurrentGame.limit*60) )
				var winner = constants.ARENA_BLUE;
			else if ( (global.arenaCurrentGame.redHillTimer/10) >= (global.arenaCurrentGame.limit*60) )
				var winner = constants.ARENA_RED;
			else if ( (global.arenaCurrentGame.yellowHillTimer/10) >= (global.arenaCurrentGame.limit*60) )
				var winner = constants.ARENA_YELLOW;
			else if ( (global.arenaCurrentGame.greenHillTimer/10) >= (global.arenaCurrentGame.limit*60) )
				var winner = constants.ARENA_GREEN;
			if (winner) {	
				arenaEcho("Time's up! The "+getTeamName(winner)+" Team has been crowned King of the Hill!");
				for each (var person in global.arenaPlayers) {
					var hillTime = (getSval(person,20799,"khTime") ? getSval(person,20799,"khTime") : 0);	
					person.send(" ");
					if (person.arenaTeam == winner)
						addArenaPoint(person, "teamWins");
					else
						addArenaPoint(person, "teamLosses");
					addArenaPoint(person, "hillTime", hillTime);
				}
				resetArena();
				return;
			}
		}
		/**change hill location**/
		if (global.arenaFreeForAll == false)
			var timeCalc = (global.arenaCurrentGame.blueHillTimer+global.arenaCurrentGame.redHillTimer+global.arenaCurrentGame.yellowHillTimer+global.arenaCurrentGame.greenHillTimer);
		else {
			var timeCalc = 0;
			for each (var player in global.arenaPlayers) {
				var tSval = getSval(player,20799,"khTime");
				if (!tSval)
					tSval = 0;
				timeCalc += tSval;
			}
		}
		var totalTime = Math.floor(( timeCalc / 10) / 60);
		var hillTime = getSval(self,20799,"hillTime");
		if (!hillTime)
			hillTime = 0;
		if (totalTime >= (hillTime+1)) {
			self.room.echo("The rocky hill suddenly plunges back into the earth, becoming flat ground.");
			var testArr = [];
			for (var i=global.arenaCurrentMap.min;i<(global.arenaCurrentMap.max+1);i++) {
				if (!arrContains(global.arenaCurrentMap.exclusions, i))
					testArr.push(i);
			}
			var newRoom = testArr[ random(0, testArr.length-1) ];
			self.moveToRoom(getRoom(newRoom));
			self.room.echo("The ground suddenly erupts, a rocky hill climbing out of the explosion.");
			setSval(self,20799,"hillTime",hillTime+1);
		}
		timer -= 1;
		waitpulse 7;
	}
}
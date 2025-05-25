var script14219 = function(self, actor, here, args, extra) {
	var date = new Date().getDay();
	if (global.arenaStage == constants.ARENA_OFF && date == 4) {
		global.arenaStage = constants.ARENA_ON;
		return;
	}
	// if (global.arenaStage > 1 && date != 4) {
		// arenaEcho("The Arena has been closed.  Come back next week!");
		// resetArena();
		// return;
	// }
	if (!global.arenaCurrentGame || global.arenaPlayers.length == 0)
		return;
	if (global.arenaCurrentGame.timer > 0) {
		if (global.arenaCurrentGame == global.arenaGameLMS) { //LMS battle, different timer rules
			var rooms = [ getRoom(14201), getRoom(14299), getRoom(14415), getRoom(14499) ];
			var players = [];
			for (var _autoKey in rooms) {
				var room = rooms[_autoKey];
				for (var _autoKey in room.people) {
					var play = room.people[_autoKey];
					if (play.vnum == -1 && (getSval(play,20860,"respawnTime") - time()) < 10 && getSval(play,20860,"deathCount") < global.arenaCurrentGame.limit) {
						players.push([play, room]);
						getCharCols(play);
						play.send(mag+"Last Man Standing means no avoiding the battle! You will be forced to enter the Arena in 10 seconds."+nrm);
					}
				}
			}
			wait 10;
			for (var _autoKey in players) {
				var player = players[_autoKey];
				if (player[0].room == player[1] && (getSval(player[0],20860,"respawnTime") - time()) < 10) {
					player[0].comm("enter arena");
				}
			}
			return;
		}
		global.arenaCurrentGame.timer -= 1;
		if (global.arenaCurrentGame.timer == 5) {
			var min = "5 minutes";
		}
		else if (global.arenaCurrentGame.timer == 1) {
			var min = "1 minute";
		}
		else if (global.arenaCurrentGame.timer == 0) { //end game for smob/timed battle
			if (global.arenaCurrentGame == global.arenaGameSM) { //Smob battle
				if (global.arenaFreeForAll == true) { //FFA smob battle
					var winner = getSmobGameWinner("ffa");
					arenaEcho("Time's up! "+winner.name +" has won the match!");
					addArenaPoint(winner, "soloWins");
					for (var _autoKey in global.arenaPlayers) {
						var player = global.arenaPlayers[_autoKey];
						player.send(" ");
						if (player != winner)
							addArenaPoint(player, "soloLosses");
					}
					resetArena();
					return;
				}
				else { //Regular smob battle
					var winner = getSmobGameWinner("reg");
					arenaEcho("Time's up! The "+getTeamName(winner) +" Team has won the match!");
					for (var _autoKey in global.arenaPlayers) {
						var player = global.arenaPlayers[_autoKey];
						if (player.arenaTeam == winner)
							addArenaPoint(player, "teamWins");
						else 
							addArenaPoint(player, "teamLosses");
						player.send(" ");
					}
					resetArena();
					return;
				}
			}
			else if (global.arenaCurrentGame == global.arenaGameTB) { // Timed battle
				if (global.arenaFreeForAll == true) { //FFA timed battle
					var winner = getTimeGameWinner("ffa");
					addArenaPoint(winner, "soloWins");
					arenaEcho("Time's up! "+winner.name +" has won the match!");
					for (var _autoKey in global.arenaPlayers) {
						var player = global.arenaPlayers[_autoKey];
						player.send(" ");
						if (player != winner)
							addArenaPoint(player, "soloLosses");
					}
					resetArena();
					return;
				}
				else { //Regular timed battle
					var winner = getTimeGameWinner("reg");
					arenaEcho("Time's up! The "+getTeamName(winner)+" Team has won the match!");
					for (var _autoKey in global.arenaPlayers) {
						var player = global.arenaPlayers[_autoKey];
						if (player.arenaTeam == winner)
							addArenaPoint(player, "teamWins");
						else 
							addArenaPoint(player, "teamLosses");
						player.send(" ");
					}
					resetArena();
					return;
				}
			}
			return;
		}
		if (min) {
			arenaEcho("The match will end in "+min+"!");
		}
	}
	
	
	
	
	
	
}
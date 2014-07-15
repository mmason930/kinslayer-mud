var script14214 = function(self, actor, here, args, extra) {
	_block;
	var vArgs = getArgList(args);
	getCharCols(actor);
	if (getSval(actor,14214,"hostMenu") == 1) {
		if (strn_cmp(vArgs[0],"teams",1)) {
			global.arenaFreeForAll = false;
			var gameMode = "Teams";
		}
		else if (strn_cmp(vArgs[0],"free",1)) {
			global.arenaFreeForAll = true;
			var gameMode = "Free For All";
		}
		else {
			actor.send("Valid choices are "+mag+"T"+nrm+" or "+mag+"F"+nrm+". Please try again.");
			return;
		}
		actor.send("This match's mode has been set to "+gameMode+".");
		for each (var player in getWaitingPlayers("all")) {
			getCharCols(player);
			player.send(mag+"The host has set the match mode to "+gameMode+"!"+nrm);
			if (global.arenaFreeForAll == true) {
				player.arenaTeam = null;
			}
		}
		actor.send(" ");
		actor.send("Select the BETTING mode:");
		actor.send(" ");
		actor.send("["+mag+"H"+nrm+"] Host Controlled      ["+mag+"M"+nrm+"] Mixed Betting");
		setSval(actor,14214,"hostMenu",5);
		return;
	}
	else if (getSval(actor,14214,"hostMenu") == 2) {
		for (var i=0; i<global.arenaAllMaps.length; i++) {
			if (vArgs[0] == (i+1)) {
				global.arenaCurrentMap = global.arenaAllMaps[i];
				actor.send("This match's map has been set to "+global.arenaCurrentMap.name+".");
				for each (var player in getWaitingPlayers("all")) {
					getCharCols(player);
					player.send(mag+"The host has set the map to "+global.arenaCurrentMap.name+"!"+nrm);
				}
				global.arenaMinToEnter = global.arenaCurrentMap.minToEnter;
				global.arenaZoneVnum = global.arenaCurrentMap.vnum;
				setSval(actor,14214,"hostMenu",0);
				actor.detach(14214);
				return;
			}
		}
		actor.send("That is not a valid choice. Pick a map by typing the "+mag+"#"+nrm+".");
		return;
	}
	else if (getSval(actor,14214,"hostMenu") == 3) {
		if (vArgs[0] == 2 || vArgs[0] == 3 || vArgs[0] == 4) {
			global.arenaTeams = vArgs[0];
			for each (var play in getWaitingPlayers("all")) {
				getCharCols(play);
				play.send(mag+"The host has set the number of teams to "+vArgs[0]+"!"+nrm);
				if (global.arenaTeams < 4) {
					if (play.arenaTeam == constants.ARENA_GREEN) {
						play.arenaTeam = null;
						play.send("Please choose another team.");
					}
				}
				if (global.arenaTeams < 3) {
					if (play.arenaTeam == constants.ARENA_YELLOW) {
						play.arenaTeam = null;
						play.send("Please choose another team.");
					}
				}
			}
			setSval(actor,14214,"hostMenu",0);
			actor.detach(14214);
			return;
		}
		else {
			getCharCols(actor);
			actor.send("The number of teams can be "+mag+"2"+nrm+", "+mag+"3"+nrm+", or "+mag+"4"+nrm+".");
			return;
		}
	}
	else if (getSval(actor,14214,"hostMenu") == 4) {
		getCharCols(actor);
		var room = getRoom(20896);
		if (strn_cmp(vArgs[0],"done",1)) {
			setSval(actor,14214,"hostMenu",0);
			actor.detach(14214);
			actor.comm("arena");
			return;
		}
		if (!vArgs[1] || isNaN(vArgs[1]) || vArgs[1] < 1) {
			actor.detach(14214);
			getHostMenus(actor,"ai");
			return;
		}
		else if (strn_cmp(vArgs[0],"add",1)) {
			if (global.arenaFreeForAll == true) { // FFA
				if (vArgs[1] > global.arenaAllAI.length) {
					actor.send("There are only "+global.arenaAllAI.length+" more AI players available.");
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				var counter = vArgs[1];
				while (counter > 0) {
					var rand = random(0,global.arenaAllAI.length-1);
					room.loadMob(global.arenaAllAI[rand]);
					room.people[0].comm("enter arena");
					global.arenaAllAI.splice(rand,1);
					counter--;
				}
				actor.detach(14214);
				getHostMenus(actor,"ai");
				return;
			}
			else { // TEAM game
				if (strn_cmp(vArgs[2],"blue",1)) {
					var aiArr = global.arenaAIBlue;
					var color = "Blue";
				}
				else if (strn_cmp(vArgs[2],"red",1)) {
					var aiArr = global.arenaAIRed;
					var color = "Red";
				}
				else if (strn_cmp(vArgs[2],"yellow",1)) {
					var aiArr = global.arenaAIYellow;
					var color = "Yellow";
				}
				else if (strn_cmp(vArgs[2],"green",1)) {
					var aiArr = global.arenaAIGreen;
					var color = "Green";
				}
				else {
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				if (vArgs[1] > aiArr.length) {
					actor.send("There are only "+aiArr.length+" more "+color+" AI players available.");
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				if (global.arenaTeams < 3 && color == "Yellow") {
					actor.send("There is no Yellow team right now.  Change this by typing "+mag+"DONE"+nrm+", then typing "+mag+"arena teams"+nrm+".");
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				if (global.arenaTeams < 4 && color == "Green") {
					actor.send("There is no Green team right now.  Change this by typing "+mag+"DONE"+nrm+", then typing "+mag+"arena teams"+nrm+".");
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				var counter = vArgs[1];
				while (counter > 0) {
					var rand = random(0,aiArr.length-1);
					room.loadMob(aiArr[rand]);
					room.people[0].comm("enter arena");
					room.people[0].comm("arena "+color);
					aiArr.splice(rand,1);
					counter--;
				}
				actor.detach(14214);
				getHostMenus(actor,"ai");
				return;
			}
		}
		else if (strn_cmp(vArgs[0],"remove",1)) {
			if (global.arenaFreeForAll == true) {
				if (vArgs[1] > getWaitingPlayers("ai").length) {
					actor.send("There are only "+getWaitingPlayers("ai").length+" AI players in the queue.");
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				var counter = vArgs[1];
				while (counter > 0) {
					var target = getWaitingPlayers("ai")[0];
					target.comm("exit arena");
					global.arenaAllAI.push(target.vnum);
					getWaitingPlayers("ai").splice(0,1);
					target.extract();
					counter--;
				}
				actor.detach(14214);
				getHostMenus(actor,"ai");
				return;
			}
			else {
				if (strn_cmp(vArgs[2],"blue",1)) {
					var aiArr = global.arenaAIBlue;
					var color = "Blue";
				}
				else if (strn_cmp(vArgs[2],"red",1)) {
					var aiArr = global.arenaAIRed;
					var color = "Red";
				}
				else if (strn_cmp(vArgs[2],"yellow",1)) {
					var aiArr = global.arenaAIYellow;
					var color = "Yellow";
				}
				else if (strn_cmp(vArgs[2],"green",1)) {
					var aiArr = global.arenaAIGreen;
					var color = "Green";
				}
				else {
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				var teamAI = [];
				for each (var play in getWaitingPlayers("ai")) {
					if (getTeamName(play.arenaTeam) == color)
						teamAI.push(play);
				}
				if (vArgs[1] > teamAI.length) {
					actor.send("There are only "+teamAI.length+" "+color+" AI players in the queue.");
					actor.detach(14214);
					getHostMenus(actor,"ai");
					return;
				}
				var counter = vArgs[1];
				while (counter > 0) {
					//sendKoradin(teamAI);
					var target = teamAI[0];
					target.comm("exit arena");
					aiArr.push(target.vnum);
					teamAI.splice(0,1);
					target.extract();
					counter--;
				}
				actor.detach(14214);
				getHostMenus(actor,"ai");
				return;
			}
		}
		else {
			actor.detach(14214);
			getHostMenus(actor,"ai");
			return;
		}
	}
	else if (getSval(actor,14214,"hostMenu") == 5) {
		if (strn_cmp(vArgs[0],"host",1)) {
			global.arenaMixedBets = false;
			var gameMode = "Host-Controlled";
		}
		else if (strn_cmp(vArgs[0],"mixed",1)) {
			global.arenaMixedBets = true;
			var gameMode = "Mixed Betting";
		}
		else {
			actor.send("Valid choices are "+mag+"H"+nrm+" or "+mag+"M"+nrm+". Please try again.");
			return;
		}
		actor.send("This match's betting mode has been set to "+gameMode+".");
		for each (var player in getWaitingPlayers("all")) {
			getCharCols(player);
			player.send(mag+"The host has set the betting mode to "+gameMode+"!"+nrm);
		}
		global.arenaCurrentWagers = [];
		arenaEcho("All wagers have been erased.");
		setSval(actor,14214,"hostMenu",0);
		actor.detach(14214);
	}
}
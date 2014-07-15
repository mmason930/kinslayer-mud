var script14211 = function(self, actor, here, args, extra) {
	//EXIT ARENA - global exit by players
	//Koradin - June 2011
	var vArgs = getArgList(args);
	getCharCols(actor);
	if (!vArgs[1] || vArgs[2]) {
		_noblock;
		return;
	}
	if (str_cmp(vArgs[1],"arena")) { //Not exiting arena
		_noblock;
		return;
	}
	_block;
	if (!arrContains(global.arenaPlayers, actor) && getSval(actor,14210,"arenaQueue") != 1 && !arrContains(global.arenaZones, actor.room.zoneVnum) && !isAI(actor)) { //Actor is not in the arena, in the queue, or in graveyard
		actor.send("You're not in the Arena!")
		return;
	}
	if (getSval(actor,14210,"arenaQueue") == 1) {
		actor.removeFromArenaQueue();
		act("$n leaves the queue for the Arena.",true,actor,null,null,constants.TO_ROOM);
		return;
	}
	if (arrContains(global.arenaPlayers, actor)) { //Actor is in the arena
		getCharCols(actor);
		for each(var thing in actor.inventory) {
			if (arrContains(global.arenaItems, thing.vnum)) {
				if (arrContains(global.arenaZones, actor.room.zoneVnum))
					actor.comm("drop "+thing.namelist.split(" ")[0]);
				else
					thing.moveToRoom(getRoom(20899));
			}
		}
		if (actor == global.arenaKickPlayer)
			var toMsg = "You have been kicked out of the match!";
		else
			var toMsg = "You leave the Arena.";
		actor.send(mag+toMsg+nrm);
		if (global.arenaStage == constants.ARENA_PLAYING) {
			if (global.arenaKickPlayer == actor) //Player was kicked
				addArenaPoint(actor, "bails", false);
			else if (arrContains(global.arenaPlayers, actor) && global.arenaPlayers.length >= (global.arenaTotalPlayers*0.8) && arrContains(global.arenaZones, actor.room.zoneVnum)) //Player was playing in a match and there are still 80% of the people left, add bail
				addArenaPoint(actor, "bails", true);
		}
		for (var i=0; i<global.arenaPlayers.length;i++) { //Remove player from arena players
			if (global.arenaPlayers[i] == actor)
				global.arenaPlayers.splice(i, 1); //Remove player from Arena players
		}
		actor.arenaTeam = null;
		setSval(actor,14210,"arenaQueue",0);
		setSval(actor,20860,"killCount",0);
		setSval(actor,20860,"deathCount",0);
		setSval(actor,20860,"assistCount",0);
		setSval(actor,20799,"khTime",0);
		setSval(actor,20930,"flagCaps",0);
		setSval(actor,20930,"flagRets",0);
		setSval(actor,20860,"smobKills",0);
		setSval(actor,20860,"doubleKills",0);
		setSval(actor,20860,"tripleKills",0);
		setSval(actor,20860,"arenaScore",0);
		setSval(actor,20860,"respawnTime",0);
		setSval(actor,14200,"kickRequest",0);
		setSval(actor,14223,"kickVote",0);
		setSval(actor,14224,"endVote",0);
		setSval(actor,14209,"endGame",0);
		setSval(actor,14209,"arenaIssue",0);
		setSval(actor,14212,"votePaging",0);
		setSval(actor,14209,"gameStart",0);
		setSval(actor,14209,"wagerExp",0);
		setSval(actor,14209,"wagerWps",0);
		setSval(actor,14209,"wagerFps",0);
		setSval(actor,14209,"wagerItem",0);
		setSval(actor,14209,"wagerMenu",0);
		actor.detach(14212);
		actor.detach(14214);
		actor.detach(14215);
		actor.detach(14222);
		actor.detach(14223);
		actor.detach(14224);
		actor.detach(14228);
		actor.detach(14234);
		if (actor.race == constants.RACE_TROLLOC)
			var nextRoom = 502;
		else //if (actor.race == constants.RACE_HUMAN)
			var nextRoom = 500;
		var room = actor.room;
		actor.moveToRoom(getRoom(nextRoom));
		if (actor == global.arenaKickPlayer)
			var kMsg = " has been kicked out of the Arena match!";
		else
			var kMsg = " has left the Arena!";
		arenaEcho(actor.name+kMsg);
		if (global.arenaCurrentGame == global.arenaGameLMS) {
			if (global.arenaCurrentGame.totalPlayers > global.arenaPlayers.length)
				global.arenaCurrentGame.totalPlayers -= 1;
			if (global.arenaCurrentGame.totalPlayers == 1) { //winning the game
				for each (var player in global.arenaPlayers) {
					if (getSval(player,20860,"deathCount") < global.arenaCurrentGame.limit)
						arenaEcho(player.name+" is the Last Man Standing!");
				}
				resetArena();
				return;
			}
		}
		actor.comm("look");
		var count = 0;
		for each (var play in global.arenaPlayers) {
			if (!isAI(play))
				count += 1;
		}
		if (count == 0) // No one left in Arena
			resetArena();
	}
	
	
	
	
}
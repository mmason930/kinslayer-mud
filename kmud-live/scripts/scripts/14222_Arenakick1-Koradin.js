var script14222 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	getCharCols(actor);
	if (strn_cmp(vArgs[0], "cancel", 1)) {
		actor.send("You cancel the kick request.");
		actor.detach(14222);
		return;
	}
	else if (vArgs[0] < 1 || vArgs[0] > global.arenaPlayers.length || isNaN(vArgs[0])) {
		actor.send(red+"That is not a valid option."+nrm);
		actor.send(" ");
		actor.send("Which player do you want to kick?");
		actor.send("______________________________________");
		actor.send(" ");
		for (i=0;i<global.arenaPlayers.length;i++) {
			actor.send(mag+(i+1)+nrm+") "+global.arenaPlayers[i].name);
		}
		actor.send("______________________________________");
		actor.send(" ");
		actor.send("["+mag+"#"+nrm+"] Select Player      ["+mag+"C"+nrm+"] Cancel");
		actor.send(" ");
		return;
	}
	for (i=0; i<global.arenaPlayers.length;i++) {
		if (vArgs[0] == (i+1)) {
			var kicked = global.arenaPlayers[i];
			break;
		}
	}
	actor.detach(14222);
	setSval(actor,14200,"kickRequest",1);
	setSval(actor,14223,"kickVote", 1);
	arenaEcho("Someone has requested to kick "+kicked.name+"!");
	global.arenaKickPlayer = kicked;
	global.arenaKickYes += 1;
	global.arenaKickTimer = time();
	putMobsToSleep(true);
	startArenaTimer(15, "Kick Player");
	for each (var player in global.arenaPlayers) {
		getCharCols(player);
		player.stopFighting();
		player.send(" ");
		player.attach(14223);
		if (player == actor)
			continue;
		if (player == kicked)
			player.send("Please wait while your fate is decided.");
		else {
			player.send("Do you want to kick "+kicked.name+"?");
			player.send(" ");
			player.send("["+mag+"Y"+nrm+"] Yes      ["+mag+"N"+nrm+"] No");
		}
	}
}
var script14233 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	_block;
	if (!vArgs[1]) {
		actor.send("Use Arena Team Chat to communicate with other players on your team: 'tchat On south of you!'");
		return;
	}
	if (!actor.arenaTeam) {
		getCharCols(actor);
		actor.send("You're not part of an Arena Team right now.");
		actor.send(" ");
		actor.send("Type "+mag+"enter arena"+nrm+" to join the battle!");
		return;
	}
	vArgs.splice(0, 1);
	vArgs = vArgs.join(" ");
	var aName = actor.name;
	var allPlayers = [];
	for (var _autoKey in global.arenaPlayers) {
		var player = global.arenaPlayers[_autoKey];
		if (player.arenaTeam == actor.arenaTeam)
			allPlayers.push(player);
	}
	for (var _autoKey in getConnectedPlayers()) {
		var imm = getConnectedPlayers()[_autoKey];
		if (imm.level > 50)
			allPlayers.push(imm);
	}
	for (var _autoKey in allPlayers) {
		var person = allPlayers[_autoKey];
		getCharCols(person);
		if (actor.arenaTeam == constants.ARENA_BLUE)
			var color = cyn;
		else if (actor.arenaTeam == constants.ARENA_RED)
			var color = red;
		else if (actor.arenaTeam == constants.ARENA_YELLOW)
			var color = yel;
		else if (actor.arenaTeam == constants.ARENA_GREEN)
			var color = grn;
		person.send(color+"[ "+aName+" ] : "+vArgs+nrm);
	}
}
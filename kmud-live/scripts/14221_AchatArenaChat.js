var script14221 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
_block;
if (!vArgs[1]) {
	actor.send("Use Arena Chat to communicate with other players in the Arena: 'achat What a kill!'");
	return;
}
if (!arrContains(global.arenaPlayers, actor) && !arrContains(getWaitingPlayers("all"), actor) && !arrContains(global.arenaWatchers, actor)) {
	getCharCols(actor);
	actor.send("You're not playing, watching, or in the queue for an Arena match right now.");
	actor.send(" ");
	actor.send("Type "+mag+"enter arena"+nrm+" to join the battle!");
	return;
}
vArgs.splice(0, 1);
vArgs = vArgs.join(" ");
var aName = actor.name;
if (arrContains(global.arenaWatchers, actor))
	aName = "Observer";
if (global.arenaStage != constants.ARENA_PLAYING) {
	if (global.arenaFreeForAll == true)
		aName = "Someone";
	else {
		if (actor.arenaTeam == constants.ARENA_BLUE)
			aName = "Blue Player";
		else if (actor.arenaTeam == constants.ARENA_RED)
			aName = "Red Player";
		else if (actor.arenaTeam == constants.ARENA_YELLOW)
			aName = "Yellow Player";
		else if (actor.arenaTeam == constants.ARENA_GREEN)
			aName = "Green Player";
		else
			aName = "Someone";
	}
	if (actor == global.arenaHost)
		aName = "Host";
}
arenaEcho("[ "+aName+" ] : "+vArgs);

};


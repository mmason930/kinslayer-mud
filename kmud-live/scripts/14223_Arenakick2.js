var script14223 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
getCharCols(actor);
_block;
if (actor == global.arenaKickPlayer) {
	actor.send("Please wait while your fate is decided.");
	return;
}
else if (getSval(actor,14223,"kickVote") == 1) {
	actor.send("You have already voted! Please wait.");
	return;
}
if (strn_cmp(vArgs[0], "yes", 1)) {
	arenaEcho("Someone has voted in favor of kicking "+global.arenaKickPlayer.name+"!");
	global.arenaKickYes += 1;
	setSval(actor,14223,"kickVote",1);
	if ((global.arenaKickYes+global.arenaKickNo) == (global.arenaPlayers.length-1)) { //Everyone has voted
		if (global.arenaKickYes > global.arenaKickNo)
			kickArenaPlayer(true);
		else
			kickArenaPlayer(false);
		return;
	}
	else if (global.arenaKickYes > ((global.arenaPlayers.length-1) / 2)) { // Yes has the majority regardless of how many votes are left
		kickArenaPlayer(true);
	}
	return;
}
else if (strn_cmp(vArgs[0], "no", 1)) {
	arenaEcho("Someone has voted not to kick "+global.arenaKickPlayer.name+"!");
	global.arenaKickNo += 1;
	setSval(actor,14223,"kickVote",1);
	if ((global.arenaKickYes+global.arenaKickNo) == (global.arenaPlayers.length-1)) { //Everyone has voted
		if (global.arenaKickYes > global.arenaKickNo)
			kickArenaPlayer(true);
		else
			kickArenaPlayer(false);
		return;
	}
	else if (global.arenaKickNo > ((global.arenaPlayers.length-1) / 2)) { // No has the majority regardless of how many votes are left
		kickArenaPlayer(false);
	}
	return;
}
else {
	actor.send(" ");
	actor.send(red+"That is not a valid choice."+nrm);
	actor.send(" ");
	actor.send("Do you want to kick "+kicked.name+"?");
	actor.send(" ");
	actor.send("["+mag+"Y"+nrm+"] Yes      ["+mag+"N"+nrm+"] No");
	return; 
}

};


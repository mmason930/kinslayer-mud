var script14224 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
getCharCols(actor);
_block;
if (getSval(actor,14224,"endVote") == 1) {
	actor.send("You have already voted! Please wait.");
	return;
}
if (strn_cmp(vArgs[0], "yes", 1)) {
	arenaEcho("Someone has voted in favor of ending the game!");
	global.arenaEndYes += 1;
	setSval(actor,14224,"endVote",1);
	sendKoradin("players: "+global.arenaPlayers.length+" yes: "+global.arenaEndYes+" no: "+global.arenaEndNo);
	if ((global.arenaEndYes+global.arenaEndNo) == global.arenaPlayers.length) { //Everyone has voted
		if (global.arenaEndYes == global.arenaPlayers.length) {
			arenaEcho("The match has ended!");
			resetArena();
		}
		else {
			function afterWait(args) {
				arenaEcho("The vote is not unanimous - let the game go on!");
				global.arenaEndYes = 0;
				global.arenaEndNo = 0;
				global.arenaEndTimer = 0;
				putMobsToSleep(false);
				for each (var player in global.arenaPlayers) {
					setSval(player, 14224, "endVote", 0);
					player.detach(14224);
				}
			}
			setTimeout(1, afterWait, [actor]);
		}
		return;
	}
	return;
}
else if (strn_cmp(vArgs[0], "no", 1)) {
	arenaEcho("Someone has voted not to end the game!");
	function afterWait(args) {
		arenaEcho("The vote is not unanimous - let the game go on!");
		global.arenaEndYes = 0;
		global.arenaEndNo = 0;
		global.arenaEndTimer = 0;
		putMobsToSleep(false);
		for each (var player in global.arenaPlayers) {
			setSval(player, 14224, "endVote", 0);
			player.detach(14224);
		}
	}
	setTimeout(1, afterWait, [actor]);
	return;
}
else {
	actor.send(" ");
	actor.send(red+"That is not a valid choice."+nrm);
	actor.send(" ");
	actor.send("Do you want to end the match?");
	actor.send(" ");
	actor.send("["+mag+"Y"+nrm+"] Yes      ["+mag+"N"+nrm+"] No");
	return; 
}

};


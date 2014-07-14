var script14213 = function(self, actor, here, args, extra)
{
	//Random arena updates
//Koradin - June 2011
var arenaPlayers = getWaitingPlayers("pc"); //Players are in the queue/voting
var stage = global.arenaStage;
if (arenaPlayers.length == 0) {
	arenaPlayers = global.arenaPlayers;  //Players are in the arena
}
if (!arenaPlayers || arenaPlayers.length == 0) { // no players waiting/voting/in arena
	return;
}
if (stage == constants.ARENA_QUEUE) { // QUEUE STAGE
	for each (var player in getWaitingPlayers("all")) {
		getCharCols(player);
		if (player.affectedBy(constants.AFF_NOQUIT)) { //Can't enter with NOQUIT
			player.send("You haven't calmed down, and are removed from the Arena queue.");
			player.removeFromArenaQueue();
			return;
		}
	}
	return;
}
else if (stage == constants.ARENA_VOTING) { // VOTING STAGE
	var timeLeft = global.arenaVotingTimeLimit;
	if (timeLeft > 1)
		var minuteS = "minutes";
	else
		var minuteS = "minute";
	var votesLeft = arenaPlayers.length - global.arenaTotalVotes;
		if (votesLeft == 1)
			var voteS = "vote has";
		else
			var voteS = "votes have";
	if (timeLeft > 0) {
		for each (var player in getWaitingPlayers("all")) {
			getCharCols(player);
			player.send("The Arena match will begin when "+mag+votesLeft+nrm+" more "+voteS+" been cast, ");
			player.send("or in "+mag+timeLeft+nrm+" "+minuteS+", whichever comes first.");
		}
		global.arenaVotingTimeLimit -= 1;
		return;
	}				
	else {
		for each (var player in getWaitingPlayers("all")) {
			getCharCols(player);
			player.send(mag+"The time allowed to vote has passed!"+nrm);
		}
		var gameType = getArenaGameType(global.arenaAllGames);
		gameType.limit = Math.floor(gameType.limit / gameType.votes);
		startArenaMatch(gameType);
		return;
	}
}


};


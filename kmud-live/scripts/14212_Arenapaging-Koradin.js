var script14212 = function(self, actor, here, args, extra)
{
	//Arena menu paging script
//Koradin - June 2011
var vArgs = getArgList(args);
getCharCols(actor);
var arenaPlayers = getWaitingPlayers("pc");
if (getSval(actor,14209,"gameStart") == 1) {
	actor.send("Waiting to begin the match...");
	_block;
	return;
}
if (strn_cmp(vArgs[0],"exit",1)) {
	_block;
	actor.removeFromArenaQueue();
	return;
}
else if (strn_cmp(vArgs[0],"arena",3)) {
	_noblock;
	return;
}
_block;
var vPaging = getSval(actor,14212,"votePaging");
if (vPaging > 0) { // second stage of voting, picking limits
	if (strn_cmp(vArgs[0],"back",1)) {
		actor.send(" ");
		setSval(actor,14212,"votePaging",0);
		getArenaStartMenu(actor);
		return;
	}
	if (isNaN(vArgs[0]) || vArgs[0] < 1 || vArgs[0] > 4) {
		actor.send("That is not a valid command.");
		actor.send(" ");
		actor.send("Vote by selecting a "+mag+"#"+nrm+", type "+mag+"arena"+nrm+" to see current votes, or type "+mag+"exit"+nrm+" to leave the Arena.");
		getArenaVoteMenu(actor, vPaging-1);
		return;
	}
	if (vPaging == 1) { //DM
		var newVote = global.arenaGameDM.limits[vArgs[0]-1];
		var choice = "Death Match - "+newVote;
		for (var i=0;i<global.arenaAllGames.length;i++) {
			if (global.arenaAllGames[i] == global.arenaGameDM) {
				var gameVote = i;
				break;
			}
		}
		global.arenaTotalVotes += 1;
		global.arenaGameDM.votes += 1;
		if (global.arenaGameDM.votes > 1)
			global.arenaGameDM.limit += newVote;
		else
			global.arenaGameDM.limit = newVote;
	}
	else if (vPaging == 2) { //TB
		var newVote = global.arenaGameTB.limits[vArgs[0]-1];
		var choice = "Timed Battle - "+newVote;
		for (var i=0;i<global.arenaAllGames.length;i++) {
			if (global.arenaAllGames[i] == global.arenaGameTB) {
				var gameVote = i;
				break;
			}
		}
		global.arenaTotalVotes += 1;
		global.arenaGameTB.votes += 1;
		if (global.arenaGameTB.votes > 1)
			global.arenaGameTB.limit += newVote;
		else
			global.arenaGameTB.limit = newVote;
	}
	else if (vPaging == 3) { //CTF
		var newVote = global.arenaGameCTF.limits[vArgs[0]-1];
		var choice = "Capture the Flag - "+newVote;
		for (var i=0;i<global.arenaAllGames.length;i++) {
			if (global.arenaAllGames[i] == global.arenaGameCTF) {
				var gameVote = i;
				break;
			}
		}
		global.arenaTotalVotes += 1;
		global.arenaGameCTF.votes += 1;
		if (global.arenaGameCTF.votes > 1)
			global.arenaGameCTF.limit += newVote;
		else
			global.arenaGameCTF.limit = newVote;
	}
	else if (vPaging == 4) { //KotH
		var newVote = global.arenaGameKH.limits[vArgs[0]-1];
		var choice = "King of the Hill - "+newVote;
		for (var i=0;i<global.arenaAllGames.length;i++) {
			if (global.arenaAllGames[i] == global.arenaGameKH) {
				var gameVote = i;
				break;
			}
		}
		global.arenaTotalVotes += 1;
		global.arenaGameKH.votes += 1;
		if (global.arenaGameKH.votes > 1)
			global.arenaGameKH.limit += newVote;
		else
			global.arenaGameKH.limit = newVote;
	}
	else if (vPaging == 5) { //LMS
		var newVote = global.arenaGameLMS.limits[vArgs[0]-1];
		var choice = "Last Man Standing - "+newVote;
		for (var i=0;i<global.arenaAllGames.length;i++) {
			if (global.arenaAllGames[i] == global.arenaGameLMS) {
				var gameVote = i;
				break;
			}
		}
		global.arenaTotalVotes += 1;
		global.arenaGameLMS.votes += 1;
		if (global.arenaGameLMS.votes > 1)
			global.arenaGameLMS.limit += newVote;
		else
			global.arenaGameLMS.limit = newVote;
	}
	else if (vPaging == 6) { //SM
		var newVote = global.arenaGameSM.limits[vArgs[0]-1];
		var choice = "SuperMOBS - "+newVote;
		for (var i=0;i<global.arenaAllGames.length;i++) {
			if (global.arenaAllGames[i] == global.arenaGameSM) {
				var gameVote = i;
				break;
			}
		}
		global.arenaTotalVotes += 1;
		global.arenaGameSM.votes += 1;
		if (global.arenaGameSM.votes > 1)
			global.arenaGameSM.limit += newVote;
		else
			global.arenaGameSM.limit = newVote;
	}
	actor.send(mag+"You voted for "+choice+"!"+nrm)
	setSval(actor,14212,"arenaVote",gameVote);
	setSval(actor,14212,"votePaging",0);
	global.arenaPlayers.push(actor);
	for each (var person in arenaPlayers) {
		if (person != actor) {
			if (actor.race != person.race)
				var playerName = "Someone";
			else
				var playerName = "$n";
			act(mag+playerName+" has voted for "+choice+"!"+nrm,true,actor,null,person,constants.TO_VICT);
		}
		if (global.arenaTotalVotes < arenaPlayers.length) {
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
			person.send(" ");
			person.send("The Arena match will begin when "+mag+votesLeft+nrm+" more "+voteS+" been cast, ");
			person.send("or in "+mag+timeLeft+nrm+" "+minuteS+", whichever comes first.");
		}
	}
	actor.detach(14212);
	if (global.arenaTotalVotes == getWaitingPlayers("pc").length) {
		var gameType = getArenaGameType(global.arenaAllGames);
		gameType.limit = Math.floor(gameType.limit / gameType.votes);
		for each (var ai in getWaitingPlayers("ai"))
			global.arenaPlayers.push(ai);
		startArenaMatch(gameType);
	}
	return;
}
if (isNaN(vArgs[0]) || vArgs[0] < 1 || vArgs[0] > 7) {
	actor.send("That is not a valid command.");
	actor.send(" ");
	actor.send("Vote by selecting a "+mag+"#"+nrm+", type "+mag+"arena"+nrm+" to see current votes, or type "+mag+"exit"+nrm+" to leave the Arena.");
	getArenaStartMenu(actor);
	return;
}
for (j=0; j<global.arenaAllGames.length; j++) {
	if (vArgs[0] == (j+1)) {
		if (global.arenaAllGames[j] == global.arenaGameCTF && global.arenaFreeForAll == true) {
			actor.send(red+"Capture the Flag matches are available in Teams Mode only."+nrm);
			getArenaStartMenu(actor);
			return;
		}
		if (global.arenaAllGames[j] == global.arenaGameLMS && global.arenaFreeForAll == false) {
			actor.send(red+"Last Man Standing matches are available in Free For All Mode only."+nrm);
			getArenaStartMenu(actor);
			return;
		}
		getArenaVoteMenu(actor, j);
		setSval(actor,14212,"votePaging",(j+1));
		return;
	}
}
if (vArgs[0] == 7) { // Random game
	var rand = random(0,5);
	getArenaVoteMenu(actor, rand);
	setSval(actor,14212,"votePaging",(rand+1));
	return;
}


};


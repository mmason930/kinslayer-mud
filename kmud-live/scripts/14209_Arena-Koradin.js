var script14209 = function(self, actor, here, args, extra)
{
	//ARENA command for imms/players
//Koradin - June 2011
var vArgs = getArgList(args);
_block;
if (actor.level < global.arenaImm || arrContains(global.arenaWatchers, actor)) {
	getCharCols(actor);
	if (vArgs[1]) {
		if (strn_cmp(vArgs[1],"help",3)) {
			actor.send("You can interact with the Arena using several commands:");
			actor.send(" ");
			actor.send(mag+"arena"+nrm+"          :  shows a variety of helpful Arena information");
			actor.send("                  (use frequently before, during, and after matches)");
			actor.send(" ");
			actor.send(mag+"arena watch"+nrm+"    :  allows you to see Arena messages and stats");
			actor.send("                  (use during a match you're not playing, type again to stop watching)");
			actor.send(" ");			
			actor.send(mag+"enter arena"+nrm+"    :  places you in the queue for a new match");
			actor.send("                  (also allows you to rejoin the match if you die or idle out)");
			actor.send(" ");
			actor.send(mag+"exit arena"+nrm+"     :  lets you leave the queue or Arena match");
			actor.send("                  (note: leaving a match early negatively affects your score)");
			actor.send(" ");
			actor.send(mag+"arena <color>"+nrm+"  :  places you on an Arena team");
			actor.send("                  (choices are RED, BLUE, YELLOW, GREEN)");
			actor.send(" ");
			actor.send(mag+"arena issue"+nrm+"    :  gives you needed items / support during a match");
			actor.send("                  (useable at your team's Home Base only)");
			actor.send(" ");
			actor.send(mag+"arena kick"+nrm+"     :  request to kick a player from the match");
			actor.send("                  (you only get 1 kick request per match - use it wisely!)");
			actor.send(" ");
			actor.send(mag+"kill arena"+nrm+"     :  targets anyone (ffa) or anyone not on your team (teams)");
			actor.send("                  (kill <color> also works for targeting specific teams)");
			actor.send(" ");
			actor.send(mag+"achat <msg>"+nrm+"    :  allows you to communicate with the other Arena players");
			actor.send("                  (you can also use TCHAT to speak with just your Team)");
			actor.send(" ");
			actor.send(mag+"arena bet"+nrm+"      :  wagers a bet on the outcome of the Arena match");
			actor.send("                  (host-controlled or free betting by all)");
			actor.send(" ");
			actor.send(mag+"arena ranks"+nrm+"    :  shows your current Arena ranking situation");
			actor.send(" ");
			actor.send(mag+"arena legends"+nrm+"  :  shows a list of the top Arena players");
			actor.send(" ");
			return;
		}
		else if (strn_cmp(vArgs[1],"ranks",3)) {
			actor.send(" ");
			var ranks = ["Unranked","Conscript","Scout","Soldier","Second Squadman","Squadman","File Leader","Patrolman","Patrol Leader","Second Bannerman","Bannerman","Hundredman","Second Sergeant","Sergeant","First Sergeant","Under-Lieutenant","Lieutenant","Senior Lieutenant","Second Captain","Captain","First Captain","Lord Captain","General","Banner-General","Lieutenant-General","Captain-General","Marshal-General","Lord General","King","King of Kings","Godlike"];
			for (i=0;i<ranks.length;i++) {
				if (ranks[i] == getArenaRank(actor)) {
					if (i>0)
						var pastRank = ranks[i-1];
					var currentRank = ranks[i];
					var nextRank = ranks[i+1];
					var farRank = ranks[i+2];
					break;
				}
			}
			if (pastRank) {
				actor.send("You were recently promoted from "+mag+pastRank+nrm+".");
				actor.send(" ");
			}
			actor.send("Your current rank is "+mag+currentRank+nrm+".");
			actor.send(" ");
			if (nextRank) {
				actor.send("You need "+mag+(getArenaScoreRange(nextRank)[0]-getArenaScore(actor))+nrm+" points to be promoted to "+mag+nextRank+nrm+".");
				actor.send(" ");
				if (farRank)
					actor.send("With more experience, you could be promoted to "+mag+farRank+nrm+" after that.");
			}
			else {
				actor.send("You have reached the top rank available in the Arena!");
			}
			return;
		}
		else if (strn_cmp(vArgs[1],"legends",3)) {
			var arenaLegends = [];
			var queryText = "SELECT u.username, u.last_logon, p.* FROM pvals p JOIN users u ON u.user_id = p.owner_id WHERE p.sKey = 'arenaStats4' AND p.value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~' AND p.value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0'"; //AND u.last_logon > '2011-08-18 16:30:00' AND u.last_logon <= '2011-08-18 22:30:00' ";
			var queryResult = sqlQuery(queryText);
			while( queryResult.hasNextRow ) {
				var row = queryResult.getRow;
				var score = row.get("value");
				var actorName = row.get("username");
				score = score.split("~");
				var totalScore = 0;
				for each (var point in score) {
					totalScore += parseInt(point);
				}
				if (totalScore > 0) {
					var rankName = score[score.length-1];
					arenaLegends.push([actorName, rankName, totalScore]);
				}
			}
			function compare(a, b) {
				return b[2] - a[2];
			}
			arenaLegends.sort(compare);
			var legendList = [["Unknown",0,0],["Unknown",0,0],["Unknown",0,0],["Unknown",0,0],["Unknown",0,0]];
			for each (var legend in arenaLegends) {
				alreadyListed = false;
				var score = legend[2];
				for (i=0; i<legendList.length; i++) {
					if (score > legendList[i][2] && alreadyListed == false) {
						legendList.splice(i,1,legend);
						alreadyListed = true;
					}
				}
			}
			global.arenaLegends = [];
			actor.send(" ");
			actor.send(mag+"           Arena Legends"+nrm);
			actor.send(" ");
			actor.send("Over the last seven matches played...");
			actor.send(" ");
			for (j=0; j<legendList.length; j++) {
				global.arenaLegends.push(legendList[j][0]);
				actor.send("~ "+(j+1)+". "+strPadding(legendList[j][0]," ",16,"right")+ "  ( "+legendList[j][2]+ " points )");
				actor.send("      - scored "+legendList[j][1]+" points last match.");
				actor.send(" ");
			}
			updateArenaLegendTitle(actor);
			return;
		}
		else if (strn_cmp(vArgs[1],"watch",3) || strn_cmp(vArgs[1],"observe",3)) {
			if (arrContains(global.arenaPlayers, actor) || arrContains(getWaitingPlayers("all"), actor)) {
				actor.send("You can't observe if you're one of the players!");
				return;
			}
			else if (arrContains(global.arenaWatchers, actor)) {
				actor.send("You stop watching the Arena match.");
				act("$n stops watching an Arena match.",true,actor,null,null,constants.TO_ROOM);
				for (i=0;i<global.arenaWatchers.length;i++) {
					if (global.arenaWatchers[i] == actor)
						global.arenaWatchers.splice(i, 1);
				}
				return;
			}
			else if (global.arenaStage < constants.ARENA_PLAYING) {
				actor.send("There's no match to watch right now!");
				return;
			}
			actor.send("You begin watching the Arena match.");
			act("$n begins watching an Arena match.",true,actor,null,null,constants.TO_ROOM);
			global.arenaWatchers.push(actor);
			return;
		}
		else if (strn_cmp(vArgs[1],"mode",3)) {
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only set the Arena mode while waiting in the Queue!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			getHostMenus(actor, "mode");
			return;
		}
		else if (strn_cmp(vArgs[1],"teams",3)) {
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only set the Arena teams while waiting in the Queue!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			getHostMenus(actor, "teams");
			return;
		}
		else if (strn_cmp(vArgs[1],"map",3)) {
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only set the Arena map while waiting in the Queue!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			getHostMenus(actor, "map");
			return;
		}
		else if (strn_cmp(vArgs[1],"ai",2)) {
			//actor.send("Coming soon!");
			if (global.arenaFreeForAll == false) {
				actor.send("AI can only be added during Free For All matches right now.");
				return;
			}
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only set the Arena AI while waiting in the Queue!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			getHostMenus(actor, "ai");
			return;
		}
		else if (strn_cmp(vArgs[1],"start",3)) { //Host starts the voting process
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only start the voting process while waiting in the Queue!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			if (isMatchReady() == false) {
				actor.send(isMatchReady("reason"));
				actor.send(" ");
				actor.send("You're not ready for the voting stage yet!  Type "+mag+"arena"+nrm+" for more information.");
				return;
			}
			for each (var person in getConnectedPlayers()) {
				getCharCols(person);
				person.send(mag+"An Arena match will start in 10 seconds!"+nrm);
				if (!arrContains(getWaitingPlayers("all"), person)) {
					person.send(" ");
					person.send("Type "+mag+"enter arena"+nrm+" to join the battle!");
				}
			}
			for each (var player in getWaitingPlayers("all")) {
				player.attach(14212);
				setSval(player,14209,"gameStart",1);
			}
			waitpulse 70;
			for each (var player in getWaitingPlayers("all")) {
				getCharCols(player);
				setSval(player,14209,"gameStart",0);
				if (isMatchReady() == true) {
					getArenaStartMenu(player);
					if (player.countJS(14212) == 0)
						player.attach(14212);
				}
				else {
					player.send(mag+"The match is no longer ready to begin!"+nrm);
					player.detach(14212);
					if (player == global.arenaHost) {
						player.send(" ");
						player.send(isMatchReady("reason"));
					}
				}
			}
			if (isMatchReady() == true)
				setArenaStage(constants.ARENA_VOTING);
			return;
		}
		else if (strn_cmp(vArgs[1],"time",3)) {
			if (global.arenaStage != constants.ARENA_VOTING) {
				actor.send("You can only ask for more voting time during the Voting phase!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			for each (var player in getWaitingPlayers("all")) {
				getCharCols(player);
				player.send(mag+"The host has provided more time to vote!"+nrm);
			}
			global.arenaVotingTimeLimit = 3;
		}
		else if (strn_cmp(vArgs[1],"end",3)) {
			if (global.arenaStage != constants.ARENA_PLAYING) {
				actor.send("You can only request to end a match you're currently playing!");
				return;
			}
			if (global.arenaHost != actor) {
				actor.send("You are not the current host of the Arena!");
				return;
			}
			var endTime = getSval(actor, 14209, "endGame");
			if (!endTime)
				endTime = 0;
			if (time() - endTime < 300) { // Can only request to end game every 5 min
				actor.send("You can only request to end the match once every five minutes!");
				return;
			}
			setSval(actor, 14209, "endGame", time());
			arenaEcho("The host has requested to end the match!");
			setSval(actor,14224,"endVote", 1);
			global.arenaEndYes += 1;
			global.arenaEndTimer = time();
			putMobsToSleep(true);
			startArenaTimer(15, "End Match");
			for each (var player in global.arenaPlayers) {
				getCharCols(player);
				player.stopFighting();
				player.attach(14224);
				if (player != actor) {
					player.send(" ");
					player.send("Do you want to end the match?");
					player.send(" ");
					player.send("["+mag+"Y"+nrm+"] Yes   ["+mag+"N"+nrm+"] No");
				}
			}
			return;
		}
		// Joining TEAMS
		else if (strn_cmp(vArgs[1],"blue",2)) {
			if (global.arenaFreeForAll == true) {
				actor.send("The current match mode is Free For All - there is no Blue Team!");
				return;
			}
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only join a team during the Queue phase!");
				return;
			}
			if (actor.arenaTeam) {
				if (actor.arenaTeam == constants.ARENA_BLUE) {
					actor.send("You're already on the Blue Team!");
					return;
				}
				for each (var player in getWaitingPlayers("all")) {
					getCharCols(player);
					if (isAI(player))
						var aName = "(AI) Player";
					else if (player.race == actor.race)
						var aName = actor.name;
					else
						var aName = "Someone";
					player.send(mag+aName+" has left the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
				}
			}
			actor.arenaTeam = constants.ARENA_BLUE;
			for each (var player in getWaitingPlayers("all")) {
				getCharCols(player);
				if (isAI(player))
					var aName = "(AI) Player";
				else if (player.race == actor.race)
					var aName = actor.name;
				else
					var aName = "Someone";
				player.send(mag+aName+" has joined the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
			}
			return;
		}
		else if (strn_cmp(vArgs[1],"red",2)) {
			if (global.arenaFreeForAll == true) {
				actor.send("The current match mode is Free For All - there is no Red Team!");
				return;
			}
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only join a team during the Queue phase!");
				return;
			}
			if (actor.arenaTeam) {
				if (actor.arenaTeam == constants.ARENA_RED) {
					actor.send("You're already on the Red Team!");
					return;
				}
				for each (var player in getWaitingPlayers("all")) {
					getCharCols(player);
					if (player.race == actor.race)
						var aName = actor.name;
					else
						var aName = "Someone";
					player.send(mag+aName+" has left the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
				}
			}
			actor.arenaTeam = constants.ARENA_RED;
			for each (var player in getWaitingPlayers("all")) {
				getCharCols(player);
				if (player.race == actor.race)
					var aName = actor.name;
				else
					var aName = "Someone";
				player.send(mag+aName+" has joined the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
			}
			return;
		}
		else if (strn_cmp(vArgs[1],"yellow",2)) {
			if (global.arenaFreeForAll == true) {
				actor.send("The current match mode is Free For All - there is no Yellow Team!");
				return;
			}
			if (global.arenaTeams < 3) {
				actor.send("There is no Yellow Team! Pick Red or Blue.");
				return;
			}
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only join a team during the Queue phase!");
				return;
			}
			if (actor.arenaTeam) {
				if (actor.arenaTeam == constants.ARENA_YELLOW) {
					actor.send("You're already on the Yellow Team!");
					return;
				}
				for each (var player in getWaitingPlayers("all")) {
					getCharCols(player);
					if (player.race == actor.race)
						var aName = actor.name;
					else
						var aName = "Someone";
					player.send(mag+aName+" has left the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
				}
			}
			actor.arenaTeam = constants.ARENA_YELLOW;
			for each (var player in getWaitingPlayers("all")) {
				getCharCols(player);
				if (player.race == actor.race)
					var aName = actor.name;
				else
					var aName = "Someone";
				player.send(mag+aName+" has joined the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
			}
			return;
		}
		else if (strn_cmp(vArgs[1],"green",2)) {
			if (global.arenaFreeForAll == true) {
				actor.send("The current match mode is Free For All - there is no Green Team!");
				return;
			}
			if (global.arenaTeams < 4) {
				if (global.arenaTeams < 3)
					actor.send("There is no Green Team! Pick Red or Blue.");
				else
					actor.send("There is no Green Team! Pick Yellow, Red, or Blue.");
				return;
			}
			if (global.arenaStage != constants.ARENA_QUEUE) {
				actor.send("You can only join a team during the Queue phase!");
				return;
			}
			if (actor.arenaTeam) {
				if (actor.arenaTeam == constants.ARENA_GREEN) {
					actor.send("You're already on the Green Team!");
					return;
				}
				for each (var player in getWaitingPlayers("all")) {
					getCharCols(player);
					if (player.race == actor.race)
						var aName = actor.name;
					else
						var aName = "Someone";
					player.send(mag+aName+" has left the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
				}
			}
			actor.arenaTeam = constants.ARENA_GREEN;
			for each (var player in getWaitingPlayers("all")) {
				getCharCols(player);
				if (player.race == actor.race)
					var aName = actor.name;
				else
					var aName = "Someone";
				player.send(mag+aName+" has joined the "+getTeamName(actor.arenaTeam)+" Team!"+nrm);
			}
			return;
		}
		else if (strn_cmp(vArgs[1], "kick", 2)) { 
			if (global.arenaStage != constants.ARENA_PLAYING) {
				actor.send("You can only request to kick a player during an Arena match!");
				return;
			}
			if (global.arenaPlayers.length < 5) {
				actor.send("There must be at least 5 players in the match in order to request a kick.");
				return;
			}
			var kickRequest = getSval(player, 14200, "kickRequest");
			if (kickRequest == 1) {
				actor.send("You can only request to kick a player once per Arena match!");
				return;
			}
			actor.send(" ");
			actor.send("Which player do you want to kick?");
			actor.send("______________________________________");
			actor.send(" ");
			for (i=0;i<global.arenaPlayers.length;i++) {
				actor.send(mag+(i+1)+nrm+") "+global.arenaPlayers[i].name);
			}
			actor.send("______________________________________");
			actor.send(" ");
			actor.send("["+mag+"#"+nrm+"] Select Player ["+mag+"C"+nrm+"] Cancel");
			actor.send(" ");
			actor.attach(14222);
			return;
		}
		else if (strn_cmp(vArgs[1],"imm",3)) {
			if (actor.level < 103) {
				actor.send("You cannot use that command.");
				return;
			}
			global.arenaImm = 51;
			actor.send("Immortals now have NO access to the mortal ARENA commands.");
			return;
		}
		else if (strn_cmp(vArgs[1],"issue",3)) {
			if (global.arenaStage != constants.ARENA_PLAYING || !arrContains(global.arenaPlayers, actor)) {
				actor.send("Wait until you're playing a match, then use this command at your home base!");
				return;
			}
			if (!global.arenaCurrentMap) {
				actor.send("There's nowhere to issue here!");
				return;
			}
			if (actor.room != getHomeBase(actor)) {
				actor.send("You can only issue at your home base ("+getHomeBase(actor).name+") !");
				return;
			}
			if (!vArgs[2]) {
				actor.send("You can currently issue:  MOUNT, LIGHT");
				actor.send(" ");
				actor.send("Example:  'arena issue mount'");
				return;
			}
			if (getSval(actor,14209,"arenaIssue") > 9) {
				actor.send("You've issued too much already!");
				return;
			}
			if (strn_cmp(vArgs[2],"mount",3)) {
				if (actor.race == constants.RACE_HUMAN || actor.race == constants.RACE_AIEL)
					var mount = 201;
				else if (actor.race == constants.RACE_TROLLOC && (actor.class == constants.CLASS_DREADLORD || actor.class == constants.CLASS_FADE) )
					var mount = 9998;
				else if (actor.race == constants.RACE_SEANCHAN)
					var mount = 10256;
				if (!mount)
					actor.send("You have no need of a mount!");
				else {
					actor.room.loadMob(mount);
					actor.room.people[0].comm("follow "+actor.name);
					if (getSval(actor,14209,"arenaIssue"))
						setSval(actor,14209,"arenaIssue", getSval(actor,14209,"arenaIssue")+1 );
					else
						setSval(actor,14209,"arenaIssue",1);
				}
				return;
			}
			else if (strn_cmp(vArgs[2],"light",3)) {
				actor.send("A torch appears in your hands.");
				actor.loadObj(2040);
				if (getSval(actor,14209,"arenaIssue"))
					setSval(actor,14209,"arenaIssue", getSval(actor,14209,"arenaIssue")+1 );
				else
					setSval(actor,14209,"arenaIssue",1);
				return;
			}
			else {
				actor.send("You can currently issue:  MOUNT, LIGHT");
				actor.send(" ");
				actor.send("Example:  'arena issue mount'");
				return;
			}
			
		}
		else if (strn_cmp(vArgs[1],"bet",1)) { //ARENA BETTING
			actor.send("Coming soon!");
			return;
			if (global.arenaStage < constants.ARENA_QUEUE) {	//no match
				actor.send("There's no arena match to bet on right now.");
				return;
			}
			else if (!arrContains(getWaitingPlayers("pc"), actor)) {
				actor.send("You haven't entered the arena queue yet!");
				return;
			}
			else if (global.arenaMixedBets == false && global.arenaCurrentWagers.length == 0 && global.arenaHost != actor) {
				actor.send("Only the host can initiate the bet in the Host-Controlled betting mode!");
				return;
			}
			else if (global.arenaFreeForAll == false && !actor.arenaTeam) {
				actor.send("You haven't joined a team yet!");
				return;
			}
			else {
				getCurrentWagers(actor); //send current wagers
				if (global.arenaStage == constants.ARENA_QUEUE)	{//betting occurs in QUEUE only 
					setSval(actor,14209,"wagerMenu",0);
					getWagerMenu(actor);
					return;
				}
			}
		}
	}
	/** NO ARGUMENT, SHOW MAIN ARENA COMMAND DISPLAY **/
	actor.send(" ");
	if (arrContains(global.arenaWatchers, actor) || actor.room.zoneVnum == global.arenaZoneVnum || ( global.arenaStage == constants.ARENA_PLAYING && arrContains(global.arenaStartRooms, actor.room.vnum) ) ) { //Player is in Arena, get Arena stats
		getArenaStats(actor);
		return;
	}
	else if (arrContains(getWaitingPlayers(actor.race),actor)) { //Player is in queue, get queue stats
		if (global.arenaStage == constants.ARENA_VOTING) { // Player is currently voting
			actor.send("Players are currently voting for the Arena match type.");
			actor.send(" ");
			
			actor.send("Time Left to Vote   :  "+mag+(global.arenaVotingTimeLimit+1)+" min"+nrm);
			var votesLeft = getWaitingPlayers("pc").length - global.arenaTotalVotes;
			actor.send("Votes Left to Cast  :  "+mag+votesLeft+nrm);
			actor.send(" ");
			actor.send(mag+"Current Votes"+nrm)
			//actor.send(" ");
			for each (var game in global.arenaAllGames)
				actor.send(strPadding(game.name," ",21,"right")+":  "+mag+game.votes+nrm);
			if (actor == global.arenaHost) { //Player is host, remind him of options
				getHostCommands(actor);
			}
		}
		else { // Player is in queue, not voting yet
			var queueStr = mag+getWaitingPlayers("all").length+nrm+" player";
			if (getWaitingPlayers("all").length > 1)
				queueStr += "s";
			var needStr = "";
			var playerS = "player";
			var pLeft = global.arenaMinToEnter - getWaitingPlayers("all").length;
			if (pLeft > 0)
				needStr += (mag+pLeft+nrm+" player");
			if (pLeft > 1)
				needStr += "s";
			if (needStr == "") {
				needStr = mag+"Ready"+nrm;
				if (areTeamsEven() == false)
					needStr = mag+"Even Teams"+nrm;
			}
			actor.send("You are currently waiting to join an Arena match.");
			actor.send(" ");
			if (global.arenaHost.race != actor.race)
				var aHost = "Someone";
			else
				var aHost = global.arenaHost.name;
			actor.send("Current Match Host      :  "+mag+aHost+nrm);
			actor.send("Current Match Map       :  "+mag+global.arenaCurrentMap.name+"  ( "+global.arenaCurrentMap.minToEnter+" players required )"+nrm);
			if (global.arenaFreeForAll == false)
				var mMode = "Teams";
			else
				var mMode = "Free For All";
			if (global.arenaMixedBets == false)
				var bMode = "Host-Controlled";
			else
				var bMode = "Mixed Betting";
			actor.send("Current Match Mode      :  "+mag+mMode+nrm);
			actor.send("Current Betting Mode    :  "+mag+bMode+nrm);
			actor.send(" ");
			actor.send("Players In Arena Queue  :  "+queueStr);
			if (mMode == "Teams") {
				var red = 0;
				var blue = 0;
				var yellow = 0;
				var green = 0;
				for each (var player in getWaitingPlayers("all")) {
				if (player.arenaTeam == constants.ARENA_BLUE)
					blue += 1;
				else if (player.arenaTeam == constants.ARENA_RED)
					red += 1;
				else if (player.arenaTeam == constants.ARENA_YELLOW)
					yellow += 1;
				else if (player.arenaTeam == constants.ARENA_GREEN)
					green += 1;
				}
				actor.send("             Blue Team  :  "+mag+blue+nrm+" players");
				actor.send("              Red Team  :  "+mag+red+nrm+" players");
				if (global.arenaTeams > 2)
					actor.send("           Yellow Team  :  "+mag+yellow+nrm+" players");
				if (global.arenaTeams > 3)
					actor.send("            Green Team  :  "+mag+green+nrm+" players");
			}
			actor.send(" ");
			actor.send("Needed to Start Match   :  "+needStr);
			if (actor == global.arenaHost) {
				getHostCommands(actor);
			}
			if (global.arenaFreeForAll == false) {
				actor.send(" ");
				actor.send("This is a "+mag+"team match"+nrm+". Choose your team by typing "+mag+"arena <color>"+nrm+".");
			}
			actor.send(" ");
			actor.send("Type "+mag+"arena bet"+nrm+" to see the current match wagers.");
			actor.send(" ");
			actor.send("Type "+mag+"arena help"+nrm+" for more information.");
		}
		return;
	}
	else { //Player is not interacting with the Arena, give general info
		if (global.arenaStage == constants.ARENA_ON || global.arenaStage == constants.ARENA_QUEUE)
			actor.send("The Arena is currently open! Type "+mag+"enter arena"+nrm+" to join the battle!");
		else if (global.arenaStage == constants.ARENA_PLAYING) {
			actor.send("There is an Arena match currently in progress.");
			actor.send(" ");
			actor.send("You can observe the match by typing "+mag+"arena watch"+nrm+".");
		}
		else
			actor.send("The Arena is only open on Thursdays (server time).");
		getArenaPermStats(actor);
		return;
	}
}
if (actor.level < 103) {
	actor.send("You are not a high enough level to use this command!");
	return;
}
if (!vArgs[1]) { //Display command arguments
	if (global.arenaStage == constants.ARENA_ON)
		var onOff = "ON";
	else if (global.arenaStage == constants.ARENA_OFF)
		var onOff = "OFF";
	else if (global.arenaStage == constants.ARENA_QUEUE)
		var onOff = "ON - QUEUE";
	else if (global.arenaStage == constants.ARENA_VOTING)
		var onOff = "ON - VOTING";
	else if (global.arenaStage == constants.ARENA_PLAYING)
		var onOff = "ON - PLAYING";
	if (global.arenaFreeForAll == true)
		var ffaOnOff = "ON";
	else
		var ffaOnOff = "OFF";
	if (global.arenaImm == 51)
		var immOnOff = "OFF";
	else
		var immOnOff = "ON";
	actor.send("The ARENA command has the following uses:");
	actor.send(" ");
	actor.send("ARENA ON/OFF     :  enables / disables the arena");
	actor.send("                    ( currently "+onOff+" )");
	actor.send(" ");
	actor.send("ARENA MINIMUM #  :  minimum TOTAL PLAYERS to enter the match");
	actor.send("                    ( currently "+global.arenaMinToEnter+" )");
	actor.send(" ");
	actor.send("ARENA ZONE #     :  sets the zone the arena match will be played in");
	actor.send("                    ( currently "+global.arenaZoneVnum+" )");
	actor.send(" ");
	// actor.send("ARENA START      :  begins the voting process for an arena match");
	// actor.send("DM/TB/CTF/KH/SM     add the game type to this command to start the game without voting");
	// actor.send(" ");
	actor.send("ARENA IMM        :  lets immortals access mortal ARENA commands");
	actor.send("                    ( currently "+immOnOff+" )");
	actor.send(" ");
	actor.send("ARENA FFA        :  toggles FREE FOR ALL mode on/off");
	actor.send("                    ( currently "+ffaOnOff+" )");
	actor.send(" ");
	actor.send("ARENA MOBS #     :  sets the density of mobs loaded to the match:  higher = less mobs");
	actor.send("                    ( currently 1 mob per "+global.arenaMobDensity+" rooms )");
	actor.send(" ");
	actor.send("ARENA TEAMS #    :  sets the number of teams for the match");
	actor.send("                    ( currently "+global.arenaTeams+" )");
	actor.send(" ");
	actor.send("ARENA RESET      :  returns the arena to its defaults");
	actor.send(" ");
}
if (strn_cmp(vArgs[1],"reset",1)) {
	resetArena(actor);
	actor.send("The arena has been reset and any players in the arena have been booted.");
}
if (strn_cmp(vArgs[1],"imm",3)) {
	global.arenaImm = 106;
	actor.send("Immortals now have access to the mortal ARENA commands.");
	return;
}
if (global.arenaStage != constants.ARENA_PLAYING) {
if (strn_cmp(vArgs[1],"on",2)) {
	actor.send("Arena ENABLED.");
	setArenaStage(constants.ARENA_ON);
	return;
}
if (strn_cmp(vArgs[1],"off",2)) {
	actor.send("Arena DISABLED.");
	setArenaStage(constants.ARENA_OFF);
	return;
}
if (strn_cmp(vArgs[1],"ffa",1)) {
	if (global.arenaFreeForAll == true) {
		global.arenaFreeForAll = false;
		var FFAOnOff = "OFF";
	}
	else {
		global.arenaFreeForAll = true;
		var FFAOnOff = "ON";
	}	
	actor.send("Arena FREE FOR ALL mode is now "+FFAOnOff+".");
	return;
}
if (strn_cmp(vArgs[1],"zone",1)) {
	if (vArgs[2]) {
		if (vArgs[2] < 0 || isNaN(vArgs[2])) {
			actor.send("Zone number must be a positive number.");
			return;
		}
		global.arenaZoneVnum = vArgs[2];
		actor.send("Arena zone changed to "+vArgs[2]+".");
		for (var i=0; i<global.arenaAllMaps.length; i++) {
			if (global.arenaZoneVnum == global.arenaAllMaps[i].vnum)
				global.arenaCurrentMap = global.arenaAllMaps[i];
		}
		return;
	}
	else {
		actor.send("Syntax:  ARENA ZONE #");
		return;
	}
}
if (strn_cmp(vArgs[1],"mobs",2)) {
	if (vArgs[2]) {
		if (vArgs[2] < 0 || isNaN(vArgs[2])) {
			actor.send("Mobs number must be a positive number.");
			return;
		}
		global.arenaMobDensity = vArgs[2];
		actor.send("Arena mob density changed to "+vArgs[2]+".");
		return;
	}
	else {
		actor.send("Syntax:  ARENA MOBS #");
		return;
	}
}
if (strn_cmp(vArgs[1],"teams",2)) {
	if (vArgs[2]) {
		if (vArgs[2] < 2 || isNaN(vArgs[2]) || vArgs[2] > 4) {
			actor.send("Teams number must be 2, 3, or 4.");
			return;
		}
		global.arenaTeams = vArgs[2];
		actor.send("Arena teams changed to "+vArgs[2]+".");
		return;
	}
	else {
		actor.send("Syntax:  ARENA TEAMS #");
		return;
	}
}
if (strn_cmp(vArgs[1],"minimum",1)) {
	if (!vArgs[2] || isNaN(vArgs[2])) {
		actor.send("Syntax:  ARENA MINIMUM #");
		return;
	}
	global.arenaMinToEnter = vArgs[2];
	actor.send("Minimum Arena Queue players set to " +vArgs[2]+" players.");
	return;
}
if (strn_cmp(vArgs[1],"start",1)) {
	if (getWaitingPlayers("all").length < 2) {
		actor.send("Wait until at least 2 players are in the queue.");
		return;
	}
	if (vArgs[2]) {
		if (strn_cmp(vArgs[2],"DM",2))
			var gameType = global.arenaGameDM;
		else if (strn_cmp(vArgs[2],"TB",2))
			var gameType = global.arenaGameTB;
		else if (strn_cmp(vArgs[2],"CTF",2))
			var gameType = global.arenaGameCTF;
		else if (strn_cmp(vArgs[2],"KH",2))
			var gameType = global.arenaGameKH;
		else if (strn_cmp(vArgs[2],"SM",2))
			var gameType = global.arenaGameSM;
		else {
			actor.send("Valid arguments are DM (Death Match), TB (Timed Battle), CTF (Capture the Flag), KH (King of the Hill), or SM (SuperMOBS).");
			return;
		}
		global.arenaMinToEnter = 1;
		startArenaMatch(gameType);
		return;
	}
	setArenaStage(constants.ARENA_VOTING);
	for each (var person in getWaitingPlayers("all")) {
		person.attach(14212);
		getArenaStartMenu(person);
	}
}
}
else {
	actor.send("There is currently an Arena match in progress. You can only use ARENA RESET at this point.");
	return;
}










};


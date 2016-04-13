//Arena functions
//Koradin - June 2011 and commented in July 2014!
function getArenaStage() { 
	return global.arenaStage;
}
/** SET ARENA STAGE **/
function setArenaStage(intStage) {
	if (!intStage || isNaN(intStage) || intStage < 1 || intStage > 5) {
		mudLog(3, 102, "setArenaStage called with no or invalid argument");
		return;
	}
	if (intStage == constants.ARENA_QUEUE) {
		for (var _autoKey in getWaitingPlayers("all")) {
			var player = getWaitingPlayers("all")[_autoKey];
			player.detach(14212);

		}
		global.arenaPlayers = [];
		global.arenaTotalVotes = 0;
		global.arenaVotingTimeLimit = 3;
		global.arenaAllGames = [];
		initGlobals();
	}
	global.arenaStage = intStage;
}
/** RETURN NUMBER OF PEOPLE IN QUEUE **/
function arenaQueue() {
	var number = getWaitingPlayers("all").length;
	return number;
}
/** CHECK TO SEE IF TEAMS ARE EVEN **/
function areTeamsEven() {
	var red = 0;
	var blue = 0;
	var yellow = 0;
	var green = 0;
	for (var _autoKey in getWaitingPlayers("all")) {
		var player = getWaitingPlayers("all")[_autoKey];
		if (player.arenaTeam == constants.ARENA_BLUE)
			blue += 1;
		else if (player.arenaTeam == constants.ARENA_RED)
			red += 1;
		else if (player.arenaTeam == constants.ARENA_YELLOW)
			yellow += 1;
		else if (player.arenaTeam == constants.ARENA_GREEN)
			green += 1;
	}
	if (global.arenaTeams == 2) { //Red, Blue
		if (red == blue)
			return true;
		else
			return false;
	}
	else if (global.arenaTeams == 3) { //Red, Blue, Yellow
		if (red == blue && blue == yellow)
			return true;
		else
			return false;
	}
	else if (global.arenaTeams == 4) { //Red, Blue, Yellow, Green
		if (red == blue && blue == yellow && yellow == green)
			return true;
		else
			return false;
	}
	else
		return null;
}
/** CHECK TO SEE IF ARENA MATCH IS READY TO BEGIN **/ //required to vote/begin
function isMatchReady(reason) {
	if (global.arenaFreeForAll == false) { //FFA Mode is off, make sure even race numbers
		var red = 0;
		var blue = 0;
		var yellow = 0;
		var green = 0;
		for (var _autoKey in getWaitingPlayers("all")) {
			var player = getWaitingPlayers("all")[_autoKey];
			if (player.arenaTeam == constants.ARENA_BLUE)
				blue += 1;
			else if (player.arenaTeam == constants.ARENA_RED)
				red += 1;
			else if (player.arenaTeam == constants.ARENA_YELLOW)
				yellow += 1;
			else if (player.arenaTeam == constants.ARENA_GREEN)
				green += 1;
		}
		if (global.arenaMinToEnter > getWaitingPlayers("all").length) //not enough players to enter
			var ending = [false,"There are not enough players to start the match."];
		else if ((red+blue+yellow+green) < getWaitingPlayers("all").length)
			var ending = [false,"Not all players have chosen a team."];
		else if (areTeamsEven() == false) // human and trolloc numbers aren't the same
			var ending = [false,"The teams are not even."];
		else if (areTeamsEven() == true && getWaitingPlayers("all").length < global.arenaTotalVotes && global.arenaVotingTimeLimit > 0) // even numbers, but everyone hasn't voted and there's still time left
			var ending = [false,"All players haven't voted and there is still time left to vote."];
		else // all checks have been made, good to go
			var ending = [true];
		if (reason)
			return ending[1];
		else
			return ending[0];
	}
	else { // FFA Mode is ON, less checks to be made
		if (getWaitingPlayers("all").length < global.arenaTotalVotes && global.arenaVotingTimeLimit > 0) // everyone hasn't voted and there's still time left
			var ending = [false,"All players haven't voted and there is still time left to vote."];
		else if (getWaitingPlayers("all").length < global.arenaMinToEnter) //Not enough players to start
			var ending = [false,"There are not enough players to start the match."];
		else // all checks have been made, good to go
			var ending = [true];
		if (reason)
			return ending[1];
		else
			return ending[0];
	}
}
/** ADD CHARACTER TO QUEUE **/
JSCharacter.prototype.addToArenaQueue = function() {
	getCharCols(this);
	this.send(mag+"You have joined the queue for the Arena!"+nrm);
	this.arenaTeam = null;
	setSval(this,14210,"arenaQueue",1);
	if ((getWaitingPlayers("all").length-getWaitingPlayers("ai").length) == 1 && !isAI(this)) { // Player is first to join queue, he is the HOST
		global.arenaHost = this;
		setArenaStage(constants.ARENA_QUEUE);
		this.send("You are the first to join the queue, and have been made HOST of the Arena.");
		getHostCommands(this);
	}
	this.send(" ");
	this.send("Type "+mag+"arena"+nrm+" for more information.");
	for (var _autoKey in getWaitingPlayers("all")) {
		var peep = getWaitingPlayers("all")[_autoKey]; //Alert rest of queue. Check for NOQUIT and remove from queue anyone who has it
		if (peep.affectedBy(constants.AFF_NOQUIT)) {
			getCharCols(peep);
			peep.send("You are too worked up to enter the Arena!");
			peep.removeFromArenaQueue();
		}
		//here.echo(peep.name);
		getCharCols(peep);
		if (isAI(this))
			var peepName = "(AI) Player";
		else if (this.race == peep.race)
			var peepName = "$n";
		else
			var peepName = "Someone";
		if (peep != this)
			act(mag+peepName+" has entered the queue for the Arena!"+nrm,true,this,null,peep,constants.TO_VICT);
	}
	if (isMatchReady() == true && !isAI(this)) { // minimum has been met, notify HOST
		getCharCols(global.arenaHost);
		for (var _autoKey in getWaitingPlayers("all")) {
			var person = getWaitingPlayers("all")[_autoKey];
			getCharCols(person);
			if (person == global.arenaHost) {
				person.send(" ");
				person.send(mag+"There are enough players in the queue to start the match."+nrm);
				getHostCommands(person);
			}
		}
	}
}
/** RETURN HOST COMMANDS **/
function getHostCommands(actor) {
	if (!actor)
		return;
	getCharCols(actor);
	actor.send(" ");
	actor.send(mag+"Host Commands"+nrm);
	actor.send(" ");
	if (global.arenaStage == constants.ARENA_QUEUE) {
		actor.send("ARENA MODE     :  set the match and betting modes"+nrm);
		if (global.arenaFreeForAll == false)
			actor.send("ARENA TEAMS    :  set the match teams"+nrm);
		actor.send("ARENA MAP      :  set the match map"+nrm);
		actor.send("ARENA AI       :  add / remove AI players"+nrm);
		actor.send("ARENA START    :  starts the voting process"+nrm);
	}
	if (global.arenaStage == constants.ARENA_VOTING) {
		actor.send("ARENA TIME     :  provides more time to vote"+nrm);
	}
	if (global.arenaStage == constants.ARENA_PLAYING) {
		actor.send("ARENA END      :  vote to end the game");
	}
	actor.send(" ");
}
/** HAVE HOST SET GAME MAP/MODE/TEAMS **/
function getHostMenus(actor, strType) {
	if (!actor || !strType)
		return;
	if (actor != global.arenaHost)
		return;
	getCharCols(actor);
	actor.send(" ");
	if (strType == "mode") { //SELECTING MODE
		actor.send("Select the MATCH mode:");
		actor.send(" ");
		actor.send("["+mag+"T"+nrm+"] Teams       ["+mag+"F"+nrm+"] Free For All");
		setSval(actor,14214,"hostMenu",1);
	}
	else if (strType == "map") { //SELECTING MAP
		actor.send("Please select one of the following maps:")
		actor.send("____________________________________________________");
		actor.send(" ");
		for (var i=0; i<global.arenaAllMaps.length; i++) {
			var pReqs = "( "+global.arenaAllMaps[i].minToEnter+" players required )";
			actor.send(mag+(i+1)+nrm+") "+strPadding(global.arenaAllMaps[i].name," ",20,"right")+pReqs);
		}
		actor.send("____________________________________________________");
		actor.send(" ");
		actor.send("["+mag+"#"+nrm+"] Select Map");
		setSval(actor,14214,"hostMenu",2);
	}
	else if (strType == "teams") { //SELECTING TEAMS
		if (global.arenaFreeForAll == true) {
			actor.send("You cannot select the teams in Free For All mode - there are none!");
			actor.send(" ");
			actor.send("Type "+mag+"arena mode"+nrm+" to change this option.");
			return;
		}
		else {
			actor.send("How many different teams will there be?");
			actor.send(" ");
			actor.send("["+mag+"2"+nrm+"] Teams       ["+mag+"3"+nrm+"] Teams       ["+mag+"4"+nrm+"] Teams");
			setSval(actor,14214,"hostMenu",3);
		}
	}
	else if (strType == "ai") { //SELECTING AI
		if (global.arenaFreeForAll == true) {
			var aiComm = "<"+mag+"add"+nrm+"/"+mag+"remove"+nrm+"> <"+mag+"# of AI players"+nrm+">";
			actor.send("AI Players in the Queue:  "+mag+getWaitingPlayers("ai").length+nrm);
		}
		else {
			var aiComm = "<"+mag+"add"+nrm+"/"+mag+"remove"+nrm+"> <"+mag+"# of AI players"+nrm+"> <"+mag+"team color"+nrm+">";
			var blue = 0;
			var reda = 0;
			var green = 0;
			var yellow = 0;
			for (var _autoKey in getWaitingPlayers("ai")) {
				var ai = getWaitingPlayers("ai")[_autoKey];
				if (ai.arenaTeam == constants.ARENA_BLUE)
					blue += 1;
				else if (ai.arenaTeam == constants.ARENA_RED)
					reda += 1;
				else if (ai.arenaTeam == constants.ARENA_GREEN)
					green += 1;
				else if (ai.arenaTeam == constants.ARENA_YELLOW)
					yellow += 1;
			}
			actor.send("AI Players in the Queue:  "+cyn+blue+nrm+" blue, "+red+reda+nrm+" red, "+yel+yellow+nrm+" yellow, "+grn+green+nrm+" green"+nrm);
		}
		actor.send(" ");
		actor.send("To add or remove AI players from the match, use the following command:");
		actor.send(" ");
		actor.send(aiComm);
		actor.send(" ");
		actor.send("When finished, type <"+mag+"done"+nrm+">.");
		actor.send(" ");
		setSval(actor,14214,"hostMenu",4);
	}
	actor.attach(14214);
	return;
}
/** REMOVE CHARACTER FROM QUEUE **/
JSCharacter.prototype.removeFromArenaQueue = function() {
	getCharCols(this);
	this.send(mag+"You stop waiting in line for the Arena."+nrm);
	this.arenaTeam = null;
	setSval(this,14210,"arenaQueue",0);
	this.detach(14212);
	this.arenaTeam = null; // Remove player from team if he's on one
	for (var i=0; i<global.arenaPlayers.length;i++) { //Player has voted, then exited
		if (global.arenaPlayers[i] == this) {
			global.arenaTotalVotes -= 1;  //Remove player's vote from total vote count
			global.arenaPlayers.splice(i, 1); //Remove player from Arena players
			global.arenaAllGames[getSval(this,14212,"arenaVote")].votes -= 1; //remove specific vote
			setSval(this,14212,"arenaVote",null);
			break;
		}
	}
	if (getWaitingPlayers("pc").length > 0) { //Still other players in queue
		if (this == global.arenaHost) { //Player who left was HOST, pick new host
			global.arenaHost = getWaitingPlayers("pc")[0];
			getCharCols(global.arenaHost);
			global.arenaHost.send(mag+"You have been made HOST of the Arena!"+nrm);
			getHostCommands(global.arenaHost);
		}
		for (var _autoKey in getWaitingPlayers("all")) {
			var player = getWaitingPlayers("all")[_autoKey]; //Alert other players
			getCharCols(player);
			if (isAI(this))
				var playerName = "(AI) Player";
			else if (this.race == player.race)
				var playerName = "$n";
			else
				var playerName = "Someone";
			if (player != this) {
				act(mag+playerName+" has left the Arena queue!"+nrm,true,this,null,player,constants.TO_VICT);
				if (global.arenaStage == constants.ARENA_VOTING) { //During voting phase
					if (isMatchReady() == false) { // player leaving caused the rest of the people not to meet the min requirement to vote
						player.send(" ");
						player.send(isMatchReady("reason"));
						setArenaStage(constants.ARENA_QUEUE);
					}
					else if (isMatchReady() == true) { //everyone still in queue has already voted, good to go
						setArenaStage(constants.ARENA_PLAYING);
						startArenaMatch(getArenaGameType(global.arenaAllGames));
						return;
					}
				}	
			}
		}
	}
	else {
		resetArena();
	}
}
/** GET ALL PLAYERS IN QUEUE **/
function getWaitingPlayers(race) {
	var peopleArr = [];
	//if (!race)
	//	race = "all";
	for (var _autoKey in getRoom(20896).people) {
		var ai = getRoom(20896).people[_autoKey];
		if (isAI(ai))
			peopleArr.push(ai);
	}
	if (race == "ai")
		return peopleArr;
	if (race == "pc")
		peopleArr = [];
	for (var _autoKey in getConnectedPlayers()) {
		var player = getConnectedPlayers()[_autoKey];
		if (getSval(player,14210,"arenaQueue") == 1) {
			if (race == "pc" || race == "all" || player.race == race) {
				peopleArr.push(player);
			}
		}
	}
	return peopleArr;
}
/** GET TEAM NAME **/
function getTeamName(intTeam) {
	if (!intTeam || isNaN(intTeam) || intTeam < 1 || intTeam > 4) {
		//mudLog(3, 102, "getTeamName() called with no or invalid argument: "+intTeam);
		return null;
	}
	if (intTeam == 1)
		return "Blue";
	else if (intTeam == 2)
		return "Red";
	else if (intTeam == 3)
		return "Yellow";
	else if (intTeam == 4)
		return "Green";
}
/** RETURN VOTING SCREEN **/
function getArenaStartMenu(actor) {
	if (!actor)
		return null;
	actor.send(" ");
	getCharCols(actor);
	actor.send(mag+"An Arena match is about to begin!"+nrm);
	actor.send(" ");
	actor.send(mag+"Vote for the match type:"+nrm);
	actor.send("____________________________");
	actor.send(" ");
	for (var i=0; i<global.arenaAllGames.length; i++) {
		actor.send(mag+(i+1)+nrm+") "+global.arenaAllGames[i].name);
	}
	actor.send(mag+"7"+nrm+") Random!");
	actor.send("____________________________");
	actor.send(" ");
	actor.send("["+mag+"#"+nrm+"] Vote   ["+mag+"ARENA"+nrm+"] Details");
	return null;
}
/** GET ARENA SECOND VOTING MENU **/
function getArenaVoteMenu(actor, intGame) {
	getCharCols(actor);
	actor.send(" ");
	var lType = clone(global.arenaAllGames[intGame].limitType);
	lType = lType.trim();
	actor.send(mag+global.arenaAllGames[intGame].name+nrm);
	actor.send(" ");
	actor.send(mag+"Vote for the goal:"+nrm);
	actor.send("____________________________");
	actor.send(" ");
	for (i=0; i<global.arenaAllGames[intGame].limits.length; i++) {
		actor.send(mag+(i+1)+nrm+") "+global.arenaAllGames[intGame].limits[i]+" "+lType);
	}
	actor.send("____________________________");
	actor.send(" ");
	actor.send("["+mag+"#"+nrm+"] Select Goal     ["+mag+"B"+nrm+"] Back");
	return;
	
}
/** SET MATCH TYPE BASED ON VOTES **/
function getArenaGameType(allGames) {
	var topVoteIndex = 0;
	var topVoteValue = 0;
	
	for(var index = 0;index < allGames.length;++index) {
	
		if(allGames[index].votes > topVoteValue) {
			topVoteValue = allGames[index].votes;
			topVoteIndex = index;
		}
	}
	
	return allGames[topVoteIndex];
}
/** START MATCH **/
function startArenaMatch(objGameType) {
	if (!objGameType) {
		objGameType = global.arenaCurrentGame;
	}
	global.arenaTotalVotes = 0;
	global.arenaVotingTimeLimit = 3;
	global.arenaStage = constants.ARENA_PLAYING;
	global.arenaCurrentGame = objGameType;
	global.arenaCurrentGame.limit = Math.ceil(global.arenaCurrentGame.limit);
	for (var _autoKey in getWaitingPlayers("all")) {
		var per = getWaitingPlayers("all")[_autoKey];
		if (!arrContains(global.arenaPlayers,per))
			global.arenaPlayers.push(per);
	}
	global.arenaTotalPlayers = global.arenaPlayers.length;
	getRoom(parseInt(global.arenaZoneVnum+"00")).zreset();
	getRoom(14200).zreset();
	getRoom(14400).zreset();
	for (var _autoKey in global.arenaPlayers) {
		var player = global.arenaPlayers[_autoKey];
		player.hps = player.maxHit;
		player.sps = player.maxSpell;
		player.mvs = player.maxMove;
		player.shadowPoints = player.maxShadow;
		setSval(player,14210,"arenaQueue",0);
		setSval(player,20860,"respawnTime",time()+30);
		setSval(player,14200,"startingScore",getArenaScore(player));
		player.detach(14212);
		getCharCols(player);
		player.send(" ");
		player.send(mag+"An Arena match has begun!"+nrm);
		player.send(" ");
		if (global.arenaFreeForAll == true) { //ffa mode, move to race-specific room.
			player.arenaTeam = null;
			if (player.race == constants.RACE_HUMAN)
				player.moveToRoom(getRoom(14201));
			else if (player.race == constants.RACE_TROLLOC)
				player.moveToRoom(getRoom(14299));
			else if (player.race == constants.RACE_AIEL)
				player.moveToRoom(getRoom(14415));
			else if (player.race == constants.RACE_SEANCHAN)
				player.moveToRoom(getRoom(14499));
		}
		else { //teams mode, move to respective places
			if (player.arenaTeam == constants.ARENA_BLUE)
				player.moveToRoom(getRoom(14201));
			else if (player.arenaTeam == constants.ARENA_RED)
				player.moveToRoom(getRoom(14299));
			else if (player.arenaTeam == constants.ARENA_YELLOW)
				player.moveToRoom(getRoom(14415));
			else if (player.arenaTeam == constants.ARENA_GREEN)
				player.moveToRoom(getRoom(14499));
		}
		player.comm("look");
		player.send(" ");
		player.send("Match Type   :  "+mag+objGameType.name+nrm);
		player.send("Goal to Win  :  "+mag+global.arenaCurrentGame.limit+" "+global.arenaCurrentGame.limitType+nrm);
		player.send(" ");
		player.send(global.arenaCurrentGame.info);
		player.send(" ");
		player.send("Type "+mag+"arena"+nrm+" for more information.");
	}
	// Add mobs to Arena
	var mobArr = [];
	var blueMobArr = [];
	var redMobArr = [];
	var yellowMobArr = [];
	var greenMobArr = [];
	for (i=global.arenaCurrentMap.min; i<(global.arenaCurrentMap.max)+1; i++) {
		if (arrContains(global.arenaCurrentMap.redCityRooms, i) && !arrContains(global.arenaCurrentMap.exclusions, i)) // red city mob
			redMobArr.push(i);
		else if (arrContains(global.arenaCurrentMap.blueCityRooms, i) && !arrContains(global.arenaCurrentMap.exclusions, i)) // blue city mob
			blueMobArr.push(i);
		else if (arrContains(global.arenaCurrentMap.yellowCityRooms, i) && !arrContains(global.arenaCurrentMap.exclusions, i)) // yellow city mob
			yellowMobArr.push(i);
		else if (arrContains(global.arenaCurrentMap.greenCityRooms, i) && !arrContains(global.arenaCurrentMap.exclusions, i)) // green city mob
			greenMobArr.push(i);
		else if (!arrContains(global.arenaCurrentMap.exclusions, i))
			mobArr.push(i);
	}
	var numMobs = Math.floor(mobArr.length/global.arenaMobDensity);
	var blueMobs = Math.floor(blueMobArr.length/(global.arenaMobDensity-1));
	var redMobs = Math.floor(redMobArr.length/(global.arenaMobDensity-1));
	var yellowMobs = Math.floor(yellowMobArr.length/(global.arenaMobDensity-1));
	var greenMobs = Math.floor(greenMobArr.length/(global.arenaMobDensity-1));
	while (numMobs > 0) { // add mobs to general arena zone
		var newRoom = getRoom(mobArr[random(0,mobArr.length-1)]);
		newRoom.loadMob(global.arenaAllMobs[random(0,global.arenaAllMobs.length-1)]);
		numMobs -= 1;
	}
	while (blueMobs > 0) { // add mobs to blue side city rooms
		var newRoom = getRoom(blueMobArr[random(0,blueMobArr.length-1)]);
		newRoom.loadMob(global.arenaBlueMobs[random(0,global.arenaBlueMobs.length-1)]);
		blueMobs -= 1;
	}
	while (redMobs > 0) { // add mobs to red side city rooms
		var newRoom = getRoom(redMobArr[random(0,redMobArr.length-1)]);
		newRoom.loadMob(global.arenaRedMobs[random(0,global.arenaRedMobs.length-1)]);
		redMobs -= 1;
	}
	while (yellowMobs > 0) { // add mobs to yellow side city rooms
		var newRoom = getRoom(yellowMobArr[random(0,yellowMobArr.length-1)]);
		newRoom.loadMob(global.arenaYellowMobs[random(0,global.arenaYellowMobs.length-1)]);
		yellowMobs -= 1;
	}
	while (greenMobs > 0) { // add mobs to green side city rooms
		var newRoom = getRoom(greenMobArr[random(0,greenMobArr.length-1)]);
		newRoom.loadMob(global.arenaGreenMobs[random(0,global.arenaGreenMobs.length-1)]);
		if (newRoom.people[0].vnum == 18804) {
			newRoom.loadMob(20974);
			newRoom.people[0].comm("follow "+newRoom.people[1].namelist.split(" ")[0]);
		}
		greenMobs -= 1;
	}
	// Add game-specific stuff
	if (global.arenaCurrentGame == global.arenaGameCTF) {
		resetFlag("all");
	}
	else if (global.arenaCurrentGame == global.arenaGameKH) {
		loadHill();
	}
	else if (global.arenaCurrentGame == global.arenaGameSM) {
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			setSval(player,20860,"arenaScore",getArenaScore(player));

		}
		loadSmobs();
		global.arenaCurrentGame.timer = global.arenaCurrentGame.limit;
	}
	else if (global.arenaCurrentGame == global.arenaGameTB) {
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			setSval(player,20860,"arenaScore",getArenaScore(player));

		}
		global.arenaCurrentGame.timer = global.arenaCurrentGame.limit;
	}
	else if (global.arenaCurrentGame == global.arenaGameLMS) {
		global.arenaCurrentGame.totalPlayers = global.arenaPlayers.length;
	}
}
/** GET A RANDOM ARENA ROOM **/  //used for loading new CTF flags, porting people, etc.
function getRandomArenaRoom(strType) {
	if (!strType)
		mudLog(3, 102, "getRandomArenaRoom() called without a valid type");
	if (strType == "red")
		var sector = global.arenaCurrentMap.redCityRooms;
	else if (strType == "blue")
		var sector = global.arenaCurrentMap.blueCityRooms;
	else if (strType == "yellow")
		var sector = global.arenaCurrentMap.yellowCityRooms;
	else if (strType == "green")
		var sector = global.arenaCurrentMap.greenCityRooms;
	else if (strType == "all") { // RETURN ANY ROOM NOT IN THE ARENA MAP'S EXCLUSIONS
		var allArr = [];
		for (i=global.arenaCurrentMap.min; i<global.arenaCurrentMap.max+1; i++) {
			if (!arrContains(global.arenaCurrentMap.exclusions, i))
				allArr.push(i);
		}
		//sendKoradin("no team "+getRoom( allArr[random(0, allArr.length-1)] ).vnum);
		return getRoom( allArr[random(0, allArr.length-1)] );
	}
	//sendKoradin("allArr: "+allArr);
	var allArr = [];
	for (var i=0; i<sector.length; i++) {
		if (!arrContains(sector.exclusions, i))
			allArr.push(sector[i]);
	}
	//sendKoradin("team "+ getRoom( allArr[random(0, allArr.length-1)] ).vnum);
	return getRoom( allArr[random(0, allArr.length-1)] ); // RETURN ANY CITY ROOM NOT IN EXCLUSIONS
}
/** DEATH MESSAGE **/
function getArenaKilledMessage(victim, killer, assists) {
	var roll = random(1,9);
	if (roll == 1)
		var msg = "OBLITERATED";
	else if (roll == 2)
		var msg = "SMOKED";
	else if (roll == 3)
		var msg = "DESTROYED";
	else if (roll == 4)
		var msg = "MAN-HANDLED";
	else if (roll == 5)
		var msg = "SLAYED";
	else if (roll == 6)
		var msg = "ENDED";
	else if (roll == 7)
		var msg = "SCHOOLED";
	else if (roll == 8)
		var msg = "OWNED";
	else if (roll == 9)
		var msg = "SPANKED";
	var secondRoll = random(1,8);
	if (secondRoll < 4) //Killer killed victim
		var endMsg = killer.name+" just "+msg+" "+victim.name+"!";
	else if (secondRoll < 7)//Victim was killed by killer
		var endMsg = victim.name+" just got "+msg+" by "+killer.name+"!";
	else {
		var rand = random(1,4);
		if (rand == 1)
			var endMsg = victim.name+" was just wiped off the map by "+killer.name+"!";
		else if (rand == 2)
			var endMsg = killer.name+" just showed "+victim.name+" who is BOSS!";
		else if (rand == 3)
			var endMsg = victim.name+" just tried to best "+killer.name+" and FAILED!";
		else if (rand == 4)
			var endMsg = killer.name+" just kicked "+victim.name+"'s bucket!";
	}
	if (killer == victim) {
		var himHerSelf = "himself";
		if (killer.sex == constants.SEX_FEMALE)
			himHerSelf = "herself";
		endMsg = (killer.name +" just killed "+ himHerSelf +"!");
	}
	else if (killer.arenaTeam == victim.arenaTeam && killer.vnum == -1 && victim.vnum == -1 && global.arenaFreeForAll == false) {
		endMsg = (victim.name+" was just betrayed by "+killer.name+"!");
	}
	if (assists[0]) {
		endMsg += "   Assists: ";
		endMsg += assists[0].name;
		if (assists[1]) {
			for (i=1;i<assists.length;i++)
				endMsg += (", "+assists[i].name);
		}
	}
	arenaEcho(endMsg);
}
/** GET ARENA STATS **/
function getArenaStats(actor) {
	getCharCols(actor);
	if (global.arenaStage == constants.ARENA_PLAYING) {
		if (arrContains(global.arenaWatchers, actor))
			actor.send("You are currently observing an Arena match.");
		else
			actor.send("You are currently playing in an Arena match.");
		actor.send(" ");
		actor.send("HOST  :  "+mag+strPadding(global.arenaHost.name," ",20,"right")+nrm+"MATCH TYPE   :  "+mag+global.arenaCurrentGame.name+nrm);
		actor.send("MAP   :  "+mag+strPadding(global.arenaCurrentMap.name," ",20,"right")+nrm+"GOAL TO WIN  :  "+mag+global.arenaCurrentGame.limit+ " "+ global.arenaCurrentGame.limitType+nrm);
		if (global.arenaFreeForAll == true)
			var ffa = "Free For All";
		else
			var ffa = "Teams";
		actor.send("MODE  :  "+mag+ffa+nrm);
		actor.send(" ");
	}
	else {
		actor.send(" ");
		actor.send(mag+"                 Arena Match Statistics"+nrm);
	}
	actor.send(" ");
	actor.send("  Players        |    K    D    A    |  "+(global.arenaCurrentGame.goalType ? global.arenaCurrentGame.goalType : ""));
	actor.send("_________________|___________________|____________________");
	actor.send("                 |                   |");
	var blueArr = [];
	var redArr = [];
	var yellowArr = [];
	var greenArr = [];
	var totalKills = 0;
	var totalDeaths = 0;
	var totalAssists = 0;
	var totalAllKills = 0;
	var totalAllDeaths = 0;
	var totalAllAssists = 0;
	if (global.arenaFreeForAll == false) {
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			if (player.arenaTeam == constants.ARENA_BLUE)
				blueArr.push(player);
			else if (player.arenaTeam == constants.ARENA_RED)
				redArr.push(player);
			else if (player.arenaTeam == constants.ARENA_YELLOW)
				yellowArr.push(player);
			else if (player.arenaTeam == constants.ARENA_GREEN)
				greenArr.push(player);
		}
	}
	else {
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			if (player.race == constants.RACE_HUMAN)
				blueArr.push(player);
			else if (player.race == constants.RACE_TROLLOC)
				redArr.push(player);
			else if (player.race == constants.RACE_AIEL)
				yellowArr.push(player);
			else if (player.race == constants.RACE_SEANCHAN)
				greenArr.push(player);
		}
	}
	for (var _autoKey in blueArr) {
		var player = blueArr[_autoKey];
		var pName = cyn+player.name+nrm;
		var pKills = getSval(player,20860,"killCount");
		if (!pKills)
			pKills = 0;
		totalKills += pKills;
		totalAllKills += pKills;
		var pDeaths = getSval(player,20860,"deathCount");
		if (!pDeaths)
			pDeaths = 0;
		totalDeaths += pDeaths;
		totalAllDeaths += pDeaths;
		var pAssists = getSval(player,20860,"assistCount");
		if (!pAssists)
			pAssists = 0;
		totalAssists += pAssists;
		totalAllAssists += pAssists;
		var pCaps = getSval(player,20930,"flagCaps");
		if (!pCaps)
			pCaps = 0;
		var pRets = getSval(player,20930,"flagRets");
		if (!pRets)
			pRets = 0;
		var smobKills = getSval(player,20860,"smobKills");
		if (!smobKills)
			smobKills = 0;
		if (global.arenaCurrentGame == global.arenaGameCTF) {
			var pPoints = cyn+"    "+pCaps+"        "+pRets+nrm;
			var tPoints = cyn+"    "+global.arenaCurrentGame.blueCaptures+"        "+global.arenaCurrentGame.blueReturns+nrm;	
		}
		else if (global.arenaCurrentGame == global.arenaGameKH) {
			var pPoints = cyn+"  "+getTimeDisplay(getSval(player,20799,"khTime"))+nrm;
			var tPoints = cyn+"  "+getTimeDisplay(global.arenaCurrentGame.blueHillTimer)+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameSM) {
			var pPoints = cyn+"  "+smobKills+nrm;
			var tPoints = cyn+"  "+global.arenaCurrentGame.blueSmobKills+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameLMS) {
			var lLeft = ( global.arenaCurrentGame.limit - ( getSval(player,20860,"deathCount") ? getSval(player,20860,"deathCount") : 0) );
			var pPoints = cyn+( lLeft>0 ? "    "+lLeft : "Eliminated" )+nrm;
		}
		else { //No specifics, show ranks instead.
			var pPoints = cyn+getArenaRank(player)+nrm;
		}
		actor.send("  "+strPadding(pName," ",16,"right")+"|"+cyn+strPadding(pKills," ",6,"left")+strPadding(pDeaths," ",6,"left")+"    "+strPadding(pAssists," ",6,"right")+nrm+"|  "+(pPoints ? pPoints : ""));
	}
	if (global.arenaFreeForAll == false) {
		// actor.send("_________________|___________________|____________________");
		actor.send("                 |                   |");
		actor.send(cyn+"  BLUE TEAM      "+nrm+"|"+cyn+strPadding(totalKills," ",6,"left")+strPadding(totalDeaths," ",6,"left")+"    "+strPadding(totalAssists," ",6,"right")+nrm+"|  "+(tPoints ? tPoints : ""));
		//actor.send(" ");
		//actor.send(" ");
		//actor.send("  Players        |    K    D    A    |  "+(global.arenaCurrentGame.goalType ? global.arenaCurrentGame.goalType : ""));
		actor.send("_________________|___________________|____________________");
		actor.send("                 |                   |");
	}
	var totalKills = 0;
	var totalDeaths = 0;
	var totalAssists = 0;
	for (var _autoKey in redArr) {
		var player = redArr[_autoKey];
		var pName = red+player.name+nrm;
		var pKills = getSval(player,20860,"killCount");
		if (!pKills)
			pKills = 0;
		totalKills += pKills;
		totalAllKills += pKills;
		var pDeaths = getSval(player,20860,"deathCount");
		if (!pDeaths)
			pDeaths = 0;
		totalDeaths += pDeaths;
		totalAllDeaths += pDeaths;
		var pAssists = getSval(player,20860,"assistCount");
		if (!pAssists)
			pAssists = 0;
		totalAssists += pAssists;
		totalAllAssists += pAssists;
		var pCaps = getSval(player,20930,"flagCaps");
		if (!pCaps)
			pCaps = 0;
		var pRets = getSval(player,20930,"flagRets");
		if (!pRets)
			pRets = 0;
		var smobKills = getSval(player,20860,"smobKills");
		if (!smobKills)
			smobKills = 0;
		if (global.arenaCurrentGame == global.arenaGameCTF) {
			var pPoints = red+"    "+pCaps+"        "+pRets+nrm;
			var tPoints = red+"    "+global.arenaCurrentGame.redCaptures+"        "+global.arenaCurrentGame.redReturns+nrm;	
		}
		else if (global.arenaCurrentGame == global.arenaGameKH) {
			var pPoints = red+"  "+getTimeDisplay(getSval(player,20799,"khTime"))+nrm;
			var tPoints = red+"  "+getTimeDisplay(global.arenaCurrentGame.redHillTimer)+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameSM) {
			var pPoints = red+"  "+smobKills+nrm;
			var tPoints = red+"  "+global.arenaCurrentGame.redSmobKills+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameLMS) {
			var lLeft = ( global.arenaCurrentGame.limit - ( getSval(player,20860,"deathCount") ? getSval(player,20860,"deathCount") : 0) );
			var pPoints = red+( lLeft>0 ? "    "+lLeft : "Eliminated" )+nrm;
		}
		else { //No specifics, show ranks instead.
			var pPoints = red+getArenaRank(player)+nrm;
		}
		actor.send("  "+strPadding(pName," ",16,"right")+"|"+red+strPadding(pKills," ",6,"left")+strPadding(pDeaths," ",6,"left")+"    "+strPadding(pAssists," ",6,"right")+nrm+"|  "+(pPoints ? pPoints : ""));
	}
	if (global.arenaFreeForAll == false) {
		//actor.send("_________________|___________________|____________________");
		actor.send("                 |                   |");
		actor.send(red+"  RED TEAM       "+nrm+"|"+red+strPadding(totalKills," ",6,"left")+strPadding(totalDeaths," ",6,"left")+"    "+strPadding(totalAssists," ",6,"right")+nrm+"|  "+(tPoints ? tPoints : ""));
		//actor.send(" ");
		if (global.arenaTeams >= 3) {
			//actor.send(" ");
			//actor.send("  Players        |    K    D    A    |  "+(global.arenaCurrentGame.goalType ? global.arenaCurrentGame.goalType : ""));
			actor.send("_________________|___________________|____________________");
			actor.send("                 |                   |");
		}
	}
	var totalKills = 0;
	var totalDeaths = 0;
	var totalAssists = 0;
	for (var _autoKey in yellowArr) {
		var player = yellowArr[_autoKey];
		var pName = yel+player.name+nrm;
		var pKills = getSval(player,20860,"killCount");
		if (!pKills)
			pKills = 0;
		totalKills += pKills;
		totalAllKills += pKills;
		var pDeaths = getSval(player,20860,"deathCount");
		if (!pDeaths)
			pDeaths = 0;
		totalDeaths += pDeaths;
		totalAllDeaths += pDeaths;
		var pAssists = getSval(player,20860,"assistCount");
		if (!pAssists)
			pAssists = 0;
		totalAssists += pAssists;
		totalAllAssists += pAssists;
		var pCaps = getSval(player,20930,"flagCaps");
		if (!pCaps)
			pCaps = 0;
		var pRets = getSval(player,20930,"flagRets");
		if (!pRets)
			pRets = 0;
		var smobKills = getSval(player,20860,"smobKills");
		if (!smobKills)
			smobKills = 0;
		if (global.arenaCurrentGame == global.arenaGameCTF) {
			var pPoints = yel+"    "+pCaps+"        "+pRets+nrm;
			var tPoints = yel+"    "+global.arenaCurrentGame.yellowCaptures+"        "+global.arenaCurrentGame.yellowReturns+nrm;	
		}
		else if (global.arenaCurrentGame == global.arenaGameKH) {
			var pPoints = yel+"  "+getTimeDisplay(getSval(player,20799,"khTime"))+nrm;
			var tPoints = yel+"  "+getTimeDisplay(global.arenaCurrentGame.yellowHillTimer)+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameSM) {
			var pPoints = yel+"  "+smobKills+nrm;
			var tPoints = yel+"  "+global.arenaCurrentGame.yellowSmobKills+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameLMS) {
			var lLeft = ( global.arenaCurrentGame.limit - ( getSval(player,20860,"deathCount") ? getSval(player,20860,"deathCount") : 0) );
			var pPoints = yel+( lLeft>0 ? "    "+lLeft : "Eliminated" )+nrm;
		}
		else { //No specifics, show ranks instead.
			var pPoints = yel+getArenaRank(player)+nrm;
		}
		actor.send("  "+strPadding(pName," ",16,"right")+"|"+yel+strPadding(pKills," ",6,"left")+strPadding(pDeaths," ",6,"left")+"    "+strPadding(pAssists," ",6,"right")+nrm+"|  "+(pPoints ? pPoints : ""));
	}
	if (global.arenaFreeForAll == false && global.arenaTeams > 2) {
		//actor.send("_________________|___________________|____________________");
		actor.send("                 |                   |");
		actor.send(yel+"  YELLOW TEAM    "+nrm+"|"+yel+strPadding(totalKills," ",6,"left")+strPadding(totalDeaths," ",6,"left")+"    "+strPadding(totalAssists," ",6,"right")+nrm+"|  "+(tPoints ? tPoints : ""));
		//actor.send(" ");
		if (global.arenaTeams == 4) {
			//actor.send(" ");
			//actor.send("  Players        |    K    D    A    |  "+(global.arenaCurrentGame.goalType ? global.arenaCurrentGame.goalType : ""));
			actor.send("_________________|___________________|____________________");
			actor.send("                 |                   |");
		}
	}
	var totalKills = 0;
	var totalDeaths = 0;
	var totalAssists = 0;
	for (var _autoKey in greenArr) {
		var player = greenArr[_autoKey];
		var pName = grn+player.name+nrm;
		var pKills = getSval(player,20860,"killCount");
		if (!pKills)
			pKills = 0;
		totalKills += pKills;
		totalAllKills += pKills;
		var pDeaths = getSval(player,20860,"deathCount");
		if (!pDeaths)
			pDeaths = 0;
		totalDeaths += pDeaths;
		totalAllDeaths += pDeaths;
		var pAssists = getSval(player,20860,"assistCount");
		if (!pAssists)
			pAssists = 0;
		totalAssists += pAssists;
		totalAllAssists += pAssists;
		var pCaps = getSval(player,20930,"flagCaps");
		if (!pCaps)
			pCaps = 0;
		var pRets = getSval(player,20930,"flagRets");
		if (!pRets)
			pRets = 0;
		var smobKills = getSval(player,20860,"smobKills");
		if (!smobKills)
			smobKills = 0;
		if (global.arenaCurrentGame == global.arenaGameCTF) {
			var pPoints = grn+"    "+pCaps+"        "+pRets+nrm;
			var tPoints = grn+"    "+global.arenaCurrentGame.greenCaptures+"        "+global.arenaCurrentGame.greenReturns+nrm;	
		}
		else if (global.arenaCurrentGame == global.arenaGameKH) {
			var pPoints = grn+"  "+getTimeDisplay(getSval(player,20799,"khTime"))+nrm;
			var tPoints = grn+"  "+getTimeDisplay(global.arenaCurrentGame.greenHillTimer)+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameSM) {
			var pPoints = grn+"  "+smobKills+nrm;
			var tPoints = grn+"  "+global.arenaCurrentGame.greenSmobKills+nrm;
		}
		else if (global.arenaCurrentGame == global.arenaGameLMS) {
			var lLeft = ( global.arenaCurrentGame.limit - ( getSval(player,20860,"deathCount") ? getSval(player,20860,"deathCount") : 0) );
			var pPoints = grn+( lLeft>0 ? "    "+lLeft : "Eliminated" )+nrm;
		}
		else { //No specifics, show ranks instead.
			var pPoints = grn+getArenaRank(player)+nrm;
		}
		actor.send("  "+strPadding(pName," ",16,"right")+"|"+grn+strPadding(pKills," ",6,"left")+strPadding(pDeaths," ",6,"left")+"    "+strPadding(pAssists," ",6,"right")+nrm+"|  "+(pPoints ? pPoints : ""));
	}
	// if (global.arenaFreeForAll == true || global.arenaTeams == 4) {
		// actor.send("_________________|___________________|____________________");
		// actor.send("                 |                   |");
	// }
	if (global.arenaCurrentGame == global.arenaGameKH) {
		var totalTime = 0;
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			var playTime = getSval(player,20799,"khTime");
			if (!playTime)
				playTime = 0;
			totalTime += playTime;
		}
		var ffaPoints = mag+"  "+getTimeDisplay(totalTime)+nrm;	
	}
	else if (global.arenaCurrentGame == global.arenaGameSM) {
		var ffaPoints = mag+"  "+(global.arenaCurrentGame.redSmobKills+global.arenaCurrentGame.blueSmobKills+global.arenaCurrentGame.yellowSmobKills+global.arenaCurrentGame.greenSmobKills)+nrm;	
	}
	else if (global.arenaCurrentGame == global.arenaGameLMS) {
		var ffaPoints = 0;
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			var lLeft = ( global.arenaCurrentGame.limit - ( getSval(player,20860,"deathCount") ? getSval(player,20860,"deathCount") : 0) );
			ffaPoints += ( lLeft>0 ? lLeft : 0);
		}
		ffaPoints = mag+"    "+ffaPoints+nrm;
	}
	if (global.arenaFreeForAll == false) {
		if (global.arenaTeams == 4) {
			actor.send("                 |                   |");
			actor.send(grn+"  GREEN TEAM     "+nrm+"|"+grn+strPadding(totalKills," ",6,"left")+strPadding(totalDeaths," ",6,"left")+"    "+strPadding(totalAssists," ",6,"right")+nrm+"|  "+(tPoints ? tPoints : ""));
		}
	}
	else {
		actor.send("_________________|___________________|____________________");
		actor.send("                 |                   |");
		actor.send("  Totals         |"+mag+strPadding((totalAllKills)," ",6,"left")+strPadding((totalAllDeaths)," ",6,"left")+"    "+strPadding((totalAllAssists)," ",6,"right")+nrm+"|  "+(ffaPoints ? ffaPoints : ""));
	}
	actor.send(" ");
}
/** ARENA BROADCAST MESSAGE **/
function arenaEcho(strMsg, arrExc) {
	var finalArr = [];
	for (var _autoKey in global.arenaPlayers) {
		var per = global.arenaPlayers[_autoKey];
		if (!arrContains(arrExc, per))
			finalArr.push(per);
	}
	for (var _autoKey in global.arenaWatchers) {
		var play = global.arenaWatchers[_autoKey];
		if (!arrContains(arrExc, play))
			finalArr.push(play);
	}
	for (var _autoKey in getWaitingPlayers("all")) {
		var playe = getWaitingPlayers("all")[_autoKey];
		if (!arrContains(arrExc, playe) && !arrContains(finalArr, playe))
			finalArr.push(playe);
	}
	for (var _autoKey in finalArr) {
		var player = finalArr[_autoKey];
		getCharCols(player);
		player.send(mag+strMsg+nrm);
	}
}
/** ADDING ARENA POINTS TO PERMANENT STATS **/
function addArenaPoint(actor, strType, special) {
	if (!actor || !strType) {
		mudLog(3,102,"addArenaPoint() called without valid argument(s)");
		return;
	}
	var arenaStats = actor.getPval("arenaStats");
	if (!arenaStats)		    //  W W L L K D
		actor.setPval("arenaStats","0~0~0~0~0~0",true);
	var arenaStats = actor.getPval("arenaStats");
	arenaStats = arenaStats.split("~");
	var arenaStats2 = actor.getPval("arenaStats2");
	if (!arenaStats2)		     //  S B C R H D T B
		actor.setPval("arenaStats2","0~0~0~0~0~0~0~0",true);
	var arenaStats2 = actor.getPval("arenaStats2");
	arenaStats2 = arenaStats2.split("~");
	var arenaStats3 = actor.getPval("arenaStats3");
	if (!arenaStats3)		     //  M S D A
		actor.setPval("arenaStats3","0~0~0~0",true);
	var arenaStats3 = actor.getPval("arenaStats3");
	arenaStats3 = arenaStats3.split("~");
	var teamWins = parseInt(arenaStats[0]);
	var soloWins = parseInt(arenaStats[1]);
	var teamLosses = parseInt(arenaStats[2]);
	var soloLosses = parseInt(arenaStats[3]);
	var pKills = parseInt(arenaStats[4]);
	var pDeaths = parseInt(arenaStats[5]);
	var suicides = parseInt(arenaStats2[0]);
	var betrayals = parseInt(arenaStats2[1]);
	var flagCaptures = parseInt(arenaStats2[2]);
	var flagReturns = parseInt(arenaStats2[3]);
	var hillTime = parseInt(arenaStats2[4]);
	var doubleKills = parseInt(arenaStats2[5]);
	var tripleKills = parseInt(arenaStats2[6]);
	var bails = parseInt(arenaStats2[7]);
	var smobKills = parseInt(arenaStats3[0]);
	var totalScore = parseInt(arenaStats3[1]);
	var mobDeaths = parseInt(arenaStats3[2]);
	var assists = parseInt(arenaStats3[3]);
	var currentRank = new Object();
		currentRank.name = getArenaRank(actor);
		currentRank.minScore = getArenaScoreRange(currentRank.name)[0];
		currentRank.maxScore = getArenaScoreRange(currentRank.name)[1];
	var numAI = 0;
	var numPC = 0;
	for (var _autoKey in global.arenaPlayers) {
		var perp = global.arenaPlayers[_autoKey];
		if (isAI(perp))
			numAI += 1;
		else
			numPC += 1;
	}
	if (strType == "teamWins") {
		teamWins += 1;
		var addScore = 10-numAI+numPC;
		totalScore += (addScore > 0 ? addScore : 0);
	}
	else if (strType == "soloWins") {
		soloWins += 1;
		var addScore = 15-numAI+numPC;
		totalScore += (addScore > 0 ? addScore : 0);
	}
	else if (strType == "teamLosses") {
		teamLosses += 1;
		var addScore = random(3,5)-numAI+numPC;
		totalScore += (addScore > 0 ? addScore : 0);
	}
	else if (strType == "soloLosses") {
		soloLosses += 1;
		var addScore = random(3,7)-numAI+numPC;
		totalScore += (addScore > 0 ? addScore : 0);
	}
	else if (strType == "pKills") {
		pKills += 1;
		if (special == "ai")
			totalScore += 1;
		else
			totalScore += 4;
	}
	else if (strType == "pDeaths") {
		pDeaths += 1;
		//no score
	}
	else if (strType == "suicides") {
		suicides += 1;
		totalScore -= getScoreModifier(actor);
	}
	else if (strType == "betrayals") {
		betrayals += 1;
		totalScore -= (10*getScoreModifier(actor));
	}
	else if (strType == "flagCaptures") {
		flagCaptures += 1;
		totalScore += 5;
	}
	else if (strType == "flagReturns") {
		flagReturns += 1;
		totalScore += 3;
	}
	else if (strType == "hillTime") {
		hillTime += special;
		totalScore += (Math.floor(special/150));
	}
	else if (strType == "smobKills") {
		smobKills += 1;
		totalScore += 2;
	}
	else if (strType == "doubleKills") {
		doubleKills += 1;
		totalScore += 5;
	}
	else if (strType == "tripleKills") {
		tripleKills += 1;
		totalScore += 10;
	}
	else if (strType == "mobDeaths") {
		mobDeaths += 1;
		totalScore -= (Math.floor(getScoreModifier(actor)/2));
	}
	else if (strType == "bails") {
		bails += 1;
		if (special == true) // player bailed on purpose
			totalScore -= (10*getScoreModifier(actor));
		else if (special == false) // player was kicked
			totalScore -= (5*getScoreModifier(actor));
	}
	else if (strType == "assists") {
		assists += 1;
		if (special != "ai")
			totalScore += 2;
	}
	else {
		mudLog(3,102,"addArenaPoint() called without valid type");
		return;
	}
	actor.setPval("arenaStats",teamWins+"~"+soloWins+"~"+teamLosses+"~"+soloLosses+"~"+pKills+"~"+pDeaths,true);
	actor.setPval("arenaStats2",suicides+"~"+betrayals+"~"+flagCaptures+"~"+flagReturns+"~"+hillTime+"~"+doubleKills+"~"+tripleKills+"~"+bails,true);
	actor.setPval("arenaStats3",smobKills+"~"+totalScore+"~"+mobDeaths+"~"+assists,true);
	var newScore = getArenaScore(actor);
	if (newScore > currentRank.maxScore)
		updateArenaRank(actor, "up");
	else if (newScore < currentRank.minScore)
		updateArenaRank(actor, "down");
}
/** GET PLAYER'S ARENA PERMANENT STATS **/
function getArenaPermStats(actor) {
	getCharCols(actor);
	var arenaStats = actor.getPval("arenaStats");
	if (!arenaStats)		    //  W W L L K D
		actor.setPval("arenaStats","0~0~0~0~0~0",true);
	var arenaStats = actor.getPval("arenaStats");
	arenaStats = arenaStats.split("~");
	var arenaStats2 = actor.getPval("arenaStats2");
	if (!arenaStats2)		     //  S B C R H D T B
		actor.setPval("arenaStats2","0~0~0~0~0~0~0~0",true);
	var arenaStats2 = actor.getPval("arenaStats2");
	arenaStats2 = arenaStats2.split("~");
	var arenaStats3 = actor.getPval("arenaStats3");
	if (!arenaStats3)		     //  M S D A
		actor.setPval("arenaStats3","0~0~0~0",true);
	var arenaStats3 = actor.getPval("arenaStats3");
	arenaStats3 = arenaStats3.split("~");
	var teamWins = parseInt(arenaStats[0]);
	if (isNaN(teamWins)) { teamWins = 0; }
	var soloWins = parseInt(arenaStats[1]);
	if (isNaN(soloWins)) { soloWins = 0; }
	var teamLosses = parseInt(arenaStats[2]);
	if (isNaN(teamLosses)) { teamLosses = 0; }
	var soloLosses = parseInt(arenaStats[3]);
	if (isNaN(soloLosses)) { soloLosses = 0; }
	var pKills = parseInt(arenaStats[4]);
	if (isNaN(pKills)) { pKills = 0; }
	var pDeaths = parseInt(arenaStats[5]);
	if (isNaN(pDeaths)) { pDeaths = 0; }
	var suicides = parseInt(arenaStats2[0]);
	if (isNaN(suicides)) { suicides = 0; }
	var betrayals = parseInt(arenaStats2[1]);
	if (isNaN(betrayals)) { betrayals = 0; }
	var flagCaptures = parseInt(arenaStats2[2]);
	if (isNaN(flagCaptures)) { flagCaptures = 0; }
	var flagReturns = parseInt(arenaStats2[3]);
	if (isNaN(flagReturns)) { flagReturns = 0; }
	var hillTime = parseInt(arenaStats2[4]);
	if (isNaN(hillTime)) { hillTime = 0; }
	var doubleKills = parseInt(arenaStats2[5]);
	if (isNaN(doubleKills)) { doubleKills = 0; }
	var tripleKills = parseInt(arenaStats2[6]);
	if (isNaN(tripleKills)) { tripleKills = 0; }
	var bails = parseInt(arenaStats2[7]);
	if (isNaN(bails)) { bails = 0; }
	var smobKills = parseInt(arenaStats3[0]);
	if (isNaN(smobKills)) { smobKills = 0; }
	var totalScore = parseInt(arenaStats3[1]);
	if (isNaN(totalScore)) { totalScore = 0; }
	var mobDeaths = parseInt(arenaStats3[2]);
	if (isNaN(mobDeaths)) { mobDeaths = 0; }
	var assists = parseInt(arenaStats3[3]);
	//sendKoradin(actor.name + " assists: "+assists);
	if (isNaN(assists)) { assists = 0; }
	var scoreTillNext = (getArenaScoreRange(getArenaRank(actor))[1]+1);
	actor.send(" ");
	actor.send(" ");
	actor.send(mag+"                       Personal Arena Stats"+nrm);
	actor.send("___________________________________________________________________");
	actor.send(" ");
	actor.send("             Rank  :  "+mag+strPadding(mag+getArenaRank(actor)+nrm," ",20,"right")+nrm+"   Score  :  "+mag+getArenaScore(actor)+" / "+scoreTillNext+nrm);
	actor.send(" ");
	actor.send("    TOTAL MATCHES  :  "+mag+strPadding((teamWins+teamLosses+soloWins+soloLosses)," ",16,"right")+nrm+" TOTAL KILLS  :  "+mag+(pKills+smobKills)+nrm); 
	actor.send(" "); 
	actor.send("  Regular Matches  :  "+mag+strPadding((teamWins+teamLosses)," ",16,"right")+nrm+"Player Kills  :  "+mag+pKills+nrm);
	actor.send("        Team Wins  :  "+mag+strPadding(teamWins," ",16,"right")+nrm+"Double Kills  :  "+mag+doubleKills+nrm);
	actor.send("      Team Losses  :  "+mag+strPadding(teamLosses," ",16,"right")+nrm+"Triple Kills  :  "+mag+tripleKills+nrm);
	actor.send("                                     "+"  Smob Kills  :  "+mag+smobKills+nrm);
	actor.send("      FFA Matches  :  "+mag+strPadding((soloWins+soloLosses)," ",16,"right")+nrm+"   Betrayals  :  "+mag+betrayals+nrm);
	actor.send("        Solo Wins  :  "+mag+strPadding(soloWins," ",16,"right")+nrm+"     Assists  :  "+mag+assists+nrm);
	actor.send("      Solo Losses  :  "+mag+strPadding(soloLosses," ",16,"right")+nrm);
	actor.send("                                     "+"TOTAL DEATHS  :  "+mag+(pDeaths+mobDeaths+suicides)+nrm);
	actor.send("    Flag Captures  :  "+mag+strPadding(flagCaptures," ",15,"right")+nrm);
	actor.send("     Flag Returns  :  "+mag+strPadding(flagReturns," ",15,"right")+nrm+"Player Deaths  :  "+mag+pDeaths+nrm);
	actor.send("        Hill Time  :  "+mag+strPadding(getTimeDisplay(hillTime, "minutes")," ",16,"right")+nrm+"  Mob Deaths  :  "+mag+mobDeaths+nrm);
	actor.send("            Bails  :  "+mag+strPadding(bails," ",16,"right")+nrm+"    Suicides  :  "+mag+suicides+nrm);
	actor.send("___________________________________________________________________");
	actor.send(" ");
	actor.send("          Type "+mag+"arena help"+nrm+" to learn more about the Arena.");
}
/** GET PLAYER'S ARENA SCORE **/
function getArenaScore(actor) {
	var arenaStats3 = actor.getPval("arenaStats3");
	if (!arenaStats3)		     //  M S D A
		actor.setPval("arenaStats3","0~0~0~0",true);
	var arenaStats3 = actor.getPval("arenaStats3");
	arenaStats3 = arenaStats3.split("~");
	return parseInt(arenaStats3[1]);
}
/** GET SCORE MODIFIER (RANK-BASED) **/
function getScoreModifier(actor) { // Used to help calculate arena score points
	var ranks = ["Unranked","Conscript","Scout","Soldier","Second Squadman","Squadman","File Leader","Patrolman","Patrol Leader","Second Bannerman","Bannerman","Hundredman","Second Sergeant","Sergeant","First Sergeant","Under-Lieutenant","Lieutenant","Senior Lieutenant","Second Captain","Captain","First Captain","Lord Captain","General","Banner-General","Lieutenant-General","Captain-General","Marshal-General","Lord General","King","King of Kings","Godlike"];
	for (i=0; i<ranks.length; i++) {
		if (getArenaRank(actor) == ranks[i]) {
			var points = i;
			break;
		}
	}
	if (points == 0 || !points)
		points = 1;
	return points;
}
/** GET PLAYER'S ARENA RANK **/
function getArenaRank(actor) {
	var score = getArenaScore(actor);
	if (score < 10)
		return "Unranked";
	else if (score < 25)
		return "Conscript";
	else if (score < 50)
		return "Scout";
	else if (score < 100)
		return "Soldier";
	else if (score < 200)
		return "Second Squadman";
	else if (score < 350)
		return "Squadman";
	else if (score < 500)
		return "File Leader";
	else if (score < 750)
		return "Patrolman";
	else if (score < 1000)
		return "Patrol Leader";
	else if (score < 1300)
		return "Second Bannerman";
	else if (score < 1750)
		return "Bannerman";
	else if (score < 2250)
		return "Hundredman";
	else if (score < 3000)
		return "Second Sergeant";
	else if (score < 3750)
		return "Sergeant";
	else if (score < 5000)
		return "First Sergeant";
	else if (score < 6500)
		return "Under-Lieutenant";
	else if (score < 8250)
		return "Lieutenant";
	else if (score < 10000)
		return "Senior Lieutenant";
	else if (score < 12000)
		return "Second Captain";
	else if (score < 15000)
		return "Captain";
	else if (score < 20000)
		return "First Captain";
	else if (score < 30000)
		return "Lord Captain";
	else if (score < 50000)
		return "General";
	else if (score < 75000)
		return "Banner-General";
	else if (score < 125000)
		return "Lieutenant-General";
	else if (score < 200000)
		return "Captain-General";
	else if (score < 300000)
		return "Marshal-General";
	else if (score < 500000)
		return "Lord General";
	else if (score < 750000)
		return "King";
	else if (score < 1000000)
		return "King of Kings";
	else
		return "Godlike";
}
function getRankByScore(score) {
	if (score < 10)
		return "Unranked";
	else if (score < 25)
		return "Conscript";
	else if (score < 50)
		return "Scout";
	else if (score < 100)
		return "Soldier";
	else if (score < 200)
		return "Second Squadman";
	else if (score < 350)
		return "Squadman";
	else if (score < 500)
		return "File Leader";
	else if (score < 750)
		return "Patrolman";
	else if (score < 1000)
		return "Patrol Leader";
	else if (score < 1300)
		return "Second Bannerman";
	else if (score < 1750)
		return "Bannerman";
	else if (score < 2250)
		return "Hundredman";
	else if (score < 3000)
		return "Second Sergeant";
	else if (score < 3750)
		return "Sergeant";
	else if (score < 5000)
		return "First Sergeant";
	else if (score < 6500)
		return "Under-Lieutenant";
	else if (score < 8250)
		return "Lieutenant";
	else if (score < 10000)
		return "Senior Lieutenant";
	else if (score < 12000)
		return "Second Captain";
	else if (score < 15000)
		return "Captain";
	else if (score < 20000)
		return "First Captain";
	else if (score < 30000)
		return "Lord Captain";
	else if (score < 50000)
		return "General";
	else if (score < 75000)
		return "Banner-General";
	else if (score < 125000)
		return "Lieutenant-General";
	else if (score < 200000)
		return "Captain-General";
	else if (score < 300000)
		return "Marshal-General";
	else if (score < 500000)
		return "Lord General";
	else if (score < 750000)
		return "King";
	else if (score < 1000000)
		return "King of Kings";
	else
		return "Godlike";
}
/** RETURN RANK SCORE RANGE **/
function getArenaScoreRange(strRank) {
	if (strRank == "Unranked")
		return [-1000000,9];
	else if (strRank == "Conscript")
		return [10,24];
	else if (strRank == "Scout")
		return [25,49];
	else if (strRank == "Soldier")
		return [50,99];
	else if (strRank == "Second Squadman")
		return [100,199];
	else if (strRank == "Squadman")
		return [200,349];
	else if (strRank == "File Leader")
		return [350,499];
	else if (strRank == "Patrolman")
		return [500,749];
	else if (strRank == "Patrol Leader")
		return [750,999];
	else if (strRank == "Second Bannerman")
		return [1000,1299];
	else if (strRank == "Bannerman")
		return [1300,1749];
	else if (strRank == "Hundredman")
		return [1750,2249];
	else if (strRank == "Second Sergeant")
		return [2250,2999];
	else if (strRank == "Sergeant")
		return [3000,3749];
	else if (strRank == "First Sergeant")
		return [3750,4999];
	else if (strRank == "Under-Lieutenant")
		return [5000,6499];
	else if (strRank == "Lieutenant")
		return [6500,8249];
	else if (strRank == "Senior Lieutenant")
		return [8250,9999];
	else if (strRank == "Second Captain")
		return [10000,11999];
	else if (strRank == "Captain")
		return [12000,14999];
	else if (strRank == "First Captain")
		return [15000,19999];
	else if (strRank == "Lord Captain")
		return [20000,29999];
	else if (strRank == "General")
		return [30000,49999];
	else if (strRank == "Banner-General")
		return [50000,74999];
	else if (strRank == "Lieutenant-General")
		return [75000,124999];
	else if (strRank == "Captain-General")
		return [125000,199999];
	else if (strRank == "Marshal-General")
		return [200000,299999];
	else if (strRank == "Lord General")
		return [300000,499999];
	else if (strRank == "King")
		return [500000,749999];
	else if (strRank == "King of Kings")
		return [750000,999999];
	else if (strRank == "Godlike")
		return [1000000,1499999];
		
	return null;
	//ADD MORE RANKS HERE
}
/** RAISE / LOWER ACTOR'S ARENA RANK **/
function updateArenaRank(actor, strDir) {
	if (!actor || !strDir)
		return;
	var players = global.arenaPlayers;
	var rankName = getArenaRank(actor);
	var ranks = ["Unranked","Conscript","Scout","Soldier","Second Squadman","Squadman","File Leader","Patrolman","Patrol Leader","Second Bannerman","Bannerman","Hundredman","Second Sergeant","Sergeant","First Sergeant","Under-Lieutenant","Lieutenant","Senior Lieutenant","Second Captain","Captain","First Captain","Lord Captain","General","Banner-General","Lieutenant-General","Captain-General","Marshal-General","Lord General","King","King of Kings","Godlike"];
	for (i=0; i<ranks.length; i++) {
		if (rankName == ranks[i]) {
			if (strDir == "up") {
				var newI = i;
				var proDem = "promoted";
			}
			else if (strDir == "down") {
				var newI = i;
				var proDem = "demoted";
			}
			else {
				mudLog(3,102,"updateArenaRank() called with invalid promotion/demotion string");
				return;
			}
			if (!ranks[newI])
				return;
			if (proDem == "promoted")
				actor.unlockTitle(ranks[newI], "normal", "arena");
			else if (proDem == "demoted")
				actor.lockTitle(ranks[newI+1], "normal");
			arenaEcho(actor.name+" has been "+proDem+" to the rank of "+ranks[newI]+"!");
			return;
		}
	}
}
/** KICK PLAYER **/
function kickArenaPlayer(bool) {
	arenaEcho(" ");
	var actor = global.arenaKickPlayer;
	if (bool == true) { // Kick player
		actor.unaffect(constants.AFF_NOQUIT);
		actor.comm("exit arena");
	}
	else {
		arenaEcho(actor.name+" will not be kicked out of the match.");
	}
	function afterWait(args) {
		global.arenaKickYes = 0;
		global.arenaKickNo = 0;
		global.arenaKickPlayer = null;
		global.arenaKickTimer = 0;
		putMobsToSleep(false);
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			setSval(player, 14223, "kickVote", 0);
			player.detach(14222);
			player.detach(14223);
		}
	}
	setTimeout(1, afterWait, [actor]);
}
/** GET SPECIFIC WAGER **/
function getWager(actor) {
	var wWps = 0;
	var wFps = 0;
	var wGold = 0;
	var wItem = null;
	if (!actor) {
		mudLog(3,102,"getWager() called without an actor or team");
		return null;
	}
	else if (actor > 0 && actor < 5) { //TEAM game
		var team = actor;
		var teamName = getTeamName(team);
		for (var _autoKey in getWaitingPlayers("pc")) {
			var person = getWaitingPlayers("pc")[_autoKey];
			if (person.arenaTeam == team) {
				for (var _autoKey in global.arenaCurrentWagers) {
					var wager = global.arenaCurrentWagers[_autoKey];
					if (wager[0] == person) {
						if (wager[2] == 3)
							wGold += parseInt(wager[3]);
						else if (wager[2] == 1)
							wWps += parseInt(wager[3]);
						else if (wager[2] == 2)
							wFps += parseInt(wager[3]);
					}
				}
			}
			var finalActor = person;
		}
	}
	else { // FFA game
		getCharCols(actor);
		var finalActor = actor;
		for (var _autoKey in global.arenaCurrentWagers) {
			var wager = global.arenaCurrentWagers[_autoKey];
			if (wager[0] == actor) {
				if (wager[2] == 3)
					wGold = wager[3];
				else if (wager[2] == 1)
					wWps = wager[3];
				else if (wager[2] == 2)
					wFps = wager[3];
				else if (wager[2] == 4)
					wItem = wager[3];
			}
		}
	}
	if (wGold > 0) { //wagered GOLD
		wGold = finalActor.numCopperToText(wGold,false);
		return wGold;
	}
	else if (wWps > 0) { //wagered WPS
		return wWps+" weave point"+(wWps>1?"s":"");
	}
	else if (wFps > 0) { //wagered FPS
		return wFps+" feat point"+(wFps>1?"s":"");
	}
	else if (wItem) { //wagered an ITEM
		return wItem.name;
	}
	else { //didn't wager anything
		return "Nothing";
	}
}
/** GET CURRENT MATCH WAGERS **/
function getCurrentWagers(actor) {
	getCharCols(actor);
	actor.send(mag+"Current Match Wagers"+nrm);
	actor.send(" ");
	if (global.arenaStage < constants.ARENA_QUEUE) // no match
		return;
	if (global.arenaFreeForAll == false) { // TEAM game, list teams
		var yelGreen = nrm+"";
		var yelGreenWagers = "";
		if (global.arenaTeams > 2) {
			yelGreen += (yel+"Yellow Team         "+nrm);
			yelGreenWagers += strPadding(getWager(3)," ",21,"right");
		}
		if (global.arenaTeams > 3) {
			yelGreen += (grn+"Green Team"+nrm);
			yelGreenWagers += strPadding(getWager(4)," ",21,"right");
		}
		actor.send(cyn+"Blue Team           "+red+"Red Team            "+yelGreen);
		actor.send(strPadding(getWager(1)," ",21,"right")+strPadding(getWager(2)," ",21,"right")+yelGreenWagers)
		return;
	}
	else { // FFA game, list players
		for (var _autoKey in getWaitingPlayers("pc")) {
			var player = getWaitingPlayers("pc")[_autoKey];
			if (player.race == constants.RACE_TROLLOC)
				var raceCol = red;
			else if (player.race == constants.RACE_HUMAN)
				var raceCol = cyn;
			else if (player.race == constants.RACE_AIEL)
				var raceCol = yel;
			else if (player.race == constants.RACE_SEANCHAN)
				var raceCol = grn;
			if (player.race != actor.race && global.arenaStage < constants.ARENA_PLAYING)
				var playName = "Someone";
			else
				var playName = player.name;
			actor.send(raceCol+playName+":  "+nrm+getWager(player));
		}
		return;
	}
}
/** GET WAGER MENU **/
function getWagerMenu(actor) {
	getCharCols(actor);
	if (global.arenaFreeForAll == false) {
		actor.send(" ");
		actor.send("Your current wager: "+nrm+getWager(actor));
	}
	actor.send(" ");
	var menu = getSval(actor,14209,"wagerMenu");
	if (!menu || menu == 0) { // first menu
		if (global.arenaMixedBets == true || global.arenaHost == actor) { // actor is host or mixed betting
			if (getWager(actor) == "Nothing") {
				actor.send("What do you want to bet?");
				actor.send(" ");
				actor.send(mag+"1"+nrm+") Weave Points");
				actor.send(mag+"2"+nrm+") Feat Points");
				actor.send(mag+"3"+nrm+") Gold");
				if (global.arenaFreeForAll == true && global.arenaMixedBets == true)
					actor.send(mag+"4"+nrm+") Item");
			}
			actor.send(mag+"R"+nrm+") Remove Wager");
			actor.send(mag+"C"+nrm+") Cancel");
			actor.send(" ");
			actor.send("Make a selection:");
			actor.attach(14234);
			return;
		}
		else { //host structured betting
			if (getWager(actor) == "Nothing") {
				var hostWager = getWager(global.arenaCurrentWagers[0][0]);
				var hostWagerType = global.arenaCurrentWagers[0][2];
				var hostWagerAmount = global.arenaCurrentWagers[0][3];
				if (hostWagerType == 1)
					var actorType = actor.weavePoints;
				else if (hostWagerType == 2)
					var actorType = actor.featPoints;
				else if (hostWagerType == 3)
					var actorType = actor.gold;
				if (actorType < hostWagerAmount) {
					actor.send("You currently do not have enough to match the host's bet of "+hostWager+".");
					arenaEcho("Someone cannot match the host's bet, and has been removed from the Queue.");
					actor.comm("exit arena");
					return;
				}
				actor.send(red+"Warning: If you enter a wager now, you will be held to it should the match start."+nrm);
				actor.send(red+"Type ("+mag+"C"+red+")ancel to back out of the bet."+nrm);
				actor.send(" ");
				actor.send("Would you like the match the host's bet of "+hostWager+"?");
				actor.send(" ");
				actor.send("["+mag+"Y"+nrm+"] Yes          ["+mag+"N"+nrm+"] No");
				actor.attach(14234);
				return;
			}
			actor.send(mag+"R"+nrm+") Remove Wager");
			actor.send(mag+"C"+nrm+") Cancel");
			actor.send(" ");
			actor.send("Make a selection:");
			actor.attach(14234);
			return;
		}
	}
	else if (menu == 1) {
		actor.send(red+"Warning: If you enter a wager now, you will be held to it should the match start."+nrm);
		actor.send(red+"Type ("+mag+"C"+red+")ancel to back out of the bet."+nrm);
		actor.send(" ");
		actor.send("Enter amount of Weave Points to wager:");
	}
	else if (menu == 2) {
		actor.send(red+"Warning: If you enter a wager now, you will be held to it should the match start."+nrm);
		actor.send(red+"Type ("+mag+"C"+red+")ancel to back out of the bet."+nrm);
		actor.send(" ");
		actor.send("Enter amount of Feat Points to wager:");
	}
	else if (menu == 3) {
		actor.send(red+"Warning: If you enter a wager now, you will be held to it should the match start."+nrm);
		actor.send(red+"Type ("+mag+"C"+red+")ancel to back out of the bet."+nrm);
		actor.send(" ");
		actor.send("Enter amount of Gold (in coppers) to wager:");
	}
	else if (menu == 4) {
		if (actor.inventory.length == 0) {
			actor.send(red+"You have nothing in your inventory to bet."+nrm);
			setSval(actor,14209,"wagerMenu",0);
			getWagerMenu(actor);
			return;
		}
		actor.send(red+"Warning: If you enter a wager now, you will be held to it should the match start."+nrm);
		actor.send(red+"Type ("+mag+"C"+red+")ancel to back out of the bet."+nrm);
		actor.send(" ");
		actor.send("Which item would you like to bet?");
		actor.send(" ");
		for (var i=0; i<actor.inventory.length; i++)
			actor.send(mag+(i+1)+nrm+") "+actor.inventory[i].name);
		actor.send(" ");
		actor.send("Make a selection ("+mag+"1-"+actor.inventory.length+nrm+"):");
	}
}
/** PUT MOBS TO SLEEP / WAKE THEM UP **/
function putMobsToSleep(bool) {
	for (i=global.arenaCurrentMap.min; i<global.arenaCurrentMap.max+1; i++) {
		for (var _autoKey in getRoom(i).people) {
			var mob = getRoom(i).people[_autoKey];
			if (mob.vnum > 0 && !isAI(mob)) {
				if (bool == true)
					mob.position = constants.POS_SLEEPING;
				else
					mob.position = constants.POS_STANDING;
			}
		}
	}
}
/** START ARENA TIMER **/
function startArenaTimer(intSec, strType) {
	if (!intSec || isNaN(intSec) || intSec < 1) {
		mudLog(3,102,"startArenaTimer() called with no or invalid argument");
		return;
	}
	function afterWait(args) {
		var intSec = args[0];
		var strType = args[1];
		if (strType == "Kick Player" && !global.arenaKickPlayer) //player kicked or not already
			return;
		else if (strType == "End Match" && global.arenaPlayers.length == 0) //match ended already
			return;
		else if (strType == "End Match" && global.arenaEndYes+global.arenaEndNo == 0) //match was not ended
			return;
		if (intSec == 10 || intSec == 5) {
			arenaEcho(strType+" Request: "+intSec+" seconds remaining to vote.");
		}
		if (intSec == 0) {
			arenaEcho(strType+" Request: Time's up!");
			arenaEcho(" ");
			if (strType == "Kick Player") {
				if (global.arenaKickYes > global.arenaKickNo) {
					kickArenaPlayer(true);
				}
				else
					kickArenaPlayer(false);
			}
			else if (strType == "End Match") {
				arenaEcho("The vote is not unanimous - let the game go on!");
				global.arenaEndYes = 0;
				global.arenaEndNo = 0;
				global.arenaEndTimer = 0;
				putMobsToSleep(false);
				for (var _autoKey in global.arenaPlayers) {
					var player = global.arenaPlayers[_autoKey];
					setSval(player, 14224, "endVote", 0);
					player.detach(14224);
				}
			}
			return;
		}
	}
	var totalTime = intSec+1;
	var i = 1;
	while (i <= totalTime) {
		setTimeout((i*7), afterWait, [intSec, strType]);
		i += 1;
		intSec -= 1;
	}
}
/** UPDATE ARENA HISTORY (for legend list) **/
function updateArenaHistory(actor, score) {
	if (isAI(actor))
		return;
	var scoreArr = actor.getPval("arenaStats4");
	if (!scoreArr)
		scoreArr = "0~0~0~0~0~0~0";
	scoreArr = scoreArr.split("~");
	//sendKoradin("original: "+scoreArr);
	var shifted = scoreArr.shift();
	//sendKoradin("shifted: "+shifted);
	//sendKoradin("new: "+scoreArr);
	scoreArr.push(score);
	//sendKoradin("after push: "+scoreArr);
	scoreArr = scoreArr.join("~");
	actor.setPval("arenaStats4",scoreArr, true);
}
/** UPDATE ARENA LEGEND TITLE **/
function updateArenaLegendTitle(actor) {
	if (isAI(actor))
		return;
	if (arrContains(global.arenaLegends, actor.name) && actor.checkTitle("Gladiator") == false) {
		actor.unlockTitle("Gladiator","prefix","arena");
	}
	else if (!arrContains(global.arenaLegends, actor.name) && actor.checkTitle("Gladiator") == true) {
		actor.lockTitle("Gladiator","prefix");
	}
}
/** RESET ARENA **/
function resetArena(actor) {
	for (var _autoKey in global.arenaWatchers) {
		var player = global.arenaWatchers[_autoKey]; //send stats to observors
		if (!isAI(player))
			getArenaStats(player);
		player.send(" ");
		player.comm("arena watch");
	}
	global.arenaStage = constants.ARENA_ON;
	if (actor) { // IMM RESET
		arenaEcho("The Arena has been reset by "+actor.name+"!");
		global.arenaStage = constants.ARENA_OFF;
	}
	for (var _autoKey in getWaitingPlayers("all")) {
		var play = getWaitingPlayers("all")[_autoKey];
		if (!arrContains(global.arenaPlayers,play))
			global.arenaPlayers.push(play);
	}
	for (var _autoKey in global.arenaPlayers) {
		var player = global.arenaPlayers[_autoKey]; // separate loop needed to retain all players in stat screen
		if (!isAI(player))
			getArenaStats(player);
	}
	for (var _autoKey in global.arenaPlayers) {
		var player = global.arenaPlayers[_autoKey];  //Reset players
		if (player) {
		for (var _autoKey in player.inventory) {
			var thing = player.inventory[_autoKey];
			if (arrContains(global.arenaItems, thing.vnum)) //DELETE ARENA-SPECIFIC OBJECTS FROM PLAYER INVENTORIES
				thing.extract();
		}
		if (arrContains(global.arenaZones, player.room.zoneVnum) || arrContains(global.arenaStartRooms, player.room.vnum)) {
			if (player.race == constants.RACE_TROLLOC)
				var nextRoom = 502;
			else if (player.race == constants.RACE_HUMAN)
				var nextRoom = 500;
			if (!isAI(player))
				player.moveToRoom(getRoom(nextRoom));
		}
		player.hps = player.maxHit;
		player.sps = player.maxSpell;
		player.mvs = player.maxMove;
		player.shadowPoints = player.maxShadow;
		player.detach(14212);
		player.detach(14214);
		player.detach(14215);
		player.detach(14222);
		player.detach(14223);
		player.detach(14224);
		player.attach(14228);
		player.detach(14234);
		setSval(player,14210,"arenaQueue",0);
		setSval(player,20860,"killCount",0);
		setSval(player,20860,"deathCount",0);
		setSval(player,20860,"assistCount",0);
		setSval(player,20799,"khTime",0);
		setSval(player,20930,"flagCaps",0);
		setSval(player,20930,"flagRets",0);
		setSval(player,20860,"smobKills",0);
		setSval(player,20860,"doubleKills",0);
		setSval(player,20860,"tripleKills",0);
		setSval(player,20860,"arenaScore",0);
		setSval(player,20860,"respawnTime",0);
		setSval(player,14200,"kickRequest",0);
		setSval(player,14223,"kickVote",0);
		setSval(player,14224,"endVote",0);
		setSval(player,14209,"endGame",0);
		setSval(player,14209,"arenaIssue",0);
		setSval(player,14212,"votePaging",0);
		setSval(player,14209,"gameStart",0);
		setSval(player,14209,"wagerExp",0);
		setSval(player,14209,"wagerWps",0);
		setSval(player,14209,"wagerFps",0);
		setSval(player,14209,"wagerItem",0);
		setSval(player,14209,"wagerMenu",0);
		player.unaffect(constants.AFF_NOQUIT);
		player.arenaTeam = null;
		var startScore = getSval(player,14200,"startingScore");
		if (!startScore)
			startScore = getArenaScore(player);
		var gameScore = getArenaScore(player)-startScore;
		updateArenaHistory(player, gameScore);
		setSval(player,14200,"startingScore",getArenaScore(player));
		if (isAI(player)) {
			player.moveToRoom(getRandomArenaRoom("all"));
			//player.extract();
		}
	}
	}
	for (var i=global.arenaCurrentMap.min;i<global.arenaCurrentMap.max+1;i++) {
		var room = getRoom(i);
		room.purgeMobs();
		room.purgeItems();
	}
	getRoom(parseInt(global.arenaZoneVnum+"00")).zreset();
	global.arenaCurrentWagers = null;
	global.arenaMixedBets = null;
	global.arenaFreeForAll = null;
	global.arenaHost = null;
	global.arenaTotalVotes = null;
	global.arenaCurrentGame = null;
	global.arenaCurrentMap = null;
	global.arenaVotingTimeLimit = null;
	global.arenaKickTimer = 0;
	global.arenaKickPlayer = null;
	global.arenaTotalPlayers = null;
	global.arenaPlayers = null;
	global.arenaWatchers = null;
	global.arenaTeams = null;
	global.arenaAllGames = null;
	global.arenaAllMaps = null;
	global.arenaEndYes = 0;
	global.arenaEndNo = 0;
	global.arenaMinToEnter = null;
	global.arenaZoneVnum = null;
	global.arenaAIBlue = null;
	global.arenaAIRed = null;
	global.arenaAIYellow = null;
	global.arenaAIGreen = null;
	global.arenaAllAI = null;
	global.arenaHill = null;
	global.arenaFlags = [];
	global.arenaStage = null;
	initGlobals();
	setSval(getRoom(500).items[0], 14200, "stall", time());
}

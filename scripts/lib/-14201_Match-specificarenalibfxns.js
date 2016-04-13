//Match-specific lib fxns
//Koradin - July 2011
// hello 
function resetFlag(flag, actor) { //CTF flag return
	if (!flag)
		return;
	if (flag == "all") {
		var bRoom = getRandomArenaRoom("blue");
		bRoom.loadObj(14200);
		global.arenaFlags.push(bRoom.items[0]);
		var rRoom = getRandomArenaRoom("red");
		rRoom.loadObj(14201);
		global.arenaFlags.push(rRoom.items[0]);
		var yRoom = getRandomArenaRoom("yellow");
		yRoom.loadObj(14203);
		global.arenaFlags.push(yRoom.items[0]);
		var gRoom = getRandomArenaRoom("green");
		gRoom.loadObj(14204);
		global.arenaFlags.push(gRoom.items[0]);
		return;
	}
	if (flag.vnum == 14200) {
		var newRoom = getRandomArenaRoom("blue");
	}
	else if (flag.vnum == 14201) {
		var newRoom = getRandomArenaRoom("red");
	}
	else if (flag.vnum == 14203) {
		var newRoom = getRandomArenaRoom("yellow");
	}
	else if (flag.vnum == 14204) {
		var newRoom = getRandomArenaRoom("green");
	}
	flag.moveToRoom(newRoom);
	if (actor)
		actor.detach(14215);
}
function loadHill() {
	var testArr = [];
	for (var i=global.arenaCurrentMap.min;i<(global.arenaCurrentMap.max+1);i++) {
		if (!arrContains(global.arenaCurrentMap.exclusions, i))
			testArr.push(i);
	}
	var newRoom = testArr[ random(0, testArr.length-1) ];
	getRoom(newRoom).loadObj(14202);
	global.arenaHill = getRoom(newRoom).items[0];
	getRoom(newRoom).echo("The ground suddenly erupts, a rocky hill climbing out of the explosion.");
}
function getTimeDisplay(intSec, strTime) {
	var pTime = parseInt(intSec)/10;
	if (!pTime)
		pTime = 0;
	pTime = pTime/60;
	if (strTime == "minutes")
		pTime = pTime/60;
	var tTime = pTime
	pTime = pTime.toString().split(".");
	var pMin = parseInt(pTime[0]);
	var pSec = tTime - pMin;
	if (pSec <= 0)
		pSec = "00";
	else {
		pSec = Math.floor(pSec*60);
		pSec = pSec.toString();
		if (pSec.length == 1)
			pSec = ("0"+pSec);
		else if (pSec.length > 2)
			pSec = pSec.slice(2);
	}
	return pMin+":"+pSec;
}
function getRandomSmobVnum() {
	var allSmobs = [4205,1121,1503,457,19301,1801,10218,5603,1041,370,998];
	return allSmobs[random(0,allSmobs.length-1)];
}
function loadSmobs() {
	var smobRooms = [];
	for (var i=global.arenaCurrentMap.min;i<global.arenaCurrentMap.max+1;i++) {
		if (!arrContains(global.arenaCurrentMap.exclusions, i) && !arrContains(global.arenaCurrentMap.redCityRooms, i) && !arrContains(global.arenaCurrentMap.blueCityRooms, i) && !arrContains(global.arenaCurrentMap.yellowCityRooms, i) && !arrContains(global.arenaCurrentMap.greenCityRooms, i))
			smobRooms.push(i);
	}
	for (var _autoKey in smobRooms) {
		var room = smobRooms[_autoKey];
		global.arenaCurrentGame.totalSmobs += 1;
		getRoom(room).loadMob(getRandomSmobVnum());
		getRoom(room).people[0].attach(14225);
	}
}
function getSmobGameWinner(strType) {
	if (strType == "ffa") {
		var pArr = [];
		var sArr = [];
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			pArr.push([player,(getSval(player,20860,"smobKills") ? getSval(player,20860,"smobKills") : 0)]);

		}
		var topVoteIndex = 0;
		var topVoteValue = 0;
		for(i=0; i<pArr.length; i++) {
			if(pArr[i][1] > topVoteValue) {
				topVoteValue = pArr[i][1];
				topVoteIndex = i;
			}
		}
		for (j=0; j<pArr.length; j++) {
			if (pArr[j][1] == topVoteValue)
				sArr.push(pArr[j][0]);
		}
		if (sArr.length == 1)
			return sArr[0];
		var topKillValue = 0;
		var topKillIndex = 0;
		for (k=0;k<sArr.length;k++) {
			var sKills = getArenaScore(sArr[k]) - (getSval(sArr[k],20860,"arenaScore") ? getSval(sArr[k],20860,"arenaScore") : 0);
			if (sKills > topKillValue) {
				topKillValue = sKills;
				topKillIndex = k;
			}
		}
		return sArr[topKillIndex];
	}
	else if (strType == "reg") {
		var blueKills = 0;
		var redKills = 0;
		var yellowKills = 0;
		var greenKills = 0;
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			var sKills = (getSval(player,20860,"smobKills") ? getSval(player,20860,"smobKills") : 0);
			if (player.arenaTeam == constants.ARENA_BLUE)
				blueKills += sKills;
			else if (player.arenaTeam == constants.ARENA_RED)
				redKills += sKills;
			else if (player.arenaTeam == constants.ARENA_YELLOW)
				yellowKills += sKills;
			else if (player.arenaTeam == constants.ARENA_GREEN)
				greenKills += sKills;
		}
		var killArr = [[blueKills,constants.ARENA_BLUE], [redKills,constants.ARENA_RED], [greenKills,constants.ARENA_GREEN], [yellowKills,constants.ARENA_YELLOW]];
		var topIndex = 0;
		var topValue = 0;
		for (var i = 0; i < killArr.length; i++) {
			if (killArr[i][0] > topValue) {
				topValue = killArr[i][0];
				topIndex = i;
			}
		}
		var winArr = [];
		for (var j=0; j<killArr.length; j++) {
			if (killArr[j][0] == topValue)
				winArr.push(killArr[j][1]);
		}
		if (winArr.length == 1) {
			return winArr[0];
		}
		else { //smob kills are the same, go by score
			var topKillValue = 0;
			var topKillIndex = 0;
			var blue = 0;
			var red = 0;
			var yellow = 0;
			var green = 0;
			for (var _autoKey in global.arenaPlayers) {
				var player = global.arenaPlayers[_autoKey];
				if (arrContains(winArr, player.arenaTeam)) {
					if (player.arenaTeam == constants.ARENA_BLUE)
						blue += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
					else if (player.arenaTeam == constants.ARENA_RED)
						red += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
					else if (player.arenaTeam == constants.ARENA_YELLOW)
						yellow += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
					else if (player.arenaTeam == constants.ARENA_GREEN)
						green += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
				}
			}
			var finalArr = [[blue,constants.ARENA_BLUE],[red,constants.ARENA_RED],[yellow,constants.ARENA_YELLOW],[green,constants.ARENA_GREEN]];
			for (i=0; i<finalArr.length; i++) {
				if (finalArr[i][0] > topKillValue) {
					topKillValue = finalArr[i][0];
					topKillIndex = i;
				}
			}
			return finalArr[topKillIndex][1];
		}
	}
	
	return null;
}
function getTimeGameWinner(strType) {
	if (strType == "ffa") {
		var pArr = [];
		var sArr = [];
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			pArr.push([player,(getSval(player,20860,"killCount") ? getSval(player,20860,"killCount") : 0)]);

		}
		var topVoteIndex = 0;
		var topVoteValue = 0;
		for(i=0; i<pArr.length; i++) {
			if(pArr[i][1] > topVoteValue) {
				topVoteValue = pArr[i][1];
				topVoteIndex = i;
			}
		}
		for (j=0; j<pArr.length; j++) {
			if (pArr[j][1] == topVoteValue)
				sArr.push(pArr[j][0]);
		}
		if (sArr.length == 1)
			return sArr[0];
		var topKillValue = 0;
		var topKillIndex = 0;
		for (k=0;k<sArr.length;k++) {
			var sKills = getArenaScore(sArr[k]) - (getSval(sArr[k],20860,"arenaScore") ? getSval(sArr[k],20860,"arenaScore") : 0);
			if (sKills > topKillValue) {
				topKillValue = sKills;
				topKillIndex = k;
			}
		}
		return sArr[topKillIndex];
	}
	else if (strType == "reg") {
		var blueKills = 0;
		var redKills = 0;
		var yellowKills = 0;
		var greenKills = 0;
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			var sKills = (getSval(player,20860,"killCount") ? getSval(player,20860,"killCount") : 0);
			if (player.arenaTeam == constants.ARENA_BLUE)
				blueKills += sKills;
			else if (player.arenaTeam == constants.ARENA_RED)
				redKills += sKills;
			else if (player.arenaTeam == constants.ARENA_YELLOW)
				yellowKills += sKills;
			else if (player.arenaTeam == constants.ARENA_GREEN)
				greenKills += sKills;
		}
		var killArr = [[blueKills,constants.ARENA_BLUE], [redKills,constants.ARENA_RED], [greenKills,constants.ARENA_GREEN], [yellowKills,constants.ARENA_YELLOW]];
		var topIndex = 0;
		var topValue = 0;
		for (var i = 0; i < killArr.length; i++) {
			if (killArr[i][0] > topValue) {
				topValue = killArr[i][0];
				topIndex = i;
			}
		}
		var winArr = [];
		for (var j=0; j<killArr.length; j++) {
			if (killArr[j][0] == topValue)
				winArr.push(killArr[j][1]);
		}
		if (winArr.length == 1) {
			return winArr[0];
		}
		else { //smob kills are the same, go by score
			var topKillValue = 0;
			var topKillIndex = 0;
			var blue = 0;
			var red = 0;
			var yellow = 0;
			var green = 0;
			for (var _autoKey in global.arenaPlayers) {
				var player = global.arenaPlayers[_autoKey];
				if (arrContains(winArr, player.arenaTeam)) {
					if (player.arenaTeam == constants.ARENA_BLUE)
						blue += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
					else if (player.arenaTeam == constants.ARENA_RED)
						red += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
					else if (player.arenaTeam == constants.ARENA_YELLOW)
						yellow += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
					else if (player.arenaTeam == constants.ARENA_GREEN)
						green += ( getSval(player, 20860, "arenaScore") ? getSval(player, 20860, "arenaScore") : 0 );
				}
			}
			var finalArr = [[blue,constants.ARENA_BLUE],[red,constants.ARENA_RED],[yellow,constants.ARENA_YELLOW],[green,constants.ARENA_GREEN]];
			for (i=0; i<finalArr.length; i++) {
				if (finalArr[i][0] > topKillValue) {
					topKillValue = finalArr[i][0];
					topKillIndex = i;
				}
			}
			return finalArr[topKillIndex][1];
		}
	}
	
	return null;
}
function getHomeBase(actor) {
	if (actor.arenaTeam == constants.ARENA_BLUE)
		var homeBase = global.arenaCurrentMap.blueHomeBase;
	else if (actor.arenaTeam == constants.ARENA_RED)
		var homeBase = global.arenaCurrentMap.redHomeBase;
	else if (actor.arenaTeam == constants.ARENA_YELLOW)
		var homeBase = global.arenaCurrentMap.yellowHomeBase;
	else if (actor.arenaTeam == constants.ARENA_GREEN)
		var homeBase = global.arenaCurrentMap.greenHomeBase;
	else {
		if (actor.race == constants.RACE_HUMAN)
			var homeBase = global.arenaCurrentMap.blueHomeBase;
		else if (actor.race == constants.RACE_TROLLOC)
			var homeBase = global.arenaCurrentMap.redHomeBase;
		else if (actor.race == constants.RACE_AIEL)
			var homeBase = global.arenaCurrentMap.yellowHomeBase;
		else if (actor.race == constants.RACE_SEANCHAN)
			var homeBase = global.arenaCurrentMap.greenHomeBase;
	}
	return getRoom(homeBase);
}
function getWeave(desc) {
	if (desc == "short") {
		var weaves = [["'create fire'",17],["'current'",30],["'air scythe'",17],["'rain of earth'",26]]
	}
	else if (desc == "long") {
		var weaves = [["'call lightning'",38],["'fireball'",34],["'rend'",41],["'arms of air'",33]]
	}
	else if (desc == "status") {
		var weaves = [["'poison'",30],["'chill'",22],["'tornado'",27],["'quicksand'",35]]
	}
	return weaves[random(0,weaves.length-1)];
}
/** FIND ARENA AI **/
function isAI(actor) {
	if (actor.vnum == -1)
		return false;
	else if (arrContains(actor.namelist.split(" "), "ARENAAI"))
		return true;
	else
		return false;
}
/** IS AI ACTING **/
JSCharacter.prototype.isActing = function() {
	if (getSval(this, 14250, "acting") > 0)
		return true;
	return false;
}
/** GET AI TARGET **/
function getTarget(actor) {
	var targets = actor.room.people;
	var target = null;
	if (global.arenaFreeForAll == false)
		var team = actor.arenaTeam;
	for (i=0;i<targets.length;i++) {
		if (team) {
			if (actor != targets[i] && arrContains(global.arenaPlayers,targets[i]) && targets[i].arenaTeam != team && !targets[i].fighting) {
				target = targets[i];
			}
		}
		else {
			if (actor != targets[i] && arrContains(global.arenaPlayers,targets[i]) && !targets[i].fighting) {
				target = targets[i];
			}
		}
	}
	if (!target) { //no unengaged targets in room
		if (global.arenaFreeForAll == false) {
			for (var _autoKey in actor.room.people) {
				var player = actor.room.people[_autoKey];
				if (arrContains(global.arenaPlayers, player) && player.arenaTeam != actor.arenaTeam)
					target = player;
			}
		}
		else {
			if (actor.room.people.length > 0) {
				var targetArr = [];
				for (var _autoKey in actor.room.people) {
					var player = actor.room.people[_autoKey];
					if (arrContains(global.arenaPlayers, player) && actor != player)
						targetArr.push(player);
				}
				if (targetArr.length > 0)
					target = targetArr[random(0,targetArr.length-1)];
			}
		}
	}	
	if (!target) { //no targets in room
		var targetArr = [];
		if (global.arenaFreeForAll == false) {
			for (var _autoKey in global.arenaPlayers) {
				var player = global.arenaPlayers[_autoKey];
				if (player.arenaTeam != actor.arenaTeam && !player.fighting) {
					targetArr.push(player);
				}
			}
			if (targetArr.length > 0)
				target = targetArr[random(0,targetArr.length-1)];
		}
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			if (actor != player && !player.fighting)
				targetArr.push(player);
		}
		if (targetArr.length > 0)
			target = targetArr[random(0,targetArr.length-1)];
	}
	if (!target) { //no unengaged targets in map
		var targetArr = [];
		if (global.arenaFreeForAll == false) {
			for (var _autoKey in global.arenaPlayers) {
				var player = global.arenaPlayers[_autoKey];
				if (player.arenaTeam != actor.arenaTeam) {
					targetArr.push(player);
				}
			}
			if (targetArr.length > 0)
				target = targetArr[random(0,targetArr.length-1)];
		}
		for (var _autoKey in global.arenaPlayers) {
			var player = global.arenaPlayers[_autoKey];
			if (actor != player)
				targetArr.push(player);
		}
		if (targetArr.length > 0)
			target = targetArr[random(0,targetArr.length-1)];
	}	
	return target;
}
/** AI FIGHT OR FLIGHT **/
function fightOrFlight(actor) {
	if (actor.class == constants.CLASS_WARRIOR) {
		var threshhold = 15;
		var meleeNeed = 6;
	}
	else if (actor.class == constants.CLASS_RANGER) {
		var threshhold = 20;
		var meleeNeed = 3;
	}
	else if (actor.class == constants.CLASS_CHANNELER) {
		var threshhold = 25;
		var meleeNeed = 5;
	}
	else if (actor.class == constants.CLASS_THIEF) {
		var threshhold = 25;
		var meleeNeed = 2;
	}
	var fofMeter = 0;
	var hitPrc = Math.floor((actor.hps/actor.maxHit) * 100);
	if (hitPrc < threshhold)
		fofMeter += 2;
	if (random(0,10) < meleeNeed)
		fofMeter += 1;
	if (fofMeter == 0)
		return "fight";
	else if (fofMeter == 1)
		return (random(0,1) == 1 ? "fight" : "regroup");
	else if (fofMeter == 2)
		return "regroup";
	else
		return "flight";
}
/** SHOULD AI FLEE **/
JSCharacter.prototype.shouldFlee = function() {
	if (!this.fighting)
		return false;
	if (this.fighting.vnum > 0 && !isAI(this))
		return true;
	var numEnemies = 0;
	var numAllies = 0;
	for (var _autoKey in this.room.people) {
		var person = this.room.people[_autoKey];
		if (person.fighting == this)
			numEnemies += 1;
		if (person.fighting == this.fighting)
			numAllies += 1;
	}
	if (this.class == constants.CLASS_WARRIOR) {
		var buffer = 150;
		var multi = 20;
	}
	else if (this.class == constants.CLASS_RANGER) {
		var buffer = 100;
		var multi = 30;
	}
	else if (this.class == constants.CLASS_CHANNELER) {
		var buffer = 50;
		var multi = 50;
	}
	else {
		var buffer = 20;
		var multi = 70;
	}
	var fleeFactor = ((numAllies - numEnemies) * multi) + ((this.hps + buffer) - this.fighting.hps);
	//sendKoradin(this.name + " Flee Factor: "+fleeFactor);
	if (fleeFactor < 0 && random(1,10) > 1)
		return true;
	else
		return false;
}
/** AI ON GUARD **/
function onGuard(actor) {
	if (actor.isActing())
		return;
	actor.findMount();
	actor.comm("embrace");
	actor.comm("seize");
	if (!actor.eq(18))
		actor.comm("wear all");
	actor.comm("enter arena");
	//actor.comm("k arena");
	actor.comm("capture flag");
}
/** AI FIND MOUNT **/
JSCharacter.prototype.findMount = function() {
	if (this.mount)
		return;
	var mobs = this.room.people;
	var horses = [];
	if (this.race == constants.RACE_SEANCHAN)
		var horse = "torm";
	else
		var horse = "horse";
	for (i=0;i<mobs.length;i++) {
		if (!mobs[i].riddenBy && arrContains(mobs[i].namelist, horse))
			horses.push([mobs[i],(i+1)]);
	}
	for (var _autoKey in horses) {
		var mount = horses[_autoKey];
		this.comm("ride "+mount[1]+"."+horse);

	}
}
/** AI ABILITIES **/
function useAbility(actor, target) {
	if (actor.class == constants.CLASS_WARRIOR) {
		var spec = "kill "+target.name;
		var main = ["bash",30];
		var alts = [["kick",30]];
	}
	else if (actor.class == constants.CLASS_RANGER) {
		if (actor.race == constants.RACE_TROLLOC)
			var main = ["skewer",18];
		else
			var main = ["charge",18];
		var alts = [["bash",30],["kick",30],["",1]];
	}
	else if (actor.class == constants.CLASS_CHANNELER) {
		var spec = "kill "+target.name;
		var lWeave = getWeave("long");
		var sWeave = getWeave("short");
		var stWeave = getWeave("status");
		var main = ["channel "+sWeave[0],sWeave[1]];
		var alts = [["channel "+lWeave[0],lWeave[1]],["channel "+stWeave[0],stWeave[1]]];
	}
	else if (actor.class == constants.CLASS_THIEF) {
		var spec = "hide";
		var main = ["backstab",34];
		var alts = [["precisestrike",30],["kick",30],["",1]];
	}
	if (actor.class == constants.CLASS_CHANNELER && actor.sex == constants.SEX_MALE)
		actor.comm("seize");
	else if (actor.class == constants.CLASS_CHANNELER && actor.sex == constants.SEX_FEMALE)
		actor.comm("embrace");
	if (spec && random(1,4) < 4)
		actor.comm(spec);
	var alt = alts[random(0,alts.length-1)];
	var abil;
	if (random(1,4) < 4)
		abil = main;
	else
		abil = alt;
	if (actor.class == constants.CLASS_THIEF || actor.class == constants.CLASS_RANGER) {
		if (actor.fighting)
			abil = alt;
	}
	//sendKoradin(actor.name+" using "+abil[0]);
	if (!actor.isBashed) {
		actor.comm(abil[0]+" "+target.name);
		setSval(actor,14250,"acting",abil[1]);
	}
}
function sendKoradin(msg) {
	for (var _autoKey in getConnectedPlayers()) {
		var player = getConnectedPlayers()[_autoKey];
		if (player.name == "Koradin") {
			player.send(msg);
			return;
		}
	}
}
function getArenaKeeper() {
	var room = getRoom(20800);
	for (var _autoKey in room.people) {
		var mob = room.people[_autoKey];
		//sendKoradin(mob.name);
		if (mob.vnum == 20804)
			var finalMob = mob;
	}
	return finalMob;
}
/** PAY WAGERS: when the match ends or someone 
                leaves, the wager must be paid
				The payer is optional! **/
function payWager(payee,payer) {
	var wWps = 0;
	var wFps = 0;
	var wGold = 0;
	var wItems = [];
	var winners = [];
	var aK = getArenaKeeper();
	var aKWeave = getSval(aK,14201,"wWps");
	if (!aKWeave)
		aKWeave = 0;
	var aKFeat = getSval(aK,14201,"wFps");
	if (!aKFeat)
		aKFeat = 0;
	if (payee == aK) { // player left match early, hold wager for the rest
		for (var i=0; i<global.arenaCurrentWagers.length; i++) {
			sendKoradin(global.arenaCurrentWagers);
			if (global.arenaCurrentWagers[i][0] == payer) {	
				sendKoradin("WORKING!");
				var payTeam = global.arenaCurrentWagers[i][1];
				var payType = global.arenaCurrentWagers[i][2];
				var payAmount = parseInt(global.arenaCurrentWagers[i][3]);
				if (payType == 1) {//wagered wps
					sendKoradin("wWORKING!");
					payer.weavePoints -= payAmount;
					setSval(aK,14201,"wWps", (aKWeave+payAmount));
				}
				else if (payType == 2) {//wagered fps
					sendKoradin("fWORKING!");
					payer.addFeatPoints(0-payAmount);
					setSval(aK,14201,"wFps", (aKFeat+payAmount));
				}
				else if (payType == 3) {//wagered gold
					sendKoradin("gWORKING!");
					payer.gold -= payAmount;
					aK.gold += payAmount;
				}
				else if (payType == 4) {//wagered item
					sendKoradin("iWORKING!");
					global.arenaCurrentWagers[i][3].moveToChar(aK);
				}
				global.arenaCurrentWagers.splice(i,1);
				i = i-1;
			}
		}
		return null;
	}
	else if (payee > 0 && payee < 5) { // payout to a TEAM, split evenly
		for (var _autoKey in global.arenaCurrentWagers) {
			var wager = global.arenaCurrentWagers[_autoKey];
			if (wager[1] == payee)//if player team is winning team
				winners.push(wager[0]);//push player
			else {
				var payer = wager[0];
				var payAmount = parseInt(wager[3]);
				if (wager[2] == 1) {
					wWps += payAmount;
					payer.weavePoints -= payAmount;
				}
				else if (wager[2] == 2) {
					wFps += payAmount;
					payer.addFeatPoints(0-payAmount);
				}
				else if (wager[2] == 3) {
					wGold += payAmount;
					payer.gold -= payAmount;
				}
			}
		}
		wGold += aK.gold;
		wWps += getSval(aK,14201,"wWps");
		wFps += getSval(aK,14201,"wFps");
		if (wWps > 0) {
			var playerCut = Math.floor(wWps/winners.length);
			for (var _autoKey in winners) {
				var winner = winners[_autoKey];
				winner.weavePoints += playerCut;

			}
			arenaEcho("Each winning player receives "+nrm+playerCut+mag+" weave points!");
		}
		else if (wFps > 0) {
			var playerCut = Math.floor(wFps/winners.length);
			for (var _autoKey in winners) {
				var winner = winners[_autoKey];
				winner.addFeatPoints(playerCut);

			}
			arenaEcho("Each winning player receives "+nrm+playerCut+mag+" feat points!");
		}
		else if (wGold > 0) {
			var playerCut = Math.floor(wGold/winners.length);
			for (var _autoKey in winners) {
				var winner = winners[_autoKey];
				winner.gold += playerCut;

			}
			arenaEcho("Each winning player receives "+winners[0].numCopperToText(playerCut,false)+mag+"!");
		}
	}
	else { //payout to one person
		for (var _autoKey in global.arenaCurrentWagers) {
			var wager = global.arenaCurrentWagers[_autoKey];
			if (wager[0] != payee) { //player is not winner
				var payer = wager[0];
				var payAmount = parseInt(wager[3]);
				if (wager[2] == 1) {
					wWps += payAmount;
					payer.weavePoints -= payAmount;
				}
				else if (wager[2] == 2) {
					wFps += payAmount;
					payer.addFeatPoints(0-payAmount);
				}
				else if (wager[2] == 3) {
					wGold += payAmount;
					payer.gold -= payAmount;
				}
				else if (wager[2] == 4) {
					wItems.push(wager[3]);
				}
			}
		}
		wGold += aK.gold;
		wWps += getSval(aK,14201,"wWps");
		wFps += getSval(aK,14201,"wFps");
		for (var _autoKey in aK.inventory) {
			var item = aK.inventory[_autoKey];
			wItems.push(item);

		}
		var arenaMessage = (payee.name+" has won ");
		if (wWps > 0) {
			payee.weavePoints += wWps;
			arenaMessage += (nrm+wWps+mag+" weave points");
		}
		if (wFps > 0) {
			if (wWps > 0)
				arenaMessage += ", ";
			payee.addFeatPoints(wFps);
			arenaMessage += (nrm+wFps+mag+" feat points");
		}
		if (wGold > 0) {
			if (wFps > 0 || wWps > 0)
				arenaMessage += (", ");
			payee.gold += wGold;
			arenaMessage += (nrm+payee.numCopperToText(wGold,false)+mag);
		}
		if (wItems.length > 0) {
			if (wFps > 0 || wWps > 0 || wGold > 0)
				arenaMessage += (", and ");
			arenaMessage += ("the following items: ");
			for (var _autoKey in wItems) {
				var itm = wItems[_autoKey];
				itm.moveToChar(payee);
				arenaMessage += itm.name+" ";
			}
		}
		arenaEcho(arenaMessage);
	}
	return null;
}







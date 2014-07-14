var script638 = function(self, actor, here, args, extra)
{
	//Alder
//May 2010
/***************************************************
 *                    [ Login ]                    *
 *                  Global Script                  *
 * Purpose: perform actions when players log in    *
 * Current events:                                 *
 *  - Add actor to global.players array            *
 *  - Create testing var for Alder                 *
 *  - Unlock clan related achievements             *
 *  - Unlock age titles                            *
 *  - Load recipes                                 *
 *  - Load quest inventory  					   *
 *  - Remove player from Arena if they're in it    *
 *  - Remove/add arena legend titles               *
 *  - Pay a property owner money that is due them  *
 *  - Alert players of emoticon quest status       *
 *  ......                                         *
 *  Please append any new events to this list      *
 * SYNTAX: actor is the person logging in          *
 ***************************************************/
// Add actor to global array of actors
	global.players.unshift(actor);
// Create testing var for Alder
	if ( actor.name == "Alder" ) {
		if ( !global.alder )
			actor.send(grn+"[ Your global variable does not currently exist "+actor.name+". ]"+nrm);
		else {
			delete global.alder;
			actor.send(grn+"[ Your global variable has been deleted "+actor.name+". ]"+nrm);
		}
		global.alder = actor;
		getCharCols(actor);
		actor.send(grn+"[ Your global variable has been created "+actor.name+". ]"+nrm);
	}
// Lead new players into the tutorial
	if (actor.race == constants.RACE_HUMAN && actor.level == 1 && actor.room.zoneVnum == 5 && actor.quest("newbieTutorial") != 2) { //Level 1 logging on in crossroads, hasn't done tutorial before
		actor.comm("change spam complete");
		actor.comm("color complete");
		actor.mood = constants.MOOD_BRAVE;
		setSval(actor,501,"tutorial",1);
		actor.attach(501);
		
		setTimeout(3, function() {
		
			getCharCols(actor);
			actor.gsend("Greetings "+actor.name+". Would you like to take the tutorial?");
			actor.send(" ");
			actor.send("["+cyn+"Y"+nrm+"]es       ["+cyn+"N"+nrm+"]o");
			actor.send(" ");
		});
	}
	if (actor.room.vnum == 8) {
		actor.moveToRoom(getRoom(500));
		actor.detach(501);
	}
// Unlock clan related achievements
	var clans = actor.findClans();
	for each ( var num in clans ) {
		var cName = clanNumToText(num);
		var council = actor.isCouncil(num);
		actor.setPval("CLAN_SCRIPT_NAME_SAVE",cName);
		actor.unlockAchievement(53,true);
		if ( council ) {
			actor.setPval("CLAN_SCRIPT_NAME_SAVE",cName+" Council");
			actor.unlockAchievement(54,true);
		}
	}
// Unlock age titles
	if ( actor.age ) {
		if ( actor.age >= 100 ) {
			actor.unlockTitle("Centenarian","normal","silly");
		}
		if ( actor.age >= 300 ) {
			actor.unlockTitle("Ancient","normal","silly");
		}
		if ( actor.age >= 500 ) {
			actor.unlockTitle("Bag O' Bones","normal","silly");
		}
	}
    actor.loadRecipes();
	actor.loadQuestInv();
// Remove from Arena (for people who quit out / go linkdead)
	if (arrContains(global.arenaZones, actor.room.zoneVnum) || arrContains(global.arenaPlayers, actor) || arrContains(getWaitingPlayers("all"), actor)) {
		actor.detach(14212);
		actor.detach(14214);
		actor.detach(14215);
		actor.detach(14222);
		actor.detach(14223);
		actor.detach(14224);
		actor.comm("exit arena");
		if (actor.race == constants.RACE_HUMAN)
			var newR = 500;
		else
			var newR = 502;
		actor.moveToRoom(getRoom(newR));
	}
// Reset Arena Pval from old to new
	if (actor.getPval("arenaStats")) {
		var stats = actor.getPval("arenaStats");
		stats = stats.split("~");
		if (stats[6]) {
			var s7 = (stats[7] ? parseInt(stats[7]) : 0);
			var s8 = (stats[8] ? parseInt(stats[8]) : 0);
			var s9 = (stats[9] ? parseInt(stats[9]) : 0);
			var s10 = (stats[10] ? parseInt(stats[10]) : 0);
			var s11 = (stats[11] ? parseInt(stats[11]) : 0);
			var s12 = (stats[12] ? parseInt(stats[12]) : 0);
			var s13 = (stats[13] ? parseInt(stats[13]) : 0);
			var s14 = (stats[14] ? parseInt(stats[14]) : 0);
			var s15 = (stats[15] ? parseInt(stats[15]) : 0);
			var s16 = (stats[16] ? parseInt(stats[16]) : 0);
			var s17 = (stats[17] ? parseInt(stats[17]) : 0);
			actor.setPval("arenaStats", parseInt(stats[0])+"~"+parseInt(stats[1])+"~"+parseInt(stats[2])+"~"+parseInt(stats[3])+"~"+parseInt(stats[4])+"~"+parseInt(stats[5]),true);
			actor.setPval("arenaStats2",parseInt(stats[6])+"~"+s7+"~"+s8+"~"+s9+"~"+s10+"~"+s13+"~"+s14+"~"+s16,true);
			actor.setPval("arenaStats3",s11+"~"+s12+"~"+s15+"~"+s17,true);
		}
		if (!actor.getPval("arenaStats4"))
			actor.setPval("arenaStats4","0~0~0~0~0~0~0",true);
	}
// Remove and add arena legend titles
	updateArenaLegendTitle(actor);
// Notify that the Arena is available
	if (global.arenaStage == constants.ARENA_ON && actor.level < 100) {
		getCharCols(actor);
		actor.send("The Arena is currently open! Type "+mag+"enter arena"+nrm+" to join the battle!");
	}
// Pay Property Owner gold that is due
	for each (var manor in global.manors) {
		if (manor.goldOwed > 0 && actor.id == manor.ownerUserId) {
			actor.gold += manor.goldOwed;
			getCharCols(actor);
			actor.send(bld+"You receive "+nrm+actor.numCopperToText(manor.goldOwed, true)+bld+" from players using your property."+nrm);
			manor.goldOwed = 0;
			saveManor(manor);
		}
	}
	
	//Emoticon Quest Status
	//setTimeout(7, emoticonQuestReportStatus, [actor]);
	
// ...














};


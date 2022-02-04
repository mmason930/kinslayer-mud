var script20860 = function(self, actor, here, args, extra) {
	//Global Death script
	//Alder - November 2010
	if (!actor || !self || actor == self)
		return;
	self.detach(14225);
	/**Humanoid mobs loading tailoring supplies**/
	var tailorDrop = getBestTailoringDrop(self);
	if (tailorDrop)
		self.loadObj(tailorDrop);
	if (arrContains(global.arenaAllMobs, self.vnum) && self.room.zoneVnum == global.arenaZoneVnum) {
		var meats = [2102,2103,2104,2105,2107,2114,2115];
		actor.loadObj(meats[random(0,meats.length-1)]);
		actor.loadObj(meats[random(0,meats.length-1)]);
	}
	
	// Damane global event July 2014
	if(global.global2014Util) {
		if(arrContains(global.global2014Util.damaneVnums, self.vnum)){
			mudLog(2, 100, self.name+" has been killed by "+actor.name);
			global.global2014Util.damaneZones[self.vnum] = 0;
		}
	}

	//Emoticon Quest
	if(actor.vnum == 50 || actor.vnum == 51 || actor.vnum == 56 || actor.vnum == 57 || actor.vnum == 58 && self.vnum == -1)
	{
		emoticonQuestStorePlayerKill(actor, self, actor.room.vnum);
		
		actor.comm("shout Another traitor has fallen to our prowess! May the worms gouge out your eye sockets, " + self.name + "!");
	}
	
	/**Tutorial command for newbies**/
	if (self.vnum == 208 && getSval(actor,501,"tutorial") == 14)
		actor.comm("imdonewiththedeer");
	/**Smobs (and humanoids) loading trade/general stuff**/
	if (self.race < 4 && self.vnum > 0) {
		if (random(0,100) < 20) // load recipe bit
			self.loadObj(2899);
		if (random(0,100) < 15) // load herb
			self.loadObj(random(20985,20998));
		if (random(0,100) < 15) // load ore
			self.loadObj(random(4585,4599));
		if (random(0,100) < 8) // load spool
			self.loadObj(random(4940,4947));
		if (random(0,100) < 20) // load sack or pack
			self.loadObj(random(2061,2064));
		var lightArr = [2040,2041,2042,2044];
		if (random(0,100) < 20) // load light
			self.loadObj(lightArr[random(0,3)]);
		if (random(0,100) < 20) // load dagger
			self.loadObj(1415);
		var rareArr = [20835,20836,20837,4525,4562,4567,4568,4584];
		if (random(0,100) < 2) // load artifact/idol/stone
			self.loadObj(rareArr[random(0,7)]);
		var vialArr = [random(2700,2729),random(2790,2799)];
		if (random(0,100) < 10) // load vial
			self.loadObj(vialArr[random(0,1)]);
	}
	if (self.vnum > 0 && self.room.zoneVnum == global.arenaZoneVnum) {
		for (i=0;i<constants.NUM_WEARS;i++) {
			if (self.eq(i)) {
				var eq = self.eq(i);
				eq.moveToRoom(getRoom(20899));
				eq.extract();
			}
		}
//This is a test
		var selfInventory = self.inventory;
		for (var _autoKey in selfInventory) {
			var item = selfInventory[_autoKey];
			item.moveToRoom(getRoom(20899));
			item.extract();
		}
	}
	if (self.mobFlagged(constants.MOB_SMOB))
	{
		var recipeDrop = random(0,100);
		//here.echo(recipeDrop);
		if (recipeDrop < 20) {
			self.loadObj(2898);
		}
		else if (recipeDrop < 100) {
			self.loadObj(2899);
		}
		if (random(1,100) < 50)
			self.loadObj(2899);
		var oreArr = [4585,4589,4590,4591,4592,4593,4594,5603,4598,4599];
		if (random(0,100) < 20)
			self.loadObj(oreArr[random(0,oreArr.length-1)]); //load ore
		if (random(0,100) < 20)
			self.loadObj(oreArr[random(0,oreArr.length-1)]); //load ore
		if (random(0,100) < 20) // load herb
			self.loadObj(random(20985,20998));
		if (random(0,100) < 20) // load herb
			self.loadObj(random(20985,20998));
		if (random(0,100) < 20) // load spool
			self.loadObj(random(4940,4947));
		if (random(0,100) < 3) // load fireworks
			self.loadObj(590);
		/**Arena SMOB game**/
		if (self.room.zoneVnum == global.arenaZoneVnum) { //smob died in the arena
			global.arenaCurrentGame.totalSmobs -= 1;
			var smobKills = getSval(actor,20860,"smobKills");
			if (!smobKills)
				smobKills = 0;
			setSval(actor,20860,"smobKills",smobKills+1);
			if (global.arenaFreeForAll == true) {
				if (actor.race == constants.RACE_HUMAN)
					global.arenaCurrentGame.blueSmobKills += 1;
				else if (actor.race == constants.RACE_TROLLOC)
					global.arenaCurrentGame.redSmobKills += 1;
			}
			else {
				if (actor.arenaTeam == constants.ARENA_BLUE)
					global.arenaCurrentGame.blueSmobKills += 1;
				else if (actor.arenaTeam == constants.ARENA_RED)
					global.arenaCurrentGame.redSmobKills += 1;
				else if (actor.arenaTeam == constants.ARENA_YELLOW)
					global.arenaCurrentGame.yellowSmobKills += 1;
				else if (actor.arenaTeam == constants.ARENA_GREEN)
					global.arenaCurrentGame.greenSmobKills += 1;
			}
			addArenaPoint(actor, "smobKills");
			if (global.arenaCurrentGame.totalSmobs < 1) {
				for (var _autoKey in global.arenaPlayers) {
					var player = global.arenaPlayers[_autoKey];
					getCharCols(player);
					player.send(mag+"More SuperMOBS have entered the Arena!"+nrm);
				}
				loadSmobs();
			}
		}
	}
	/**Arena PC Deaths**/
	//sendKoradin(self.name+" just died in zone "+self.room.zoneVnum+"!!!");
	if ((self.room.zoneVnum == global.arenaZoneVnum && self.vnum == -1) || isAI(self)) {  //If person who dies is inside arena and is a PC
		//sendKoradin(self.name+" ARENA");
		for (var _autoKey in self.inventory) {
			var thing = self.inventory[_autoKey]; //Make person drop any flags from CTF
			if (arrContains(global.arenaItems, thing.vnum)) {
				thing.moveToRoom(self.room);
				self.detach(14215);
			}
		}
		//ASSISTS v
		var assistArr = [];
		for (var _autoKey in self.room.people) {
			var person = self.room.people[_autoKey];
			if (global.arenaFreeForAll == true) {//FFA on, count assist for anyone
				if (person != self && (person.vnum == -1 || isAI(person)) && person.fighting == self) {
					var assistCount = getSval(person,20860,"assistCount");
					if (!assistCount)
						assistCount = 0;
					setSval(person,20860,"assistCount", (assistCount+1));
					if (isAI(self))
						addArenaPoint(person, "assists", "ai");
					else
						addArenaPoint(person, "assists");
					assistArr.push(person);
				}
			}
			else { //REG mode, count only opposing race assists
				if (person != self && (person.vnum == -1 || isAI(person)) && person.fighting == self && person.arenaTeam != self.arenaTeam) {
					assistArr.push(person);
					var assistCount = getSval(person,20860,"assistCount");
					if (!assistCount)
						assistCount = 0;
					setSval(person,20860,"assistCount", (assistCount+1));
					if (isAI(self))
						addArenaPoint(person, "assists", "ai");
					else
						addArenaPoint(person, "assists");
				}
			}
		}
		if (isAI(self)) {
			for (var _autoKey in global.arenaPlayers) {
				var player = global.arenaPlayers[_autoKey];
				if (player.vnum == self.vnum)
					self = player;
			}
		}
		function afterWait(args) {
			var killer = args[0];
			var victim = args[1];
			var assists = args[2];
			//var tripleReset = false;
			if (!killer) {
				killer = victim;
			}
			var doubleKill = (getSval(killer,20860,"doubleKill") ? getSval(killer,20860,"doubleKill") : 0);
			var tripleKill = (getSval(killer,20860,"tripleKill") ? getSval(killer,20860,"tripleKill") : 0);
			if (time() - tripleKill < 11) {
				arenaEcho(killer.name +" has scored a Triple Kill!");
				setSval(killer,20860,"tripleKill",time());
				addArenaPoint(killer, "tripleKills");
			}
			else if (time() - doubleKill < 11) {
				arenaEcho(killer.name +" has scored a Double Kill!");
				setSval(killer,20860,"doubleKill",0);
				setSval(killer,20860,"tripleKill",time());
				addArenaPoint(killer, "doubleKills");
			}
			var killCount = getSval(killer,20860,"killCount");
			if (!killCount)
				setSval(killer,20860,"killCount",0);
			var deathCount = getSval(victim,20860,"deathCount");
			if (!deathCount)
				setSval(victim,20860,"deathCount",0);
			setSval(victim,20860,"deathCount",(getSval(victim,20860,"deathCount")+1)); //victim deathcount goes up no matter what
			addArenaPoint(victim, "pDeaths");
			if (killer.vnum > -1) //killed by a mob or ai
				addArenaPoint(victim, "mobDeaths");
			if (killer == victim) //suicided
				addArenaPoint(victim, "suicides");
			if (killer.vnum == -1 || isAI(killer)) { //Killer must be a PC for kill to count
				if (global.arenaFreeForAll == true) {  //FFA Mode is on, count any kill except suicide
					if (killer == victim)
						setSval(killer,20860,"killCount",(getSval(killer,20860,"killCount")-1));
					else {
						setSval(killer,20860,"killCount",(getSval(killer,20860,"killCount")+1));
						setSval(killer,20860,"doubleKill",time());
						if (isAI(self))
							addArenaPoint(killer, "pKills", "ai");
						else
							addArenaPoint(killer, "pKills");
					}
				}
				else { //Regular mode
					if (killer != victim) { // not suicide
						if (victim.arenaTeam == killer.arenaTeam) { //same-side kill, bad
							setSval(killer,20860,"killCount",(getSval(killer,20860,"killCount")-1));
							addArenaPoint(killer, "betrayals");
						}
						else { //cross-race kill, good
							setSval(killer,20860,"killCount",(getSval(killer,20860,"killCount")+1));
							setSval(killer,20860,"doubleKill",time());
							if (isAI(self))
								addArenaPoint(killer, "pKills", "ai");
							else	
								addArenaPoint(killer, "pKills");
						}
					}
				}
			}
			getArenaKilledMessage(victim, killer, assists);
			victim.hps = Math.floor(self.maxHit * global.arenaCurrentGame.regen);
			victim.sps = Math.floor(self.maxSpell * global.arenaCurrentGame.regen);
			victim.mvs = Math.floor(self.maxMove * global.arenaCurrentGame.regen);
			victim.shadowPoints = Math.floor(self.maxShadow * global.arenaCurrentGame.regen);
			victim.position = constants.POS_STANDING;
			setSval(victim,20860,"respawnTime",(time()+global.arenaCurrentGame.respawn));
			if (global.arenaCurrentGame == global.arenaGameLMS) { //LAST MAN STANDING
				if (getSval(victim,20860,"deathCount") == global.arenaCurrentGame.limit) {
					arenaEcho(victim.name+" has been eliminated!");
					global.arenaCurrentGame.totalPlayers -= 1;
					victim.send(" ");
					victim.send("You're out of lives!  Rest here until the match is finished.");
					victim.send(" ");
					victim.send("Note: Leaving the match early will negatively affect your score.");
					victim.send(" ");
				}
				if (global.arenaCurrentGame.totalPlayers == 1) { //winning the game
					for (var _autoKey in global.arenaPlayers) {
						var player = global.arenaPlayers[_autoKey];
						if (getSval(player,20860,"deathCount") < global.arenaCurrentGame.limit) {
							arenaEcho(player.name+" is the Last Man Standing!");
							addArenaPoint(player, "soloWins");
						}
						else {
							addArenaPoint(player, "soloLosses");
						}
					}
					resetArena();
					return;
				}
			}
			if (global.arenaCurrentGame == global.arenaGameDM) { //DEATH MATCH WINS
				if (global.arenaFreeForAll == true) { //FFA mode is ON
					if (getSval(killer,20860,"killCount") == global.arenaCurrentGame.limit) { //Player wins
						arenaEcho(killer.name+" has won the Death Match!");
						for (var _autoKey in global.arenaPlayers) {
							var player = global.arenaPlayers[_autoKey];
							if (player != killer)
								addArenaPoint(player, "soloLosses");
							player.send(" ");
						}
						addArenaPoint(killer, "soloWins");
						resetArena();
						return;
					}
				}
				else { // Regular match
					var redKills = 0;
					var blueKills = 0;
					var yellowKills = 0;
					var greenKills = 0;
					for (var _autoKey in global.arenaPlayers) {
						var player = global.arenaPlayers[_autoKey];
						if (player.arenaTeam == constants.ARENA_BLUE)
							blueKills += getSval(player,20860,"killCount");
						else if (player.arenaTeam == constants.ARENA_RED)
							redKills += getSval(player,20860,"killCount");
						else if (player.arenaTeam == constants.ARENA_YELLOW)
							yellowKills += getSval(player,20860,"killCount");
						else if (player.arenaTeam == constants.ARENA_GREEN)
							greenKills += getSval(player,20860,"killCount");
					}
					if (blueKills == global.arenaCurrentGame.limit)
						var winner = constants.ARENA_BLUE;
					else if (redKills == global.arenaCurrentGame.limit)
						var winner = constants.ARENA_RED;
					else if (yellowKills == global.arenaCurrentGame.limit)
						var winner = constants.ARENA_YELLOW;
					else if (greenKills == global.arenaCurrentGame.limit)
						var winner = constants.ARENA_GREEN;
					if (winner) { //Team won the match
						arenaEcho("The "+getTeamName(winner)+" Team has won the Death Match!");
						for (var _autoKey in global.arenaPlayers) {
							var player = global.arenaPlayers[_autoKey];
							if (player.arenaTeam == winner)
								addArenaPoint(player, "teamWins");
							else
								addArenaPoint(player, "teamLosses");
							player.send(" ");
						}
						resetArena();
						return;
					}
				}
			}
			return;
		}
		setTimeout(1,afterWait,[actor,self,assistArr]);
		self.send(" ");
		self.send("The Guardian of Life says, 'You look hungry. Say you'd like some food and I'll give you some.'");
		return;
	}
	/**Wait Function**/
	var victimLegendPos = self.legend;
	//Place your code inside the below function under the appropriate situation\\
	function afterWait(args) {
		var killer = args[0];
		var victim = args[1];
		if ( !killer.isValid || !victim.isValid ) {
			return;
		}
		var mobKiller = killer.vnum != -1;
		var mobVictim = victim.vnum != -1;
		if ( mobKiller && mobVictim ) {
			return;
		}
		/**Mob kills player**/
		if ( mobKiller && !mobVictim ) {
			if ( victim.mobDeaths >= 75 ) {
				victim.unlockAchievement(9800);
			}
		}
		/**Player kills mob**/
		if ( !mobKiller && mobVictim ) {
			//Do something here...
		}
		/**Player kills player**/
		if ( !mobKiller && !mobVictim ) {
			// Death stuff
			if ( victim.playerDeaths >= 100 ) {
				victim.unlockAchievement(9801);
			}
			// Increment kill streak if victim over level 30
			var killStreak = killer.quest(killer.name+"_KILL_STREAK");
			if ( victim.level >= 30 ) {
				killer.qval(killer.name+"_KILL_STREAK",++killStreak);
			}
			if ( victimLegendPos > 0 && victimLegendPos < 9 ) {
				killer.send("You have killed a legend!");
				killer.unlockAchievement(9816);
				killer.addFeatPoints(9-victimLegendPos);
			}
			// Kill streak awards
			if ( killStreak == 3 ) {
				killer.unlockAchievement(9811);						// Streaker (Has a title)
			}
			if ( killStreak == 5 ) {
				killer.unlockAchievement(9812);						// Deathly Sequence
			}
			if ( killStreak == 8 ) {
				killer.unlockAchievement(9813);						// Bloody Rampage
			}
			if ( killStreak == 11 ) {
				killer.unlockAchievement(9814);						// Running Riot
			}
			if ( killStreak == 15 ) {
				killer.unlockAchievement(9815);						// Invincible
			}
			// Kill awards
			if ( killer.killCount() >= 10 ) {
				killer.unlockAchievement(9804);						// Title pack 1
			}
			if ( killer.killCount() >= 20 ) {
				killer.unlockAchievement(9805);						// Title pack 2
			}
			if ( killer.killCount() >= 35 ) {
				killer.unlockTitle("Impaler","normal","pk",9806);
			}
			if ( killer.killCount() >= 50 ) {
				killer.unlockAchievement(9808);						// Title pack 3
			}
			if ( killer.killCount() >= 75 ) {
				killer.unlockTitle("Ripper","normal","pk");
			}
			if ( killer.killCount() >= 110 ) {
				killer.unlockTitle("Warmonger","normal","pk");
			}
			if ( killer.killCount() >= 200 ) {
				killer.unlockAchievement(9809);						// Title pack 4
			}
			if ( killer.killCount() >= 350 ) {
				killer.unlockAchievement(9806);						// Title pack 5
			}
			if ( killer.killCount() >= 500 ) {
				killer.unlockAchievement(9807);						// Title pack 6
			}
			if ( killer.killCount() >= 750 ) {
				killer.unlockTitle("Legion of Death","normal","pk");
				killer.addFeatPoints(10);
			}
			if ( killer.killCount() >= 1000 ) {
				killer.unlockAchievement(9810);
			}
			// Koradin's PK Quests
			var crossRaceQuests = getQuestsByTag("Cross-Race PK",killer);
			for (var _autoKey in crossRaceQuests) {
				var quest = crossRaceQuests[_autoKey];
				if ( killer.race != victim.race ) {
					quest.updateTask(killer, 0);
				}
			}
			var sameRaceQuests = getQuestsByTag("Same-Race PK",killer);
			for (var _autoKey in sameRaceQuests) {
				var qst = sameRaceQuests[_autoKey];
				if ( killer.race == victim.race ) {
					qst.updateTask(killer, 0);
				}
			}
			//Send messages
			if ( killStreak > 1 ) {
				getCharCols(killer);
				if ( victim.level < 30 ) {
					killer.send(bld+red+victim.name+" is too wimpy to count toward your kill streak."+nrm);
				}
				else {
					killer.send(bld+red+"You have a running streak of "+intToText(killStreak)+" kills!"+nrm);
				}
			}
		}
		/**Player dies**/
		if ( !mobVictim ) {
			victim.deletePval("NEWBIE_KIT",true); // reset need for newbie kit
			if (  ( victim.mobDeaths + victim.playerDeaths ) >= 275 ) {
				victim.unlockAchievement(9802);
			}
			var oldStreak = victim.quest(victim.name+"_KILL_STREAK");
			victim.qval(victim.name+"_KILL_STREAK",0);	// Wipe kill streak
			getCharCols(victim);
			if ( oldStreak ) {
				victim.send(bld+red+"\nYou have lost your kill streak!"+nrm);
			}
			// If player is manor owner/allowed, keep NOQUIT for 6 tics.
			var allowed = false;
			for (var _autoKey in global.manors) {
				var manor = global.manors[_autoKey];
				if (victim.id == manor.ownerUserId || arrContains(manor.allowedUsers, victim.id)) {
					allowed = true;
					var mr = manor;
				}
			}
			if (allowed == true) {
				victim.affect(constants.AFF_NOQUIT,5,0,0);
				victim.send("You died while responsible for the "+mr.name+" near "+mr.area+", and cannot logout.");
			}
		}
	}
	setTimeout(1,afterWait,[actor,self]);
};

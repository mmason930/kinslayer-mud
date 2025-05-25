var script14251 = function(self, actor, here, args, extra) {
	//Timer AI
	var timer = 380;
	while (timer > 0) {
		if (!self)
			return;
		if (!self.room)
			return;
		if (global.arenaStage != constants.ARENA_PLAYING)
			return;
		//sendKoradin(self.name+"'s action timer is "+getSval(self,14250,"acting"));
		if (arrContains([20896,14201,14299,14499,14415], self.room.vnum)) { // AI is in beginning room
			onGuard(self);
			timer -= 7;
			waitpulse 7;
			continue;
		}
		if (self.isActing()) {
			timer -= 1;
			waitpulse 1;
			setSval(self,14250,"acting", (getSval(self,14250,"acting")-1) );
			continue;
		}
		if (self.race != constants.RACE_TROLLOC && !self.mount && self.class != constants.CLASS_THIEF) {
			var base = getHomeBase(self);
			if (self.fighting) {
				self.comm("flee");
				timer -= 7;
				waitpulse 7;
				continue;
			}
			sendKoradin(self.name+" seeking base "+base.vnum);
			while (self.room != base) {
				if (!self)
					return;
				if (self.fighting)
					self.comm("flee");
				if (self.room)
					self.comm(dirToText(self.room.firstStep(base)))
				timer -= 1;
				waitpulse 1;
				if (timer < 1)
					return;
			}
			if (getSval(self,14251,"issued") != 1) {
				self.comm("arena issue mount");
				setSval(self,14251,"issued",1);
			}
			timer -= 1;
			waitpulse 1;
			if (self.race == constants.RACE_SEANCHAN)
				self.comm("ride torm");
			else
				self.comm("ride horse");
			onGuard(self);
			timer -= 7;
			waitpulse 7;
			continue;
		}
		//sendKoradin("dong");
		if (global.arenaHill) { // King of the hill
			if (self.room != global.arenaHill.room) {
				if (self.fighting)
					self.comm("flee");
				self.comm(dirToText(self.room.firstStep(global.arenaHill.room)))
				onGuard(self);
				timer -= 7;
				waitpulse 7;
				continue;
			}
		}
		else if (global.arenaFlags.length > 0) { // Capture the flag
			var targetFlag = null;
			for (i=0;i<global.arenaFlags.length;i++) { // set own team's flag
				if (getFlagTeam(global.arenaFlags[i]) == self.arenaTeam) {
					var ownFlag = global.arenaFlags[i];
				}
			}
			while (!targetFlag) { // set target flag
				var randFlag = global.arenaFlags[random(0,global.arenaFlags.length-1)];
				if (getFlagTeam(randFlag) != self.arenaTeam) {
					targetFlag = randFlag;
				}
			}
			var allies = ownFlag.room.people;
			for (j=0;j<allies.length;j++) {
				if (allies[j].arenaTeam == self.arenaTeam) { // someone's guarding the team flag, go get the target
					var targetRoom = targetFlag.room;
				}
			}
			if (!targetRoom) // no one is guarding the flag, defend it
				var targetRoom = ownFlag.room;
			if (self.room != targetRoom) { // hasn't reached room yet
				if (self.fighting)
					self.comm("flee");
				self.comm(dirToText(self.room.firstStep(targetRoom)))
			}
			else { // in flag room
				if (targetRoom == ownFlag.room && ownFlag.carriedBy) { // someone just stole it
					self.comm("k "+ownFlag.carriedBy.name);
				}
				else if (targetRoom == targetFlag.room && !targetFlag.carriedBy){ // there for the taking
					self.comm("capture flag");
					timer -= 14;
					waitpulse 14;
					if (timer < 0)
						return;
				}
			}
			if (targetFlag.carriedBy == self) { // AI has the enemy flag
				self.comm(dirToText(self.room.firstStep(getHomeBase(self))));
				timer -= 14;
				waitpulse 14;
				if (timer < 0)
					return;
				onGuard(self);
			}
		}
		if (self.fighting) { // basic combat
			if (self.shouldFlee()) {
				self.comm("flee");
				timer -= 7;
				waitpulse 7;
				if (timer < 1)
					return;
				if (fightOrFlight(self) == "regroup") {
					var roomsAway = random(1,5);
					while (roomsAway > 0) {
						if (!self)
							return;
						if (self.room)
							self.comm(dirToText(self.room.getRandomExitDirection()));
						roomsAway -= 1;
						timer -= 7;
						waitpulse 7;
						if (timer < 1)
							return;
						if (self.fighting)
							roomsAway -= 5;
					}
				}
				else if (fightOrFlight(self) == "flight") {
					var base = getHomeBase(self);
					while (self.room != base) {
						if (!self)
							return;
						if (self.room)
							self.comm(dirToText(self.room.firstStep(base)))
						timer -= 1;
						waitpulse 1;
						if (timer < 1)
							return;
					}
					onGuard(self);
				}
				continue;
			}
			if (!self.eq(18))
				self.comm("wear all");
			useAbility(self, self.fighting);
		}
		else { // not in combat
			if (self.room.vnum == 14514 || self.room.vnum == 14519) {
				if (self.room.items[0]) {
					if (self.room.items[0].vnum == 14205) {
						self.comm("push boulder");
						waitpulse 20;
						timer -= 20;
						if (timer < 0)
							return;
					}
				}
			}
			var target = getTarget(self);
			if (!target) {
				timer -= 7;
				waitpulse 7;
				continue;
			}
			//sendKoradin(self.name+"'s target is "+target.name);
			if (target.room != self.room) { // need to travel first
				//sendKoradin("moving "+dirToText(self.room.firstStep(target.room)));
				self.comm(dirToText(self.room.firstStep(target.room)))
				if (target.room == self.room) { // arrived
					if (random(0,1) == 1) {
						timer -= 1;
						waitpulse 1;
					}
					useAbility(self, target);
				}
				onGuard(self);
			}
			else { // already in room
				useAbility(self, target);
			}
		}
		timer -= 7;
		waitpulse 7;
		setSval(self,14250,"acting", (getSval(self,14250,"acting")-7) );
	}
	
	
	
}
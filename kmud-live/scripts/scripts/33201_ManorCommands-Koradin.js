var script33201 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	getCharCols(actor);
	var manor = getManor(self);
	if (!manor) {
		if (!global.manors)
			loadManors();
		else if (global.manors.length == 0)
			loadManors();
		_noblock;
		return;
	}
	if (strn_cmp(vArgs[0], "buy", 3)) { // actor is buying services
		_block;
		if (manor.ownerUserId == 0) {
			actor.send(bld+"This "+manor.name+" is available! CLAIM it while standing near its flag."+nrm);
			return;
		}
		else if (actor.id != manor.ownerUserId) {
			actor.send("You're not the owner of this "+manor.name+"!  CLAIM it first.");
			return;
		}
		if (!vArgs[1]) { // buy command by itself, show options to owner
			actor.send(bld+"You can buy the following services here:\n"+nrm);
			actor.send(bld+strPadding("UPKEEP"," ",12,"right")+"Price: "+nrm+actor.numCopperToText(getUpkeep(manor),true)+" per day");
			if (arrContains(manor.guardRooms, self.vnum)) {
				var price = nrm+actor.numCopperToText(getGuardPrice(self),true)+", +3 upkeep";
				if (numGuardsBought(self) == 3)
					price = grn+" Purchased"+nrm;
				actor.send(bld+strPadding("GUARD"," ",12,"right")+"Price: "+price+nrm);
			}
			if (self.vnum == manor.insideGateRoomVnum || self.vnum == manor.outsideGateRoomVnum)
				actor.send(bld+strPadding("GATE"," ",12,"right")+"Price: "+(manor.hasGate == true ? grn+" Purchased"+nrm : nrm+actor.numCopperToText(global.gateCost,true)+", +3 upkeep")+nrm);
			if (self.hasNeighbor(manor.storageRoomVnum))
				actor.send(bld+strPadding("WAREHOUSE"," ",12,"right")+"Price: "+(manor.hasStorage == true ? grn+" Purchased"+nrm : nrm+actor.numCopperToText(global.warehouseCost,true)+", +3 upkeep")+nrm);
			if (self.hasNeighbor(manor.innRoomVnum))
				actor.send(bld+strPadding("INN"," ",12,"right")+"Price: "+(manor.hasInn == true ? grn+" Purchased"+nrm : nrm+actor.numCopperToText(global.innCost,true)+", +3 upkeep")+nrm);
			if (self.hasNeighbor(manor.barRoomVnum))
				actor.send(bld+strPadding("BAR"," ",12,"right")+"Price: "+(manor.hasBar == true ? grn+" Purchased"+nrm : nrm+actor.numCopperToText(global.barCost,true)+", +3 upkeep")+nrm);
			if (self.hasNeighbor(manor.portalRoomVnum))
				actor.send(bld+strPadding("PORTAL"," ",12,"right")+"Price: "+(manor.hasPortal == true ? grn+" Purchased"+nrm : nrm+actor.numCopperToText(global.portalCost,true)+", +3 upkeep")+nrm);
			actor.send(bld+"\nTo purchase upkeep, type BUY UPKEEP <amount in coppers>."+nrm);
			actor.send(bld+"To purchase other services, type BUY <name of service>."+nrm);
			return;
		}
		else if (strn_cmp(vArgs[1],"upkeep",6)) { // buying more upkeep
			if (!vArgs[2]) {
				actor.send(bld+"\nTo purchase upkeep, type BUY UPKEEP <amount in coppers>. Example: BUY UPKEEP 100"+nrm);
				return;
			}
			var price = parseInt(vArgs[2]);
			if (actor.gold < price) {
				actor.send(bld+"You don't have enough coin to pay for it!"+nrm);
				return;
			}
			var timeAdded = price/getUpkeep(manor);
			if (timeAdded < 1) {
				actor.send(bld+"You must purchase at least a day of upkeep at one time."+nrm);
				return;
			}
			var maxDays = (518400 - timeTillManorExpires(manor));
			if ((timeAdded*24*60) > maxDays) {
				actor.send(bld+"You can only purchase up to a total of one year's worth of upkeep."+nrm);
				return;
			}
			actor.gold -= price;
			actor.send(bld+"You purchase "+Math.floor(timeAdded)+" additional days based on your current upkeep price of"+actor.numCopperToText(getUpkeep(manor))+" per day."+nrm);
			timeAdded *= 24;
			timeAdded *= 60;
			var curPval = manor.expireDatetime.getTime();
			if (!curPval)
				curPval = time();
			//sendKoradin(curPval);
			manor.expireDatetime = new Date(curPval + (timeAdded*1000));
			actor.send(bld+"Upkeep has been paid for a total of "+manorTimeToText(timeTillManorExpires(manor))+"."+nrm);
			saveManor(manor);
			return;
		}
		else if (strn_cmp(vArgs[1],"warehouse",9)) { // buying storage
			if (!self.hasNeighbor(manor.storageRoomVnum)) {
				actor.send(bld+"You can only buy the warehouse while standing outside it."+nrm);
				return;
			}
			if (manor.hasStorage == true) {
				actor.send(bld+"You've already purchased the warehouse in this "+manor.name+"!"+nrm);
				return;
			}
			if (actor.gold < global.warehouseCost) {
				actor.send(bld+"You don't have enough coin to pay for it!"+nrm);
				return;
			}
			actor.send(bld+"You purchase the warehouse for "+nrm+actor.numCopperToText(global.warehouseCost, true)+bld+"."+nrm);
			//actor.room.openDoor(actor.room.firstStep(getRoom(manor.storageRoomVnum)), true);
			actor.gold -= global.warehouseCost;
			manor.hasStorage = true;
			saveManor(manor);
			return;
		}
		else if (strn_cmp(vArgs[1],"gate",4)) { // buying the gate
			if (self.vnum != manor.insideGateRoomVnum && self.vnum != manor.outsideGateRoomVnum) {
				actor.send(bld+"You can only purchase a gate at your "+manor.name+"'s entrance."+nrm);
				return;
			}
			if (manor.hasGate == true) {
				actor.send(bld+"You've already purchased a gate for this "+manor.name+"!"+nrm);
				return;
			}
			if (actor.gold < global.gateCost) {
				actor.send(bld+"You don't have enough coin to pay for it!"+nrm);
				return;
			}
			actor.send(bld+"You purchase a gate for your "+manor.name+"."+nrm);
			act("$n purchases a gate for the "+manor.name+".", true, actor, null, null, constants.TO_ROOM);
			actor.gold -= global.gateCost;
			manor.hasGate = true;
			saveManor(manor);
			if (actor.room.vnum == manor.outsideGateRoomVnum)
				var dir = actor.room.firstStep(getRoom(manor.insideGateRoomVnum));
			else
				var dir = actor.room.firstStep(getRoom(manor.outsideGateRoomVnum));
			actor.room.closeDoor(dir, true);
			actor.room.lockDoor(dir, true);
			return;
		}
		else if (strn_cmp(vArgs[1],"inn",3)) { // buying the inn
			if (!self.hasNeighbor(manor.innRoomVnum)) {
				actor.send(bld+"You can only buy the inn while standing outside it."+nrm);
				return;
			}
			if (manor.hasInn == true) {
				actor.send(bld+"You've already purchased the inn for this "+manor.name+"!"+nrm);
				return;
			}
			if (actor.gold < global.innCost) {
				actor.send(bld+"You don't have enough coin to pay for it!"+nrm);
				return;
			}
			actor.send(bld+"You purchase the inn for "+nrm+actor.numCopperToText(global.innCost, true)+bld+"."+nrm);
			//actor.room.openDoor(actor.room.firstStep(getRoom(manor.innRoomVnum)), true);
			actor.gold -= global.innCost;
			manor.hasInn = true;
			saveManor(manor);
			return;
		}
		else if (strn_cmp(vArgs[1],"portal",6)) { // buying the trainins
			if (!self.hasNeighbor(manor.portalRoomVnum)) {
				actor.send(bld+"You can only buy the portal while standing outside them."+nrm);
				return;
			}
			if (manor.hasPortal == true) {
				actor.send(bld+"You've already purchased the portal for this "+manor.name+"!"+nrm);
				return;
			}
			if (actor.gold < global.portalCost) {
				actor.send(bld+"You don't have enough coin to pay for it!"+nrm);
				return;
			}
			actor.send(bld+"You purchase the portal for "+nrm+actor.numCopperToText(global.portalCost, true)+bld+"."+nrm);
			//actor.room.openDoor(actor.room.firstStep(getRoom(manor.portalRoomVnum)), true);
			actor.gold -= global.portalCost;
			manor.hasPortal = true;
			saveManor(manor);
			return;
		}
		else if (strn_cmp(vArgs[1],"bar",3)) { // buying the bar
			if (!self.hasNeighbor(manor.barRoomVnum)) {
				actor.send(bld+"You can only buy the bar while standing outside it."+nrm);
				return;
			}
			if (manor.hasBar == true) {
				actor.send(bld+"You've already purchased the bar for this "+manor.name+"!"+nrm);
				return;
			}
			if (actor.gold < global.barCost) {
				actor.send(bld+"You don't have enough coin to pay for it!"+nrm);
				return;
			}
			actor.send(bld+"You purchase the bar for "+nrm+actor.numCopperToText(global.barCost, true)+bld+"."+nrm);
			//actor.room.openDoor(actor.room.firstStep(getRoom(manor.barRoomVnum)), true);
			actor.gold -= global.barCost;
			manor.hasBar = true;
			saveManor(manor);
			return;
		}
		else if (strn_cmp(vArgs[1],"guards",5)) { // buying guards
			if (!arrContains(manor.guardRooms, self.vnum)) {
				actor.send(bld+"You can't hire anyone to guard this area."+nrm);
				return;
			}
			if (numGuardsBought(self) == 3) {
				actor.send(bld+"You can't hire any more guards for this area... too crowded!"+nrm);
				return;
			}
			//loadManorGuardsPurchased(manor.id);
			var guardsPurchased = manor.guardsPurchased[self.vnum];
			if (!guardsPurchased)
				var curGuards = 0;
			else
				var curGuards = parseInt(guardsPurchased);
			if (actor.gold < getGuardPrice(self)) { // actor can't afford another guard in this room
				actor.send(bld+"You don't have the coin to hire one!"+nrm);
				actor.send(bld+"\nType BUY to see the current hire price for this room."+nrm);
				return;
			}
			actor.send(bld+"You hire a guard for "+nrm+actor.numCopperToText(getGuardPrice(self), true)+"."+nrm);
			actor.send(bld+"Your hired guard will arrive to this post before tomorrow evening."+nrm);
			actor.gold -= getGuardPrice(self);
			manor.guardsPurchased[self.vnum] = curGuards+1;
			saveManor(manor);
			return;
		}
		else {
			actor.send(bld+"Type BUY by itself to see a list of services you can buy in this room."+nrm);
			return;
		}
	}
	else if (strn_cmp(vArgs[0], "close", 3) || strn_cmp(vArgs[0], "open", 1)) { // actor tries to open/close the gate
		if (strn_cmp(vArgs[0],"close", 3))
			var openClose = "close";
		else
			var openClose = "open";
		if (actor.room.vnum != manor.outsideGateRoomVnum && actor.room.vnum != manor.insideGateRoomVnum) { // not at the gate
			_noblock;
			return;
		}
		if (!vArgs[1] || str_cmp(vArgs[1], "gate")) { // actor isn't trying to open/close the gate
			_noblock;
			return;
		}
		if (manor.hasGate == false) { // owner hasn't purchased feature
			_block;
			// if (!getManorOwnerById(manor.ownerUserId)) { //owner is offline
			// 	actor.send("The gate is protected by an unseen force! You can't touch it.");
			//	return;
			// }
			if (openClose == "open")
				actor.send("The gate is already open.");
			else
				actor.send("The gate is stuck open.");
			if (actor.id == manor.ownerUserId) { // actor is owner, offer to buy
				actor.send(bld+"\nPrice: "+nrm+actor.numCopperToText(global.gateCost, true));
				actor.send(bld+"\nType BUY GATE to purchase a working gate."+nrm);
			}
			return;
		}
		else if (manor.gateIsBroken == true) {
				_block;
				var timeTillRebuilt = getSval(getRoom(manor.outsideGateRoomVnum),33212,"gate");
				timeTillRebuilt = timeTillRebuilt - time();
				timeTillRebuilt = Math.floor(timeTillRebuilt/60);
				var sAdd = "hour";
				if (timeTillRebuilt != 1)
					sAdd += "s";
				actor.send("The gate has been demolished! It will be rebuilt in "+timeTillRebuilt+" "+sAdd+".");
				if (actor.room.vnum == manor.outsideGateRoomVnum)
					var dir = actor.room.firstStep(getRoom(manor.insideGateRoomVnum));
				else
					var dir = actor.room.firstStep(getRoom(manor.outsideGateRoomVnum));
				actor.room.unlockDoor(dir, true);
				actor.room.openDoor(dir, true);
				return;
		}
	}
	else if (strn_cmp(vArgs[0],"claim",4)) { // Actor tries to claim the house
		_block;
		if (actor.room.vnum != manor.flagRoomVnum) {
			actor.send(bld+"This "+manor.name+" can only be claimed at its flag!"+nrm);
			return;
		}
		if (actor.position != constants.POS_STANDING) {
			actor.send("You're not in the right position to do that right now!");
			return;
		}
		if (manor.ownerUserId > 0) {
			if (manor.ownerUserId == actor.id) {
				actor.send(bld+"You already own this "+manor.name+"!"+nrm);
				return;
			}
			for each (var mr in global.manors) {
				if (mr.ownerUserId == actor.id) {
					actor.send(bld+"You already own the "+mr.name+" near "+mr.area+"!"+nrm);
					return;
				}
			}
			var allowedOnline = false;
			for each (var player in getCurrentPlayers()) {
				if (arrContains(manor.allowedUsers, player.id))
					allowedOnline = true;
			}
			// if (!getManorOwnerById(manor.ownerUserId) && allowedOnline == false) { // manor owner isn't online
			//	actor.send("The owner of this "+manor.name+" isn't around to defend it!");
			//	return;
			// }
			if (arrContains(manor.allowedUsers, actor.id)) { // capturer is currently allowed, disallow him
				actor.send(bld+getUserNameByUserId(manor.ownerUserId)+" has revoked your permission into the "+manor.name+"!"+nrm);
				updateAllowed(manor, actor.id);
				if (!getManorOwnerById(manor.ownerUserId))
					return;
			}
			actor.send(bld+"You attempt to capture the "+manor.name+"..."+nrm);
			setSval(actor,33200,"claim",1);
			act("$n starts to capture the "+manor.name+"...", true, actor, null, null, constants.TO_ROOM);
			var owner = getManorOwnerById(manor.ownerUserId);
			if (owner) { //manor owner is online, warn him
				getCharCols(owner);
				owner.send(bld+"Your "+manor.name+" near "+manor.area+" is under attack!"+nrm);
			}
			for each (var play in getConnectedPlayers()) {
				if (arrContains(manor.allowedUsers, play.id)) {
					getCharCols(play);
					play.send(bld+getUserNameByUserId(manor.ownerUserId)+"'s "+manor.name+" near "+manor.area+" is under attack!"+nrm);
					play.send(bld+"RECALL there to help defend it!"+nrm);
				}
			}
			actor.startTimer(65);
			var success = runTimer(actor);
			if (!success || actor.fighting) {
				actor.send("\nYour claim on the "+manor.name+" has been interrupted.");
				setSval(actor,33200,"claim",0);
				return;
			}
			else {
				getCharCols(actor);
				setSval(actor,33200,"claim",0);
				recordManorCapture(manor.id, actor.id)
				actor.send(bld+"\nYou successfully claim the "+manor.name+"!"+nrm);
				ctfEcho(actor.name+" has captured the "+manor.name+" near "+manor.area+"!");
				resetManor(manor);
				var newManor = getManor(self);
				newManor.ownerUserId = actor.id;
				newManor.race = actor.race;
				newManor.lastCapturedDatetime = new Date(time()*1000);
				newManor.expireDatetime = new Date((time()+14400)*1000);
				saveManor(newManor);
				actor.send(bld+"You receive ten free days of upkeep for successful conquest."+nrm);
				actor.send(" ");
				actor.comm("look");
				return;
			}
		}
		else {
			setSval(actor,33200,"claim",1);
			actor.send(bld+"You attempt to claim the "+manor.name+"..."+nrm);
			act("$n starts to claim the "+manor.name+"...", true, actor, null, null, constants.TO_ROOM);
			var owner = getManorOwnerById(manor.ownerUserId);
			if (owner) { //manor owner is online, warn him
				getCharCols(owner);
				owner.send(bld+"Your "+manor.name+" near "+manor.area+" is under attack!"+nrm);
			}
			actor.startTimer(30);
			var success = runTimer(actor);
			if (!success || actor.fighting) {
				setSval(actor,33200,"claim",0);
				actor.send("\nYour claim on the "+manor.name+" has been interrupted.");
				return;
			}
			else {
				getCharCols(actor);
				setSval(actor,33200,"claim",0);
				recordManorCapture(manor.id, actor.id)
				actor.send(bld+"\nYou successfully claim the "+manor.name+"!"+nrm);
				ctfEcho(actor.name+" has claimed the "+manor.name+" near "+manor.area+"!");
				resetManor(manor);
				var newManor = getManor(self);
				newManor.ownerUserId = actor.id;
				newManor.race = actor.race;
				newManor.lastCapturedDatetime = new Date(time()*1000);
				newManor.expireDatetime = new Date((time()+1440)*1000);
				saveManor(newManor);
				actor.send(bld+"You receive one free day of upkeep for claiming an available "+manor.name+"."+nrm);
				return;
			}
		}
	}
	else if (strn_cmp(vArgs[0],"drop",3)) { // Actor tries to drop something in the room
		if (actor.room.vnum != manor.storageRoomVnum) { // not in vault room, go ahead
			//sendKoradin("boo");
			_noblock;
			return;
		}
		if (strn_cmp(vArgs[1],"all",3) || vArgs[1].split(".")[0].toLowerCase() == "all") {
			_block;
			actor.send("You can only drop items one at a time here.");
			return;
		}
		if (manor.hasStorage == true && actor == getManorOwnerById(manor.ownerUserId)) { // bought storage, is owner
			if (actor.room.items.length > 19) {
				_block;
				actor.send(bld+"You can only store 20 items in your warehouse."+nrm);
				return;
			}
			_noblock;
			return;
		}
		else if (manor.hasStorage == false && actor == getManorOwnerById(manor.ownerUserId)) { // hasn't bought, is owner
			_block;
			actor.send(bld+"You can purchase the ability to store items in your warehouse.\n"+nrm);
			actor.send(bld+"Price: "+nrm+actor.numCopperToText(global.warehouseCost, true)+nrm);
			actor.send(bld+"\nType BUY STORAGE to unlock this feature."+nrm);
			return;
		}
		else { // actor isn't the owner, block dropping in courtyard
			_block;
			actor.send("You don't want to clutter up the warehouse!");
			return;
		}
	}
	else if (strn_cmp(vArgs[0],"allow",3)) { // Owner allows someone to come in to the house
		_block;
		if (manor.ownerUserId != actor.id) {
			actor.send("This isn't your "+manor.name+"!");
			return;
		}
		if (!vArgs[1]) {
			actor.send("Allow who?");
			return;
		}
		var allowed = getUserIdByUserName(vArgs[1]);
		if (!allowed) {
			actor.send("There is no player by the name '"+capFirstLetter(vArgs[1])+"'.");
			return;
		}
		if (allowed == actor.id) {
			actor.send("You own this "+manor.name+"... of course you're allowed in!");
			return;
		}
		if (arrContains(manor.allowedUsers, allowed)) {
			actor.send(bld+"You revoke your permission for "+getUserNameByUserId(allowed)+"!"+nrm);
			for each (var player in getConnectedPlayers()) {
				if (player.id == allowed)
					var aPlayer = player;
			}
			if (aPlayer) {
				getCharCols(aPlayer);
				aPlayer.send(bld+actor.name+" has revoked your permission into the "+manor.name+"!"+nrm);
			}
			updateAllowed(manor, allowed);
			return;
		}
		actor.send(bld+"You allow "+getUserNameByUserId(allowed)+" to peacefully enter the "+manor.name+"."+nrm);
		updateAllowed(manor, allowed);
		saveManor(manor);
		for each (var player in getConnectedPlayers()) {
			if (player.id == allowed)
				var aPlayer = player;
		}
		if (aPlayer) {
			getCharCols(aPlayer);
			aPlayer.send(bld+actor.name+" has allowed you to enter the "+manor.name+"!"+nrm);
			for each (var thing in aPlayer.room.people) {
				if (thing.fighting == aPlayer)
					thing.stopFighting();
				if (aPlayer.fighting) {
					if (aPlayer.fighting.vnum > 0) {
						if (isName("manorGuard", aPlayer.fighting.namelist))
							aPlayer.stopFighting();
					}
				}
			}
		}
		return;
	}
	else if (strn_cmp(vArgs[0],"report",6)) { // give owner a report
		_block;
		if (manor.ownerUserId != actor.id) {
			actor.send("You don't own this "+manor.name+"!");
			return;
		}
		var manorTime = manorTimeToText(timeTillManorExpires(manor));
		actor.send(bld+"You currently own the "+manor.name+" near "+manor.area+"."+nrm);
		actor.send(bld+"\nUpkeep has been paid for "+manorTime+"."+nrm);
		actor.send(bld+"Current Upkeep: "+nrm+actor.numCopperToText(getUpkeep(manor), true)+bld+" per day."+nrm);
		actor.send(bld+"\nYou've allowed the following players into the "+manor.name+":"+nrm);
		var users = [];
		for each (var id in manor.allowedUsers)
			users.push(getUserNameByUserId(id));
		if (users.length == 0)
			actor.send("None"+nrm);
		else
			actor.send(bld+users.join(", ")+nrm);
	}
	else if (strn_cmp(vArgs[0],"get",1) || strn_cmp(vArgs[0],"take",3)) {
		if (actor.room.vnum != manor.storageRoomVnum) {
			_noblock;
			return;
		}
		if (actor.id != manor.ownerUserId && manor.ownerUserId > 0) {
			_block;
			actor.send("An unseen force prevents you from taking it!");
			return;
		}
		_noblock;
		return;
	}
	else if (strn_cmp(vArgs[0],"jump",4)) {
		_block;
		if (!arrContains(manor.rangeRooms, actor.room.vnum)) {
			actor.send("You can only jump from a tower!");
			return;
		}
		if (actor.position != constants.POS_STANDING) {
			actor.send("You're not in the right position to do that now!");
			return;
		}
		act("$n takes a flying leap off the tower!",true,actor,null,null,constants.TO_ROOM);
		actor.send("You take a flying leap from the tower!");
		actor.moveToRoom(getRoom(manor.outsideGateRoomVnum));
		actor.comm("look");
		act("$n leaps from the tower above the gate, crashing to the ground.",true,actor,null,null,constants.TO_ROOM);
		var hDmg = random(actor.maxHit/5,actor.maxHit/4);
		var mDmg = random(actor.maxMove/5,actor.maxMove/4);
		actor.mvs -= mDmg;
		actor.damage(hDmg);
		getCharCols(actor);
		actor.send(red+"You crash to the ground below!"+nrm);
	}
	else {
		_noblock;
		return;
	}
	
	
	
	
	
	
	
	
	
	
}
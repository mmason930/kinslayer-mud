var script33200 = function(self, actor, here, args, extra) {
	getCharCols(actor);
	var manor = getManor(self);
	//sendKoradin(manor);
	if (!manor) {
		if (!global.manors)
			loadManors();
		else if (global.manors.length == 0)
			loadManors();
		_noblock;
		return;
	}
	var ownerOnline = getManorOwnerById(manor.ownerUserId);
	if (manor.outsideGateRoomVnum == self.vnum) { // Walking past manor gate
		if (actor.vnum > 18599 && actor.vnum < 18608) {
			_block;
			if (actor.leader)
				actor.comm("tell "+actor.leader.name+" I'll stay here and keep watch.");
			actor.comm("say I'm too afraid to go there!");
			return;
		}
		for (var _autoKey in self.neighbors) {
			var neighbor = self.neighbors[_autoKey];
			if (neighbor) {
				if (neighbor.vnum == manor.insideGateRoomVnum)
					var dir = dirToText(self.firstStep(neighbor));
			}
		}
		if (manor.ownerUserId == 0 && actor.room.vnum != manor.insideGateRoomVnum)
			var msg = bld+"The "+manor.name+" to the "+dir+" is available! Find the flag and CLAIM it fast!"+nrm;
		else if (manor.ownerUserId == 0 && actor.room.vnum != manor.insideGateRoomVnum && actor.id != manor.ownerUserId && !arrContains(manor.allowedUsers, actor.id)) {
			for (var _autoKey in manor.rangeRooms) {
				var rRoom = manor.rangeRooms[_autoKey];
				rRoom = getRoom(rRoom);
				for (var _autoKey in rRoom.people) {
					var guard = rRoom.people[_autoKey];
					if (guard.vnum > 0) {
						if (isName("manorArcher", guard.namelist)) {
							var msg = capFirstLetter(guard.name)+" yells from atop a tower, 'This is the "+manor.name+" of "+manor.ownerUserId+"! Leave now or die!'";
							if (getSval(actor,33200,"warning") < (time()-180))
								setSval(actor,33200,"warning",(time()+60));
							break;
						}
					}
				}
			}
		}
		waitpulse 1;
		if (msg) {
			getCharCols(actor);
			actor.send(msg);
		}
	}
	else if (manor.insideGateRoomVnum == self.vnum) { // Trying to enter manor
		if (actor.room.vnum != manor.outsideGateRoomVnum) { // Actor already inside manor
			_noblock;
			return;
		}
		if (actor.affectedBy(constants.AFF_NOQUIT) && !manor.gateIsBroken) {
			_block;
			actor.send("You're too riled up to enter the "+manor.name+". Calm down first.");
			return;
		}
		if (actor.vnum > 0) {
			_block;
			return;
		}
		// if (!ownerOnline && manor.ownerUserId > 0) { // manor is owned but owner is offline
		// 	_block;
		// 	actor.send(getUserNameByUserId(manor.ownerUserId)+"'s "+manor.name+" is protected by an unseen force! You cannot enter.");
		// 	return;
		// }
		var manorTime = manorTimeToText(timeTillManorExpires(manor));
	    if (getManorOwnerById(manor.ownerUserId) == actor)
			var msg = bld+"Welcome home, "+actor.name+".\nUpkeep has been paid for "+manorTime+"."+nrm;
		else if (manor.ownerUserId > 0) {
			if (arrContains(manor.allowedUsers, actor.id))
				var colorM = bld;
			else
				var colorM = red;
			var msg = colorM+"You are entering the "+manor.name+" of "+getUserNameByUserId(manor.ownerUserId)+"."+nrm;
		}
		waitpulse 1;
		if (msg) {
			getCharCols(actor);
			actor.send(msg);
		}
	}
	else if (manor.innRoomVnum == self.vnum && actor.room.sector != constants.SECT_INSIDE) { // trying to enter inn
		//sendKoradin("yo");
		//if (actor.id != manor.ownerUserId && !arrContains(manor.allowedUsers, actor.id)) { // no owner or nonowner
		//	_block;
		//	actor.send("The inn is protected by an unseen force...");
		//	return;
		//}
		if (manor.hasInn == false) {
			_block;
			actor.send("The inn is protected by an unseen force...");
			if (actor.id == manor.ownerUserId) {
				actor.send(bld+"\nPrice: "+nrm+actor.numCopperToText(global.innCost, true));
				actor.send(bld+"\nType BUY INN to purchase the inn."+nrm);
			}
			return;
		}
		//if (actor.affectedBy(constants.AFF_NOQUIT)) {
		//	_block;
		//	actor.send("Calm down before you enter the inn.");
		//	return;
		//}
	}
	else if (manor.storageRoomVnum == self.vnum  && actor.room.sector != constants.SECT_INSIDE) { // trying to enter storage
		//if (actor.id != manor.ownerUserId && !arrContains(manor.allowedUsers, actor.id)) { // no owner or nonowner
		//	_block;
		//	actor.send("The warehouse is protected by an unseen force...");
		//	return;
		//}
		if (manor.hasStorage == false) {
			_block;
			actor.send("The warehouse is protected by an unseen force...");
			if (actor.id == manor.ownerUserId) {
				actor.send(bld+"\nPrice: "+nrm+actor.numCopperToText(global.warehouseCost, true));
				actor.send(bld+"\nType BUY WAREHOUSE to purchase the warehouse."+nrm);
			}
			return;
		}
		//if (actor.affectedBy(constants.AFF_NOQUIT)) {
		//	_block;
		//	actor.send("Calm down before you enter the warehouse.");
		//	return;
		//}
	}
	else if (manor.portalRoomVnum == self.vnum  && actor.room.sector != constants.SECT_INSIDE) { // trying to enter prac area
		//sendKoradin("yo");
		//if (actor.id != manor.ownerUserId && !arrContains(manor.allowedUsers, actor.id)) { // no owner or nonowner
		//	_block;
		//	actor.send("The portal protected by an unseen force...");
		//	return;
		//}
		if (manor.hasPortal == false) {
			_block;
			actor.send("The portal protected by an unseen force...");
			if (actor.id == manor.ownerUserId) {
				actor.send(bld+"\nPrice: "+nrm+actor.numCopperToText(global.portalCost, true));
				actor.send(bld+"\nType BUY PORTAL to purchase the portal."+nrm);
			}
			return;
		}
		//if (actor.affectedBy(constants.AFF_NOQUIT)) {
		//	_block;
		//	actor.send("Calm down before you enter the portal.");
		//	return;
		//}
	}
	else if (manor.barRoomVnum == self.vnum  && actor.room.sector != constants.SECT_INSIDE) { // trying to enter bar
		//sendKoradin("yo");
		//if (actor.id != manor.ownerUserId && !arrContains(manor.allowedUsers, actor.id)) { // no owner or nonowner
		//	_block;
		//	actor.send("The bar is protected by an unseen force...");
		//	return;
		//}
		if (manor.hasBar == false) {
			_block;
			actor.send("The bar is protected by an unseen force...");
			if (actor.id == manor.ownerUserId) {
				actor.send(bld+"\nPrice: "+nrm+actor.numCopperToText(global.barCost, true));
				actor.send(bld+"\nType BUY BAR to purchase the bar."+nrm);
			}
			return;
		}
		//if (actor.affectedBy(constants.AFF_NOQUIT)) {
		//	_block;
		//	actor.send("Calm down before you enter the bar.");
		//	return;
		//}
	}
	
	
	
	
	
	
	
	
	
	
}
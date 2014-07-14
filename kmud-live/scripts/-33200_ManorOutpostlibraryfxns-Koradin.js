/** RETURN MANOR OBJECT **/
function getManor(room) {
	if (!room) {
		return null;
	}
	for each (var manor in global.manors) {
		if (manor.zoneId == room.zoneVnum)
			return manor;
	}
	return null;
}
/** RETURN MANOR OWNER PLAYER IF ONLINE **/
function getManorOwnerById(id) {
	for each (var player in getCurrentPlayers()) {
		if (player.id == id)
			return player;
	}
	return null;
}
function recordManorCapture(manorId, userId) {

	var sql = " INSERT INTO manorCapture("
			+ "   `manor_id`,"
			+ "   `user_id`,"
			+ "   `captured_datetime`"
			+ ") VALUES ("
			+ manorId + ","
			+ userId + ","
			+ sqlEncodeQuoteDate(new Date()) + ")";
	
	sqlQuery(sql);
}
function loadManors() {

	global.manors = [];
	var sql =
			  " SELECT *"
			+ " FROM manor"
			+ " ORDER BY id";
			
	var resultSet = sqlQuery(sql);
	
	while(resultSet.hasNextRow) {
		
		var row = resultSet.getRow;
		var manor = getManorFromResultSet(row);
		
		manor.allowedUsers = loadManorAllowedUsers(manor.id);
		manor.guardRooms = loadManorGuardRooms(manor.id);
		manor.rangeRooms = loadManorRangeRooms(manor.id);
		manor.shotRooms = loadManorShotRooms(manor.id);
		manor.guardsPurchased = loadManorGuardsPurchased(manor.id);
		manor.gateIsBroken = false;
		manor.isUnderSiege = false;
		global.manors.push(manor);
		/***
		getRoom(33210).echo("Manor ID: " + manor.id);
		getRoom(33210).echo("Manor Name: " + manor.name);
		getRoom(33210).echo("Created: " + manor.createdDate
time);
		getRoom(33210).echo("Expire: " + manor.expireDatetime);
		getRoom(33210).echo("Number Of Allowed Users: " + manor.allowedUsers.length);
		getRoom(33210).echo("Number Of Guard Rooms: " + manor.guardRooms.length);
		getRoom(33210).echo("Number Of Range Rooms: " + manor.rangeRooms.length);
		getRoom(33210).echo("Number Of Shot Rooms: " + manor.shotRooms.length);
		***/
	}
}
function loadManorAllowedUsers(manorId) {

	var sql =
			  " SELECT user_id"
			+ " FROM manorAllowedUser"
			+ " WHERE manor_id = " + manorId;
	
	var resultSet = sqlQuery(sql);
	var allowedUsers = [];
	
	while(resultSet.hasNextRow) {
	
		allowedUsers.push(resultSet.getRow.getInteger("user_id"));
	}
	
	return allowedUsers;
}
function loadManorGuardRooms(manorId) {

	var sql =
			  " SELECT room_id"
			+ " FROM manorGuardRoom"
			+ " WHERE manor_id = " + manorId;
	
	var resultSet = sqlQuery(sql);
	var guardRooms = [];
	
	while(resultSet.hasNextRow) {
	
		guardRooms.push(resultSet.getRow.getInteger("room_id"));
	}
	
	return guardRooms;
}
function loadManorRangeRooms(manorId) {

	var sql =
			  " SELECT room_id"
			+ " FROM manorRangeRoom"
			+ " WHERE manor_id = " + manorId;
	
	var resultSet = sqlQuery(sql);
	var rangeRooms = [];
	
	while(resultSet.hasNextRow) {
	
		rangeRooms.push(resultSet.getRow.getInteger("room_id"));
	}
	
	return rangeRooms;
}
function loadManorShotRooms(manorId) {

	var sql =
			  " SELECT room_id"
			+ " FROM manorShotRoom"
			+ " WHERE manor_id = " + manorId;

	var resultSet = sqlQuery(sql);
	var shotRooms = [];
	
	while(resultSet.hasNextRow) {
	
		shotRooms.push(resultSet.getRow.getInteger("room_id"));
	}
	
	return shotRooms;
}
function loadManorGuardsPurchased(manorId) {

	var sql =
			  " SELECT room_id, number_of_guards"
			+ " FROM manorGuardsPurchased"
			+ " WHERE manor_id = " + manorId;
	
	var resultSet = sqlQuery(sql);
	var guardsPurchased = new Object();
	
	while(resultSet.hasNextRow) {
	
		var row = resultSet.getRow;
		guardsPurchased[ row.getInteger("room_id") ] = row.getInteger("number_of_guards");
	}
	
	return guardsPurchased;
}
function getManorFromResultSet(resultSet) {
	var manor = new Object();
	
	manor.id = resultSet.getInteger("id");
	manor.ownerUserId = resultSet.getInteger("owner_user_id");
	manor.race = resultSet.getInteger("race");
	manor.goldOwed = resultSet.getInteger("gold_owed");
	manor.name = resultSet.get("name");
	manor.area = resultSet.get("area");
	manor.zoneId = resultSet.getInteger("zone_id");
	manor.hasStorage = parseInt(resultSet.get("has_storage")) == 1 ? true : false;
	manor.outsideGateRoomVnum = resultSet.getInteger("outside_gate_room_vnum");
	manor.insideGateRoomVnum = resultSet.getInteger("inside_gate_room_vnum");
	manor.innRoomVnum = resultSet.getInteger("inn_room_vnum");
	manor.portalRoomVnum = resultSet.getInteger("portal_room_vnum");
	manor.storageRoomVnum = resultSet.getInteger("storage_room_vnum");
	manor.barRoomVnum = resultSet.getInteger("bar_room_vnum");
	manor.flagRoomVnum = resultSet.getInteger("flag_room_vnum");
	manor.createdDatetime = resultSet.getTimestamp("created_datetime");
	manor.createdByUserId = resultSet.getInteger("created_by_user_id");
	manor.lastCapturedDatetime = resultSet.getTimestamp("last_captured_datetime");
	manor.expireDatetime = resultSet.getTimestamp("expire_datetime");
	manor.hasGate = parseInt(resultSet.get("has_gate")) == 1 ? true : false;
	manor.hasInn = parseInt(resultSet.get("has_inn")) == 1 ? true : false;
	manor.hasPortal = parseInt(resultSet.get("has_portal")) == 1 ? true : false;
	manor.hasBar = parseInt(resultSet.get("has_bar")) == 1 ? true : false;
	
	return manor;
}
function saveManor(manor) {

	var sql =
			  " REPLACE INTO manor("
			+ "   `id`,"
			+ "   `owner_user_id`,"
			+ "   `race`,"
			+ "   `gold_owed`,"
			+ "   `name`,"
			+ "   `area`,"
			+ "   `zone_id`,"
			+ "   `has_storage`,"
			+ "   `outside_gate_room_vnum`,"
			+ "   `inside_gate_room_vnum`,"
			+ "   `inn_room_vnum`,"
			+ "   `portal_room_vnum`,"
			+ "   `storage_room_vnum`,"
			+ "   `bar_room_vnum`,"
			+ "   `flag_room_vnum`,"
			+ "   `created_datetime`,"
			+ "   `created_by_user_id`,"
			+ "   `last_captured_datetime`,"
			+ "   `expire_datetime`,"
			+ "   `has_gate`,"
			+ "   `has_inn`,"
			+ "   `has_portal`,"
			+ "   `has_bar`"
			+ " ) VALUES ("
			+ manor.id + ","
			+ manor.ownerUserId + ","
			+ manor.race + ","
			+ manor.goldOwed + ","
			+ sqlEscapeQuoteString(manor.name) + ","
			+ sqlEscapeQuoteString(manor.area) + ","
			+ manor.zoneId + ","
			+(manor.hasStorage ? 1 : 0) + ","
			+ manor.outsideGateRoomVnum + ","
			+ manor.insideGateRoomVnum + ","
			+ manor.innRoomVnum + ","
			+ manor.portalRoomVnum + ","
			+ manor.storageRoomVnum + ","
			+ manor.barRoomVnum + ","
			+ manor.flagRoomVnum + ","
			+ sqlEncodeQuoteDate(manor.createdDatetime) + ","
			+ manor.createdByUserId + ","
			+ sqlEncodeQuoteDate(manor.lastCapturedDatetime) + ","
			+ sqlEncodeQuoteDate(manor.expireDatetime) + ","
			+(manor.hasGate ? 1 : 0) + ","
			+(manor.hasInn ? 1 : 0) + ","
			+(manor.hasPortal ? 1 : 0) + ","
			+(manor.hasBar ? 1 : 0) + ")";
	//getRoom(33210).echo(sql);
	sqlQuery(sql);
	
	sqlQuery("DELETE FROM manorAllowedUser WHERE manor_id = " + manor.id);
	sql = " INSERT INTO manorAllowedUser(`manor_id`, `user_id`) VALUES"
	for(var index = 0;index < manor.allowedUsers.length;++index) {
		sql += (index == 0 ? "" : ",") + "("
			+ manor.id + ","
			+ manor.allowedUsers[ index ]
			+ ")"
	}
	if(manor.allowedUsers.length)
		sqlQuery(sql);
	
	
	sqlQuery("DELETE FROM manorShotRoom WHERE manor_id = " + manor.id);
	sql = " INSERT INTO manorShotRoom(`manor_id`, `room_id`) VALUES"
	for(var index = 0;index < manor.shotRooms.length;++index) {
		sql += (index == 0 ? "" : ",") + "("
			+ manor.id + ","
			+ manor.shotRooms[ index ]
			+ ")"
	}
	if(manor.shotRooms.length)
		sqlQuery(sql);
	
	sqlQuery("DELETE FROM manorRangeRoom WHERE manor_id = " + manor.id);
	sql = " INSERT INTO manorRangeRoom(`manor_id`, `room_id`) VALUES"
	for(var index = 0;index < manor.rangeRooms.length;++index) {
		sql += (index == 0 ? "" : ",") + "("
			+ manor.id + ","
			+ manor.rangeRooms[ index ]
			+ ")"
	}
	if(manor.rangeRooms.length)
		sqlQuery(sql);
	
	sqlQuery("DELETE FROM manorGuardRoom WHERE manor_id = " + manor.id);
	sql = " INSERT INTO manorGuardRoom(`manor_id`, `room_id`) VALUES"
	for(var index = 0;index < manor.guardRooms.length;++index) {
		sql += (index == 0 ? "" : ",") + "("
			+ manor.id + ","
			+ manor.guardRooms[ index ]
			+ ")"
	}
	if(manor.guardRooms.length)
		sqlQuery(sql);
		
	sqlQuery("DELETE FROM manorGuardsPurchased WHERE manor_id = " + manor.id);
	var totalGuardsPurchased = 0;
	sql = " INSERT INTO manorGuardsPurchased(`manor_id`, `room_id`, `number_of_guards`) VALUES";
	for(var key in manor.guardsPurchased) {
	
		sql += (totalGuardsPurchased == 0 ? "" : ",") + "("
			+ manor.id + ","
			+ key + ","
			+ manor.guardsPurchased[ key ]
			+ ")"
		++totalGuardsPurchased;
	}
	if(totalGuardsPurchased)
		sqlQuery(sql);
	
}
/** RESET MANOR **/
function resetManor(manor) {
	manor.ownerUserId = 0;
	manor.race = -1;
	manor.hasInn = false;
	manor.hasBar = false;
	manor.hasPortal = false;
	manor.hasGate = false;
	manor.hasStorage = false;
	manor.goldOwed = 0;
	manor.gateIsBroken = false;
	manor.isUnderSiege = false;
	manor.expireDatetime = new Date(time()*1000);
	getRoom(manor.outsideGateRoomVnum).unlockDoor(getRoom(manor.outsideGateRoomVnum).firstStep(getRoom(manor.insideGateRoomVnum)), true);
	getRoom(manor.outsideGateRoomVnum).openDoor(getRoom(manor.outsideGateRoomVnum).firstStep(getRoom(manor.insideGateRoomVnum)), true);
	for each (var gRoom in manor.guardRooms)
		manor.guardsPurchased[gRoom] = 0;
	manor.allowedUsers = [];

	var rooms = getAllRoomsInZone(manor.zoneId);
	var dumpRoom = getRoom(20899);
	var outsideGateRoom = getRoom(manor.outsideGateRoomVnum);
	for each (var room in rooms) {
		if (room) {
			if (room.vnum == manor.storageRoomVnum) {
				for each (var item in room.items)
					item.moveToRoom(outsideGateRoom);
			}
			else {
				for each (var item in room.items) {
					item.moveToRoom(dumpRoom);
					//item.extract();
				}
			}
			for each (var mob in room.people) {
				if (mob.vnum > -1 && !mob.riddenBy && !isName("merc",mob.namelist)) {
					mob.moveToRoom(dumpRoom);
					//mob.extract();
				}
				else {
					mob.moveToRoom(outsideGateRoom);
					mob.comm("look");
					mob.send("You have been kicked out of the "+manor.name+".");
				}
			}
		}
	}

	getRoom(manor.flagRoomVnum).loadObj(21010);
	saveManor(manor);
	getRoom(manor.flagRoomVnum).zreset();
	//loadManors();
}

/** RETURN NUMBER OF GUARDS OWNER HAS BOUGHT FOR ROOM **/
function numGuardsBought(room) {
	var manor = getManor(room);
	if (!manor)
		return 0;
	var curGuards = manor.guardsPurchased[room.vnum];
	if (!curGuards)
		return 0;
	else {
		return curGuards;
	}
}

/** RETURN MANOR EXPIRE TIME **/
function timeTillManorExpires(manor) {
	if (!manor)
		return 0;
	var eTime = Math.floor(manor.expireDatetime.getTime()/1000);
	if (!eTime)
		return 0;
	//sendKoradin("eTime: "+eTime);
	eTime = parseInt(eTime);
	//sendKoradin("eTime: "+eTime);
	var curTime = time();
	var timeLeft = eTime - curTime;
	//sendKoradin(timeLeft);
	if (timeLeft < 0)
		return 0;
	else
		return timeLeft;
}

/** MANOR TIME TO TEXT **/
function manorTimeToText(intTime) {
	var days = (intTime/60)/24;
	if (days < 1)
		return "less than a day";
	else if (days > 29) {
		var mS = "s";
		var months = days/30;
		if (Math.floor(months) == 1)
			mS = "";
		return Math.floor(months)+" month"+mS;
	}
	else {
		var dS = "s";
		if (Math.floor(days) == 1)
			dS = "";
		return Math.floor(days)+" day"+dS;
	}
}

/** GECHO FOR CTF MESSAGES **/
function ctfEcho(msg) {
	for each (var player in getConnectedPlayers()) {
		getCharCols(player);
		player.send(bld+msg+nrm);
	}
}

/** GET UPKEEP PRICE (IN COPPERS PER 24 TICS) FOR MANOR **/
function getUpkeep(manor) {
	if (!manor)
		return 0;
	var price = 10;
	if (manor.hasInn == true)
		price += 3;
	if (manor.hasStorage == true)
		price += 3;
	if (manor.hasGate == true)
		price += 3;
	if (manor.hasPortal == true)
		price += 3;
	if (manor.hasBar == true)
		price += 3;
	for each (var room in manor.guardRooms) {
		var add = (numGuardsBought(getRoom(room))*3)
		price += add;
	}
	return price;
}

/** GET GUARD PRICE **/
function getGuardPrice(room) {
	var price = 0;
	var curNum = numGuardsBought(room);
	if (curNum == 0)
		price = 1000;
	else if (curNum == 1)
		price = 2500;
	else if (curNum == 2)
		price = 5000;
	else
		price = 10000000;
	return price;
}

/** GET MANOR TARGET **/
function getManorTarget(guard) {
	var manor = getManor(guard.room);
	if (!manor)
		return null;
	 var manorOwner = getManorOwnerById(manor.ownerUserId);
	// if (!manorOwner)
	//	return null;
	if (guard.vnum == 20700 || guard.vnum == 20705) {
		if (random(0,1) > 0)
			return null;
	}
	var targetArr = [];
	if (!isName("manorArcher", guard.namelist)) { // if a scout, warrior, or channie, look in room first
		//sendKoradin("searching room for "+guard.name);
		for each (var targ in guard.room.people) {
			if ( targ.vnum != 20720 && targ.level < 100 && !isName("horse", targ.namelist) && (manorOwner ? !targ.isGroupedWith(manorOwner) : true) && !targ.riddenBy && (targ.vnum < 20700 || targ.vnum > 20715) && targ.id != manor.ownerUserId && !arrContains(manor.allowedUsers, targ.id)) {
				//sendKoradin(targ.vnum + " "+guard.vnum+ "pushing first");
				targetArr.push(targ);
			}
		}
		if (targetArr.length > 0)
			return targetArr[random(0,targetArr.length-1)];
	}
	if (isName("manorArcher", guard.namelist) || isName("manorChanneler", guard.namelist)) { //channie/archer ranged
		//sendKoradin("searching shot rooms for "+guard.name);
		for each (var mRoom in manor.shotRooms) {
			//sendKoradin("searching "+mRoom);
			mRoom = getRoom(mRoom);
			//sendKoradin("searching "+mRoom.name);
			for each (var fTarg in mRoom.people) {
				if ( fTarg.vnum != 20720 && fTarg.level < 100 && !isName("horse", fTarg.namelist) && (manorOwner ? !fTarg.isGroupedWith(manorOwner) : true) && !fTarg.riddenBy && (fTarg.vnum < 20700 || fTarg.vnum > 20715) && fTarg.id != manor.ownerUserId && !arrContains(manor.allowedUsers, fTarg.id))
					targetArr.push(fTarg);
			}
		}
		if (targetArr.length > 0)
			return targetArr[random(0,targetArr.length-1)];
	}
	//sendKoradin("couldn't find a target for "+guard.name);
	return null;
}

/** GET ALL ROOMS IN ZONE **/
function getAllRoomsInZone(zoneVnum) {
	if (!zoneVnum)
		return [];
	var zoneBot = parseInt(zoneVnum+"00");
	var returnArr = [];
	var zoneTop = zoneBot+99;
	for (var i = 0; i < 100; i++) {
		if (getRoom(zoneBot+i)) {
			//sendKoradin("pushing");
			returnArr.push(getRoom(zoneBot+i));
		}
		else
			break;
	}
	return returnArr;
}

/** RETURN WHETHER ACTOR IS STANDING OUTSIDE A ROOM **/
JSRoom.prototype.hasNeighbor = function(roomVnum) {
	for each (var room in this.neighbors) {
		if (room) {
			if (room.vnum == roomVnum)
				return true;
		}
	}
	return false;
}

/** RETURN TRUE/FALSE IF THE MANOR IS BEING CLAIMED **/
function isBeingClaimed(manor) {
	if (!manor)
		return false;
	for each (var person in getRoom(manor.flagRoomVnum).people) {
		//sendKoradin(getSval(person,33200,"claim"));
		if (getSval(person,33200,"claim") == 1)
			return true;
	}
	return false;
}

/** UPDATE ALLOW LIST **/
function updateAllowed(manor, id) {
	if (!manor || !id)
		return;
	for (i=0; i<manor.allowedUsers.length; i++) {
		if (manor.allowedUsers[i] == id) {	
			manor.allowedUsers.splice(i,1);
			saveManor(manor);
			return;
		}
	}
	manor.allowedUsers.push(id);
	saveManor(manor);
}

/** LOAD SIEGE TEAM **/
function loadSiegeTeam(actor, manor, intType) {
	if (actor.race == constants.RACE_HUMAN)
		var mobVnum = 20716;
	else
		var mobVnum = 20717;
	if (!intType || intType == 1)
		var weaponVnum = 20716;
	//else if more weapon types here...
	if (manor.zoneId == 332) //gap prop 
		var loadRoom = getRoom(random(20690,20696));
	else if (manor.zoneId == 333) //stone road prop
		var loadRoom = getRoom(random(1058,1065));
	else if (manor.zoneId == 334) //tv prop
		var loadRoom = getRoom(random(1944,1951));
	//else if more properties here...
	loadRoom.loadMob(mobVnum);
	loadRoom.loadMob(mobVnum);
	loadRoom.people[1].comm("follow siege");
	loadRoom.people[0].comm("group all");
	loadRoom.loadObj(weaponVnum);
	setSval(loadRoom.items[0],33212,"owner",actor.id);
	setSval(loadRoom.items[0],33212,"goal",manor.outsideGateRoomVnum);
	getCharCols(actor);
	loadRoom.people[0].comm("tell "+actor.name+" We've received your orders to attack the "+manor.name+" near "+manor.area+"!");
	loadRoom.people[0].comm("tell "+actor.name+" We'll meet you near "+loadRoom.name+".");
}

/** RETURN TRUE IF ACTOR IS IN A MANOR AND IS OWNER/ALLOWED **/
function isCharAllowedAndInManor(actor, zoneId) {
	if (!actor || !zoneId)
		return false;
	var allowed = false;
	var inManor = false;
	for each (var manor in global.manors) {
		if (actor.id == manor.ownerUserId || arrContains(manor.allowedUsers, actor.id))
			allowed = true;
		if (zoneId == manor.zoneId)
			inManor = true;
	}
	if (allowed && inManor)
		return true;
	return false;
}




















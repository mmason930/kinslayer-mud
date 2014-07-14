var script33202 = function(self, actor, here, args, extra)
{
	waitpulse 1;
//sendKoradin("hey");
var manor = getManor(self);
if (!manor)
	return;
if (timeTillManorExpires(manor) == 0 && manor.ownerUserId > 0) {
	//sendKoradin("workin!");
	var owner = getManorOwnerById(manor.ownerUserId);
	if (owner) { //manor owner is online, warn him
		getCharCols(owner);
		owner.send(bld+"You have not paid the upkeep for your "+manor.name+" near "+manor.area+"!"+nrm);
		owner.send(bld+"Any stored items have been dumped outside the gate."+nrm);
	}
	ctfEcho("The owner of the "+manor.name+" near "+manor.area+" has been evicted!");
	waitpulse 1;
	resetManor(manor);
	return;
}
var outsideRoom = getRoom(manor.outsideGateRoomVnum);
if (!manor.gateIsBroken && manor.hasGate) {
	outsideRoom.closeDoor(outsideRoom.firstStep(getRoom(manor.insideGateRoomVnum)), true);
	outsideRoom.lockDoor(outsideRoom.firstStep(getRoom(manor.insideGateRoomVnum)), true);	
}
else {
	outsideRoom.unlockDoor(outsideRoom.firstStep(getRoom(manor.insideGateRoomVnum)), true);
	outsideRoom.openDoor(outsideRoom.firstStep(getRoom(manor.insideGateRoomVnum)), true);	
}
if (manor.race == constants.RACE_HUMAN) {
	var innKeepToFind = 20710;
	var barKeepToFind = 20712;
	var barTap = 21011;
	var bankerToFind = 4588;
}
else {
	var innKeepToFind = 20711;
	var barKeepToFind = 20713;
	var barTap = 21216;
	var bankerToFind = 4589;
}
var portToFind = 8278;
var innKeepFound = false;
var barKeepFound = false;
var barTapFound = false;
var bankerFound = false;
var portFound = false;
for each (var dude in getRoom(manor.innRoomVnum).people) {
	if (dude.vnum == innKeepToFind)
		innKeepFound = true;
	if (dude.vnum == bankerToFind)
		bankerFound = true;
}
if (innKeepFound == false)
	getRoom(manor.innRoomVnum).loadMob(innKeepToFind);
if (bankerFound == false)
	getRoom(manor.innRoomVnum).loadMob(bankerToFind);
for each (var dude in getRoom(manor.portalRoomVnum).people) {
	if (dude.vnum == portToFind)
		portFound = true;
}
if (portFound == false)
	getRoom(manor.portalRoomVnum).loadMob(portToFind);
for each (var dude in getRoom(manor.barRoomVnum).people) {
	if (dude.vnum == barKeepToFind)
		barKeepFound = true;
}
if (barKeepFound == false)
	getRoom(manor.barRoomVnum).loadMob(barKeepToFind);
for each (var thingie in getRoom(manor.barRoomVnum).items) {
	if (thingie.vnum == barTap)
		barTapFound = true;
}
if (barTapFound == false)
	getRoom(manor.barRoomVnum).loadObj(barTap);
sendKoradin("Resetting the "+manor.area+" "+manor.name+"...");
sendKoradin("Current Owner: "+getUserNameByUserId(manor.ownerUserId));
for each (var room in manor.guardRooms) {
	room = getRoom(room);
	var curGuards = numGuardsBought(room);
	sendKoradin("Current Guards for room "+room.vnum+": "+curGuards);
	if (manor.race == constants.RACE_HUMAN) {
		if (room.vnum == manor.flagRoomVnum)
			var guardArr = [20700,20701,20702];//scout, warrior, captain
		else if (arrContains(manor.rangeRooms, room.vnum))
			var guardArr = [20700,20703,20704];//scout, archer, channie
		else if (room.vnum == manor.outsideGateRoomVnum)
			var guardArr = [20714,20714,20714];
		else
			var guardArr = [20700,20700,20701];//scout, warrior, warrior
	}
	else {
		if (room.vnum == manor.flagRoomVnum)
			var guardArr = [20705,20706,20707];//scout, warrior, captain
		else if (arrContains(manor.rangeRooms, room.vnum))
			var guardArr = [20705,20708,20709];//scout, archer, channie
		else if (room.vnum == manor.outsideGateRoomVnum)
			var guardArr = [20715,20715,20715];
		else
			var guardArr = [20705,20705,20706];//scout, warrior, warrior
	}
	var flagFound = false;
	if (room.vnum == manor.flagRoomVnum) {
		for each (var item in room.items) {
			if (item.vnum == 21010)
				flagFound = true;
		}
		if (flagFound == false)
			room.loadObj(21010);
	}
	if (curGuards > 0) {
		if (room.vnum == manor.outsideGateRoomVnum) {
			if (room.vnum == 33210) //gap outpost
				var checkArr = [33210,33214,33215,33216];
			else if (room.vnum == 33300) //gap outpost
				var checkArr = [33300,33314,33315,33316];
			else if (room.vnum == 33400) //gap outpost
				var checkArr = [33400,33413,33414,33415];
			var countOutsideGuards = 0;
			for each (var rm in checkArr) {
				for each (var mb in getRoom(rm).people) {
					if (mb.vnum == guardArr[0])
						countOutsideGuards += 1;
				}
			}
			var guardsToLoad = curGuards - countOutsideGuards;
			while (guardsToLoad > 0) {
				sendKoradin("Loading an outside guard");
				room.loadMob(guardArr[0]);
				guardsToLoad -= 1;
			}
			continue;
		}
		for (var i=0; i<curGuards; i++) {
			var found = 0;
			for each (var mob in room.people) {
				if (mob.vnum == guardArr[i])
					found += 1;
			}
			if (found == 0) {
				sendKoradin("Loading "+guardArr[i]);
				room.loadMob(guardArr[i]);
			}
			else if (numGuardsBought(room) > 1 && found == 1 && ((guardArr[i] == 20700 || guardArr[i] == 20705) && manor.flagRoomVnum != room.vnum && !arrContains(manor.rangeRooms, room.vnum))) {
				sendKoradin("Loading "+guardArr[i]);
				room.loadMob(guardArr[i]);
			}
		}
	}
}

















};


var script33205 = function(self, actor, here, args, extra) {
	_block;
	var manors = [];
	for each (var mr in global.manors) {
		if (mr.ownerUserId == actor.id || arrContains(mr.allowedUsers, actor.id))
			manors.push(mr);
	}
	if (manors.length == 0) {
		actor.send("You must either own or be allowed at a property in order to recall.");
		return;
	}
	getCharCols(actor);
	for each (var mnr in manors) {
		if (isBeingClaimed(mnr))
			var manor = mnr;
	}
	if (!manor) {
		actor.send(bld+"No one is attempting to claim any properties you're allowed in right now..."+nrm);
		return;
	}
	if (!manor.hasPortal) {
		actor.send(bld+"The portal for this "+manor.name+" is currently closed."+nrm);
		if (actor.id == manor.ownerUserId)
			actor.send(bld+"\nStand outside it and type BUY PORTAL."+nrm);
		return;
	}
	if (!isBeingClaimed(manor)) {
		actor.send(bld+"No one is attempting to claim the "+manor.name+" near "+manor.area+" right now..."+nrm);
		return;
	}
	// if (actor.gold < 20000) {
	// 	actor.send(bld+"You don't have the coin to pay for it!"+nrm);
	// 	return;
	// }
	if (actor.room.vnum == 500 || actor.room.vnum == 502) { // trying to recall from crossroads
		actor.send("You can't recall from here...");
		return;
	}
	actor.send(bld+"You attempt to recall to the "+manor.name+" near "+manor.area+"..."+nrm);
	actor.startTimer(20);
	var success = runTimer(actor);
	if (!success || actor.fighting) {
		actor.send("Your recall attempt has been interrupted.");
		return;
	}
	actor.send(bld+"\nYou recall to the "+manor.name+"."+nrm);
	act("$n recalls to $s "+manor.name+", winking out of existence.", true, actor, null, null, constants.TO_ROOM);
	// actor.gold -= 20000;
	actor.moveToRoom(getRoom(manor.portalRoomVnum));
	actor.comm("look");
	
}
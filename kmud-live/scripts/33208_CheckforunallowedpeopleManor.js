var script33208 = function(self, actor, here, args, extra)
{
	return; // maybe bring back later?
var timeLeft = 78;
var manor = getManor(self);
if (!manor)
	return;
if (self.vnum == manor.innRoomVnum)
	var trueCheck = manor.hasInn;
else if (self.vnum == manor.barRoomVnum)
	var trueCheck = manor.hasBar;
else if (self.vnum == manor.storageRoomVnum)
	var trueCheck = manor.hasStorage;
else if (self.vnum == manor.portalRoomVnum)
	var trueCheck = manor.hasPortal;
while (timeLeft > 0) {
	//sendKoradin("checking "+self.vnum);
	for each (var dude in self.people) {
		if (dude.vnum < 0) {
			if ( (dude.id != manor.ownerUserId && !arrContains(manor.allowedUsers, dude.id)) || trueCheck == false) {
				getCharCols(dude);
				dude.send(bld+"You aren't allowed in this room!"+nrm);
				for each (var neighbor in self.neighbors) {
					if (neighbor) {
						if (neighbor.sector != constants.SECT_INSIDE)
							var dir = neighbor;
					}
				}
				dude.moveToRoom(dir);
				dude.comm("look");
				act("$n stumbles into the room from the "+dirToText(dude.room.firstStep(self))+".",true,dude,null,null,constants.TO_ROOM);
			}
		}
	}
	timeLeft -= 1;
	waitpulse 1;
}


};


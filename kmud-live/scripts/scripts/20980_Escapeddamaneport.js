var script20980 = function(self, actor, here, args, extra) {
	var roomArr = [81, 10, 333, 80, 85, 20, 332, 18, 66, 181, 206, 193, 64, 17, 93, 19, 334, 103, 70, 100, 3, 1, 70, 214, 217, 4, 31, 201, 9, 230, 235, 100, 186, 53, 101, 102, 45, 134, 132];
	var newRoom = getRandomRoom();
	var aMob = getMobAtRoom(20800, 20804);
	var neck1 = self.eq(constants.WEAR_NECK_1);
	if(neck1){ neck1 = neck1.vnum; }
	var neck2 = self.eq(constants.WEAR_NECK_2);
	if(neck2){ neck2 = neck2.vnum; }
	if(newRoom && !self.isBashed && random(1,20) == 1 && neck1 != 1120 && neck2 != 1120){
		if(arrContains(roomArr, newRoom.zoneVnum)){
			act(self.name+" disappears in a flash of bright light.",true,self,null,null,constants.TO_ROOM);
			self.moveToRoom(newRoom);
			act(self.name+" arrives in a flash of bright light!",true,self,null,null,constants.TO_ROOM);
			if(aMob){
				setSval(aMob, 20980, self.vnum, newRoom.zoneVnum);
				//sendKoradin(getSval(aMob, 20980, self.vnum));
			}
		}
	}else{
		if(aMob){
			setSval(aMob, 20980, self.vnum, self.room.zoneVnum);
		}
	}
}

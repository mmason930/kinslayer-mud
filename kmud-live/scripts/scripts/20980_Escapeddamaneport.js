var script20980 = function(self, actor, here, args, extra) {
	var newRoom = getRandomRoom();
	if(newRoom && !self.isBashed){
		act(self.name+" disappears in a flash of bright light.",true,self,null,null,constants.TO_ROOM);
		self.moveToRoom(newRoom);
		act(self.name+" arrives in a flash of bright light!",true,self,null,null,constants.TO_ROOM);
	}
}
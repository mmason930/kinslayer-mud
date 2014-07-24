var script20980 = function(self, actor, here, args, extra) {
	var newRoom = getRandomRoom();
	if(newRoom){
		act(self.name+" takes a last look around before disappearing in a flash of bright light.",true,actor,null,null,constants.TO_ROOM);
		self.moveToRoom(newRoom);
		act(self.name+" arrives in a flash of bright light!",true,actor,null,null,constants.TO_ROOM);
	}
}
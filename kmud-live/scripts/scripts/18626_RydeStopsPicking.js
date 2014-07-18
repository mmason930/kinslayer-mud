var script18626 = function(self, actor, here, args, extra) {
	//Ryde stops you from picking
	//Rhollor APril 2010
	_block;
	if(self.room.vnum != 18657) {
		_noblock;
		return;
	}
	actor.send(self.name + " glances at what you're doing.");
	waitpulse 1;
	self.comm("say Stop what you're doing!");
	if(actor.position != constants.POS_FIGHTING) {
		actor.send(self.name + " gives you a good kick as he pushes you out the door!");
		echoaround(actor, self.name + " gives " + actor.name + " a good kick as he pushes " + actor.himHer() + " out the door!");
		actor.moveToRoom(getRoom(18640));
		actor.comm("look");
	}
	return;
}
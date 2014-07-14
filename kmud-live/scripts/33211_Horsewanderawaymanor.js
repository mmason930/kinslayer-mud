var script33211 = function(self, actor, here, args, extra)
{
	var timer = getSval(self,33209,"horse");
if (!timer)
	return;
if (timer < time() && !self.riddenBy) {
	act("$n wanders off.",true,self,null,null,constants.TO_ROOM);
	self.moveToRoom(getRoom(20899));
	return;
}

};


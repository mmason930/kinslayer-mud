var script13218 = function(self, actor, here, args, extra) {
	_noblock;
	var mount = actor.mount;
	
	if(!mount && actor.isHorse())
		mount = actor;
	
	if(mount && mount.hitchedBy)
	{
		var roomBefore = mount.room;
		waitpulse 1;
		
		var roomNow = mount.room;
		
		if(roomNow != roomBefore)
		{
			act("$n pulls $p from behind.", false, mount, mount.hitchedBy, null, constants.TO_NOTVICT);
			act("You pull $p from behind.", false, mount, mount.hitchedBy, null, constants.TO_CHAR);
			mount.hitchedBy.moveToRoom(mount.room);
			mount.mvs -= 1;
		}
	}
}
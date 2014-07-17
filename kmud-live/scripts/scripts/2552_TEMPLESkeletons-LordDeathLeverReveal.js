var script2552 = function(self, actor, here, args, extra) {
	act("$n shatters into a heap on the ground!", false, self, null, null, constants.TO_ROOM);
	var scepter = self.eq(constants.WEAR_HOLD);
	
	if(scepter != null)
		scepter.moveToRoom(here);
	
	setTimeout(5, function() {
	
		if(scepter.isValid)
		{
			act("$p lands on the ground with thud.", false, null, scepter, null, constants.TO_ROOM);
			scepter.attach(TempleManager.skeletonScepterShatterScriptVnum);
		}
	});
}
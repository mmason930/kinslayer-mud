var script14230 = function(self, actor, here, args, extra) {
	_block;
	actor.send("An unseen force prevents you from going there...");
	act("$n tries to leave, but an unseen force stops $m.",true,actor,null,null,constants.TO_ROOM);
}
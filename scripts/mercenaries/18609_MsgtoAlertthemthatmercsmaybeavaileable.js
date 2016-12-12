var script18609 = function(self, actor, here, args, extra) {

	if(MercUtil.getMercenaryArray(self.vnum) != null) {

		waitpulse 2;
		getCharCols(actor);
		actor.send(bld + yel + "Type 'list' to view available mercenaries." + nrm);
	}
};
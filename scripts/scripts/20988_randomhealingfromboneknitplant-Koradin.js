var script20988 = function(self, actor, here, args, extra) {
	act(self.cyan(constants.CL_SPARSE) + "You feel the healing properties of a boneknit plant soothing you..." + self.normal(constants.CL_SPARSE), true, self, null, null, constants.TO_CHAR);
	act("$n stands a little taller, looking stronger and more healthy.", true, self, null, null, constants.TO_ROOM);
	var decay = getSval(self,20988,"decay status");
	if (decay == null)
		decay = 0;
	decay = (100 - decay);
	var heal = Math.floor( random(12,15) * (decay / 100) );
	self.hps += heal;
	setSval(self, 20988, "healing effect", (getSval(self, 20988, "healing effect") + 1))
	if (getSval(self, 20988, "healing effect") > 5)
	{
		setSval(self, 20988, "healing effect", 0);
		setSval(self, 20988, "decay status", 0);
		self.detach(20988);
		return;
	}
}
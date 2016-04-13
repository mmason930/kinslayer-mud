var script2506 = function(self, actor, here, args, extra) {
	//talisman maintain sanc
	//Koradin - April 2010
	if (!self.wornBy)
		return;
	if (self.wornBy.affectedBy(constants.AFF_SANCTUARY))
		return;
	self.wornBy.affect(constants.AFF_SANCTUARY,1000,0);
}
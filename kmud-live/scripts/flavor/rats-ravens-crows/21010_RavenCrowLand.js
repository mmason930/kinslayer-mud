var 21010_ravenCrowLand = function(self, actor, here, args, extra) {
	if(self.position == constants.POS_FLYING) {
		self.comm("say Flying!");
	} else {
		self.comm("say "+self.position);
	}
};
var ravenCrowLand = function(self, actor, here, args, extra) {
	if(self.position == constants.POS_FLYING) {
		self.comm("emote starts to slowly descend from the sky and lands, eyes flashing in your direction.");
		self.position = constants.POS_STANDING;
	} else {
		self.comm("emote flaps " + self.hisHer() + " wings and begins circling above you.");
		self.position = constants.POS_FLYING;
	}
};
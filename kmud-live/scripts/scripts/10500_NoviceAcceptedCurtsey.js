var script10500 = function(self, actor, here, args, extra) {
	if (self.vnum == 10501)
		var rank = 2;
	else
		var rank = 4;
	//sendKoradin(actor.getRank(16));
	var name = actor.name;
	if (actor.vnum == -1 && actor.inClan(16) && actor.getRank(16) > rank) {
		waitpulse 3;
		self.comm("curtsey "+name);
	}
}
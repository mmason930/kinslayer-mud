var script187 = function(self, actor, here, args, extra) {
	var mobs = here.people;
	for(var mobIndex = 0;mobIndex < mobs.length;++mobIndex) {
		var mob = mobs[ mobIndex ];
		if( mob.inClan(2) )
			self.comm("flee");
	}
}
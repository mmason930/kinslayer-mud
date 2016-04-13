var script21381 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var pets = 	[
					["parrot", 400, 21322],
					["dog"   , 300, 10317],
					["cat"   , 600, 21323]
				];
	for( var i = 0;i < pets.length;++i )
	{
		var petData = pets[i];
		if( !str_cmp(petData[0], vArgs[1]) ) {
			if( actor.gold - petData[1] < 0 ) {
				self.say("You cannot afford it.");
				return;
			}
			self.say("Thank you for your purchase!");
			var p = here.loadMob( petData[2] );
			p.comm("follow " + actor.namelist.split(" ")[0]);
			actor.gold -= petData[1];
			return;
		}
	}
	self.say("We don't sell that here.");
}
var script23197 = function(self, actor, here, args, extra) {
	wait 2;
	switch( random(1,3) )
	{
	case 1:
		self.comm("emote rolls around on the ground.");
		break;
	case 2:
		self.comm("emote bats its paws at the air.");
		break;
	case 3:
		self.comm("emote cocks its head to the side.");
		break;
	}
}
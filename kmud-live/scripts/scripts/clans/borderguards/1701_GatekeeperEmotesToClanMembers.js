var script1701 = function(self, actor, here, args, extra) {

	if(!actor.inClan(3) || self.position <= constants.POS_FIGHTING || !self.canSee(actor))
		return;

	waitpulse 2;

	if(actor.room != self.room || !self.canSee(actor) || self.position <= constants.POS_FIGHTING)
		return;

	switch(random(1, 5))
	{
		case 1:
			self.comm("nod " + actor.name);
			break;
		case 2:
			self.say("Peace favor your sword, " + actor.name + ".");
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
	}
};
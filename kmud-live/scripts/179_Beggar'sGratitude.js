var script179 = function(self, actor, here, args, extra)
{
	if( extra.amount > 0 ) {
	wait 1;
	self.say("Thank you, " + actor.name + ". Thank you so much!");
	wait 2;
	self.comm("emote slips the coins into an old brown sack.");
	wait 1;
	self.comm("emote smiles, producing a mouthful of rotting teeth.");
}

};


var script13203 = function(self, actor, here, args, extra) {
	if ( getSval(actor, 13203, "Story") != 1 )
		return;
	if( extra.obj.vnum == 21302 ) {
		extra.obj.extract();
		waitpulse 6;
		self.inventory[0].extract(21302);
		waitpulse 6;
		self.say("Ah, you want to here my story, eh?");
		waitpulse 20;
		self.say("Not much to tell, sadly. I was a hunter of the Horn, one of the best!");
		waitpulse 20;
		self.comm ("sigh");
		waitpulse 20;
		self.say("I was sure the Horn was in the Mountains of Mist, so I hiked the trails, explored caves, excavated all types of ores...");
		waitpulse 22;
		here.echo("The guest glances at " + actor.name + " before taking a long swig of ale.");
		waitpulse 24;
		self.say("I don't know...maybe if I had some reason...another way to start looking for the Horn again...");
		waitpulse 22;
		here.echo("The guest shrugs, and returns to his now cold meal.");
		setSval(actor, 13203, "Story", 2);
	}
}
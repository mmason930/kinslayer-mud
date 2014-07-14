var script298 = function(self, actor, here, args, extra)
{
	if( actor.name == "Raiden" ) {
waitpulse 6;
    self.say("Well, well, I figured we'd be getting a visit from the Queen's Guards. What took you so long to get here?");
waitpulse 15;
here.echo("A few of the hooded men in the room snicker softly.");
waitpulse 18;
self.say("I suppose you're here to....talk about the daughter-heir missing?");
waitpulse 15;
self.comm("growl");
waitpulse 18;
self.say("As embarrassed as it is to admit this, we were double-crossed. The bandits we worked with never met with us at the rendezvous camp.");
waitpulse 18;
here.echo("Lord Comar shrugs before continuing on.");
waitpulse 15;
self.say("They're just east of Whitebridge though, so my masters will take care of them soon enough. But now that you know the plan, I can't let you leave alive.");
waitpulse 6;
self.comm("close door");
here.echo("Lord Comar quickly shuts and locks the door.");
waitpulse 6;
self.comm("lock door");
waitpulse 6;
self.comm("kill Raiden");
}
else if( actor.race == constants.RACE_TROLLOC ) {
waitpulse 6;
    self.say("I live to serve!");
waitpulse 12;
self.comm("grovel");
waitpulse 12;
self.say("Master, how may I serve you?");
waitpulse 25;
self.say("I'm afraid we...don't have the daughter-heir. The bandits we hired to assist us never rendezvoused with us.");
waitpulse 20;
self.say("Their base is located somewhere east of WhiteBridge.");
}

};


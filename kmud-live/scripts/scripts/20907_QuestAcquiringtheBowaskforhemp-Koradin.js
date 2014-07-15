var script20907 = function(self, actor, here, args, extra) {
	//For quest: Acquiring the Bow
	//Koradin - January 2010
	if (actor.quest('Acquiring the Bow') < 1)
		return;
	else if (actor.hasItem(1715,true,true,true))
	{
		waitpulse 3;
		self.say("What's wrong with the string you've got already?");
		return;
	}
	waitpulse 3;
	self.say("That darn ranger is always telling folks to ask me for handouts!");
	wait 2;
	self.say("I suppose it's for a good cause, though...");
	self.comm("emote brightens up a bit.");
	wait 2;
	self.say("Here you are. Remember to visit me in the future for all your tailoring needs!");
	actor.loadObj(1715);
	actor.send(self.name + " hands you a coiled length of hemp string.");
}
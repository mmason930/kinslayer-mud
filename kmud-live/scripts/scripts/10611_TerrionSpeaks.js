var script10611 = function(self, actor, here, args, extra) {
	if (actor.quest('Search and Rescue') > 0 && !actor.questTaskProgress('Search and Rescue')[3].comp ) {
	waitpulse 7
	self.comm("emote scrubs a hand through his hair, scowling darkly.");
	waitpulse 21;
	self.comm("emote yells 'Where the devil is my daughter?! Find her!!!'");
	waitpulse 21;
	self.comm("emote notices your arrival and approaches you quickly.");
	waitpulse 21;
	self.comm("say So...a " + classNumToText( actor.class ) + "...that will have to do I suppose.");
	waitpulse 21;
	self.comm("stroke");
	waitpulse 21;
	self.comm("say I think I can find a use for you, but you must follow my instructions to the letter.");
	waitpulse 21;
	self.comm("emote paces around the room as he talks, whipping dark looks at anyone who crosses his path.");
	waitpulse 21;
	self.comm("say Thieves and tricksters are always after my treasures - I have quite the collection of artifacts here, if you haven't noticed.");
	waitpulse 21;
	self.comm("say In the past they have tried a plot or two to get them in their grasp, but I have always managed to keep my daughter out of all that.");
	waitpulse 21;
	self.comm("say This time, it would seem she is the innocent victim of their greed...they have kidnapped her! I am sure of it!");
	waitpulse 21;
	self.comm("emote mutters to himself, 'Who else could it be?'");
	waitpulse 21;
	self.comm("say My daughter Mara was last seen on her way to Whitebridge. She planned to spend the day shopping in the seasonal bazaar in the city center.");
	waitpulse 21;
	self.comm("say Find her, " + actor.name + ", and you shall find the rewards much to your liking...I will compensate you with coin");
	waitpulse 21;
	self.comm("say and if you bring her back quickly I might even part with one of my treasures.");
	waitpulse 21;
	self.comm("scowl");
	}
}
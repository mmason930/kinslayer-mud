var script10601 = function(self, actor, here, args, extra)
{
	var qName = 'Search and Rescue';
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
     waitpulse 14
     actor.send("A shifty-eyed woman produces a crumpled piece of paper from her pocket and quickly motions for you to take it.");
     self.loadObj(10605);
     waitpulse 14
     self.comm("give map " + actor.name);
     waitpulse 14
     self.comm("say Follow the instructions on that paper to find Lord Terrion's camp out the Eastern Plains.");
     waitpulse 14
     self.comm("say Terrion will surely wish to speak to you now. Do not keep him waiting.")
     waitpulse 21
     actor.updateJournalTask('Search and Rescue', 0);
}

};


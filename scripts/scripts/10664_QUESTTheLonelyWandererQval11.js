var script10664 = function(self, actor, here, args, extra) {
	//Jenkin
	//October 2010
	//Quest: The Cover of Darkness
	var qName = 'The Lonely Wanderer'
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
	   actor.send("Your purpose has been achieved.");
	   waitpulse 5;
	   actor.updateJournalTask(qName, 0);
	}
}
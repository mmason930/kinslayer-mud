var script10650 = function(self, actor, here, args, extra) {
	//Jenkin 
	//October 2010
	//Quest: An Artifact of a Lost Age
	var qName = 'An Artifact of a Lost Age';
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
	     waitpulse 5;
	     getCharCols(actor);
	     actor.send(bld+cyn+"As you enter the laboratory, you feel a sense of dread, and awe."+nrm);
	     waitpulse 10;
	     actor.updateJournalTask(qName, 0);
	}
}
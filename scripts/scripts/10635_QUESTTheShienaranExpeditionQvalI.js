var script10635 = function(self, actor, here, args, extra) {
	//Jenkin 
	//September 2010
	//Quest: Shienaran Expedition
	var qName = 'The Shienaran Expedition';
	var quest = Quest.getByName(qName);
	if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[1].comp ) {
	     waitpulse 5;
	     getCharCols(actor);
	     actor.send(bld+cyn+"You spot a large package of provisions and strap it to your back."+nrm);
	     waitpulse 10;
	     actor.addQuestItem("a large package of provisions",1,quest);
	     actor.updateJournalTask(qName, 0);
	}
}
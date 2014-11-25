var script6671 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Armor of Ghosts
	/** Entry Trigger **/
	var qName = "Armor of Ghosts";
	if (Quest.getByName(qName).tasks[0].isInProgress(actor)) {
		waitpulse 3;
		getCharCols(actor);
		actor.send(bld+cyn+"The mist swirls around you and the faint sound of a whispering voice is carried to you as if by an invisible wind."+nrm);
		actor.updateJournalTask(qName,0);
		actor.qval(qName,2);
	}
}
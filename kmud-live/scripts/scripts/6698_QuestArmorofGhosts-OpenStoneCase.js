var script6698 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Armor of Ghosts
	/** Command Trigger: open case/stone **/
	var qName = "Armor of Ghosts";
	var quest = Quest.getByName(qName);
	var vArgs = getArgList(args);
	if ( actor.quest(qName) == 6 && arrContains( "stone case".split(" "), vArgs[1] ) ) {
	        _block;
	        getCharCols(actor);
	        actor.send(bld+cyn+"You strain yourself but manage to slide the lid of the stone case aside, revealing the lost armor of Hok'dar."+nrm);
	        actor.addQuestItem("the lost armor of Hok'dar",1,quest);
	        actor.qval(qName,7);
	        actor.updateJournalTask(qName,4);
	}
}
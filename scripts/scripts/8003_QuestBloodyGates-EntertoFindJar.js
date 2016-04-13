var script8003 = function(self, actor, here, args, extra) {
	//Alder
	//August 2010
	//Quest: Bloody Gates
	/** Command Trigger: paint (room 6401) **/
	var qName = "Bloody Gates";
	var quest = Quest.getByName(qName);
	if ( Quest.getByName(qName).hasBegun(actor) && !actor.questTaskProgress(qName)[1].comp ) {
		waitpulse 5;
		getCharCols(actor);
		actor.send(bld+cyn+"You spot a jar by the blood-filled well and fill it with the red liquid."+nrm);
		waitpulse 10;
		actor.addQuestItem("a jar of thick blood",1,quest);
		Quest.getByName(qName).updateTask(actor, 0);
	}
}
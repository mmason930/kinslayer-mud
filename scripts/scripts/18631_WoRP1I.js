var script18631 = function(self, actor, here, args, extra) {
	//Initiation of Part one of WoR_P1
	//Rhollor April 2010
	if(actor.race != constants.RACE_HUMAN) {
		return;
	}
	/********QUEST BEGIN********/
	var qName = "Finding the Way";
	var quest = getQuestByName(qName);
	if ( actor.quest(qName) != 0 )   {
		return;
	}
	/**SPECIAL DIALOGUE**/
	var special_dialogue = quest.dialogue[0];
	wait 1;
	actor.send("You notice the title of the pamphlet, \"The Way of ...\" but it is partially burned away.");
	actor.send("You place the remains of the pamphlet in your journal.");
	actor.send("You begin the quest '" + bld + cyn + qName + nrm + "'");
	actor.recreateJournal();
	/**CREATE JOURNAL ENTRY FOR QUEST**/
	waitpulse 1;
	actor.journalEdit("ADD",qName);
	self.extract();
}
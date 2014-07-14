var script18633 = function(self, actor, here, args, extra)
{
	//WoR_P2 Enter Kharsk house
//Rhollor April 2010
var qName = "Confront the Merchant";
if(extra.direction == constants.NORTH && actor.quest(qName) > 0) {
	var quest = getQuestByName(qName);
	waitpulse 1;
	actor.updateJournalTask(qName,0);
}

};


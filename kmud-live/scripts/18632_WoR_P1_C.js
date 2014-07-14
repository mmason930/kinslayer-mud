var script18632 = function(self, actor, here, args, extra)
{
	//Completion of WoR_P1
//Rhollor April 2010
//Attached in room 18608
var vArgs = getArgList(args);
if ( vArgs.length < 2) {
	return;
}
else if(strn_cmp(vArgs[1].toLowerCase(),"south",1)) {
	var qName = "Finding the Way";
	var quest = getQuestByName(qName);
	if(actor.quest(qName) > 1) {
		return;
	}
	else if(actor.quest(qName) > 0) {
		wait 1;
		actor.send("You notice a symbol that you have seen before below this house's plaque!");
		actor.updateJournalTask(qName, 0);
	}
}

};


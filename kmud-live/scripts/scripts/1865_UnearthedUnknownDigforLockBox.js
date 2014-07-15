var script1865 = function(self, actor, here, args, extra) {
	// Alder
	// Quest: Unearthed, Unknown
	var qName = "Unearthed, Unknown";
	if ( actor.quest(qName) && !actor.questTaskProgress(qName)[1].comp ) {
		_block;
		if ( !actor.hasItem(13205,false,true,false) ) {
			actor.send("You need a shovel in your hands to dig here.");
			return;
		}
		act("You begin digging...\n", true, actor, null, null, constants.TO_CHAR);
		act("$n begins digging for something.", true, actor, null, null,constants.TO_ROOM);
		actor.startTimer(5);
		var success = runTimer(actor);
		if ( success ) {
			act("\nYour shovel strikes something metallic and you pry the object from the ground.", true, actor, null, null, constants.TO_CHAR);
			act("$n's shovel strikes something metallic and $e pries the object from the ground.", true, actor, null, null, constants.TO_ROOM);
			actor.addQuestItem("a rusty lock box",1,qName);
			actor.updateJournalTask(qName,0);
		}
		else {
			actor.send("\nYou stop digging.");
			return;
		}
	}
}